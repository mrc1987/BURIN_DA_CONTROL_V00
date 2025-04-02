/*
*********************************************************************************************************
*
*	ģ������ : I2C��������ģ��
*	�ļ����� : bsp_i2c_gpio.c
*	��    �� : V1.0
*	˵    �� : ��gpioģ��i2c���ߡ���ģ�鲻����Ӧ�ò�����֡��������I2C���߻�������������
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		
*
*
*********************************************************************************************************
*/

#include "DRV_UART.h"
#include "modbus_slave.h"

#include <string.h>

#define USART_DATA_ADDRESS    ((uint32_t)&USART_DATA(USART2))

#define BAUD             9600
#define UART_PORT        USART2
#define UART_BUFF_SIZE   64

//#define  UART_RX_DMA_CH   DMA_CH4  //UART0
//#define  UART_TX_DMA_CH   DMA_CH3  //UART0

#define  UART_RX_DMA_CH   DMA_CH2    //UART2
#define  UART_TX_DMA_CH   DMA_CH1    //UART2
/*************************************************/
uint8_t Rx_buffer[UART_BUFF_SIZE];
uint8_t Tx_buffer[UART_BUFF_SIZE];
 
#define USE_UART2
 

void  uart_dmainit(void);

UART_CallBackDef UART_CallBack; 
/**************************************************************************
*��������  UART_Init(void)
*������ 
*���أ�
*˵����
*
*****************************************************************************/
void UART_Init(UART_CallBackDef CallBack)
{

	UART_CallBack = NULL;
	
	UART_CallBack = CallBack;

 
	/*************************************************/
	 Modbus_Init(Tx_buffer ,Rx_buffer,(pSend)UART_DMASend);//ע��Modbus 

#ifdef USE_UART0
	   rcu_periph_clock_enable(RCU_USART0);
	   rcu_periph_clock_enable(RCU_GPIOA);
    /* connect port to USARTx_Tx */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    /* connect port to USARTx_Rx */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
#else
	rcu_periph_clock_enable(RCU_USART2);
	rcu_periph_clock_enable(RCU_GPIOB);
	    /* connect port to USARTx_Tx */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    /* connect port to USARTx_Rx */
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
#endif

    /* USART configure */
    usart_deinit(UART_PORT);
    usart_baudrate_set(UART_PORT, BAUD);
	
    usart_receive_config(UART_PORT, USART_RECEIVE_ENABLE);
    usart_transmit_config(UART_PORT, USART_TRANSMIT_ENABLE);
	
	uart_dmainit();
	
    /* USART interrupt configuration */
    nvic_irq_enable(USART2_IRQn, 2, 1);
	usart_interrupt_enable(UART_PORT, USART_INT_IDLE);//���տ����ж�
	
	/* USART DMA enable for transmission and reception */
    usart_dma_transmit_config(UART_PORT, USART_DENT_ENABLE);
    usart_dma_receive_config(UART_PORT, USART_DENR_ENABLE);
    usart_enable(UART_PORT);
	
	RS485_RX_ENABLE(ON) ;
}
/**************************************************************************
*��������  uart_dmainit(void)
*������ 
*���أ�
*˵����
*
*****************************************************************************/

void  uart_dmainit(void)
{
    dma_parameter_struct dma_init_struct;
    /* enable DMA0 */
    rcu_periph_clock_enable(RCU_DMA0);
 
        /* deinitialize DMA channel4 (UART_PORT Rx) */
    dma_deinit(DMA0, UART_RX_DMA_CH);
	dma_struct_para_init(&dma_init_struct);
	dma_channel_disable(DMA0, UART_TX_DMA_CH);//��ֹ 
    dma_channel_disable(DMA0, UART_RX_DMA_CH);//��ֹ
	
  
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr  = (uint32_t)&Rx_buffer;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number       = UART_BUFF_SIZE;
	
    dma_init_struct.periph_addr  = USART_DATA_ADDRESS;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, UART_RX_DMA_CH, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, UART_RX_DMA_CH); 
	
	
	
	
	/* deinitialize DMA channel3(USART0 Tx) */
    dma_deinit(DMA0, UART_TX_DMA_CH);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr  = (uint32_t)Tx_buffer;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number       =  sizeof(Tx_buffer);
    dma_init_struct.periph_addr  = USART_DATA_ADDRESS;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, UART_TX_DMA_CH, &dma_init_struct);
	
	/* enable DMA channel3 */
    dma_channel_disable(DMA0, UART_TX_DMA_CH);//��ֹ����
    /* enable DMA channel4 */
    dma_channel_enable(DMA0, UART_RX_DMA_CH);//ʹ�ܽ���
    usart_dma_receive_config(UART_PORT, USART_DENR_ENABLE);
}
/**************************************************************************
*�������� UART_DMARx_Enable
*������ 
*���أ�
*˵����
*
*****************************************************************************/
void UART_DMARx_Enable()
{
   RS485_RX_ENABLE(ON); 
   dma_channel_disable(DMA0, UART_TX_DMA_CH);//��ֹ����
   dma_channel_disable(DMA0, UART_RX_DMA_CH);//��ֹ����
   dma_transfer_number_config(DMA0,UART_RX_DMA_CH, sizeof(Rx_buffer));
   dma_channel_enable(DMA0, UART_RX_DMA_CH);//��ֹ����	
}


