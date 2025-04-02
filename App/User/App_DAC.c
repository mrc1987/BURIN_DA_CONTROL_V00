#include "APP_DAC.h"
#include "DRV_DAC.h"
#include "main.h"

#define  q10_SetIoutRef_K   95325L    //1A 对应124.1212字
#define  q10_SetVoutRef_K   57772L    //1V 对应56.418字  4096/3.3*2.5=55V

uint16_t u16SetIout_Ref;          //设置输出电流
uint16_t u16SetVout_Ref;          //设置输出电压

int16_t pre_POW_ERR;
 

/*
*********************************************************************************************************
*	函 数 名: SetIoutRef
*	功能说明: 设置输出电流参考
*	形    参: Value : 设置的输出电流 单位：0.01A
*	返 回 值: 无
*********************************************************************************************************
*/
void SetIoutRef(uint16_t Value)
{
  uint32_t u32Tem;
	
  u32Tem = Value*q10_SetIoutRef_K/100;//1A 对应124.1212字
  u32Tem = u32Tem>>10;
  if(u32Tem > 4000)
  {
	  u32Tem = 4000;
  }

  DAC_SET_CURR((uint16_t)u32Tem);
}


/*
*********************************************************************************************************
*	函 数 名: SetVoutRef
*	功能说明: 设置输出电压参考
*	形    参: Value : 设置的输出电流 单位：0.01V
*	返 回 值: 无
*********************************************************************************************************
*/
void SetVoutRef(uint16_t Value)
{
  uint32_t u32Tem;
	
  u32Tem = Value*q10_SetIoutRef_K/100;//1A 对应124.1212字
  u32Tem = u32Tem>>10;
  if(u32Tem > 4000)
  {
	  u32Tem = 4000;
  }

  DAC_SET_VOL((uint16_t)u32Tem);
}

/*
*********************************************************************************************************
*	函 数 名: DAC_Process
*	功能说明: DAC设置处理
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/

void DAC_Process(void)
{
	
  
  int32_t i32Tem;
  
//---------------------------设置输出电压------------------------------------------------------------
  i32Tem = u16SetVout_Ref*PowerEEP.SetVoutRef_K  + PowerEEP.SetVoutRef_B ;	
  i32Tem = i32Tem>>10;
  if(i32Tem < 100) 
  {
	  i32Tem = 100;
  }

  SetVoutRef(i32Tem); 
  

  //---------------------------设置输出电流------------------------------------------------------------
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


