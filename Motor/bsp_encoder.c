//
// Created by lynliam on 23-4-8.
//
#include "bsp_encoder.h"

float temp[9];
float_to_byte data_imu[13];
uint8_t begin_code = 0x55;
uint8_t end_code = 0xff;

void trans_float()
{
    HAL_UART_Transmit(&huart3,&begin_code,1,100);
    for(int i=0;i<9;i++)
    {
        data_imu[i].data=temp[i];
        for(int j=0;j<4;j++)
        {
            HAL_UART_Transmit(&huart3,&data_imu[i].byte[j],1,100);;
        }
    }
    for(int i=9;i<13;i++)
    {
        data_imu[i].data = motor[i-9].actual_speed;
        for(int j=0;j<4;j++)
        {
            HAL_UART_Transmit(&huart3,&data_imu[i].byte[j],1,100);;
        }
    }
    HAL_UART_Transmit(&huart3,&end_code,1,100);
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==&htim6)
    {
        //printf("ddd\n");
        motor_pid_control(&motor[0]);
        motor_pid_control(&motor[1]);
        motor_pid_control(&motor[2]);
        motor_pid_control(&motor[3]);
    }

    if (htim == &htim7) {
        /*IMU test*/
        CmdProcess();
        //printf("ddd\n");
        if (s_cDataUpdate) {
            for (int i = 0; i < 3; i++) {
                fAcc[i] = sReg[AX + i] / 32768.0f * 16.0f;
                fGyro[i] = sReg[GX + i] / 32768.0f * 2000.0f;
                fAngle[i] = sReg[Roll + i] / 32768.0f * 180.0f;
            }
            if (s_cDataUpdate & ACC_UPDATE) {
                //printf("acc:%.3f %.3f %.3f\r\n", fAcc[0], fAcc[1], fAcc[2]);
                temp[0]=fAcc[0];
                temp[1]=fAcc[1];
                temp[2]=fAcc[2];
                s_cDataUpdate &= ~ACC_UPDATE;
            }
            if (s_cDataUpdate & GYRO_UPDATE) {
                //printf("gyro:%.3f %.3f %.3f\r\n", fGyro[0], fGyro[1], fGyro[2]);
                temp[3]= fGyro[0];
                temp[4]= fGyro[1];
                temp[5]= fGyro[2];
                s_cDataUpdate &= ~GYRO_UPDATE;
            }
            if (s_cDataUpdate & ANGLE_UPDATE) {
                //printf("angle:%.3f %.3f %.3f\r\n", fAngle[0], fAngle[1], fAngle[2]);
                temp[6]=fAngle[0];
                temp[7]=fAngle[1];
                temp[8]=fAngle[2];
                s_cDataUpdate &= ~ANGLE_UPDATE;
            }
            if (s_cDataUpdate & MAG_UPDATE) {
                //printf("mag:%d %d %d\r\n", sReg[HX], sReg[HY], sReg[HZ]);
                s_cDataUpdate &= ~MAG_UPDATE;
            }
        }
        //printf("ddd\n");
        /*IMU test*/
    }

    if(htim == &htim14)
    {
        //printf("acc:%.3f %.3f %.3f, gyro:%.3f %.3f %.3f, angle:%.3f %.3f %.3f %f,%f,%f,%f\r\n",temp[0],temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7], temp[8], motor[0].actual_speed,motor[1].actual_speed,motor[2].actual_speed,motor[3].actual_speed);
    }

    /*
    else if(htim==&(motor[0].TIM_EncoderHandle))
    {
        if(__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2))
            motor[0].motor_overflow_counter.Encoder_Overflow_Count--;
        else
            motor[0].motor_overflow_counter.Encoder_Overflow_Count++;
    }
     */
    //printf("%f,%f,%d,%f,%ld\n",motor[1].motor_pid.target,motor[1].motor_pid.fdb,motor[1].dutyfactor,motor[1].actual_speed,motor[1].motor_overflow_counter.Capture_Count);
}
