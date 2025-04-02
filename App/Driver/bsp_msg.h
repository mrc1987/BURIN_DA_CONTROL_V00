/*
*********************************************************************************************************
*
*	ģ������ : ��Ϣ����ģ��
*	�ļ����� : bsp_msg.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*
*********************************************************************************************************
*/

#ifndef __BSP_MSG_H
#define __BSP_MSG_H

#include "main.h"

#define MSG_FIFO_SIZE    32	   		/* ��Ϣ���� */
typedef  enum
{
  Msg_None,
  Msg_ModuleCANRx,
  Msg_ModuleCANTx,
  Msg_PCCANRx,
  Msg_PCCANTx,
}CAN_MsgCode;
 

/* ����FIFO�õ����� */
typedef struct
{
	uint16_t MsgCode;		/* ��Ϣ���� */
	uint32_t MsgParam;		/* ��Ϣ��������, Ҳ������ָ�루ǿ��ת���� */
}MSG_T;

/* ����FIFO�õ����� */
typedef struct
{
	MSG_T Buf[MSG_FIFO_SIZE];	    /* ��Ϣ������ */
	uint8_t Read;					/* ��������ָ��1 */
	uint8_t Write;					/* ������дָ�� */
	uint8_t Read2;					/* ��������ָ��2 */
}MSG_FIFO_T;

/* ���ⲿ���õĺ������� */
void bsp_InitMsg(void);
void bsp_PutMsg(uint16_t _MsgCode, uint32_t _MsgParam);
uint8_t bsp_GetMsg(MSG_T *_pMsg);
uint8_t bsp_GetMsg2(MSG_T *_pMsg);
void bsp_ClearMsg(void);

#endif

 
