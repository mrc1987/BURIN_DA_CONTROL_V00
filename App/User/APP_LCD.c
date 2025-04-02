#include "main.h"
#include "app_lcd.h"

/**************************************************************************
*函数名： LCD_State_Show()
*描述：   LCD 状态显示 
*返回：
*说明：
*
*****************************************************************************/ 
void LCD_State_Show()
{
  static uint8_t State=0;
   LCD_Fill(20,200,240,240,0);
  if(Un_WorkStatus.BIT.bPowerErr )
   {
	   switch(State++)
	   {
		   case 0:
		   {
               LCD_ShowChinese(20,200,(uint8_t*)"充电停止",RED,WHITE,32,1);				   
			   break;
		   }
		  case 1:
		   { 
			   if(!Un_PowerFault.BIT.bACOK )
			   {
				
			     LCD_ShowChinese(20,200,(uint8_t*)"市电故障",RED,WHITE,32,1);
			   }				   
			   break;
		   }
		  case 2:
		   {
			   if(!Un_PowerFault.BIT.bOCP )
			   {
			     LCD_ShowChinese(20,200,(uint8_t*)"过流",RED,WHITE,32,1);
			   }				   
			   break;
		   }
		  case 3:
		   {
			   if(Un_PowerFault.BIT.bOVP )
			   {
			     LCD_ShowChinese(20,200,(uint8_t*)"过压",RED,WHITE,32,1);
			   }				   
			   break;
		   }
		  case 4:
		   {
			   if(Un_PowerFault.BIT.bOPP )
			   {
			     LCD_ShowChinese(20,200,(uint8_t*)"过功率",RED,WHITE,32,1);
			   }				   
			   break;
		   }
		  case 5:
		   {
			   if(Un_PowerFault.BIT.bOTP1|| Un_PowerFault.BIT.bOTP2||Un_PowerFault.BIT.bPFC_OTP)
			   {
			     LCD_ShowChinese(20,200,(uint8_t*)"过温",RED,WHITE,32,1);
			   }	
   
			   break;
		   }
		  case 6:
		   {
			   if(Un_PowerFault.BIT.bVbat_Full)
			   {
			     LCD_ShowChinese(20,200,(uint8_t*)"电池充满",RED,WHITE,32,1);
			   }	
    			   
			   break;
		   }	
		  case 7:
		   {
			  if(!Un_PowerFault.BIT.bVbat_Low )
			   {
			     LCD_ShowChinese(20,200,(uint8_t*)"电池电压低",RED,WHITE,32,1);
			   }	
               State = 0;  			   
			   break;
		   }			   
		   default:
		   {
			    LCD_ShowChinese(20,200,(uint8_t*)"充电停止",RED,WHITE,32,1);	
			 	State = 0;  
		   }break;
	   }	
   	   
   }
  else if(Un_WorkStatus.BIT.bCharging )
  {
	State = 0;
	LCD_ShowChinese(20,200,(uint8_t*)"充电中",GREEN,WHITE,32,1);	   
  }  
  else if(!Un_PowerFault.BIT.bCOM_ONOFF )
  {
	State = 0;
	LCD_ShowChinese(20,200,(uint8_t*)"关机",RED,WHITE,32,1);	   
  }  
}
 
/**************************************************************************
*函数名： LCD_Show()
*描述：   LCD显示 
*返回：
*说明：
*
*****************************************************************************/ 
 void LCD_Show()
 {
 static uint8_t Step=0;
   float f32Tem;
   f32Tem = 0;
 

 switch(Step)
  {
	case 0:
	  {
        f32Tem = ADC_Info.u16VoutRMS/100.0;
        LCD_ShowFloatNum1(40,40,f32Tem,4,GREEN,BLACK,48);
		break;
	   }
	case 1:
	  {
         f32Tem = ADC_Info.u16IoutRMS/100.0;	 
         LCD_ShowFloatNum1(40,80,f32Tem,4,GREEN,BLACK,48);
		break;
	   }
	case 2:
	  {
        LCD_ShowIntNum(40,124,ADC_Info.u16Power,4,GREEN,BLACK,48);
		break;
	   }
	case 3:
	  {
        LCD_State_Show();
		break;
	   }
	default:
	{
	  Step = 0;
	  f32Tem = ADC_Info.u16VoutRMS/100.0;
      LCD_ShowFloatNum1(40,40,f32Tem,4,GREEN,BLACK,48);
	}		
  }		 

   Step++;
 }
 
 /**************************************************************************
*函数名： Init_LCD_Show()
*描述：   LCD初始化显示 
*返回：
*说明：
*
*****************************************************************************/ 
 void Init_LCD_Show()
 {
   float f32Tem;
   f32Tem = 0;
   LCD_ShowChinese(10,0,"厦门科司特电子",RED,WHITE,32,1);	 
	 
   f32Tem = ADC_Info.u16VoutRMS/100.0;
   LCD_ShowFloatNum1(40,40,f32Tem,4,GREEN,BLACK,48);
   LCD_ShowString(164,40,"V",GREEN,WHITE,48,1);
	
   f32Tem = ADC_Info.u16IoutRMS/100.0;	 
   LCD_ShowFloatNum1(40,80,f32Tem,4,GREEN,BLACK,48);
   LCD_ShowString(164,80,"A",GREEN,WHITE,48,1);
		
   LCD_ShowIntNum(40,124,ADC_Info.u16Power,4,GREEN,BLACK,48);
   LCD_ShowString(164,124,"w",GREEN,WHITE,48,1);
   LCD_State_Show();
	 
 }
 /**************************************************************************
*函数名： LCD_thread_entry
*描述：   LCD显示任务
*返回：
*说明：
*
*****************************************************************************/
void LCD_thread_entry(void *parameter)
{
	
	LCD_GPIO_Init();
	LCD_Init();
	LCD_Fill(0,0,240,240,0x0);//清屏
	Init_LCD_Show();
 	while(1)
	{
	   rt_thread_delay(200);//200ms 刷新一次
       LCD_Show();
 	}
}