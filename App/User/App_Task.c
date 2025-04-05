#include "main.h"

 


uint8_t WorkMode; 
uint32_t Work_RUN_ms;
//---------------thread--------------------------------------------
 
 
 ALIGN(RT_ALIGN_SIZE)
 static rt_uint8_t MainTask_stack[512];
  
 static rt_uint8_t PC_UART_stack[512];
 
 
 struct rt_thread  MainTask_thread; 
 
 struct rt_thread  PC_UART_thread;
 
 static rt_uint8_t DA_Task_stack[512];
 
 
 struct rt_thread  DA_Task_thread; 

 

 
 
/*
*********************************************************************************************************
*	函 数 名: Auto_CNTL_DAC
*	功能说明:  
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
void InitVariant()
{
   WorkMode = 0;
 
}	
 
 	
 

/*
*********************************************************************************************************
*	函 数 名: ADDR_check
*	功能说明:  
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
void ADDR_check()
{
  static uint8_t addr_tem= 0;
 
	WorkSpace.ADDR_Flags.all = 0;
	if(ADD1_IO_IN == 0)
	{
		WorkSpace.ADDR_Flags.bit.ADDR1 = 1;
	}
	else
	{
		WorkSpace.ADDR_Flags.bit.ADDR1 = 0;
	}	
	
	if(ADD2_IO_IN == 0)
	{
		WorkSpace.ADDR_Flags.bit.ADDR2 = 1;
	}
	else
	{
		WorkSpace.ADDR_Flags.bit.ADDR2 = 0;
	}	
	
	if(ADD3_IO_IN == 0)
	{
		WorkSpace.ADDR_Flags.bit.ADDR3 = 1;
	}
	else
	{
		WorkSpace.ADDR_Flags.bit.ADDR3 = 0;
	}	
	
	if(ADD4_IO_IN == 0)
	{
		WorkSpace.ADDR_Flags.bit.ADDR4 = 1;
	}
	else
	{
		WorkSpace.ADDR_Flags.bit.ADDR4 = 0;
	}	
 
	addr_tem = WorkSpace.ADDR_Flags.all +1;
   if(WorkSpace.modbus_addr != addr_tem)
   {
		WorkSpace.modbus_addr = addr_tem;
   }
}


/*
*********************************************************************************************************
*	函 数 名: Input_IO_check
*	功能说明:  
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
void Input_IO_check()
{
	static uint8_t IO_Select_State = 0;	
	static uint8_t IO_Read_State = 0;	
	IO_Select_State++;
	switch (IO_Select_State&0x03)
	{
	 case 0:
	   {
		 S0(0);
		 S1(0); 
		 rt_thread_delay(2);
         WorkSpace.WorkFlags.bit.DA1_CNTL_Rise_Mode = S1_IO_IN1;
		 WorkSpace.DA1_SetFlags.BIT.DA4       = S1_IO_IN2;
 
         WorkSpace.WorkFlags.bit.DA2_CNTL_Rise_Mode = S2_IO_IN1;
		 WorkSpace.DA2_SetFlags.BIT.DA4       = S2_IO_IN2;

		 break;
	   }
	   case 1:
	   {
		 S0(1);
		 S1(0); 
		 rt_thread_delay(2);
         WorkSpace.WorkFlags.bit.DA1_CNTL_ONOFF     = S1_IO_IN1;
		 WorkSpace.DA1_SetFlags.BIT.DA1       = S1_IO_IN2;
 
         WorkSpace.WorkFlags.bit.DA2_CNTL_ONOFF    = S2_IO_IN1;
		 WorkSpace.DA2_SetFlags.BIT.DA1       = S2_IO_IN2;

		 break;
	   }
	   case 2:
	   {
		 S0(0);
		 S1(1); 
		 rt_thread_delay(2);
         WorkSpace.WorkFlags.bit.DA1_CNTL_5V_ONOFF     = S1_IO_IN1;
		 WorkSpace.DA1_SetFlags.BIT.DA3       = S1_IO_IN2;
 
         WorkSpace.WorkFlags.bit.DA2_CNTL_5V_ONOFF    = S2_IO_IN1;
		 WorkSpace.DA2_SetFlags.BIT.DA3       = S2_IO_IN2;

		 break;
	   }
	   case 3:
	   {
		 S0(1);
		 S1(1); 
		 rt_thread_delay(2);
         WorkSpace.WorkFlags.bit.DA1_CNTL_PWM_EN     = S1_IO_IN1;
		 WorkSpace.DA1_SetFlags.BIT.DA2         = S1_IO_IN2;
 
         WorkSpace.WorkFlags.bit.DA2_CNTL_PWM_EN     = S2_IO_IN1;
		 WorkSpace.DA2_SetFlags.BIT.DA2         = S2_IO_IN2;


		 break;
	   }

	
	default:
	{
		S0(0);
		S1(0); 
		rt_thread_delay(2);
         WorkSpace.WorkFlags.bit.DA1_CNTL_Rise_Mode = S1_IO_IN1;
		 WorkSpace.DA1_SetFlags.BIT.DA4       = S1_IO_IN2;
 
         WorkSpace.WorkFlags.bit.DA2_CNTL_Rise_Mode = S2_IO_IN1;
		 WorkSpace.DA2_SetFlags.BIT.DA4       = S2_IO_IN2;
        IO_Select_State = 0;	
		break;
	  }
 
	}
 


 
	
}
/*
*********************************************************************************************************
*	函 数 名: Manu_CNTL_DAC
*	功能说明:  
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
void Manu_CNTL_DAC()
{
	static U16 DA1_SetValue = 0;
	static U16 DA2_SetValue = 0;
	U16    U16Tem = 0;
 
	if(WorkSpace.WorkFlags.bit.DA1_CNTL_5V_ONOFF == 1)
	{
		DA1_5V_ONOFF(1);
	}
	else
	{
		DA1_5V_ONOFF(0);
	}
	
	if(WorkSpace.WorkFlags.bit.DA2_CNTL_5V_ONOFF == 1)
	{
		DA2_5V_ONOFF(1);
	}
	else
	{
		DA2_5V_ONOFF(0);
	}
	/******************************************/
	U16Tem = 0;
    if(WorkSpace.DA1_SetFlags.BIT.DA0 == 1)
	{
		U16Tem += 10;// 
	}
	else if(WorkSpace.DA1_SetFlags.BIT.DA1 == 1)
	{
		U16Tem += 20;// 
	}
	else if(WorkSpace.DA1_SetFlags.BIT.DA2 == 1)
	{
		U16Tem += 50;// 
	}
	else if(WorkSpace.DA1_SetFlags.BIT.DA3 == 1)
	{
		U16Tem += 100;// 
	}
	else if(WorkSpace.DA1_SetFlags.BIT.DA4 == 1)
	{
		U16Tem += 150;// 
	}
	else if(WorkSpace.DA1_SetFlags.BIT.DA5 == 1)
	{
		U16Tem += 200;// 
	}
    if(DA1_SetValue == U16Tem)
	{
		WorkSpace.DA1_V  = DA1_SetValue  ;
	}
 
	DA1_SetValue = U16Tem;
 /***************************************************/
 U16Tem = 0;
 if(WorkSpace.DA2_SetFlags.BIT.DA0 == 1)
 {
	U16Tem += 10;// 
 }
 else if(WorkSpace.DA2_SetFlags.BIT.DA1 == 1)
 {
	U16Tem += 20;// 
 }
 else if(WorkSpace.DA2_SetFlags.BIT.DA2 == 1)
 {
	U16Tem += 50;// 
 }
 else if(WorkSpace.DA2_SetFlags.BIT.DA3 == 1)
 {
	U16Tem += 100;// 
 }
 else if(WorkSpace.DA2_SetFlags.BIT.DA4 == 1)
 {
	U16Tem  += 150;// 
 }
 else if(WorkSpace.DA2_SetFlags.BIT.DA5 == 1)
 {
	U16Tem += 200;// 
 }
 
 if(DA2_SetValue == U16Tem)
 {
	 WorkSpace.DA2_V  = DA2_SetValue  ;
 }

 DA2_SetValue = U16Tem;	

 
 
	
}	
 
