
#include "main.h"
#include "app_adc.h"



#define ADC_AcqFre  10000u

#define ADC_SAM_TIME    ADC_SAMPLETIME_55POINT5  //ADC 采样时间

#define ADC_AQN         3  //ADC 采集通道数量

#define CH_VP     0
#define CH_VN     1
#define CH_VPN    2
 

 
#define Q10_Vol_K   2599  //x100 _Q10
#define Q10_Vol_K2  260  //x100 _Q10
//*****************************************************************//
uint16_t  ADC_Buffer[ADC_AQN];  //DMA 采集缓冲区
uint16_t  ADC_offset[ADC_AQN];

ADC_INFO_DEF ADC_Info;//ADC 采集信息

 int32_t Vrms_ADC_Pre;
 int32_t VPrms_ADC_Pre;
 int32_t VNrms_ADC_Pre;
 
 int32_t Vrms_ADC_Filter; //Q12值
 int32_t VPrms_ADC_Filter;//Q12值
 int32_t VNrms_ADC_Filter;//Q12值
//-------------------------------------------------------------------------------
#define N50K_3950

#ifdef N47K_3950
/**************************************************************************************
* 47K B=3950 NTC阻值 AD 对应值
* 此表AD值为上拉电阻=10K 得到的值
* 
**************************************************************************************/
const uint16_t NTC_ROM[14][2]={
{3844,0},   
{3700,10},   
{3499,20},   
{3239,30},   
{2922,40},    
{2571,50},   
{2199,60},    
{1839,70},     
{1506,80},     
{1215,90},    
{969,100},    
{765,110},     
{610,120},     
{500,130},
};
#endif 


#ifdef N50K_3950
/**************************************************************************************
* 50K B=3950 NTC阻值 AD 对应值
* 此表AD值为上拉电阻=10K 得到的值
* 
**************************************************************************************/
const uint16_t NTC_ROM[14][2]={
{3857,0},   
{3721,10},   
{3508,20},   
{3281,30},   
{2975,40},    
{2627,50},   
{2259,60},    
{1893,70},     
{1553,80},     
{1255,90},    
{1002,100},    
{790,110},     
{624,120},     
{490,130},
};
#endif 

/**************************************************************************************
* FunctionName   :  GetTemPerature
* Description    :  获取对应温度值

* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
uint8_t GetTemPerature(uint16_t AD)
{

 	uint8_t StatT=0,EndT =0, i=0;
 	uint16_t  T=0 ,SADC=0,EADC=0;
	
    if(AD > 4090)   return 0;
    if(AD < 10  )  return 199;
	for(i = 0; i < 14; i++)
	  {
         if(AD <= NTC_ROM[i][0])
		 {
			StatT = NTC_ROM[i][1];
			SADC = NTC_ROM[i][0];
		 }
		 else  if(AD > NTC_ROM[i][0])
		 {
			EndT = NTC_ROM[i][1];
			EADC = NTC_ROM[i][0];
			break;
		 }
      }
    if((EndT == 0) && (StatT==0)) { return 0; }
	else    if((EndT == 0) && (StatT > 0)){ return 199;}
	else
	{
		T= StatT + __fabs(((AD-SADC)*(EndT-StatT))/(EADC-SADC));
		
		
	}
    return (uint8_t)(T);
 } 	
 

///**************************************************************************
//*函数名： ADC_dma_config
//*描述：   
//*返回：
//*说明：
//*
//*****************************************************************************/

void ADC_dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;
    
    /* ADC DMA_channel configuration */
    dma_deinit(DMA0, DMA_CH0);
    
    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr  = (uint32_t)(&ADC_Buffer);
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;  
    dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number       = ADC_AQN;
    dma_data_parameter.priority     = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_data_parameter);

    dma_circulation_enable(DMA0, DMA_CH0);
  
 
    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);
}

/**************************************************************************
*函数名： adc_config
*描述：    
*返回：
*说明：
*
*****************************************************************************/
void adc_config(void)
{
	
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_ADC0);
    rcu_periph_clock_enable(RCU_DMA0);  
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);

    
	
	ADC_dma_config();

	 /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE); 
 

	
    /* ADC scan function enable */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
 

    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
  

 	/* ADC DMA function enable */
    adc_dma_mode_enable(ADC0);
    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, ADC_AQN);
 
	/* ADC regular channel config */
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_0, ADC_SAM_TIME);
    adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_1, ADC_SAM_TIME);
    adc_regular_channel_config(ADC0, 2, ADC_CHANNEL_2, ADC_SAM_TIME);
//    adc_regular_channel_config(ADC0, 3, ADC_CHANNEL_3, ADC_SAM_TIME);
//    adc_regular_channel_config(ADC0, 4, ADC_CHANNEL_6, ADC_SAM_TIME);
 
 
	
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_T1_CH1); 
    /* ADC external trigger enable */
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
	
 
    /* clear the ADC flag */
    adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOC);
 
    /* enable ADC interrupt */
    adc_interrupt_enable(ADC0, ADC_INT_EOC);

    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(ADC0_1_IRQn, 1, 1);

	
    /* enable ADC interface */
    adc_enable(ADC0);
	
    delay_us(1000);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);

 
	
}


/**************************************************************************
*函数名： ADC0_1_IRQHandler(void)
*描述：   中断频率10KHZ
*返回：   时间20us
*说明：
*
*****************************************************************************/

void ADC0_1_IRQHandler(void)
{
    int32_t s32tem;
    /* clear the ADC flag */
    adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOIC);
	adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOC);
    

	
 }


/**************************************************************************
*函数名： void ADC_Process()
*描述： 
*返回：
*说明：
*
*****************************************************************************/

void ADC_Process()
{

}



