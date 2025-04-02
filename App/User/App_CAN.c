#include "main.h"
#include "app_can.h"

 #define CAN_BAUDRATE  250
 #define PS_ModuleBuff_Size  32
 #define PC_BuffSize         4
 
 
  #define CMD_Status         0xA3
  #define CMD_Vin            0xA4
  #define CMD_Vout           0xA5

  #define CMD_Iout           0xA6
  #define CMD_Iverf          0xA7
  #define CMD_Tem            0xA8
  
#define PC_CAN      CAN0
#define Module_CAN  CAN1
can_parameter_struct can_init_parameter;

can_filter_parameter_struct can_filter_parameter;

can_trasnmit_message_struct Module_transmit_message;
can_trasnmit_message_struct PC_transmit_message;

extern uint8_t  EEP_DAC_KBWrite(uint8_t RegAddr,uint16_t K,int16_t B);
 extern uint8_t  EEP_BootEntry(void);
//=================模块接收消息数组==================================
ByteBitArea PS_Module_CanStatus;//模块CAN接收状态
can_receive_message_struct PS_Module_CANMsg[PS_ModuleBuff_Size];
can_receive_message_struct *PModule_DataPoint;

_CANExID *PC_CANID;
_CANExID_U Module_Tx_CANID;

	
uint8_t Modlue_RxIndex; 
//uint8_t Modlue_Qcmd; 
uint32_t Modlue_RxID; 

//=================上位机接收数组==================================
ByteBitArea PC_CanStatus;//上位机接收状态
can_receive_message_struct PC_CANMsg ;
uint8_t PC_RxIndex; 
extern  struct rt_semaphore PC_ISR_sem;
//===============================================================================
void Module_CAN_Process(void);
void PC_CAN_Poll_Task(void) ;
 void Module_Query_Task(void);
/**************************************************************************************
* FunctionName   : can_gpio_config(void)
* Description    :   

* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void can_gpio_config(void)
{
    /* enable CAN clock */

    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
    
    /* configure CAN0 GPIO */
    gpio_init(GPIOA,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_11);
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_12);
    
    /* configure CAN1 GPIO */
    gpio_init(GPIOB,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_12);
    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_13);

}

/**************************************************************************************
* FunctionName   :  can0_config
* Description    :   

* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void can0_config(can_parameter_struct can_parameter, can_filter_parameter_struct can_filter)
{
	rcu_periph_clock_enable(RCU_CAN0);

    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    can_struct_para_init(CAN_INIT_STRUCT, &can_filter);
	
	/* initialize receive message */
    can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &PC_CANMsg);
	
 
    can_deinit(PC_CAN);
    
 
	
    /* initialize CAN parameters */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = DISABLE;
    can_parameter.auto_wake_up = ENABLE;
    can_parameter.no_auto_retrans = DISABLE;
    can_parameter.rec_fifo_overwrite = DISABLE;
    can_parameter.trans_fifo_order = DISABLE;
    can_parameter.working_mode = CAN_NORMAL_MODE;
    can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1 = CAN_BT_BS1_5TQ;
    can_parameter.time_segment_2 = CAN_BT_BS2_4TQ;
    
    /* 1MBps */
#if CAN_BAUDRATE == 1000
    can_parameter.prescaler = 6;
    /* 500KBps */
#elif CAN_BAUDRATE == 500
    can_parameter.prescaler = 12;
    /* 250KBps */
#elif CAN_BAUDRATE == 250
    can_parameter.prescaler = 24;
    /* 125KBps */
#elif CAN_BAUDRATE == 125
    can_parameter.prescaler = 48;
    /* 100KBps */
#elif  CAN_BAUDRATE == 100
    can_parameter.prescaler = 60;
    /* 50KBps */
#elif  CAN_BAUDRATE == 50
    can_parameter.prescaler = 120;
    /* 20KBps */
#elif  CAN_BAUDRATE == 20
    can_parameter.prescaler = 300;
#else
    #error "please select list can baudrate in private defines in main.c "
