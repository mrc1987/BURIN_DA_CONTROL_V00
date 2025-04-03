
/******************************************************************************
* @file     includes.h
* @author   MRC
* @version  V0.0.0
* @date     2020-12-14
* @brief    
******************************************************************************/
#include "main.h"
#include "app_tim.h"

U16 DAC_T1_Period = 0;  

U16 DAC1_pwmout_Period = 0;  
U16 DAC1_pwmout_pluse = 0;  

U16 DAC2_pwmout_Period = 0;  
U16 DAC2_pwmout_pluse = 0;  

#define Timer_Clock      100000U //ADC采集频率20K
/**************************************************************************
*函数名： drv_Timer_Init
*描述：   初始化定时器
*返回：
*说明：

    TIMER1 Configuration: 
    TIMER1CLK = SystemCoreClock / 12000 = 10K,

*****************************************************************************/
void drv_Timer_Init(void)
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
   timer_initpara.period            = 0xfff;
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
		timer_initpara.period            = 0x0fff;
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
	
    /**************** TIMER1   ************************************/
	
	DAC_T1_Period = SystemCoreClock / DAC_T1_PWM_FRE;
    timer_initpara.prescaler         = 0;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = DAC_T1_Period-1;;
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


	timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_0, 0);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);


    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1,0);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_1, TIMER_OC_MODE_PWM0);
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
	
    //  timer_channel_output_state_config(TIMER1, TIMER_CH_0, TIMER_CCX_DISABLE);
    //  timer_channel_output_state_config(TIMER1, TIMER_CH_1, TIMER_CCX_DISABLE);
	/****************************************************** */

	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX,  GPIO_PIN_0 ); //T1-CH0
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX,  GPIO_PIN_1 ); //T1-CH1
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX,  GPIO_PIN_5 ); //T2-CH1
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX,  GPIO_PIN_6 ); //T3-CH0
}
/**************************************************************************
*函数名： DA_SetValue
*描述：   
*返回：
*说明：
*****************************************************************************/
void DA_SetValue(U8 CH, U16 q10_Duty)
{
  u32 Tem = (DAC_T1_Period * q10_Duty) >> 10;
  if(Tem > DAC_T1_Period)
  {
      Tem = DAC_T1_Period + 1;
  } 
  if(CH == 0)
  {
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_0, Tem);
  }
  else
  {
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, Tem);
  }
}


/**************************************************************************
*函数名：  DA_PWM_OUT
*描述：    SetFre =[0-10KHz]  q10_Duty = [0-1000]
*返回：
*说明：
*****************************************************************************/
void DA_PWM_OUT(U8 CH, U16 SetFre,U16 q10_Duty)
{
  U32 Tem = 0;
  U32 Prescaler = 0;
  U32 u32_Timer_Clock = 0;

  if( SetFre > 10000)
  {
      Prescaler = 0 ;//[10KHz-20KHz]  Clocke = 120MHZ
 
  }
 else if( SetFre > 1000)
  {
      Prescaler =  12-1;//[1KHz-10KHz]   Clocke = 10MHZ
 
  }
  else if( SetFre > 100)
  {
      Prescaler = 120-1 ;//[100Hz-1KHz]  Clocke = 1MHZ
 
  }
  else 
  {
      Prescaler = 12000-1 ;//[0Hz-100Hz]  Clocke = 10KHZ
 
  }

  if(CH == 0)
  {
 
    DAC1_pwmout_Period = SystemCoreClock / (Prescaler + 1);   
    timer_prescaler_config(TIMER2, Prescaler, TIMER_PSC_RELOAD_UPDATE);
    Tem = (DAC1_pwmout_Period * q10_Duty) >> 10;
    if(Tem > DAC1_pwmout_Period)
    {
        Tem = DAC1_pwmout_Period + 1;
    } 
    timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_1, Tem);
  }
  else
  {
    DAC2_pwmout_Period = SystemCoreClock / (Prescaler + 1);   
    timer_prescaler_config(TIMER3, Prescaler, TIMER_PSC_RELOAD_UPDATE);

    Tem = (DAC2_pwmout_Period * q10_Duty) >> 10;
    if(Tem > DAC2_pwmout_Period)
    {
        Tem = DAC2_pwmout_Period + 1;
    } 
    timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_0, Tem);
  }
}


/**************************************************************************
*函数名： DA_ENABLE
*描述：    
*返回：
*说明：
*****************************************************************************/
void DA_ENABLE(U8 CH, U8 State)
{
    if(CH == 0)
    {
        if(State == DAC_OUT_PWM)
        {
            gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX,  GPIO_PIN_5 ); //T2-CH1
            timer_channel_output_state_config(TIMER2, TIMER_CH_1, TIMER_CCX_ENABLE);
        }
        else if(State == DAC_OUT_HIGH)
        {
            //timer_channel_output_state_config(TIMER2, TIMER_CH_1, TIMER_CCX_ENABLE);
            gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,  GPIO_PIN_5 ); //T2-CH1
            gpio_bit_write(GPIOB,GPIO_PIN_5,(bit_status)0);
        }
        else
        {
            // timer_channel_output_state_config(TIMER2, TIMER_CH_1, TIMER_CCX_DISABLE);
            // timer_channel_output_pulse_value_config(TIMER2,TIMER_CH_1, 0);
            gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,  GPIO_PIN_5 ); //T2-CH1
            gpio_bit_write(GPIOB,GPIO_PIN_5,(bit_status)1);
        }
    }
    else
    {
        if(State == DAC_OUT_PWM)
        {
            gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX,  GPIO_PIN_6 ); //T3-CH0
            timer_channel_output_state_config(TIMER3, TIMER_CH_0, TIMER_CCX_ENABLE);
        }
        else if(State == DAC_OUT_HIGH)
        {
            //timer_channel_output_state_config(TIMER3, TIMER_CH_0, TIMER_CCX_ENABLE);
            gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,  GPIO_PIN_6 ); //T3-CH0
            gpio_bit_write(GPIOB,GPIO_PIN_6,(bit_status)0);
        }
        else
        {
            // timer_channel_output_state_config(TIMER3, TIMER_CH_0, TIMER_CCX_DISABLE);
            // timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_0, 0);
            gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,  GPIO_PIN_6 ); //T3-CH0
            gpio_bit_write(GPIOB,GPIO_PIN_6,(bit_status)1);
        }
    }
 
}





 

