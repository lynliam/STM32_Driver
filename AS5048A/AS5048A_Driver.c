/**
 * AS5048A SPI HALL 编码器
 * @file AS5048A_Driver.c
 * @author Liam (Quan.2003@outlook.com)
 *
 * 驱动相关参数
 *
 * MSB first  （大端模式）
 *
 * SPI Timing
 * Parameter       Min              Description
 * t_T             350ns            CS下拉后下一次CLK信号上升沿
 * T_CLK           100ns            Serial clock period
 * t_H             50ns             Time between last falling edge of CLK and rising edge of CSn
 * T_CSnH          350ns            High time of CSn between two transmissions
 *
 * 故SPI速率不得大于 10e7 Hz = 10 MHz
 *
 * Created by lyn on 23-11-2.
 */

#include "AS5048A_Driver.h"

//Register Address
#define     NOP                             0x0000
#define     CLEAR_ERROR_FLAG                0x0001
#define     PROGRAMMING_CONTROL             0x0003
#define     OTP_REGISTER_ZERO_POSITION_HI   0x0016
#define     OTP_REGISTER_ZERO_POSITION_LOW_6_LSBS 0x0017
#define     DIAGNOSTICS                     0x3FFD
#define     AUTOMATIC_GAIN_CONTROL          0x3FFD
#define     MAGNITUDE                       0x3FFE
#define     ANGLE                           0x3FFF

void EVEN_Parity_Cal( AS5048A_Pack_t *data);

#ifdef SPI_DMA_MODE
/**
 * SPI读写函数
 * @param TxData
 * @return
 */
uint8_t AS5048A_SPI_RW(uint8_t TxData)
{
    uint8_t RxData;
    HAL_SPI_TransmitReceive(&hspi1,&TxData,&RxData,1,0xff);
    delay_us(5);
    return RxData;
}


/**
 * SPI读写函数
 * @param TxData
 * @return
 */
uint8_t AS5048A_SPI_RW_DMA(uint8_t TxData)
{
    uint8_t RxData;
    HAL_SPI_TransmitReceive_DMA(&hspi1,&TxData,&RxData,1);
    delay_us(5);
    return RxData;
}
#endif

AS5048A_Pack_t AS5048A_SPI_RDATA(uint16_t address)
{
    AS5048A_Pack_t data;
    AS5048A_Pack_t RxData;
    data.RWn = 1;
    data.data = address;
    EVEN_Parity_Cal(&data);
    HAL_SPI_TransmitReceive(&hspi1,(uint8_t *)&data,(uint8_t *)&RxData,sizeof(data),0xff);
    return RxData;
}

AS5048A_Pack_t AS5048A_SPI_WDATA(uint16_t address_, uint16_t data_)
{
    AS5048A_Pack_t data;
    AS5048A_Pack_t RxData;
    data.RWn = 0;
    data.data = address_;
    EVEN_Parity_Cal(&data);
    HAL_SPI_TransmitReceive(&hspi1,(uint8_t *)&data,(uint8_t *)&RxData,sizeof(data),0xff);


    data.data = data_;
    EVEN_Parity_Cal(&data);
    HAL_SPI_TransmitReceive(&hspi1,(uint8_t *)&data,(uint8_t *)&RxData,sizeof(data),0xff);


    data.data = 0;
    data.RWn = 0;
    data.par = 0;
    HAL_SPI_TransmitReceive(&hspi1,(uint8_t *)&data,(uint8_t *)&RxData,sizeof(data),0xff);

    return RxData;
}

uint16_t AS5048A_Init()
{
    AS5048A_SPI_CS_DOWN;
    delay_us(400);

    AS5048A_Pack_t data = AS5048A_Read_Diagnositics();
    if((data.data>>8)&0x0001)
        return HAL_OK;
    else
        return HAL_ERROR;
}

uint16_t AS5048A_Read_Angle()
{
    AS5048A_Pack_t data = AS5048A_SPI_RDATA(ANGLE);
    uint16_t angle = data.data;
    return angle;
}

uint16_t AS5048A_Read_Magnitude()
{
    AS5048A_Pack_t data = AS5048A_SPI_RDATA(MAGNITUDE);
    uint16_t magnitude = data.data;
    return magnitude;
}

AS5048A_Pack_t AS5048A_Read_Diagnositics()
{
    AS5048A_SPI_RDATA(DIAGNOSTICS);
    return AS5048A_SPI_RDATA(DIAGNOSTICS);
}

void AS5048A_Write_Zero_Position(uint16_t data_)
{
    uint16_t data_h = data_>>6;
    uint16_t data_l = (data_&0x003F);
    AS5048A_SPI_WDATA(OTP_REGISTER_ZERO_POSITION_HI,data_h);
    AS5048A_SPI_WDATA(OTP_REGISTER_ZERO_POSITION_LOW_6_LSBS,data_l);
}

void AS5048A_Clear_Flags()
{
    AS5048A_Pack_t data;
    data.data = 0x01;
    data.RWn = 1;
    data.par = 0;

    HAL_SPI_Transmit_DMA(&hspi1,(uint8_t *)&data,sizeof(data));
}

void EVEN_Parity_Cal( AS5048A_Pack_t *data)
{
    uint8_t par = 0;
    for(int i=0;i<14;i++)
    {
        par+=(data->data>>i)&0x01;
    }
    par += data->RWn;
    if(par%2 == 1)
        data->par = 1;
    else
        data->par = 0;
}




