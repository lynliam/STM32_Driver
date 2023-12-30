//
// Created by Quan2 on 2023/4/23.
//

#ifndef STM32F407_DEMO2_BSP_DELAY_H
#define STM32F407_DEMO2_BSP_DELAY_H

#ifndef     __delay_H
#define     __delay_H

#include "stm32f4xx.h"  // 相当于51单片机中的  #include <reg51.h>
#include "tim.h"
void Delay_Init(void);
void Delay_us(uint32_t nus);
void Delay_ms(uint32_t nms);
int delay_ms(uint32_t nms);
#endif

#endif //STM32F407_DEMO2_BSP_DELAY_H
