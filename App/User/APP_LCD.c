#include "main.h"
#include "app_lcd.h"

/**************************************************************************
*�������� LCD_State_Show()
*������   LCD ״̬��ʾ 
*���أ�
*˵����
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
               LCD_ShowChinese(20,200,(uint8_t*)"���ֹͣ",RED,WHITE,32,1);				   
			   break;
		   }
		  case 1:
		   { 
			   if(!Un_PowerFault.BIT.bACOK )
			   {
				
			     LCD_ShowChinese(20,200,(uint8_t*)"�е����",RED,WHITE,32,1);
			   }				   
			   break;
		   }
		  case 2:
		   {
			   if(!Un_PowerFault.BIT.bOCP )
			   {
			     LCD_ShowChinese(20,200,(uint8_t*)"����",RED,WHITE,32,1);
			   }				   
			   break;
		   }
		  case 3:
		   {
			   if(Un_PowerFault.BIT.bOVP )
			   {
			     LCD_ShowChinese(20,200,(uint8_t*)"��ѹ",RED,WHITE,32,1);
			   }				   
			   break;
		   }
		  case 4:
		   {
			   if(Un_PowerFault.BIT.bOPP )
			   {
			     LCD_ShowChinese(20,200,(uint8_t*)"������",RED,WHITE,32,1);
			   }				   
			   break;
		   }
		  case 5:
		   {
			   if(Un_PowerFault.BIT.bOTP1|| Un_PowerFault.BIT.bOTP2||Un_PowerFault.BIT.bPFC_OTP)
			   {
			     LCD_ShowChinese(20,200,(uint8_t*)"����",RED,WHITE,32,1);
			   }	
   
			   break;
		   }
		  case 6:
		   {
			   if(Un_PowerFault.BIT.bVbat_Full)
			   {
			     LCD_ShowChinese(20,200,(uint8_t*)"��س���",RED,WHITE,32,1);
			   }	
    			   
			   break;
		   }	
		  case 7:
		   {
			  if(!Un_PowerFault.BIT.bVbat_Low )
			   {
			     LCD_ShowChinese(20,200,(uint8_t*)"��ص�ѹ��",RED,WHITE,32,1);
			   }	
               State = 0;  			   
			   break;
		   }			   
		   default:
		   {
			    LCD_ShowChinese(20,200,(uint8_t*)"���ֹͣ",RED,WHITE,32,1);	
			 	State = 0;  
		   }break;
	   }	
   	   
   }
  else if(Un_WorkStatus.BIT.bCharging )
  {
	State = 0;
	LCD_ShowChinese(20,200,(uint8_t*)"�����",GREEN,WHITE,32,1);	   
  }  
  else if(!Un_PowerFault.BIT.bCOM_ONOFF )
  {
	State = 0;
	LCD_ShowChinese(20,200,(uint8_t*)"�ػ�",RED,WHITE,32,1);	   
  }  
}
 
/**************************************************************************
*�������� LCD_Show()
*������   LCD��ʾ 
*���أ�
*˵����
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
*�������� Init_LCD_Show()
*������   LCD��ʼ����ʾ 
*���أ�
*˵����
*
*****************************************************************************/ 
 void Init_LCD_Show()
 {
   float f32Tem;
   f32Tem = 0;
   LCD_ShowChinese(10,0,"���ſ�˾�ص���",RED,WHITE,32,1);	 
	 
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
*�������� LCD_thread_entry
*������   LCD��ʾ����
*���أ�
*˵����
*
*****************************************************************************/
void LCD_thread_entry(void *parameter)
{
	
	LCD_GPIO_Init();
	LCD_Init();
	LCD_Fill(0,0,240,240,0x0);//����
	Init_LCD_Show();
 	while(1)
	{
	   rt_thread_delay(200);//200ms ˢ��һ��
       LCD_Show();
 	}
}