/*
*********************************************************************************************************
*	函 数 名: Auto_CNTL_DAC
*	功能说明:  
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
void Auto_CNTL_DAC()
{
 
	WorkSpace.WorkFlags.bit.DA1_CNTL_5V_ONOFF = gEEP_data.DA1_5V_EN;
	WorkSpace.WorkFlags.bit.DA2_CNTL_5V_ONOFF = gEEP_data.DA2_5V_EN;

	WorkSpace.WorkFlags.bit.DA1_CNTL_ONOFF = gEEP_data.DA1_ONOFF;
	WorkSpace.WorkFlags.bit.DA1_CNTL_ONOFF = gEEP_data.DA1_ONOFF;

	WorkSpace.WorkFlags.bit.DA1_CNTL_Rise_Mode = gEEP_data.DA1_Out_Mode;
	WorkSpace.WorkFlags.bit.DA2_CNTL_Rise_Mode = gEEP_data.DA2_Out_Mode;

	WorkSpace.WorkFlags.bit.DA1_CNTL_PWM_EN = gEEP_data.DA1_PWM_EN;
	WorkSpace.WorkFlags.bit.DA2_CNTL_PWM_EN = gEEP_data.DA2_PWM_EN;

	/******************************************/
	WorkSpace.DA1_V = gEEP_data.DA1_V;
	WorkSpace.DA2_V = gEEP_data.DA2_V;
 
 
	
}	

