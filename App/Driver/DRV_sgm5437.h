/*
*********************************************************************************************************
*
*	模块名称 : 串行EEPROM 24xx02驱动模块
*	文件名称 : bsp_eeprom_24xx.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	修改记录 :
*		版本号  日期       作者    说明
*
*********************************************************************************************************
*/

#ifndef _DRV_SGM5437_H
#define _DRV_SGM5437_H

 void SGM5437_SetDAC(uint32_t SPIx,uint8_t CH,uint16_t V);
void SGM5437_SynSetDAC(uint32_t SPIx,uint16_t *Data,uint8_t N);
void SGM5437_Exit_Simultaneous(uint32_t SPIx );
#endif

 
