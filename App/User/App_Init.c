#include "main.h"





 



 
 
 ST_WorkSpace_def WorkSpace; //�����ռ�����	
 //*****************************************************



 
 
 extern void InitEEP(void);
/**************************************************************************
*�������� Init_RCC
*������   ����ʱ�ӳ�ʼ��
*���أ�
*˵����
*
*****************************************************************************/
void Init_RCC()
{

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
 
    rcu_periph_clock_enable(RCU_ADC0);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_DMA0);  

    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV4);
}
	
	
/**************************************************************************
*�������� APP_gpio_Init
*������   IO��ʼ��
*���أ�
*˵����
*
*****************************************************************************/
void APP_GPIO_Init(void)
{
    
	//=============ADC/DAC CONFIG==========================================================================
   // gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2 ); 
	

	//==============GPIO INPUT===========================================================================================
 
    gpio_init(GPIOA, GPIO_MODE_IPD, GPIO_OSPEED_MAX, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_15); 
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,  GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15); 
	//==================gpio out=====================================================================================
	
	
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,  GPIO_PIN_2|GPIO_PIN_11); 

	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,  GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15); 
	
 	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,  GPIO_PIN_13 ); 

	gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	
 /*******************************************************************************************/
 
}



/**************************************************************************
*�������� APP_gpio_Init
*������   IO��ʼ��
*���أ�
*˵����
*
*****************************************************************************/
void APP_Init(void)
{
    __disable_irq();
	nvic_vector_table_set(NVIC_VECTTAB_FLASH, AppFlashBase);//�ض���������
	__enable_irq();

    Init_RCC();//��ʼ��ʱ��
	APP_GPIO_Init();//��ʼ��IO
	bsp_InitMsg();//��ʼ����Ϣ����
	adc_config();//��ʼ��ADC
 
    bsp_InitI2C();
	InitEEP();
	Read_EEP();
	drv_Timer_Init();

	//Flash_EEP_Read();
}






/**************************************************************************
*�������� Flash_EEP_Write
*������   
*���أ�
*˵����
*****************************************************************************/
void Flash_EEP_Write()
{
	uint32_t s32tem = 0,i,addr;
	uint32_t *pValue;
    fmc_unlock();	
	
	addr = 0;
	pValue = (uint32_t *)&gEEP_data;
	    __disable_irq();
	if(FMC_READY == fmc_page_erase(AppFlashEEPBase))
	{
	   for(i = 0;  i < sizeof(gEEP_data);i++)
		{
	      if(FMC_READY != fmc_word_program(AppFlashEEPBase + i,*pValue))
		   {
		 	  return;
		   }
		  i += 4;	
		}
	}
	
  fmc_lock();	
  __enable_irq();
}

/**************************************************************************
*�������� Flash_EEP_Read
*������   
*���أ�
*˵����
*****************************************************************************/
void Flash_EEP_Read()
{
	uint32_t s32tem = 0,i;
	uint8_t *pValue;
 	
	
	i = 0;
	pValue = (uint8_t *)&gEEP_data;
 
	for(i = 0; i< sizeof(gEEP_data);i++)
	{
	   *pValue = *(uint8_t *)(AppFlashEEPBase + i );
	}
 
 
}




 


