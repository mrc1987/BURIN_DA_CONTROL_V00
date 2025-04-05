/*
*********************************************************************************************************
*
*	ģ������ : MODEBUS ͨ��ģ�� (��վ��
*	�ļ����� : modbus_slave.h
*	��    �� : V1.4
*	˵    �� : ͷ�ļ�
*
*
*********************************************************************************************************
*/

#ifndef __MODBUS_SLAVE_H
#define __MODBUS_SLAVE_H

#include "main.h"

/******************************
*  �Ĵ�����ַ����
*
****************************************/
#define REG_DA1_ONOFF	            0x0001 
#define REG_DA1_5V_ONOFF	        0x0002 
#define REG_DA1_Out_SoftStartEnable	0x0003 
#define REG_DA1_PWM_Enable      	0x0004

#define REG_DA1_V	                0x0005 
#define REG_DA1_PWM_FRE	            0x0006 
#define REG_DA1_PWM_DUTY    	    0x0007 
 

#define REG_DA2_ONOFF	            0x0008 
#define REG_DA2_5V_ONOFF	        0x0009 
#define REG_DA2_Out_SoftStartEnable	0x000A 
#define REG_DA2_PWM_Enable      	0x000B

#define REG_DA2_V	                0x000C 
#define REG_DA2_PWM_FRE	            0x000D 
#define REG_DA2_PWM_DUTY    	    0x000E 
 

 
/*************************************************/


#define REG_V_K		    0x0E00
#define REG_V_B		    0x0E01
 
#define REG_V2_K		0x0E02
#define REG_V2_B		0x0E03 

#define REG_FAWREVER	0xF000
#define REG_EEP_SAVE    0xF001
#define REG_EEP_INIT	0xF002

#define REG_Boot		0xFA01



/****************** RTU Ӧ����� ******************************/
#define RSP_OK				0		/* �ɹ� */
#define RSP_ERR_CMD			0x01	/* ��֧�ֵĹ����� */
#define RSP_ERR_REG_ADDR	0x02	/* �Ĵ�����ַ���� */
#define RSP_ERR_VALUE		0x03	/* ����ֵ����� */
#define RSP_ERR_WRITE		0x04	/* д��ʧ�� �豸���� */

#define S_RX_BUF_SIZE		32
#define S_TX_BUF_SIZE		32

//***********************************************************************
typedef void (*pSend)(uint8_t *pData,uint8_t Len) ;
//********************************************************************
typedef struct
{
	uint8_t *pRxBuf;
	pSend   Modbus_Send;
	uint8_t RxCount;

	uint8_t RspCode;

	uint8_t *pTxBuf;
	uint8_t TxCount;
	
	uint8_t addr;
}MODS_T;





void MODS_Poll(void);
 
void Modbus_Poll_Task(void);
void Modbus_Init(uint8_t *pTxData,uint8_t *pRxData,pSend pSendFun);
void USART_ClearErr(void);

extern MODS_T g_tModS;

#endif

/********************************************************************************************/
