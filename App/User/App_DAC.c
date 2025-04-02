#include "APP_DAC.h"
#include "DRV_DAC.h"
#include "main.h"

#define  q10_SetIoutRef_K   95325L    //1A ��Ӧ124.1212��
#define  q10_SetVoutRef_K   57772L    //1V ��Ӧ56.418��  4096/3.3*2.5=55V

uint16_t u16SetIout_Ref;          //�����������
uint16_t u16SetVout_Ref;          //���������ѹ

int16_t pre_POW_ERR;
 

/*
*********************************************************************************************************
*	�� �� ��: SetIoutRef
*	����˵��: ������������ο�
*	��    ��: Value : ���õ�������� ��λ��0.01A
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SetIoutRef(uint16_t Value)
{
  uint32_t u32Tem;
	
  u32Tem = Value*q10_SetIoutRef_K/100;//1A ��Ӧ124.1212��
  u32Tem = u32Tem>>10;
  if(u32Tem > 4000)
  {
	  u32Tem = 4000;
  }

  DAC_SET_CURR((uint16_t)u32Tem);
}


/*
*********************************************************************************************************
*	�� �� ��: SetVoutRef
*	����˵��: ���������ѹ�ο�
*	��    ��: Value : ���õ�������� ��λ��0.01V
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SetVoutRef(uint16_t Value)
{
  uint32_t u32Tem;
	
  u32Tem = Value*q10_SetIoutRef_K/100;//1A ��Ӧ124.1212��
  u32Tem = u32Tem>>10;
  if(u32Tem > 4000)
  {
	  u32Tem = 4000;
  }

  DAC_SET_VOL((uint16_t)u32Tem);
}

/*
*********************************************************************************************************
*	�� �� ��: DAC_Process
*	����˵��: DAC���ô���
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void DAC_Process(void)
{
	
  
  int32_t i32Tem;
  
//---------------------------���������ѹ------------------------------------------------------------
  i32Tem = u16SetVout_Ref*PowerEEP.SetVoutRef_K  + PowerEEP.SetVoutRef_B ;	
  i32Tem = i32Tem>>10;
  if(i32Tem < 100) 
  {
	  i32Tem = 100;
  }

  SetVoutRef(i32Tem); 
  

  //---------------------------�����������------------------------------------------------------------
  if(ADC_Info.u16Power > POWER_LIM)
  {
	 i32Tem =   POWER_LIM*100/ADC_Info.u16VoutRMS;
  }
  else
  {
	i32Tem = u16SetIout_Ref;
  }
  if(i32Tem > SetIout_up)
  {
	 i32Tem =  SetIout_up; 
  }
  i32Tem = i32Tem*PowerEEP.SetIoutRef_K + PowerEEP.SetIoutRef_B;	
  i32Tem = i32Tem>>10;
  if(i32Tem < 50) 
  {
	  i32Tem =  50;
  }
  
  SetIoutRef(i32Tem); 
	
}


