//
// Created by Quan2 on 2023/4/5.
//
#include "motor_control.h"

motor_data motor[4];

void set_motor_speed(motor_data *motor_,float v)
{
    motor_->dutyfactor=(v > motor_->motor_pid.outputMax) ? motor_->motor_pid.outputMax:v;
    if(motor_->MOTOR_DIE == 1)
    {
        __HAL_TIM_SET_COMPARE(&motor_->TIM_PWMHandle2,motor_->TIM_CHANNELHanle2,0);
        __HAL_TIM_SET_COMPARE(&motor_->TIM_PWMHandle1,motor_->TIM_CHANNELHanle1,motor_->dutyfactor);
    }
    else
    {
        __HAL_TIM_SET_COMPARE(&motor_->TIM_PWMHandle1,motor_->TIM_CHANNELHanle1,0);
        __HAL_TIM_SET_COMPARE(&motor_->TIM_PWMHandle2,motor_->TIM_CHANNELHanle2,motor_->dutyfactor);
    }
}

void set_motor_direction(motor_data *motor_, uint8_t dir)
{
    motor_->MOTOR_DIE=dir;
}

void set_motor_enable(motor_data *motor_)
{
    motor_->is_motor_enable=1;
}

void set_motor_disable(motor_data *motor_)
{
    motor_->is_motor_enable=0;
}

void motor_init(uint8_t i,TIM_HandleTypeDef htim,TIM_HandleTypeDef htim_PWM1, uint16_t TIM_Channel1,TIM_HandleTypeDef htim_PWM2, uint16_t TIM_Channel2)
{
    motor[i].MOTOR_DIE=0;
    motor[i].dutyfactor=0;
    motor[i].is_motor_enable=0;
    motor[i].actual_speed=0;
    motor[i].TIM_EncoderHandle=htim;
    motor[i].TIM_PWMHandle1=htim_PWM1;
    motor[i].TIM_PWMHandle2=htim_PWM2;
    motor[i].TIM_CHANNELHanle1=TIM_Channel1;
    motor[i].TIM_CHANNELHanle2=TIM_Channel2;
    motor[i].motor_overflow_counter.Capture_Count=65536.0/2.0;
    PID_Param_init(&motor[i].motor_pid);
}

void motor_pid_control(motor_data *motor_)
{
    if(motor_->is_motor_enable==1)   //电机使能
    {
        float current_val = 0;      //PID 计算值

        motor_->motor_overflow_counter.Capture_Count= __HAL_TIM_GET_COUNTER(&motor_->TIM_EncoderHandle);   //+(motor_->motor_overflow_counter.Encoder_Overflow_Count*65535);
        __HAL_TIM_SET_COUNTER(&motor_->TIM_EncoderHandle,65536/2);
        motor_->actual_speed=((float)(motor_->motor_overflow_counter.Capture_Count-65536/2) / ENCODER_TOTAL_RESOLUTION / REDUCTION_RATIO) / 0.1;    // 单位是r/s
        /*
        if(motor_->actual_speed<0)
        {
            set_motor_direction(motor_,0);
            motor_->actual_speed=-motor_->actual_speed;
        }
        */
        current_val= PID_realize(&motor_->motor_pid,motor_->actual_speed/5.5*1000.0);
        if(current_val>0)
            set_motor_direction(motor_,0);
        else{
            current_val=-current_val;
            set_motor_direction(motor_,1);  //反转
        }
        set_motor_speed(motor_,current_val);
    }
}

void motor_start(uint8_t i)
{
    motor[i].is_motor_enable=1;
    //编码器启动
    HAL_TIM_Encoder_Start(&motor[i].TIM_EncoderHandle,TIM_CHANNEL_ALL);
    __HAL_TIM_SET_COUNTER(&motor[i].TIM_EncoderHandle,65536/2);
    //PWM启动
    HAL_TIM_PWM_Start(&motor[i].TIM_PWMHandle1,motor[i].TIM_CHANNELHanle1);
    HAL_TIM_PWM_Start(&motor[i].TIM_PWMHandle2,motor[i].TIM_CHANNELHanle2);
}

void motor_stop(uint8_t i)
{
    motor[i].is_motor_enable=0;
    //编码器关闭
    HAL_TIM_Encoder_Stop(&motor[i].TIM_EncoderHandle,TIM_CHANNEL_ALL);
    //PWM关闭
    HAL_TIM_PWM_Stop(&motor[i].TIM_PWMHandle1,motor[i].TIM_CHANNELHanle1);
    HAL_TIM_PWM_Stop(&motor[i].TIM_PWMHandle2,motor[i].TIM_CHANNELHanle2);
}
