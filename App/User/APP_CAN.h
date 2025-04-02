#ifndef __APP_CAN_H
#define __APP_CAN_H
#include "main.h"
 
extern ByteBitArea PS_Module_CanStatus;//ģ��CAN����״̬
extern ByteBitArea PC_CanStatus;//��λ������״̬


typedef  struct 
{
        
	uint32_t DestAddr:6;   //Ŀ��ڵ��ַ
	uint32_t R1:1;         //����
	uint32_t ACK:1;        //��Ӧ
	uint32_t SourceAddr:6; //Դ�ڵ��ַ
	uint32_t R2:1;         //����
	uint32_t Subpolo:1;    //�ְ�
	uint32_t SubCnt:2;     //�ְ�����
	uint32_t R3:6;         //����
	uint32_t FuncID:5;     //������
  
} _CANExID;


typedef  union 
{
        
  struct 
  {
        
	uint32_t DestAddr:6;   //Ŀ��ڵ��ַ
	uint32_t R1:1;         //����
	uint32_t ACK:1;        //��Ӧ
	uint32_t SourceAddr:6; //Դ�ڵ��ַ
	uint32_t R2:1;         //����
	uint32_t Subpolo:1;    //�ְ�
	uint32_t SubCnt:2;     //�ְ�����
	uint32_t R3:6;         //����
	uint32_t FuncID:5;     //������
  
  }BIT ;
 uint32_t u32CAN_ExID;
  
} _CANExID_U;


typedef  enum 
{
  MasterRead=4,//���ض�
  MasterWrite=5, //����д
  SlaverRead=6,   //�ӿض�
  SlaverWrite=7, //�ӿ�д

  SlaverAutoUp=9, //�ӿ��Զ���
  CustCMD=24, //�Զ���
  BootCMD=28, //boot ָ��
} _FunCode;


//======================================================================
#define PC_CAN_Err         PC_CanStatus.Bit.b0  //CAN����
#define PC_CAN_Rx_flag     PC_CanStatus.Bit.b1  //���ձ�־
#define PC_CAN_Tx_flag     PC_CanStatus.Bit.b2  //���ͱ�־
#define PC_CAN_Full_flag   PC_CanStatus.Bit.b3  //����������־

#define PSModule_CAN_Err         PC_CanStatus.Bit.b0  //CAN����
#define PSModule_CAN_Rx_flag     PC_CanStatus.Bit.b1  //���ձ�־
#define PSModule_CAN_Tx_flag     PC_CanStatus.Bit.b2  //���ͱ�־
#define PSModule_CAN_Full_flag   PC_CanStatus.Bit.b3  //����������־
//======================================================================

 void app_caninit (void);

void CAN0_RX0_IRQHandler(void);
void CAN1_RX0_IRQHandler(void);
#endif

