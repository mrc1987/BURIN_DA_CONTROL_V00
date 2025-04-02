#include "main.h"

 

 


uint8_t WorkMode;//工作模式
uint32_t Work_RUN_ms;
//---------------thread--------------------------------------------
 
 /*  变量分配4字节对齐 */
 ALIGN(RT_ALIGN_SIZE)
 static rt_uint8_t MainTask_stack[512];
  
 static rt_uint8_t PC_UART_stack[512];
 
 
 struct rt_thread  MainTask_thread; 
 
 struct rt_thread  PC_UART_thread;
 
 static rt_uint8_t DA_Task_stack[512];
 
 
 struct rt_thread  DA_Task_thread; 

 
 /**************************************************************************
*函数名： void InitEEP()
*描述：  
*返回：
*说明：
*
*****************************************************************************/
 
 void InitEEP()
 {
    gEEP_data.EEPFlag =0xA5;
	 
	 
	gEEP_data.DMM_V_B = 0;
	gEEP_data.DMM_V_K = 1<<10;
	 
 
	gEEP_data.DMM_V2_B = 0;
	gEEP_data.DMM_V2_K = 1<<10;	 

	 
 }
 
 /**************************************************************************
*函数名： void ClearAlarm()
*描述：  清除故障
*返回：
*说明：
*
*****************************************************************************/
 
 void ClearAlarm()
 {
 
 }
 /**************************************************************************
*函数名：InitVariant
*描述：  初始化所有变量
*返回：
*说明：
*
*****************************************************************************/
void InitVariant()
{
   WorkMode = 0;
 
}	
 
 	
/*
*********************************************************************************************************
*   函 数 名: Relay_Control
*   功能说明:
*   形    参:
*   返 回 值: 无
*********************************************************************************************************
*/
void Relay_Control()
{
	
 
}

/*
*********************************************************************************************************
*   函 数 名: in_check()
*   功能说明:
*   形    参:
*   返 回 值: 无
*********************************************************************************************************
*/
void ADDR_check()
{

	if(ADD1_IO_IN == 0)
	{
		WorkSpace.ADDR_Flags.Bit.ADDR1 = 1;
	}
	else
	{
		WorkSpace.ADDR_Flags.Bit.ADDR1 = 0;
	}	
	
	if(ADD2_IO_IN == 0)
	{
		WorkSpace.ADDR_Flags.Bit.ADDR2 = 1;
	}
	else
	{
		WorkSpace.ADDR_Flags.Bit.ADDR2 = 0;
	}	
	
	if(ADD3_IO_IN == 0)
	{
		WorkSpace.ADDR_Flags.Bit.ADDR3 = 1;
	}
	else
	{
		WorkSpace.ADDR_Flags.Bit.ADDR3 = 0;
	}	
	
	if(ADD4_IO_IN == 0)
	{
		WorkSpace.ADDR_Flags.Bit.ADDR4 = 1;
	}
	else
	{
		WorkSpace.ADDR_Flags.Bit.ADDR4 = 0;
	}	
 
   if(WorkSpace.modbus_addr != WorkSpace.ADDR_Flags.all)
   {
		WorkSpace.modbus_addr = WorkSpace.ADDR_Flags.all;
   }
}


/*
*********************************************************************************************************
*   函 数 名: in_check()
*   功能说明:
*   形    参:
*   返 回 值: 无
*********************************************************************************************************
*/
void Input_IO_check()
{
	static uint8_t IO_Select_State = 0;	
	static uint8_t IO_Read_State = 0;	
	IO_Select_State++
	switch (IO_Select_State&0x03)
	{
	 case 0:
	   {
		 S0(0);
		 S1(0); 
		 rt_thread_delay(2);
         WorkSpace.WorkFlags.BIT.DA1_OUT_Mode = S1_IO_IN1;
		 WorkSpace.DA1_SetFlags.BIT.DA4       = S1_IO_IN2;
 
         WorkSpace.WorkFlags.BIT.DA2_OUT_Mode = S2_IO_IN1;
		 WorkSpace.DA2_SetFlags.BIT.DA4       = S2_IO_IN2;

		 break;
	   }
	   case 1:
	   {
		 S0(1);
		 S1(0); 
		 rt_thread_delay(2);
         WorkSpace.WorkFlags.BIT.DA1_ONOFF     = S1_IO_IN1;
		 WorkSpace.DA1_SetFlags.BIT.DA1       = S1_IO_IN2;
 
         WorkSpace.WorkFlags.BIT.DA2_ONOFF    = S2_IO_IN1;
		 WorkSpace.DA2_SetFlags.BIT.DA1       = S2_IO_IN2;

		 break;
	   }
	   case 2:
	   {
		 S0(0);
		 S1(1); 
		 rt_thread_delay(2);
         WorkSpace.WorkFlags.BIT.DA1_5V_ONOFF     = S1_IO_IN1;
		 WorkSpace.DA1_SetFlags.BIT.DA3       = S1_IO_IN2;
 
         WorkSpace.WorkFlags.BIT.DA2_5V_ONOFF    = S2_IO_IN1;
		 WorkSpace.DA2_SetFlags.BIT.DA3       = S2_IO_IN2;

		 break;
	   }
	   case 3:
	   {
		 S0(1);
		 S1(1); 
		 rt_thread_delay(2);
         WorkSpace.WorkFlags.BIT.DA2_PWM_EN     = S1_IO_IN1;
		 WorkSpace.DA1_SetFlags.BIT.DA2         = S1_IO_IN2;
 
         WorkSpace.WorkFlags.BIT.DA2_PWM_EN     = S2_IO_IN1;
		 WorkSpace.DA2_SetFlags.BIT.DA2         = S2_IO_IN2;


		 break;
	   }

	
	default:
	{
		S0(0);
		S1(0); 
		rt_thread_delay(2);
		WorkSpace.WorkFlags.BIT.DA1_OUT_Mode = S1_IO_IN1;
		WorkSpace.DA1_SetFlags.BIT.DA4       = S1_IO_IN2;

		WorkSpace.WorkFlags.BIT.DA2_OUT_Mode = S2_IO_IN1;
		WorkSpace.DA2_SetFlags.BIT.DA4       = S2_IO_IN2;
        IO_Select_State = 0;	
		break;
	  }
 
	}
 


 
	
}

