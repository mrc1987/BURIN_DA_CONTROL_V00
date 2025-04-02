/*
*********************************************************************************************************
*
*	模块名称 : I2C总线驱动模块
*	文件名称 : bsp_i2c_gpio.c
*	版    本 : V1.0
*	说    明 : 用gpio模拟i2c总线。该模块不包括应用层命令帧，仅包括I2C总线基本操作函数。
*
*	修改记录 :
*		版本号  日期        作者     说明
*		
*
*
*********************************************************************************************************
*/

 

#include "DRV_SPI.h"
#include "gd32f30x.h"


 

 #define LCD_SPI       SPI1
 #define LCD_SPI_RCU   RCU_SPI1
 #define Timeout 0x500
/**************************************************************************
*函数名： CAN_gpio_config(void)
*描述：  
*返回：
*说明：
*
*****************************************************************************/
void SPI_GPIO_Config(void)
{
 
  rcu_periph_clock_enable(RCU_GPIOB);
 
  
  /* SPI0 GPIO config:SCK/PB13,MOSI/PB15 */
  gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15);
 
  /* PB14  DC */
  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
}



/**************************************************************************
*函数名： SPI_Init
*描述： SPI 初始化
*返回：
*说明：
*
*****************************************************************************/
void SPI_Init(void)
{

   spi_parameter_struct  spi_init_struct;
	
 

    rcu_periph_clock_enable(LCD_SPI_RCU);
    rcu_periph_clock_enable(RCU_AF);
	


    SPI_GPIO_Config();
 
 
    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_BDTRANSMIT;//SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_128;    //120/64=1.875M
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(LCD_SPI, &spi_init_struct);
	
	
	
 
	
	
	#ifdef SPI_DMA
	
    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_16;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(LCD_SPI, &spi_init_struct);

	
	SET_SPI0_NSS_HIGH;
	
	    /* NVIC config */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(SPI0_IRQn,1,1);
 
	spi_i2s_interrupt_enable(SPI0, SPI_I2S_INT_RBNE);

    /* SPI ti mode enable */
 
    spi_ti_mode_enable(SPI0);

  #endif
    /* SPI enable */
 
    spi_enable(LCD_SPI);
 
}


/**************************************************************************
*函数名： SPI_OnlySendByte
*描述： 发送 
*返回：
*说明：
*
*****************************************************************************/
uint8_t SPI_OnlySendByte(uint32_t SPIx,uint8_t byte)
{
	uint8_t Re;
	uint32_t T=0;


    spi_i2s_data_transmit(SPIx, byte);
     while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_TBE))
	{
		if(++T > 1000)return 0;
	} 
	return Re;
	
}
 
 
/**************************************************************************
*函数名： SPI_OnlySendByte
*描述： 发送 
*返回：
*说明：
*
*****************************************************************************/
uint8_t SPI_SendByte2(uint32_t SPIx,uint8_t byte)
{
	uint8_t Re;
	uint32_t T=0;
 	//WIZ_CS_ENABLE
    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_TBE))
	{
		if(++T > Timeout)return 0;
	}
    spi_i2s_data_transmit(SPIx, byte);
	T=0;
    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_RBNE))
	{
		if(++T > Timeout)return 0;
	}
     Re = spi_i2s_data_receive(SPIx);
    
	return Re;
	
}


/**************************************************************************
*函数名： SPI_ReceiveByte
*描述：   接收
*返回：
*说明：
*
*****************************************************************************/
uint8_t SPI_ReceiveByte(uint32_t SPIx)
{	uint32_t T=0;
	uint8_t Re;
 	//WIZ_CS_ENABLE
    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_TBE))
	{
		if(++T > Timeout)return 0;
	}
    spi_i2s_data_transmit(SPIx,0xFF);
	
	T=0;
    while(RESET == spi_i2s_flag_get(SPIx, SPI_FLAG_RBNE))
	{
		if(++T > Timeout)return 0;
	}
     Re = spi_i2s_data_receive(SPIx);
    
	return Re;
}

 
