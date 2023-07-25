//
// Created by lynliam on 23-7-3.
//

#include "My_spi.h"

//模拟MOSI
void SPI_MOSI(unsigned char Status)
{
    if(Status)
        HAL_GPIO_WritePin(SPI_MOSI_PORT,SPI_MOSI_PIN,GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(SPI_MOSI_PORT,SPI_MOSI_PIN,GPIO_PIN_RESET);
}
//模拟CLK
void SPI_CLK(unsigned char Status)
{
    if(Status)
        HAL_GPIO_WritePin(SPI_CLK_PORT,SPI_CLK_PIN,GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(SPI_CLK_PORT,SPI_CLK_PIN,GPIO_PIN_RESET);
}

//模拟MISO
unsigned char SPI_MISO(void)
{
    if(HAL_GPIO_ReadPin(SPI_MISO_PORT,SPI_MISO_PIN))
        return 1;
    else
        return 0;
}

void SPI_PIN_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitStructure.Pin = SPI_MOSI_PIN; //MOSI 写入，主机向从机
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;	//推挽输出
    GPIO_InitStructure.Pull=GPIO_NOPULL;
    HAL_GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStructure);
    SPI_MOSI(1);

    GPIO_InitStructure.Pin = SPI_CLK_PIN; //SCK
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;	//推挽输出
    HAL_GPIO_Init(SPI_CLK_PORT, &GPIO_InitStructure);
    SPI_CLK(1);

    GPIO_InitStructure.Pin = SPI_MISO_PIN ;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(SPI_MISO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = SPI_CS_PIN;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(SPI_CS_PORT, &GPIO_InitStructure);
    SPI_FLASH_CS_HIGH();

    GPIO_InitStructure.Pin = SPI_WP_PIN;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(SPI_WP_PORT, &GPIO_InitStructure);
    SPI_FLASH_WP_HIGH();
}

void SendandReceiveByte(uint8_t byte)
{
    unsigned char i;
    for(i=0;i<8;i++)
    {
        if(byte & 0x80)	SPI_MOSI(1);
        else SPI_MOSI(0);
        byte <<= 1;

        SPI_CLK(0);
        Delay_us(1);
        SPI_CLK(1);
        Delay_us(1);
        
        Result <<= 1;
        if(SPI_MISO()) Result |= 0x01;
    }
    SPI_CLK(0);
    return(Result);
     
}

void SPI_SendByte(uint8_t byte) {
    uint8_t bit;

    for (bit = 0x80; bit; bit >>= 1) {
        if (byte & bit) {
            SPI_MOSI(1);
        } else {
            SPI_MOSI(0);
        }
        // 上升沿时钟
        SPI_CLK(0);
        for(uint32_t i=0;i<16;i++)
        {
        }
        //Delay_us(1);
        SPI_CLK(1);
        for(uint32_t i=0;i<16;i++)
        {

        }
        //Delay_us(1);
    }
}