#endif  
    /* initialize CAN */
  
    can_init(PC_CAN, &can_parameter);
 
    
    /* initialize filter */ 
    can_filter.filter_number=0;
    can_filter.filter_mode = CAN_FILTERMODE_MASK;
    can_filter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter.filter_list_high = 0x0000;
    can_filter.filter_list_low =  0x0000;
    can_filter.filter_mask_high = 0x0000;
    can_filter.filter_mask_low =  0x0000;
    can_filter.filter_fifo_number = CAN_FIFO0;
    can_filter.filter_enable = ENABLE;
    
    can_filter_init(&can_filter);
    
    /* CAN1 filter number */
    can_filter.filter_number = 15;
    can_filter_init(&can_filter);


    nvic_irq_enable(CAN0_RX0_IRQn,0,0);
 
    can_interrupt_enable(PC_CAN, CAN_INT_RFNE0);

}


/**************************************************************************************
* FunctionName   :  GetTemPerature
* Description    :  获取对应温度值

* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void can1_config(can_parameter_struct can_parameter, can_filter_parameter_struct can_filter)
{
 
    rcu_periph_clock_enable(RCU_CAN1);
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    can_struct_para_init(CAN_INIT_STRUCT, &can_filter);
	
 
	
    /* initialize CAN register */
    can_deinit(Module_CAN);
 
    
 
	
    /* initialize CAN parameters */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = DISABLE;
    can_parameter.auto_wake_up = ENABLE;
    can_parameter.no_auto_retrans = DISABLE;
    can_parameter.rec_fifo_overwrite = DISABLE;
    can_parameter.trans_fifo_order = DISABLE;
    can_parameter.working_mode = CAN_NORMAL_MODE;
    can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1 = CAN_BT_BS1_5TQ;
    can_parameter.time_segment_2 = CAN_BT_BS2_4TQ;
    
    /* 1MBps */
#if CAN_BAUDRATE == 1000
    can_parameter.prescaler = 6;
    /* 500KBps */
#elif CAN_BAUDRATE == 500
    can_parameter.prescaler = 12;
    /* 250KBps */
#elif CAN_BAUDRATE == 250
    can_parameter.prescaler = 24;
    /* 125KBps */
#elif CAN_BAUDRATE == 125
    can_parameter.prescaler = 48;
    /* 100KBps */
#elif  CAN_BAUDRATE == 100
    can_parameter.prescaler = 60;
    /* 50KBps */
#elif  CAN_BAUDRATE == 50
    can_parameter.prescaler = 120;
    /* 20KBps */
#elif  CAN_BAUDRATE == 20
    can_parameter.prescaler = 300;
#else
    #error "please select list can baudrate in private defines in main.c "
#endif  
    /* initialize CAN */
    can_init(Module_CAN, &can_parameter);
 
 
    
    /* initialize filter */ 
    can_filter.filter_number=0;
    can_filter.filter_mode = CAN_FILTERMODE_MASK;
    can_filter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter.filter_list_high = 0x0000;
    can_filter.filter_list_low =  0x0000;
    can_filter.filter_mask_high = 0x0000;
    can_filter.filter_mask_low =  0x0000;
    can_filter.filter_fifo_number = CAN_FIFO0;
    can_filter.filter_enable = ENABLE;
    
    can_filter_init(&can_filter);
    
    /* CAN1 filter number */
    can_filter.filter_number = 15;
    can_filter_init(&can_filter);


 
    nvic_irq_enable(CAN1_RX0_IRQn,0,0);

    can_interrupt_enable(Module_CAN, CAN_INT_RFNE0);
 
}
/**************************************************************************************
* FunctionName   : void app_caninit ( )
* Description    :  

* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void app_caninit ( )
{
	uint8_t *InitP;
	uint16_t i;
	//===============初始化变量=======================================
	InitP=(uint8_t*)&PS_Module_CANMsg;
	
	for(i=0;i<sizeof(PS_Module_CANMsg);i++)
	{
		*InitP=0;
	}
	InitP=(uint8_t*)&PC_CANMsg;
	for(i=0;i<sizeof(PC_CANMsg);i++)
	{
		*InitP=0;
	}
	PModule_DataPoint =(can_receive_message_struct*)&PS_Module_CANMsg;
	Modlue_RxIndex=0;
	PC_RxIndex=0;
	//====================初始化CAN=======================================
	can_gpio_config();
    can0_config(can_init_parameter, can_filter_parameter);//pc
    can1_config(can_init_parameter, can_filter_parameter);//module
 
}

/**************************************************************************************
* FunctionName   :  CAN1_RX0_IRQHandler(void)
* Description    :  Module_CAN

* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void CAN1_RX0_IRQHandler(void)
{

	can_message_receive(Module_CAN, CAN_FIFO0,PModule_DataPoint+Modlue_RxIndex);
		
	if((CAN_FF_EXTENDED == PModule_DataPoint->rx_ff)&&(8 == PModule_DataPoint->rx_dlen))
	{ 
            PSModule_CAN_Rx_flag=1;
			bsp_PutMsg(Msg_ModuleCANRx, (uint32_t)(PModule_DataPoint+Modlue_RxIndex));//将指针压入队列
			Modlue_RxIndex++;
			if(Modlue_RxIndex > PS_ModuleBuff_Size)//接收数据大于缓冲大小设置满标志
			{
			  Modlue_RxIndex=0;
			}
	}                   

}
/**************************************************************************************
* FunctionName   :  CAN1_RX1_IRQHandler(void)
* Description    :  PC_CAN 

* EntryParameter :  None
* ReturnValue    :  None
**************************************************************************************/