/**************************************************************************
*�������� UART_DMASend
*������ 
*���أ�
*˵����
*
*****************************************************************************/
void UART_DMASend(uint8_t *_ucaBuf, uint16_t _usLen)
{
  uint32_t TimeOutCNT;
  TimeOutCNT = 0;
  RS485_RX_ENABLE(OFF); 
  memcpy(Tx_buffer,_ucaBuf,_usLen);
  dma_transfer_number_config(DMA0,UART_TX_DMA_CH, _usLen);
  dma_channel_enable(DMA0, UART_TX_DMA_CH);
  /* wait DMA Channel transfer complete */
  while((RESET == dma_flag_get(DMA0, UART_TX_DMA_CH, DMA_INTF_FTFIF)) ||(usart_flag_get(UART_PORT, USART_FLAG_TC)==RESET))
  {
	  if(++TimeOutCNT > 500)
	   {
			   break;
	   }
     rt_thread_delay(1); 
  }
  dma_channel_disable(DMA0, UART_TX_DMA_CH);//��ֹ����
  RS485_RX_ENABLE(ON); 
}
///**************************************************************************
//*�������� RS485_SendBuf
//*������ 
//*���أ�
//*˵����
//*
//*****************************************************************************/
//void UART_Send(uint8_t *_ucaBuf, uint16_t _usLen)
//{
//  uint8_t i;
//  uint32_t TimeOutCNT;
//  TimeOutCNT = 0;
//	 RS485_RX_ENABLE(OFF); 
//  for(i=0;i<_usLen;i++)
//   {
//       usart_data_transmit(UART_PORT, *(uint8_t*)(_ucaBuf+i));
//        while (RESET == usart_flag_get(UART_PORT, USART_FLAG_TBE))
//		{
//		   if(++TimeOutCNT > 200)
//		   {
//			   break;
//		   }
//		  rt_thread_delay(1);
//		};
//   }

//   while (RESET == usart_flag_get(UART_PORT, USART_FLAG_TC))
//	{
//	  if(++TimeOutCNT > 200)
//		 {
//			   break;
//		 }
//	    rt_thread_delay(1); 
//	};
//  rt_thread_delay(1); 
//  RS485_RX_ENABLE(ON); 
//}

/**************************************************************************
*��������  USART0_IRQHandler
*������ 
*���أ�
*˵����
*
*****************************************************************************/
 
 void USART0_IRQHandler(void)
{
	uint8_t  u8Tem;
    if(RESET != usart_interrupt_flag_get(UART_PORT, USART_INT_FLAG_IDLE))
	{
	   usart_data_receive(UART_PORT);
	   u8Tem = UART_BUFF_SIZE - dma_transfer_number_get(DMA0, UART_RX_DMA_CH);
       if(UART_CallBack != NULL)
	   {
		   dma_channel_disable(DMA0, UART_RX_DMA_CH);//��ֹ����
		   UART_CallBack((uint8_t *)&Rx_buffer,u8Tem);
	   }
    }
  
	
  //*****************************************/
  // ��������ж�
  //*****************************************/
  if(RESET != usart_flag_get(UART_PORT, USART_FLAG_ORERR))
    {
       usart_flag_clear(UART_PORT,USART_FLAG_ORERR);
    }
}


/**************************************************************************
*��������  USART0_IRQHandler
*������ 
*���أ�
*˵����
*
*****************************************************************************/
 
 void USART2_IRQHandler(void)
{
	uint8_t  u8Tem;
    if(RESET != usart_interrupt_flag_get(UART_PORT, USART_INT_FLAG_IDLE))
	{
	   usart_data_receive(UART_PORT);
	   u8Tem = UART_BUFF_SIZE - dma_transfer_number_get(DMA0, UART_RX_DMA_CH);
       if(UART_CallBack != NULL)
	   {
		   dma_channel_disable(DMA0, UART_RX_DMA_CH);//��ֹ����
		   UART_CallBack((uint8_t *)&Rx_buffer,u8Tem);
	   }
    }
  
	
  //*****************************************/
  // ��������ж�
  //*****************************************/
  if(RESET != usart_flag_get(UART_PORT, USART_FLAG_ORERR))
    {
       usart_flag_clear(UART_PORT,USART_FLAG_ORERR);
    }
}
/**************************************************************************
*��������  USART_ClearErr
*������ 
*���أ�
*˵����
*
*****************************************************************************/
 
 void USART_ClearErr(void)
{
 	
   //*****************************************/
   // ��������ж�
   //*****************************************/
   if(RESET != usart_flag_get(UART_PORT, USART_FLAG_ORERR))
    {
       usart_flag_clear(UART_PORT,USART_FLAG_ORERR);
    }
	
   if(RESET != usart_flag_get(UART_PORT, USART_FLAG_PERR|USART_FLAG_FERR|USART_FLAG_NERR))
    {
       usart_flag_clear(UART_PORT,USART_FLAG_PERR|USART_FLAG_FERR|USART_FLAG_NERR);
    }
}



