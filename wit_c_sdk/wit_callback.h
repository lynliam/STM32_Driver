//
// Created by lynliam on 23-7-6.
//

#ifndef STM32F407_DEMO2_WIT_CALLBACK_H
#define STM32F407_DEMO2_WIT_CALLBACK_H
#include "stm32f4xx.h"
#include "wit_c_sdk.h"
#include "bsp_delay.h"
#include "usart.h"
#include <string.h>
#include "tim.h"

#define ACC_UPDATE		0x01
#define GYRO_UPDATE		0x02
#define ANGLE_UPDATE	0x04
#define MAG_UPDATE		0x08
#define READ_UPDATE		0x80

extern float fAcc[3], fGyro[3], fAngle[3];
extern volatile char s_cDataUpdate, s_cCmd;
extern const uint32_t c_uiBaud[10];
extern unsigned char rx_buffer[11];


void CmdProcess(void);
void AutoScanSensor(void);
void SensorUartSend(uint8_t *p_data, uint32_t uiSize);
void SensorDataUpdata(uint32_t uiReg, uint32_t uiRegNum);
void Delayms(uint16_t ucMs);
void Usart2Init(unsigned int uiBaud);

#endif //STM32F407_DEMO2_WIT_CALLBACK_H