void CAN0_RX0_IRQHandler(void)
{
    /* check the receive message */
	//if(PC_CAN_Rx_flag != 0) return;  //不能加，中断标志通过读取清除。否则MCU会死机
    can_message_receive(PC_CAN, CAN_FIFO0, &PC_CANMsg);
	PC_CANID=(_CANExID*)&PC_CANMsg.rx_efid;	
    if((PC_CANID->FuncID==MasterRead)||(PC_CANID->FuncID==MasterWrite)||(PC_CANID->FuncID==CustCMD)||(PC_CANID->FuncID==BootCMD))
	{
	    PC_CAN_Rx_flag= SET; 
	    /* 释放一次信号量 */
        rt_sem_release(&PC_ISR_sem);    //释放—rt_sem_release()；
		
	}

}

/**************************************************************************
*函数名： Modbus_thread_entry
*描述：   Modbus 处理任务
*返回：
*说明：
*
*****************************************************************************/
void PC_CAN_thread_entry(void *parameter)
{
   rt_err_t result;
   uint16_t TimeoutCNT=0;
   rt_thread_delay(10);
	 rt_sem_release(&PC_ISR_sem); //
	while(1)
	{
 
		 /* 永久等待方式持有信号量 */
       result = rt_sem_take(&PC_ISR_sem, 1000);//获取—rt_sem_take();
        if(result == RT_EOK)
		{
	      if(PC_CAN_Rx_flag)
		  {
			 PC_CAN_Rx_flag=0;
		     PC_CAN_Poll_Task() ;//上位机接收信息处理
	         PC_CAN_Rx_flag=0;
		   }
		  TimeoutCNT=0;
	     }
		else
		{
			if(can_error_get(PC_CAN) != CAN_ERROR_NONE)//获取到CAN错误5S保护
			{
              if(++TimeoutCNT > 5)
		      {
                 can0_config(can_init_parameter, can_filter_parameter);//module 5秒超时复位
			     TimeoutCNT=0;
		       }
			}
			
		}
	}
}



/**************************************************************************
*函数名： Modbus_thread_entry
*描述：   Modbus 处理任务
*返回：
*说明：
*
*****************************************************************************/
void Module_CAN_thread_entry(void *parameter)
{
    uint16_t TimeoutCNT=0;
 
	while(1)
	{
	    rt_thread_delay(10);
 
		if(PSModule_CAN_Rx_flag==0)
		{
		   if((can_error_get(Module_CAN) != CAN_ERROR_NONE)&&(++TimeoutCNT > 500))//获取到CAN错误5S保护
		   {
             can1_config(can_init_parameter, can_filter_parameter);//module 5秒超时复位
			 TimeoutCNT=0;
		   }
		}
		else
		{
			TimeoutCNT=0;
			PSModule_CAN_Rx_flag=0;
		}
		
		Module_CAN_Process();//模块接收数据处理
	}
	
}



