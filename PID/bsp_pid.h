//
// Created by Quan2 on 2023/4/5.
//

#ifndef STM32F103_DEMO1_BSP_PID_H
#define STM32F103_DEMO1_BSP_PID_H

#include<stdio.h>
#include<math.h>
#include<stdlib.h>

#define OUTPUT_MAX 1000.0                               //最大输出值的绝对值

typedef struct{
    float KP;											//PID参数P
    float KI;											//PID参数I
    float KD;											//PID参数D

    float fdb;											//PID反馈值
    float target;									    //PID目标值
    float cur_error;									//当前误差
    float error[2];										//前两次误差
//    float output;										//输出值
    float outputMax;									//最大输出值的绝对值
    //float outputMin;                                  //最小输出值的绝对值用于防抖
}_PID_t;


void PID_Param_init(_PID_t *pid);                       //PID参数初始化
void set_pid_target(_PID_t *pid,float temp_target);     //设置PID目标值
void set_p_i_d(_PID_t *pid,float p,float i,float d);    //设置PID参数
float PID_realize(_PID_t *pid,float fdb);               //PID实现


#endif //STM32F103_DEMO1_BSP_PID_H
