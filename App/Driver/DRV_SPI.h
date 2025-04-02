/*
*********************************************************************************************************
*
*	模块名称 : SPI总线驱动模块
*	文件名称 : DRV_SPI_H.h
*	版    本 : V1.0
*	说    明 : 头文件。
*
*********************************************************************************************************
*/

#ifndef _DRV_SPI_H
#define _DRV_SPI_H
#include "gd32f30x.h"

uint8_t SPI_OnlySendByte(uint32_t SPIx,uint8_t byte);
uint8_t SPI_ReceiveByte(uint32_t SPIx);
uint8_t SPI_OnlySendByte(uint32_t SPIx,uint8_t byte);
#endif
