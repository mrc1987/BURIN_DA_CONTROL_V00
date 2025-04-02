#include "main.h"

 
 
 #define Timeout 0x2000
/**************************************************************************
*函数名： CAN_gpio_config(void)
*描述：  
*返回：
*说明：
*
*****************************************************************************/
void SPI_gpio_config(void)
{
	   rcu_periph_clock_enable(RCU_GPIOA);
     rcu_periph_clock_enable(RCU_GPIOB);
	 rcu_periph_clock_enable(RCU_GPIOC);
    /* SPI0 GPIO config:SCK/PA5, MISO/PA6, MOSI/PA7 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    /* PA3 as NSS */
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    
 
    /* SPI2 GPIO config:  SCK/PB3, MISO/PB4, MOSI/PB5 */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_5);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
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
	
 

    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(RCU_SPI2);
    rcu_periph_clock_enable(RCU_AF);
	


    SPI_gpio_config();
 
 
    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_64;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);
	
	
	
	/* SPI2 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_64;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI2, &spi_init_struct);
	
	
	#ifdef SPI_DMA
	
    /* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_16;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

	
	SET_SPI0_NSS_HIGH;
	
	    /* NVIC config */
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(SPI0_IRQn,1,1);
 
	spi_i2s_interrupt_enable(SPI0, SPI_I2S_INT_RBNE);

    /* SPI ti mode enable */
 
    spi_ti_mode_enable(SPI0);

  #endif
    /* SPI enable */
 
    spi_enable(SPI0);
    spi_enable(SPI2);
    /* DMA channel enable */
    //dma_channel_enable(DMA0, DMA_CH1);

	
    

}

 
/**************************************************************************
*函数名： SPI_CS 
*描述： 
*返回：
*说明：
*
*****************************************************************************/
void SPI_CS(uint32_t SPIx,uint8_t val)
{
   if(SPIx==SPI0)
   {
	   if(val==0)
		   SPI0_CS(0)
	    else  
			SPI0_CS(1)
   }
 
   if(SPIx==SPI2)
   {
	   if(val==0)
		 SPI2_CS(0)
	   else  
		 SPI2_CS(1)	   
   }
   
}
 
/**************************************************************************
*函数名： SPI_SendByte
*描述： 发送 
*返回：
*说明：
*
*****************************************************************************/
uint8_t SPI_SendByte(uint32_t SPIx,uint8_t byte)
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

 