void Manu_DA_Setting()
{
	//手动设置DA输出电压值
	if(WorkSpace.WorkFlags.BIT.DA1_5V_ONOFF == 1)
	{
		DA1_5V_ONOFF(1);
	}
	else
	{
		DA1_5V_ONOFF(0);
	}
	
	if(WorkSpace.WorkFlags.BIT.DA2_5V_ONOFF == 1)
	{
		DA2_5V_ONOFF(1);
	}
	else
	{
		DA2_5V_ONOFF(0);
	}
	/******************************************/
	WorkSpace.DA1_V = 0;
    if(WorkSpace.DA1_SetFlags.BIT.DA0 == 1)
	{
		WorkSpace.DA1_V += 10;// 
	}
	else if(WorkSpace.DA1_SetFlags.BIT.DA1 == 1)
	{
		WorkSpace.DA1_V += 20;// 
	}
	else if(WorkSpace.DA1_SetFlags.BIT.DA2 == 1)
	{
		WorkSpace.DA1_V += 50;// 
	}
	else if(WorkSpace.DA1_SetFlags.BIT.DA3 == 1)
	{
		WorkSpace.DA1_V += 100;// 
	}
	else if(WorkSpace.DA1_SetFlags.BIT.DA4 == 1)
	{
		WorkSpace.DA1_V += 150;// 
	}
	else if(WorkSpace.DA1_SetFlags.BIT.DA5 == 1)
	{
		WorkSpace.DA1_V += 200;// 
	}
 /***************************************************/
 WorkSpace.DA2_V = 0;
 if(WorkSpace.DA2_SetFlags.BIT.DA0 == 1)
 {
	 WorkSpace.DA2_V += 10;// 
 }
 else if(WorkSpace.DA2_SetFlags.BIT.DA1 == 1)
 {
	 WorkSpace.DA2_V += 20;// 
 }
 else if(WorkSpace.DA2_SetFlags.BIT.DA2 == 1)
 {
	 WorkSpace.DA2_V += 50;// 
 }
 else if(WorkSpace.DA2_SetFlags.BIT.DA3 == 1)
 {
	 WorkSpace.DA2_V += 100;// 
 }
 else if(WorkSpace.DA2_SetFlags.BIT.DA4 == 1)
 {
	 WorkSpace.DA2_V += 150;// 
 }
 else if(WorkSpace.DA2_SetFlags.BIT.DA5 == 1)
 {
	 WorkSpace.DA2_V += 200;// 
 }
 
	
 
	
}	
 
/**************************************************************************
*函数名： MainTask_thread_entry
*描述：  电源检测控制任务
*返回：
*说明：
*
*****************************************************************************/
void MainTask_thread_entry(void *parameter)
{
   while(1)
	{
	 
		rt_thread_delay(10);

	}

}
 
/**************************************************************************
*函数名： MainTask_thread_entry
*描述：  电源检测控制任务
*返回：
*说明：
*
*****************************************************************************/
void DA_Task_thread_entry(void *parameter)
{
   while(1)
	{
	 
		rt_thread_delay(5);
		Input_IO_check();
		ADDR_check();
	}

}
 



/**************************************************************************
*函数名： APP_TaskInit
*描述：   IO初始化
*返回：
*说明：
*
*****************************************************************************/
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



 

