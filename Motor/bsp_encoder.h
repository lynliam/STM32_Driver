//
// Created by lynliam on 23-4-8.
//

#ifndef STM32F103_DEMO1_BSP_ENCODER_H
#define STM32F103_DEMO1_BSP_ENCODER_H

#include "motor_control.h"
#include "wit_callback.h"

extern float temp[9];
typedef union
{
    float data;
    uint8_t byte[4];
}float_to_byte;

extern float_to_byte data_imu[13];

void trans_float();

#endif //STM32F103_DEMO1_BSP_ENCODER_H
