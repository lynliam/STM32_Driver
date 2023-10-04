#ifndef _IIC_H
#define _IIC_H

#include <inttypes.h>

#define IIC_WR	0		/* 写控制bit */
#define IIC_RD	1		/* 读控制bit */

void _IIC_Start(void);
void _IIC_Stop(void);
void _IIC_Send_Byte(uint8_t _ucByte);
uint8_t _IIC_Read_Byte(uint8_t ack);
uint8_t _IIC_Wait_Ack(void);
void _IIC_Ack(void);
void _IIC_NAck(void);
uint8_t _IIC_CheckDevice(uint8_t _Address);
void _IIC_GPIO_Init(void);

#endif