/**************************************************************************
*函数名： Modbus_thread_entry
*描述：   Modbus 处理任务
*返回：
*说明：  此任务没做任何处理，直接将接收到的数据自动发送到PC
*
*****************************************************************************/
void Module_CAN_Process( )
{
    MSG_T Msg;
   _CANExID *CANID;
   uint16_t u16Tem;
	
   can_receive_message_struct *Can_RxMsgP;
   uint8_t ret;
   ret= bsp_GetMsg2((MSG_T *)&Msg);
	if((ret != 0 )&&(Msg.MsgCode== Msg_ModuleCANRx))
	{
      Can_RxMsgP=(can_receive_message_struct*)Msg.MsgParam;
 
      Module_transmit_message.tx_efid = Can_RxMsgP->rx_efid;
      Module_transmit_message.tx_ft = CAN_FT_DATA;
      Module_transmit_message.tx_ff = CAN_FF_EXTENDED;
      Module_transmit_message.tx_dlen = 8;
	  memcpy(Module_transmit_message.tx_data,Can_RxMsgP->rx_data,8);
	  if(PC_CAN_Tx_flag==0)
	  {
		  CANID=(_CANExID*)&Module_transmit_message.tx_efid;
		  if( CANID->FuncID ==27)  //模块状态
		  {
             Module_transmit_message.tx_data[4]=(Module_transmit_message.tx_data[4]&0xFB)+(((PowerModule_Enable.Value>>(CANID->SourceAddr-1)&0x01))<<2);
			 u16Tem =  (Module_transmit_message.tx_data[2]<<8)+Module_transmit_message.tx_data[3];
			  if( u16Tem > 10000)
			  {
				 Module_transmit_message.tx_data[2] =0;
                 Module_transmit_message.tx_data[3] = 0;				  
			  }
		  }

	     /* transmit message */
		 PC_CAN_Tx_flag=1;
         can_message_transmit(PC_CAN, &Module_transmit_message);//模块接收到的数据原样返回到PC口
		 PC_CAN_Tx_flag=0;
	  }
	}
}

///**************************************************************************
//*函数名： void Module_Query_Task( )
//*描述：   
//*返回：
//*说明：   
//*
//*****************************************************************************/
//void Module_Query_Task( )
//{
//   static uint8_t Addr=1;
//   uint8_t ret;
//   memset(Module_transmit_message.tx_data,0,8);

//	Module_Tx_CANID.ACK=1;
//    Module_Tx_CANID.FuncID=0x5;
//    Module_Tx_CANID.DestAddr=Addr;
//    Module_Tx_CANID.SourceAddr=62;
//	
//	
//   switch(Modlue_Qcmd)
//   {
//	   case CMD_Status:
//           {
//		     Module_transmit_message.tx_data[0]=0x07;
//			 Module_transmit_message.tx_data[1]=Addr;
//			 Module_transmit_message.tx_data[2]=0;
//			 Module_transmit_message.tx_data[3]=CMD_Status;
// 
//	         break;
//		   }
//	   case CMD_Vin:
//		              {
//		     Module_transmit_message.tx_data[0]=0x06;
//			 Module_transmit_message.tx_data[1]=Addr;
//			 Module_transmit_message.tx_data[2]=0;
//			 Module_transmit_message.tx_data[3]=CMD_Vin;
// 
//	         break;
//		   }
//	   case CMD_Vout:
//		              {
//		     Module_transmit_message.tx_data[0]=0x06;
//			 Module_transmit_message.tx_data[1]=Addr;
//			 Module_transmit_message.tx_data[2]=0;
//			 Module_transmit_message.tx_data[3]=CMD_Vout;
//			 Modlue_Qcmd=CMD_Iout;
//	         break;
//		   }
//	   case CMD_Iout:
//		              {
//		     Module_transmit_message.tx_data[0]=0x06;
//			 Module_transmit_message.tx_data[1]=Addr;
//			 Module_transmit_message.tx_data[2]=0;
//			 Module_transmit_message.tx_data[3]=CMD_Iout;
//			 Modlue_Qcmd=CMD_Iverf;
//	         break;
//		   }
//	   case CMD_Iverf:
//		   {
//		     Module_transmit_message.tx_data[0]=0x06;
//			 Module_transmit_message.tx_data[1]=Addr;
//			 Module_transmit_message.tx_data[2]=0;
//			 Module_transmit_message.tx_data[3]=CMD_Iverf;
//			 Modlue_Qcmd=CMD_Tem;
//	         break;
//		   }
//	   case CMD_Tem:
//		   {
//		     Module_transmit_message.tx_data[0]=0x06;
//			 Module_transmit_message.tx_data[1]=Addr;
//			 Module_transmit_message.tx_data[2]=0;
//			 Module_transmit_message.tx_data[3]=CMD_Tem;


