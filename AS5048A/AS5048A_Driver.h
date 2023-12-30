//
// Created by lyn on 23-11-2.
//

#ifndef AS5048A_AS5048A_DRIVER_H
#define AS5048A_AS5048A_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include "spi.h"
#include "bsp_delay.h"

typedef struct
{
    uint8_t par   :1;
    uint8_t RWn   :1;
    uint16_t data :14;
}AS5048A_Pack_t;

uint16_t AS5048A_Read_Angle();
uint16_t AS5048A_Read_Magnitude();
AS5048A_Pack_t AS5048A_Read_Diagnositics();
void AS5048A_Clear_Flags();
void AS5048A_Write_Zero_Position(uint16_t data_);

#ifdef __cplusplus
}
#endif


//CS片选引脚
#define AS5048A_CS_GPIO_PORT   GPIOA
#define AS5048A_CS_GPIO_PIN    GPIO_PIN_1

#define AS5048A_SPI_CS_UP        HAL_GPIO_WritePin(AS5048A_CS_GPIO_PORT,AS5048A_CS_GPIO_PIN,GPIO_PIN_SET)
#define AS5048A_SPI_CS_DOWN      HAL_GPIO_WritePin(AS5048A_CS_GPIO_PORT,AS5048A_CS_GPIO_PIN,GPIO_PIN_RESET)

#endif //AS5048A_AS5048A_DRIVER_H
