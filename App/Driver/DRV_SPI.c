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

 

#include "DRV_SPI.h"
#include "gd32f30x.h"


 

 #define LCD_SPI       SPI1
 #define LCD_SPI_RCU   RCU_SPI1
 #define Timeout 0x500
/**************************************************************************
*�������� CAN_gpio_config(void)
*������  
*���أ�
*˵����
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
*�������� SPI_Init
*������ SPI ��ʼ��
*���أ�
*˵����
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
*�������� SPI_OnlySendByte
*������ ���� 
*���أ�
*˵����
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
*�������� SPI_OnlySendByte
*������ ���� 
*���أ�
*˵����
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
*�������� SPI_ReceiveByte
*������   ����
*���أ�
*˵����
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

 
