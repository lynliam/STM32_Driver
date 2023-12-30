//
// Created by tony on 2023/4/12.
//

#include "Mpu6050.h"

#include "main.h"
#include <stdlib.h>
#include <math.h>
#include "retarget.h"

#include "./driver/eMPL/inv_mpu_dmp_motion_driver.h"
#include "mllite/data_builder.h"
#include "mllite/ml_math_func.h"

#define q30 1073741824.0
#define MPL_LOGI printf
struct platform_data_s {
    signed char orientation[9];
};
struct Mpu6050 mpu6050;


/* The sensors can be mounted onto the board in any orientation. The mounting
 * matrix seen below tells the MPL how to rotate the raw data from the
 * driver(s).
 * TODO: The following matrices refer to the configuration on internal test
 * boards at Invensense. If needed, please modify the matrices to match the
 * chip-to-body matrix for your particular set up.
 */
static struct platform_data_s gyro_pdata = {
        .orientation = { 1, 0, 0,
                         0, 1, 0,
                         0, 0, 1}
};

static inline int run_self_test(void)
{
    int result;
    long gyro[3], accel[3];

//#if defined (MPU6500) || defined (MPU9250)
//    result = mpu_run_6500_self_test(gyro, accel, 0);
//#elif defined (MPU6050) || defined (MPU9150)
//    result = mpu_run_self_test(gyro, accel);
//#endif
    result = mpu_run_self_test(gyro, accel);
    if (result == 0x7) {
        MPL_LOGI("Passed!\n");
        MPL_LOGI("accel: %7.4f %7.4f %7.4f\n",
                 accel[0]/65536.f,
                 accel[1]/65536.f,
                 accel[2]/65536.f);
        MPL_LOGI("gyro: %7.4f %7.4f %7.4f\n",
                 gyro[0]/65536.f,
                 gyro[1]/65536.f,
                 gyro[2]/65536.f);

        /* Push the calibrated data to the MPL library.
         *
         * MPL expects biases in hardware units << 16, but self test returns
		 * biases in g's << 16.
		 */
        unsigned short accel_sens;
        float gyro_sens;

        mpu_get_accel_sens(&accel_sens);
        accel[0] *= accel_sens;
        accel[1] *= accel_sens;
        accel[2] *= accel_sens;
        inv_set_accel_bias(accel, 3);
        mpu_get_gyro_sens(&gyro_sens);
        gyro[0] = (long) (gyro[0] * gyro_sens);
        gyro[1] = (long) (gyro[1] * gyro_sens);
        gyro[2] = (long) (gyro[2] * gyro_sens);
        inv_set_gyro_bias(gyro, 3);
        return 0;
    } else
    {
        if (!(result & 0x1))
            MPL_LOGI("Gyro failed.\n");
        if (!(result & 0x2))
            MPL_LOGI("Accel failed.\n");
        if (!(result & 0x4))
            MPL_LOGI("Compass failed.\n");
        return -1;
    }


}

uint8_t mpu_dmp_init() {
    struct int_param_s intParams;
    int state = mpu_init(&intParams);
    uint8_t res = 0;
    res = mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);//设置所需要的传感器
    if (res)return 1;
    res = mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);//设置FIFO
    if (res)return 2;
    res = mpu_set_sample_rate(20);    //设置采样率
    if (res)return 3;
    res = dmp_load_motion_driver_firmware();        //加载dmp固件
    if (res)return 4;
    res = dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_pdata.orientation));//设置陀螺仪方向
    if (res)return 5;
    res = dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |    //设置dmp功能
                             DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
                             DMP_FEATURE_GYRO_CAL);
    if (res)return 6;
    res = dmp_set_fifo_rate(20);    //设置DMP输出速率(最大不超过200Hz)
    if (res)return 7;
    res = run_self_test();        //自检
    if (res)return 8;
    res = mpu_set_dmp_state(1);    //使能DMP
    if (res)return 9;
    return 0;
}

uint8_t mpu_dmp_get_data()
{

    float q0=1.0f,q1=0.0f,q2=0.0f,q3=0.0f;
    unsigned long sensor_timestamp;
    short gyro[3], accel[3], sensors;
    unsigned char more;
    long quat[4];
    if(dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors,&more))return 1;
    /* Gyro and accel data are written to the FIFO by the DMP in chip frame and hardware units.
     * This behavior is convenient because it keeps the gyro and accel outputs of dmp_read_fifo and mpu_read_fifo consistent.
    **/
    /*if (sensors & INV_XYZ_GYRO )
    send_packet(PACKET_TYPE_GYRO, gyro);
    if (sensors & INV_XYZ_ACCEL)
    send_packet(PACKET_TYPE_ACCEL, accel); */
    /* Unlike gyro and accel, quaternions are written to the FIFO in the body frame, q30.
     * The orientation is set by the scalar passed to dmp_set_orientation during initialization.
    **/
    if(sensors&INV_WXYZ_QUAT)
    {
        q0 = quat[0] / q30;	//q30格式转换为浮点数 q30 = 2^30
        q1 = quat[1] / q30;
        q2 = quat[2] / q30;
        q3 = quat[3] / q30;
        //计算得到俯仰角/横滚角/航向角
        mpu6050.pitch = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3;	// pitch
        mpu6050.roll  = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3;	// roll
        mpu6050.yaw   = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;	//yaw
    }else return 2;
    return 0;
}








