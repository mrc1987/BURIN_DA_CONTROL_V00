#include "main.h"





 
#define DAC_T1_PWM_FRE      20000U //ADC采集频率20K


 
 
 ST_WorkSpace_def WorkSpace; //工作空间数据	
 //*****************************************************
ST_EEPStruct       gEEP_data;             //EEP 保存数据


 void Timer_config(void);
 
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
 
 
	InitEEP();
 	Timer_config();//初始化Tiemr

	Flash_EEP_Read();
}




/**************************************************************************
*函数名： timer_config
*描述：   初始化定时器
*返回：
*说明：

    TIMER1 Configuration: 
    TIMER1CLK = SystemCoreClock / 12000 = 10K,

*****************************************************************************/
void Timer_config(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;
    uint32_t u32Tem;
	
	
	rcu_periph_clock_enable(RCU_TIMER1);
	rcu_periph_clock_enable(RCU_TIMER2 );
	rcu_periph_clock_enable(RCU_TIMER3 );
	
	
 
 
	/************ TIMER3 configuration *************************/
   timer_initpara.prescaler         = 120-1;
   timer_initpara.alignedmode       = TIMER_COUNTER_EDGE; //1-10KHz	  
   timer_initpara.counterdirection  = TIMER_COUNTER_UP;
   timer_initpara.period            = 1000;
   timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
   timer_initpara.repetitioncounter = 0;
   timer_init(TIMER3,&timer_initpara);
	
   /* CH0 configuration in OC active mode */
   timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
   timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
   timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
   timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
   timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
   timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

   timer_channel_output_config(TIMER3,TIMER_CH_0,&timer_ocintpara);
  

   /* CH0 configuration in OC active mode */
   timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_0, 0);
   timer_channel_output_mode_config(TIMER3,TIMER_CH_0,TIMER_OC_MODE_PWM0);
   timer_channel_output_shadow_config(TIMER3,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);





 

   /**************** TIMER2 IO快速中的检测************************************/
 

		timer_initpara.prescaler         = 120-1;
		timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
		timer_initpara.counterdirection  = TIMER_COUNTER_UP;
		timer_initpara.period            = 1000;
		timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
		timer_initpara.repetitioncounter = 0;
		timer_init(TIMER2,&timer_initpara);

	   /* CH0 configuration in OC active mode */
	   timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
	   timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
	   timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
	   timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
	   timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
	   timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
	
	   timer_channel_output_config(TIMER2,TIMER_CH_1,&timer_ocintpara);
	  
	
	   /* CH0 configuration in OC active mode */
	   timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_1, 0);
	   timer_channel_output_mode_config(TIMER2,TIMER_CH_1,TIMER_OC_MODE_PWM0);
	   timer_channel_output_shadow_config(TIMER2,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);
	
    /**************** TIMER1   触发ADC 采样************************************/
	
	u32Tem = SystemCoreClock / DAC_T1_PWM_FRE;
    timer_initpara.prescaler         = 0;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = u32Tem-1;;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);



    /* CH0 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER1, TIMER_CH_0, &timer_ocintpara);

	timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER1, TIMER_CH_1, &timer_ocintpara);


	timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_0, u32Tem>>1);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_0, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);


    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, u32Tem>>1);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_1, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);
	 
	
    /******************************************************************************************/
    timer_auto_reload_shadow_enable(TIMER1);
 	timer_auto_reload_shadow_enable(TIMER2);
 	timer_auto_reload_shadow_enable(TIMER3);
//	
//	/***************TIMER 2 中断配置************************************/
//	nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
//    nvic_irq_enable(TIMER2_IRQn, 1, 1);
//	
//    timer_interrupt_flag_clear(TIMER2,TIMER_INT_FLAG_UP);
//    timer_interrupt_enable(TIMER2,TIMER_INT_UP);

    /*************************************************************/
    timer_enable(TIMER1);
 	timer_enable(TIMER2);
 	timer_enable(TIMER3);
	
 
	/****************************************************** */

	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX,  GPIO_PIN_0 ); 
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX,  GPIO_PIN_1 ); 
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX,  GPIO_PIN_5 ); 
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX,  GPIO_PIN_6 ); 
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


