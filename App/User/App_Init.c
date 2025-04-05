#include "main.h"





 



 
 
 ST_WorkSpace_def WorkSpace; //工作空间数据	
 //*****************************************************



 
 
 extern void InitEEP(void);
/**************************************************************************
*函数名： Init_RCC
*描述：   外设时钟初始化
*返回：
*说明：
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
*函数名： APP_gpio_Init
*描述：   IO初始化
*返回：
*说明：
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
*函数名： APP_gpio_Init
*描述：   IO初始化
*返回：
*说明：
*
*****************************************************************************/
void APP_Init(void)
{
    __disable_irq();
	nvic_vector_table_set(NVIC_VECTTAB_FLASH, AppFlashBase);//重定义向量表
	__enable_irq();

    Init_RCC();//初始化时钟
	APP_GPIO_Init();//初始化IO
	bsp_InitMsg();//初始化消息队列
	adc_config();//初始化ADC
 
    bsp_InitI2C();
	InitEEP();
	Read_EEP();
	drv_Timer_Init();

	//Flash_EEP_Read();
}






/**************************************************************************
*函数名： Flash_EEP_Write
*描述：   
*返回：
*说明：
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
*函数名： Flash_EEP_Read
*描述：   
*返回：
*说明：
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




 


