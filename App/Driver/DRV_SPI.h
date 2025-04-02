/*
*********************************************************************************************************
*
*	ģ������ : SPI��������ģ��
*	�ļ����� : DRV_SPI_H.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ���
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