/*
*********************************************************************************************************
*	函 数 名: MainTask_thread_entry
*	功能说明:  
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
void MainTask_thread_entry(void *parameter)
{
   while(1)
	{
	 
		rt_thread_delay(10);
		Input_IO_check();
		ADDR_check();
		EEP_SaveTask();
	}

}
 
/*
*********************************************************************************************************
*	函 数 名: DA_Task_thread_entry
*	功能说明:  
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
void DA_Task_thread_entry(void *parameter)
{
	static uint8_t DA_RunStep = 0;
   while(1)
	{
	 
		rt_thread_delay(10);
		if(WorkSpace.WorkFlags.bit.DA_AutoMode == 0)
		{
			Manu_CNTL_DAC();//锟街讹拷锟斤拷锟斤拷
		}
		else
		{
			Auto_CNTL_DAC();//锟皆讹拷锟斤拷锟斤拷
		}
		DAC_Process();//
	}

}
 



/*
*********************************************************************************************************
*	函 数 名: APP_TaskInit
*	功能说明:  
*	形    参: 
*	返 回 值: 无
*********************************************************************************************************
*/
void APP_TaskInit(void)
{

   rt_err_t result;

	
	result = rt_thread_init(&MainTask_thread, "MainTask_thread_entry", MainTask_thread_entry, RT_NULL,MainTask_stack, sizeof(MainTask_stack), 3, 5);
    if(result == RT_EOK)
	{
		RTT_msg("CNTL_thread_entry init Successed \n\r");
		rt_thread_startup(&MainTask_thread);
	}
	
 
	result = rt_thread_init(&DA_Task_thread, "DA_thread_entry", MainTask_thread_entry, RT_NULL,DA_Task_stack, sizeof(DA_Task_stack), 3, 5);
    if(result == RT_EOK)
	{
	 
		rt_thread_startup(&DA_Task_thread);
	}

 
    result = rt_thread_init(&PC_UART_thread, "PC_CAN_thread_entry", PC_UART_thread_entry, RT_NULL,PC_UART_stack, sizeof(PC_UART_stack), 1,5);
    if(result == RT_EOK)
	{
		rt_thread_startup(&PC_UART_thread);
	}


	
}



 

