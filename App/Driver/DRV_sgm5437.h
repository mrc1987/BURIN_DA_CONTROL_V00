/*
*********************************************************************************************************
*
*	ģ������ : ����EEPROM 24xx02����ģ��
*	�ļ����� : bsp_eeprom_24xx.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*
*********************************************************************************************************
*/

#ifndef _DRV_SGM5437_H
#define _DRV_SGM5437_H

 void SGM5437_SetDAC(uint32_t SPIx,uint8_t CH,uint16_t V);
void SGM5437_SynSetDAC(uint32_t SPIx,uint16_t *Data,uint8_t N);
void SGM5437_Exit_Simultaneous(uint32_t SPIx );
#endif

 
