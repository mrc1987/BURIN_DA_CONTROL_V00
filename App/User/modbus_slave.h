/*
*********************************************************************************************************
*
*	模块名称 : MODEBUS 通信模块 (从站）
*	文件名称 : modbus_slave.h
*	版    本 : V1.4
*	说    明 : 头文件
*
*
*********************************************************************************************************
*/

#ifndef __MODBUS_SLAVE_H
#define __MODBUS_SLAVE_H

#include "main.h"

/******************************
*  寄存器地址定义
*
****************************************/
#define REG_DMM_VRMS	0x0001 
#define REG_DMM_VP_RMS	0x0002 
#define REG_DMM_VN_RMS	0x0003 
#define REG_DMM_VPN_RMS	0x0004 

#define REG_IN		    0x0005
#define REG_DMM_RLY	    0x0006  
#define REG_RLY		    0x0007

/*****************************************************/
#define REG_RLY1		0x0000
#define REG_RLY2		0x0001
#define REG_RLY3		0x0002
#define REG_RLY4		0x0003
#define REG_RLY5		0x0004
#define REG_RLY6		0x0005
#define REG_RLY7		0x0006
#define REG_RLY8		0x0007

#define REG_DMM1		0x0100
#define REG_DMM2		0x0101
#define REG_DMM3		0x0102
#define REG_DMM4		0x0103
#define REG_DMM5		0x0104
#define REG_DMM6		0x0105
#define REG_DMM7		0x0106
#define REG_DMM8		0x0107
/*************************************************/


#define REG_V_K		    0x0100
#define REG_V_B		    0x0101
 
#define REG_V2_K		0x0102
#define REG_V2_B		0x0103 

#define REG_FAWREVER	0xFA00
#define REG_Boot		0xFA01
#define REG_EEP_SAVE    0xFA02


/****************** RTU 应答代码 ******************************/
#define RSP_OK				0		/* 成功 */
#define RSP_ERR_CMD			0x01	/* 不支持的功能码 */
#define RSP_ERR_REG_ADDR	0x02	/* 寄存器地址错误 */
#define RSP_ERR_VALUE		0x03	/* 数据值域错误 */
#define RSP_ERR_WRITE		0x04	/* 写入失败 设备故障 */

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
