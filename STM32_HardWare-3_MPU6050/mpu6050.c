#include "stm32f1xx_hal.h"
#include "mpu6050.h"

/**
 * @brief ��ʼ��MPU6050
*/
uint8_t _MPU_Init(void)
{
    uint8_t res;

    MPU_IIC_Init();                                         // ��ʼ��IIC����
    _MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X80);               // ��λMPU6050
    delay_ms(100);
    _MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X00);               // ����MPU6050
    _MPU_Set_Gyro_Fsr(3);                                   // �����Ǵ�����,��2000dps
    _MPU_Set_Accel_Fsr(0);                                  // ���ٶȴ�����,��2g
    _MPU_Set_Rate(50);                                      // ���ò�����50Hz
    _MPU_Write_Byte(MPU_INT_EN_REG, 0X00);                  // �ر������ж�
    _MPU_Write_Byte(MPU_USER_CTRL_REG, 0X00);               // I2C��ģʽ�ر�
    _MPU_Write_Byte(MPU_FIFO_EN_REG, 0X00);                 // �ر�FIFO
    _MPU_Write_Byte(MPU_INTBP_CFG_REG, 0X80);               // INT���ŵ͵�ƽ��Ч
    res = _MPU_Read_Byte(MPU_DEVICE_ID_REG);
    if (res == MPU_ADDR)                                    // ����ID��ȷ
    {
        _MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X01);           // ����CLKSEL,PLL X��Ϊ�ο�
        _MPU_Write_Byte(MPU_PWR_MGMT2_REG, 0X00);           // ���ٶ��������Ƕ�����
        _MPU_Set_Rate(50);                                  // ���ò�����Ϊ50Hz
    }
    else
        return 1;
    return 0;
}

/**
 * @brief   ����MPU6050�����Ǵ����������̷�Χ
 * @param   fsr     0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
*/
uint8_t _MPU_Set_Gyro_Fsr(uint8_t fsr)
{
    return _MPU_Write_Byte(MPU_GYRO_CFG_REG, fsr << 3);     // ���������������̷�Χ
}


/**
 * @brief   ����MPU6050���ٶȴ����������̷�Χ
 * @param   fsr     0,��2g;1,��4g;2,��8g;3,��16g
*/
uint8_t _MPU_Set_Accel_Fsr(uint8_t fsr)
{
    return _MPU_Write_Byte(MPU_ACCEL_CFG_REG, fsr << 3);    // ���ü��ٶȴ����������̷�Χ
}

/**
 * @brief   ����MPU6050���ٶȴ����������̷�Χ
 * @param   lpf     ���ֵ�ͨ�˲�Ƶ��(Hz)
*/
uint8_t _MPU_Set_LPF(uint16_t lpf)
{
    uint8_t data = 0;
    if (lpf >= 188)
        data = 1;
    else if (lpf >= 98)
        data = 2;
    else if (lpf >= 42)
        data = 3;
    else if (lpf >= 20)
        data = 4;
    else if (lpf >= 10)
        data = 5;
    else
        data = 6;
    return _MPU_Write_Byte(MPU_CFG_REG, data);              // �������ֵ�ͨ�˲���
}

/**
 * @brief   ����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
 * @param   rate     4~1000(Hz)
 */
uint8_t _MPU_Set_Rate(uint16_t rate)
{
    uint8_t data;
    if (rate > 1000)
        rate = 1000;
    if (rate < 4)
        rate = 4;
    data = 1000 / rate - 1;
    data = _MPU_Write_Byte(MPU_SAMPLE_RATE_REG, data);      // �������ֵ�ͨ�˲���
    return _MPU_Set_LPF(rate / 2);                          // �Զ�����LPFΪ�����ʵ�һ��
}

/**
 * @brief   �õ��¶�
 * @return  �¶�ֵ(������100��)
*/
short _MPU_Get_Temperature(void)
{
    uint8_t buf[2];
    short raw;
    float temp;
    _MPU_Read_Len(MPU_ADDR, MPU_TEMP_OUTH_REG, 2, buf);
    raw = ((uint16_t)buf[0] << 8) | buf[1];
    temp = 36.53 + ((double)raw) / 340;
    return temp * 100;
}

/**
 * @brief   �õ�������ֵ(ԭʼֵ)
 * @param   gx  ������x��ԭʼ����(������)
 * @param   gy  ������y��ԭʼ����(������)
 * @param   gz  ������z��ԭʼ����(������)
*/
uint8_t _MPU_Get_Gyroscope(short *gx, short *gy, short *gz)
{
    uint8_t buf[6], res;
    res = _MPU_Read_Len(MPU_ADDR, MPU_GYRO_XOUTH_REG, 6, buf);
    if (res == 0)
    {
        *gx = ((uint16_t)buf[0] << 8) | buf[1];
        *gy = ((uint16_t)buf[2] << 8) | buf[3];
        *gz = ((uint16_t)buf[4] << 8) | buf[5];
    }
    return res;
    ;
}

