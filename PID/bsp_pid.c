//
// Created by Quan2 on 2023/4/5.
//
#include "bsp_pid.h"

void PID_Param_init(_PID_t *pid)
{
    pid->KD=0.15;
    pid->KI=0.10;
    pid->KP=1.0;

    pid->target=0.0;
    pid->cur_error=0.0;
    pid->error[0]=0.0;
    pid->error[1]=0.0;
    pid->fdb=0.0;
    pid->outputMax=OUTPUT_MAX;                             //最大输出值的绝对值
    //pid->outputMin=-900.0;
}

void set_pid_target(_PID_t *pid,float temp_target)
{
    pid->target=temp_target;
}

void set_p_i_d(_PID_t *pid,float p,float i,float d)
{
    pid->KP=p;
    pid->KI=i;
    pid->KD=d;
}

float PID_realize(_PID_t *pid,float fdb)
{
    pid->cur_error=pid->target-fdb;
    pid->fdb+=pid->KP*(pid->cur_error-pid->error[0])+pid->KI*pid->cur_error+pid->KD*(pid->cur_error-2*pid->error[0]+pid->error[1]);
    if(pid->fdb>OUTPUT_MAX)                             //电机正转限幅
        pid->fdb=OUTPUT_MAX;
    else if(pid->fdb<-OUTPUT_MAX)                       //电机反转限幅
        pid->fdb=-OUTPUT_MAX;
    pid->error[1]=pid->error[0];
    pid->error[0]=pid->cur_error;

    return pid->fdb;
}
