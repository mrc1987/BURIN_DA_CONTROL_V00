#include "main.h"
#include "app_uart.h"
#include "drv_uart.h" 
#include "modbus_slave.h"
/********************************************************/

 WordBitArea  RS485_Status;
 uint8_t *pRxPoint;
 uint8_t  u8RxLen;
 
  /* 信号量控制块 */
 struct rt_semaphore PC_UART_ISR_Sem;
 

 uint8_t Send_BUF[8]={0};

 /**************************************************************************
*函数名： CostomCrc16Calc
*描述：   客户CRC 效验
*返回：
*说明：
*
*****************************************************************************/ 
 uint16_t CostomCrc16Calc(uint8_t*data_arr, uint8_t data_len) 
 { 
   uint16_t crc16 ;
   uint8_t i ; 
   crc16 = 0; 
   for(i = 0; i < data_len; i++) 
	{ crc16 = (uint16_t)(( crc16 >> 8) | (crc16 << 8)); 
	   crc16 ^= data_arr[i]; 
	   crc16 ^= (uint16_t)(( crc16 & 0xFF) >> 4); 
	   crc16 ^= (uint16_t)(( crc16 << 8) << 4); 
	   crc16 ^= (uint16_t)((( crc16 & 0xFF) << 4) << 1);
	}
	return crc16; 
  }
 /**************************************************************************
*函数名： MCU_ASK()
*描述：   串口任务处理
*返回：
*说明：
*
*****************************************************************************/
void MCU_ASK(void)
{
	uint16_t CalCRC;
	Send_BUF[0] = 0xBC;
	Send_BUF[1] = 0x00;

 
	CalCRC = CRC16_Modbus(Send_BUF,2)>>8;
    Send_BUF[2] = CalCRC>>8;
 
	UART_DMASend(Send_BUF, 3);
	//UART_Send(Send_BUF, 3);
}
 
/**************************************************************************
*函数名： UART_Process()
*描述：   串口任务处理
*返回：
*说明：
*
*****************************************************************************/
void  UART_Process(void)
{
  uint8_t CalCRC;
  uint8_t RxCRC;
  uint8_t CMDCode;
  uint16_t u16Tem;
 
 
   Modbus_Poll_Task( );//ModBus协议 
 
   UART_DMARx_Enable();

}
 
/**************************************************************************
*函数名： PC_UART_ISR
*描述：   串口中断调用任务
*返回：
*说明：
*
*****************************************************************************/

void  PC_UART_ISR(uint8_t *pRx,uint8_t Len)
{
  if(Len > 0)
  {
	  	g_tModS.RxCount  = Len;
	 // u8RxLen  = Len; 
	  //pRxPoint =  pRx;
	  rt_sem_release(&PC_UART_ISR_Sem);
  }
}
 
 
 /**************************************************************************
*函数名： PS_CNTL_Task
*描述：  电源检测控制任务
*返回：
*说明：
*
*****************************************************************************/
void PC_UART_thread_entry(void *parameter)
{
   
   rt_err_t result;
   pRxPoint =  NULL;
   u8RxLen = 0;
	g_tModS.addr = 0x01;
	g_tModS.RxCount = 0;
   result = rt_sem_init(&PC_UART_ISR_Sem, "PC_UART_ISR_Sem", 0, RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        RTT_msg("PC_ISR_sem Init Fail \n\r");
    }
	//*****************************************************//
	 UART_Init(PC_UART_ISR);//串口初始化，并注册中断函数
   
	//***************************************************//
	while(1)
	{
	   rt_thread_delay(5);//防止平凡进入中断导致打断其他线程
	   //等待获取中断信号量，超时1000ms
	   rt_sem_take(&PC_UART_ISR_Sem,1000);//
       UART_Process();
	   USART_ClearErr();
 
 	}
}