/**
 * @brief   �õ����ٶ�ֵ(ԭʼֵ)
 * @param   ax  ���ٶȼ�x��ԭʼ����(������)
 * @param   ay  ���ٶȼ�y��ԭʼ����(������)
 * @param   az  ���ٶȼ�z��ԭʼ����(������)
*/
uint8_t _MPU_Get_Accelerometer(short *ax, short *ay, short *az)
{
    uint8_t buf[6], res;
    res = _MPU_Read_Len(MPU_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf);
    if (res == 0)
    {
        *ax = ((uint16_t)buf[0] << 8) | buf[1];
        *ay = ((uint16_t)buf[2] << 8) | buf[3];
        *az = ((uint16_t)buf[4] << 8) | buf[5];
    }
    return res;
    ;
}

/**
 * @brief   IIC����д
 * @param   addr    ������ַ
 * @param   reg     �Ĵ�����ַ
 * @param   len     д�볤��
 * @param   buf     ������
*/
uint8_t _MPU_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    uint8_t i;
    MPU_IIC_Start();
    MPU_IIC_Send_Byte((addr << 1) | 0);                         // ����������ַ+д����
    if (MPU_IIC_Wait_Ack())                                     // �ȴ�Ӧ��
    {
        MPU_IIC_Stop();
        return 1;
    }
    MPU_IIC_Send_Byte(reg);                                     // д�Ĵ�����ַ
    MPU_IIC_Wait_Ack();                                         // �ȴ�Ӧ��
    for (i = 0; i < len; i++)
    {
        MPU_IIC_Send_Byte(buf[i]);                              // ��������
        if (MPU_IIC_Wait_Ack())                                 // �ȴ�ACK
        {
            MPU_IIC_Stop();
            return 1;
        }
    }
    MPU_IIC_Stop();
    return 0;
}

/**
 * @brief   IIC������
 * @param   addr    ������ַ
 * @param   reg     �Ĵ�����ַ
 * @param   len     д�볤��
 * @param   buf     ������
*/
uint8_t _MPU_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    MPU_IIC_Start();
    MPU_IIC_Send_Byte((addr << 1) | 0);             // ����������ַ+д����
    if (MPU_IIC_Wait_Ack())                         // �ȴ�Ӧ��
    {
        MPU_IIC_Stop();
        return 1;
    }
    MPU_IIC_Send_Byte(reg);                         // д�Ĵ�����ַ
    MPU_IIC_Wait_Ack();                             // �ȴ�Ӧ��
    MPU_IIC_Start();
    MPU_IIC_Send_Byte((addr << 1) | 1);             // ����������ַ+������
    MPU_IIC_Wait_Ack();                             // �ȴ�Ӧ��
    while (len)
    {
        if (len == 1)
            *buf = MPU_IIC_Read_Byte(0);            // ������,����nACK
        else
            *buf = MPU_IIC_Read_Byte(1);            // ������,����ACK
        len--;
        buf++;
    }
    MPU_IIC_Stop();                                 // ����һ��ֹͣ����
    return 0;
}

/**
 * @brief   IICд�ֽ�
 * @param   reg     �Ĵ�����ַ
 * @param   data    ����
*/
uint8_t _MPU_Write_Byte(uint8_t reg, uint8_t data)
{
    MPU_IIC_Start();
    MPU_IIC_Send_Byte((MPU_ADDR << 1) | 0);             // ����������ַ+д����
    if (MPU_IIC_Wait_Ack())                             // �ȴ�Ӧ��
    {
        MPU_IIC_Stop();
        return 1;
    }
    MPU_IIC_Send_Byte(reg);                             // д�Ĵ�����ַ
    MPU_IIC_Wait_Ack();                                 // �ȴ�Ӧ��
    MPU_IIC_Send_Byte(data);                            // ��������
    if (MPU_IIC_Wait_Ack())                             // �ȴ�ACK
    {
        MPU_IIC_Stop();
        return 1;
    }
    MPU_IIC_Stop();
    return 0;
}

/**
 * @brief   IIC���ֽ�
 * @param   reg     �Ĵ�����ַ
*/
uint8_t _MPU_Read_Byte(uint8_t reg)
{
    uint8_t res;
    MPU_IIC_Start();
    MPU_IIC_Send_Byte((MPU_ADDR << 1) | 0);             // ����������ַ+д����
    MPU_IIC_Wait_Ack();                                 // �ȴ�Ӧ��
    MPU_IIC_Send_Byte(reg);                             // д�Ĵ�����ַ
    MPU_IIC_Wait_Ack();                                 // �ȴ�Ӧ��
    MPU_IIC_Start();
    MPU_IIC_Send_Byte((MPU_ADDR << 1) | 1);             // ����������ַ+������
    MPU_IIC_Wait_Ack();                                 // �ȴ�Ӧ��
    res = MPU_IIC_Read_Byte(0);                         // ��ȡ����,����nACK
    MPU_IIC_Stop();                                     // ����һ��ֹͣ����
    return res;
}
