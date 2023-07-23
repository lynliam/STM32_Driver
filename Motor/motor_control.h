//
// Created by Quan2 on 2023/4/5.
//

#ifndef STM32F103_DEMO1_MOTOR_CONTROL_H
#define STM32F103_DEMO1_MOTOR_CONTROL_H

#include "stm32f4xx.h"
#include "bsp_GPIO_ENABLE.h"
#include "bsp_pid.h"
#include "tim.h"
#include <stdio.h>


#define PWM_MAX 900
#define REDUCTION_RATIO                        30.0         //减速比
#define ENCODER_RESOLUTION                     13.0
#define ENCODER_TOTAL_RESOLUTION             (ENCODER_RESOLUTION * 4)

typedef struct
{
    __IO int32_t Capture_Count;
}encoder_data;

typedef struct
{
    uint8_t MOTOR_DIE;
    uint8_t is_motor_enable;
    uint16_t dutyfactor;
    TIM_HandleTypeDef TIM_EncoderHandle;
    TIM_HandleTypeDef TIM_PWMHandle1;
    TIM_HandleTypeDef TIM_PWMHandle2;
    uint16_t TIM_CHANNELHanle1;
    uint16_t TIM_CHANNELHanle2;
    _PID_t motor_pid;
    encoder_data motor_overflow_counter;
    float actual_speed;
}motor_data;

extern motor_data motor[4];

void motor_init(uint8_t i,TIM_HandleTypeDef htim,TIM_HandleTypeDef htim_PWM1, uint16_t TIM_Channel1,TIM_HandleTypeDef htim_PWM2, uint16_t TIM_Channel2);

void set_motor_speed(motor_data *motor_,float v);

void set_motor_direction(motor_data *motor_,uint8_t dir);

void motor_pid_control(motor_data *motor_);

void set_motor_enable(motor_data *motor_);

void set_motor_disable(motor_data *motor_);

void motor_start(uint8_t i);

void motor_stop(uint8_t i);

#endif //STM32F103_DEMO1_MOTOR_CONTROL_H