//	         break;
//		   }
//	   default:
//	   {
//		 Module_transmit_message.tx_data[0]=0x07;
//	     Module_transmit_message.tx_data[1]=Addr;
//	     Module_transmit_message.tx_data[2]=0;
//		 Module_transmit_message.tx_data[3]=CMD_Status;
//		 Modlue_Qcmd=CMD_Status;
//		 break;  
//	   }
//   }
//      Module_transmit_message.tx_efid = *(uint32_t*)&Module_Tx_CANID;
//      Module_transmit_message.tx_ft = CAN_FT_DATA;
//      Module_transmit_message.tx_ff = CAN_FF_EXTENDED;
//      Module_transmit_message.tx_dlen = 8;
// 
//	  /* transmit message */
//      can_message_transmit(Module_CAN, &Module_transmit_message);
//   
//      Addr++;
//	  if(Addr>12)
//	  {
//		 Addr=1;
//		 Modlue_Qcmd++;
//		  if(Modlue_Qcmd>CMD_Tem)
//		  {
//			  Modlue_Qcmd=CMD_Status;
//	      }
//	  }	

//}


/**************************************************************************
*函数名： uint8_t SynFun( ) 
*描述：  同步设置参数 
*返回：
*说明：   
*
*****************************************************************************/
void SynProcess(uint8_t *Rd)
{
  uint16_t i;
  uint16_t CH,ParaValue;
	
   CH= ((*(Rd+4)) <<8)+*(Rd+5);
   ParaValue= ((*(Rd+6)) <<8)+*(Rd+7);
   switch(*(Rd+2))
   {
	   case 1://同步选定通道ONOFF关机
           {

			  if(ParaValue ==0x0000)
			  {
			     PM_ONOFF.Value &=  ~CH;//同步选定通道ONOFF关机	
			  }
			  else  if(ParaValue ==0xA5A5)
			  {
				 PM_ONOFF.Value |=  CH;//同步选定通道ONOFF开机	 
			  }

			 // PS_ON();
			  *(Rd+4) =  0xA5;
			  *(Rd+5) =  0x00;
			  *(Rd+6) = PM_ONOFF.Value>>8;
			  *(Rd+7) = PM_ONOFF.Value;
	         break;
		   } 
	   case 2:
           {
			  if(ParaValue ==0x0000)
			  {
			     PowerModule_Enable.Value &=  ~CH;//同步选定通道使能禁止
			  }
			  else  if(ParaValue ==0xA5A5)
			  {
				 PowerModule_Enable.Value |=  CH;//同步选定通道使能使能	 
			  }
			  //PS_ON();
			  *(Rd+4) =  0xA5;
			  *(Rd+5) =  0x00;
			  *(Rd+6) =  PowerModule_Enable.Value>>8;
			  *(Rd+7) =  PowerModule_Enable.Value;
	         break;
		   }
	   case 3://同步选定通道设置电流：单位：0.01A
           {
             for(i=0; i< 12;i++)
              {			
                 if((CH&(1<<i)) != 0) wSetDAC_out[i]=ParaValue;
              }
			 *(Rd+4) =  0xA5;
			 *(Rd+5) =  0x00;
	         break;
		   }
   }
   

}
/**************************************************************************
*函数名： Modbus_thread_entry
*描述：   Modbus 处理任务
*返回：
*说明：  此任务没做任何处理，直接将接收到的数据自动发送到PC
*
*****************************************************************************/
void PC_CAN_Poll_Task() 
{
    _CANExID *CANID;
	uint8_t R[8],i;
    uint32_t wTem;
	int16_t int16_Tem;
    float fValue;
	uint8_t SrcAddr,DestAddr,CNTL_Respone;
    
    CNTL_Respone=0;
	CANID=(_CANExID*)&PC_CANMsg.rx_efid;
	if(((CANID->FuncID==MasterRead)||(CANID->FuncID==MasterWrite)||(CANID->FuncID==CustCMD)||(CANID->FuncID==BootCMD))&(CANID->DestAddr>0))
	{
	   SrcAddr  = CANID->SourceAddr; 
	   DestAddr = CANID->DestAddr; 
       //if(DestAddr == 0)return;
	   //memset((uint8_t*)&PC_transmit_message.tx_data,0,8);
	   memcpy(R,PC_CANMsg.rx_data,8);
       memcpy(PC_transmit_message.tx_data,PC_CANMsg.rx_data,8);//原样返回
	 //===================命令执行=======================================================================================  
   
	switch(R[3])
	{
	  case 0xA9: //开关机
	   {
		 if( R[0]==0x04 ) 
		 {
			if(DestAddr<63)
			{
			   if((R[4]==0xAA)  && (R[5]==0x55) && (R[6]==0xAA) && (R[7]==0x55))
			   {
					 PM_ONOFF.Value |= 1<<(DestAddr-1);	//开机
			   }
			   else if((R[4]==0x33)  && (R[5]==0xCC) && (R[6]==0x33) && (R[7]==0xCC))
			   {
					 PM_ONOFF.Value &= ~(1<<(DestAddr-1));	//关机
			   }			   

	         }
			else if(DestAddr == 63)//广播设置开关
			{
			   if((R[4]==0xAA)  &&(R[5]==0x55)&&(R[6]==0xAA)&& (R[7]==0x55))
			   {
					 PM_ONOFF.Value =0x0FFF;	//开机
			   }
			   else if((R[4]==0x33)  && (R[5]==0xCC) && (R[6]==0x33) && (R[7]==0xCC))
			   {
					 PM_ONOFF.Value =0x0000;	//关机
			   }		
			}
			//PS_ON();
			PC_transmit_message.tx_data[6]=PM_ONOFF.Value>>8;
			PC_transmit_message.tx_data[7]=PM_ONOFF.Value;
			CNTL_Respone=1;

		  }
		 else
           {
		       CNTL_Respone=5;
		    }
		 break;
	  }
	  case 0xAA: //设置输出电流
	   {
		if(R[0]==0x06)
		 {
		   if(DestAddr == 63)//广播设置开关
			{
				wTem=(R[4]<<24)+(R[5]<<16)+(R[6]<<8)+(R[7]<<0);
				fValue =  *(float*)&wTem;
				wTem=  fValue*100;	
                for(i=0; i< 12;i++)
                {				
				  wSetDAC_out[i]=wTem;
                }
			}
			else if (DestAddr <= 12)
			{
				wTem=(R[4]<<24)+(R[5]<<16)+(R[6]<<8)+(R[7]<<0);
				fValue =  *(float*)&wTem;
				wTem=  fValue*100;
				wSetDAC_out[DestAddr-1]=wTem;
				CNTL_Respone=1;
			}
		  }else{CNTL_Respone=5;}
		  break;
	   }
      case 0xAB: //模块使能
	   {
	      if( R[0]==0x04 )
		  {
		  	wTem= (R[4]<<24)+ (R[5]<<16)+ (R[6]<<8)+ (R[7]<<0);
			PowerModule_Enable.Value=(uint16_t)wTem;
			PC_transmit_message.tx_data[6]=PowerModule_Enable.Value>>8;
			PC_transmit_message.tx_data[7]=PowerModule_Enable.Value;
 	  	    CNTL_Respone=1;
		  }else{CNTL_Respone=5;}
		 break;
	   }
	  case 0xAC: //同步设置参数
	   {
	      if( R[0]==0x04 )
		  {
		    SynProcess((uint8_t*)PC_transmit_message.tx_data);
 	  	    CNTL_Respone=1;
		  }else
          {CNTL_Respone=5;}
		 break;
	   }  

	  case 0xAF: //设置DAC KB值
	    {
	      if((R[0]==0x06)&&(R[2]==0xAA))  //DAC 校准 
		   {
             if (DestAddr<=12)
			 {
				wTem=(R[4]<<8)+(R[5]<<0);
				int16_Tem= (int16_t)((R[6]<<8)+(R[7]<<0));
				if((wTem >12000) || (wTem < 8000)  || (int16_Tem<-5000) || (int16_Tem >5000))
				{
				   PC_CANMsg.rx_data[4]=0xF0;
				   PC_CANMsg.rx_data[5]=0x00;
				   PC_CANMsg.rx_data[6]=0xF0;
				   PC_CANMsg.rx_data[7]=0x00;
				}
				else
				{
				   EEP_Data.DAC_K[DestAddr-1]=(uint16_t)wTem;
				   EEP_Data.DAC_B[DestAddr-1]=(int16_t)int16_Tem;
				   if(EEP_DAC_KBWrite(DestAddr-1,EEP_Data.DAC_K[DestAddr-1],EEP_Data.DAC_B[DestAddr-1]))
				   {
			        PC_CANMsg.rx_data[4]=0xAA;
				    PC_CANMsg.rx_data[5]=0x55;
				    PC_CANMsg.rx_data[6]=0xAA;
				    PC_CANMsg.rx_data[7]=0x55;
				   }
				   else
				   {
				     PC_CANMsg.rx_data[4]=0xF0;
				     PC_CANMsg.rx_data[5]=0x00;
				     PC_CANMsg.rx_data[6]=DestAddr;
				     PC_CANMsg.rx_data[7]=0x01;					   
				   }
				}
				CNTL_Respone=1;
				memcpy(PC_transmit_message.tx_data,PC_CANMsg.rx_data,8);
			 }
		   }
      	else  if((R[0]==0x06)&&(R[2]==0xA5)) //发送给模块电流校准
		  {
             if (DestAddr<=12)
			 {
				wTem=(R[4]<<8)+(R[5]<<0);
				int16_Tem= (int16_t)((R[6]<<8)+(R[7]<<0));
				if((wTem >12000) || (wTem < 8000)  || (int16_Tem<-5000) || (int16_Tem >5000))
				{
				   PC_CANMsg.rx_data[4]=0xF0;
				   PC_CANMsg.rx_data[5]=0x00;
				   PC_CANMsg.rx_data[6]=0xF0;
				   PC_CANMsg.rx_data[7]=0x00;
				}
				else
				{
				  PC_transmit_message.tx_efid = PC_CANMsg.rx_efid;
				  PC_transmit_message.tx_ft = CAN_FT_DATA;
				  PC_transmit_message.tx_ff = CAN_FF_EXTENDED;
				  PC_transmit_message.tx_dlen = 8;
				  can_message_transmit(Module_CAN, &PC_transmit_message);//发送数据的模块口
				}
				CNTL_Respone=1;
				memcpy(PC_transmit_message.tx_data,PC_CANMsg.rx_data,8);
			 }
		   }
         else
          {
		     CNTL_Respone=5;
		   }//错误响应
		  break;
		}
	 case 0xB0: // 进入Boot
	   {
	      if( R[0]==0x04 && R[1]==0xA5 && R[2]==0x5A)
		  {
		     if( EEP_BootEntry())
			 {
				  NVIC_SystemReset();//重启进入BootLoader
			 }
             else{
				  CNTL_Respone=5; 
			 }

		  }
		  else
          {
			  CNTL_Respone=5;
		  }
		 break;
	   }  
	  default: //转发给模块
		{

		  PC_transmit_message.tx_efid = PC_CANMsg.rx_efid;
		  PC_transmit_message.tx_ft = CAN_FT_DATA;
		  PC_transmit_message.tx_ff = CAN_FF_EXTENDED;
 		  PC_transmit_message.tx_dlen = 8;
		  can_message_transmit(Module_CAN, &PC_transmit_message);//发送数据的模块口
		  return;
		}
	 }
		
	 
	 //=========================================================================================================================	
	  if((CANID->ACK != 0)&&(CNTL_Respone != 0))//需要响应,只有控制板响应的才回应，需要模块回应的在模块线程中处理
	  {   //CNTL_Respone=1 正常响应 CNTL_Respone=5 数据类型错误响应
		  
		
		  CANID->SourceAddr =  DestAddr; 
	      CANID->DestAddr = SrcAddr; 
		  PC_transmit_message.tx_efid = *(uint32_t*)CANID;
		  PC_transmit_message.tx_ft = CAN_FT_DATA;
		  PC_transmit_message.tx_ff = CAN_FF_EXTENDED;
		  PC_transmit_message.tx_dlen = 8;
		  if(PC_CAN_Tx_flag==0)
		  {
		  	PC_CAN_Tx_flag=1;
			if(CNTL_Respone==5)PC_transmit_message.tx_data[0] |= (CNTL_Respone<<4);//第一字节高4位为错误类型
		    can_message_transmit(PC_CAN, &PC_transmit_message);//模块接收到的数据原样返回到PC口
			PC_CAN_Tx_flag=0;
		  }
	  }
	}
	
	memset((uint8_t*)&PC_CANMsg.rx_data,0,8);

}


