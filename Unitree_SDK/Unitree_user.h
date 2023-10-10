//
// Created by liam on 2023/10/4.
//

#ifndef UNITREE_M8010_UNITREE_USER_H
#define UNITREE_M8010_UNITREE_USER_H

#include "motor_control.h"

typedef struct{
    MOTOR_send cmd;
    MOTOR_recv data;
}UnitreeMotor;

extern UnitreeMotor Unitree_Motor[1];

HAL_StatusTypeDef Unitree_init(UnitreeMotor *MotorInstance);

HAL_StatusTypeDef Unitree_UART_tranANDrev(UnitreeMotor *MotorInstance, unsigned char motor_id,
                                          unsigned char mode, float T,float W,
                                          float Pos, float K_P, float K_W);

HAL_StatusTypeDef Unitree_Encoder_Autoclibrating(UnitreeMotor *MotorInstance);

#endif //UNITREE_M8010_UNITREE_USER_H


#ifndef UNITREE_M8010_UNITREE_USER_H_
#define  UNITREE_M8010_UNITREE_USER_H_

#define RS485_DE_GPIO_Port           GPIOF
#define RS485_DE_Pin                 GPIO_PIN_0

#define RS485_RE_GPIO_Port           GPIOF
#define RS485_RE_Pin                 GPIO_PIN_1

#define SET_485_DE_UP()              HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET)
#define SET_485_DE_DOWN()            HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET)

#define SET_485_RE_UP()              HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_SET)
#define SET_485_RE_DOWN()            HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_RESET)

#define UART_UNITREE_HANDLER         huart6

#endif //UNITREE_M8010_UNITREE_USER_H_