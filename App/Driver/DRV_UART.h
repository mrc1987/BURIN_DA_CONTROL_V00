/*
*********************************************************************************************************
*
*	模块名称 : I2C总线驱动模块
*	文件名称 : bsp_i2c_gpio.h
*	版    本 : V1.0
*	说    明 : 头文件。
*
*	Copyright (C), 2012-2013, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _DRV_UART_H
#define _DRV_UART_H
#include "gd32f30x.h"

typedef void (*UART_CallBackDef)(uint8_t *pRxData,uint8_t Len);

void UART_DMASend(uint8_t *_ucaBuf, uint16_t _usLen);
void UART_Send(uint8_t *_ucaBuf, uint16_t _usLen);
void UART_Init(UART_CallBackDef CallBack);

void UART_DMARx_Enable(void);
#endif
