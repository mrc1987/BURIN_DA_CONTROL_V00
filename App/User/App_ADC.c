
#include "main.h"
#include "app_adc.h"



#define ADC_AcqFre  10000u

#define ADC_SAM_TIME    ADC_SAMPLETIME_55POINT5  //ADC ����ʱ��

#define ADC_AQN         3  //ADC �ɼ�ͨ������

#define CH_VP     0
#define CH_VN     1
#define CH_VPN    2
 

 
#define Q10_Vol_K   2599  //x100 _Q10
#define Q10_Vol_K2  260  //x100 _Q10
//*****************************************************************//
uint16_t  ADC_Buffer[ADC_AQN];  //DMA �ɼ�������
uint16_t  ADC_offset[ADC_AQN];

ADC_INFO_DEF ADC_Info;//ADC �ɼ���Ϣ

 int32_t Vrms_ADC_Pre;
 int32_t VPrms_ADC_Pre;
 int32_t VNrms_ADC_Pre;
 
 int32_t Vrms_ADC_Filter; //Q12ֵ
 int32_t VPrms_ADC_Filter;//Q12ֵ
 int32_t VNrms_ADC_Filter;//Q12ֵ
//-------------------------------------------------------------------------------
#define N50K_3950

#ifdef N47K_3950
/**************************************************************************************
* 47K B=3950 NTC��ֵ AD ��Ӧֵ
* �˱�ADֵΪ��������=10K �õ���ֵ
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
* 50K B=3950 NTC��ֵ AD ��Ӧֵ
* �˱�ADֵΪ��������=10K �õ���ֵ
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
* Description    :  ��ȡ��Ӧ�¶�ֵ

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
//*�������� ADC_dma_config
//*������   
//*���أ�
//*˵����
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
*�������� adc_config
*������    
*���أ�
*˵����
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
*�������� ADC0_1_IRQHandler(void)
*������   �ж�Ƶ��10KHZ
*���أ�   ʱ��20us
*˵����
*
*****************************************************************************/

void ADC0_1_IRQHandler(void)
{
    int32_t s32tem;
    /* clear the ADC flag */
    adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOIC);
	adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_EOC);
    

	s32tem =(ADC_Buffer[CH_VP] - ADC_Buffer[CH_VN]);
	if(ADC_Info.index < ADC_BUF_SIZE)
	{
		
	   ADC_Info.VDIFF_ADC_BUF[ADC_Info.index] = (int16_t)s32tem;
	   ADC_Info.VP_ADC_BUF[ADC_Info.index]	 = ADC_Buffer[CH_VP];
	   ADC_Info.VN_ADC_BUF[ADC_Info.index]	 = ADC_Buffer[CH_VN];
	   ADC_Info.VPN_ADC_BUF[ADC_Info.index]	 = ADC_Buffer[CH_VPN];	

       ADC_Info.index++;
       ADC_Info.len++;	
       if(ADC_Info.len > ADC_BUF_SIZE )
	   {
         ADC_Info.len = ADC_BUF_SIZE;
	   }		   
	}
	else
	{
		ADC_Info.index = 0;
	}


     Vrms_ADC_Filter  = Vrms_ADC_Filter*4045 + 25*s32tem + 25*Vrms_ADC_Pre;//0.9875120932*4096   0.006243953391*4096=25.5
     VPrms_ADC_Filter = Vrms_ADC_Filter*4045 + 25*ADC_Buffer[CH_VP] + 25*VPrms_ADC_Pre;;
     VNrms_ADC_Filter = Vrms_ADC_Filter*4045 + 25*ADC_Buffer[CH_VN] + 25*VNrms_ADC_Pre;;
	 
	 Vrms_ADC_Pre  = s32tem;
     VPrms_ADC_Pre = ADC_Buffer[CH_VP];
     VNrms_ADC_Pre = ADC_Buffer[CH_VN];
	
 }


/**************************************************************************
*�������� void ADC_Process()
*������ 
*���أ�
*˵����
*
*****************************************************************************/

void ADC_Process()
{
	uint8_t i = 0;
   int32_t i32VP_Sum;
   int32_t i32VN_Sum;
   int32_t i32VPN_Sum;
   int32_t i32Vdiff_Sum;
   int32_t s32tem;

    i32VP_Sum = 0;
    i32VN_Sum = 0;;
    i32VPN_Sum = 0;
    i32Vdiff_Sum = 0;
	
 
   for(i = 0; i<  ADC_BUF_SIZE ; i++)
	{
		
	    i32VP_Sum  += ADC_Info.VP_ADC_BUF[i];
        i32VN_Sum  += ADC_Info.VN_ADC_BUF[i];
        i32VPN_Sum += ADC_Info.VPN_ADC_BUF[i];
        i32Vdiff_Sum += ADC_Info.VDIFF_ADC_BUF[i];	
		
	}


	ADC_Info.VP_ADC_Fliter     = i32VP_Sum>>6; 
	ADC_Info.VN_ADC_Fliter     = i32VN_Sum>>6; 
	ADC_Info.VPN_ADC_Filter    = i32VPN_Sum>>6; 
	ADC_Info.Vdiff_ADC_Filter  = i32Vdiff_Sum>>6;  
	
  /**********************VRMS**********************************/
//	s32tem = (Q10_Vol_K*ADC_Info.Vdiff_ADC_Filter)>>10;
//	s32tem = (s32tem*gEEP_data.DMM_V_K) + gEEP_data.DMM_V_B;

//	//if(s32tem < 0) s32tem = -s32tem;
//    ADC_Info.DMM_VRMS = (int16_t)(s32tem>>10);

  /**********************VPRMS****************************/
	s32tem = (Q10_Vol_K*ADC_Info.VP_ADC_Fliter)>>10;
    ADC_Info.DMM_VP_RMS = (int16_t)(s32tem>>0);

  /*********************VNRMS**********************************/
	s32tem = (Q10_Vol_K*ADC_Info.VN_ADC_Fliter)>>10;
    ADC_Info.DMM_VN_RMS = (int16_t)(s32tem>>0);

  /*********************VPNRMS**********************************/
	s32tem = (Q10_Vol_K2*ADC_Info.VPN_ADC_Filter)>>10;
	s32tem = (s32tem*gEEP_data.DMM_V2_K) + gEEP_data.DMM_V2_B;
    ADC_Info.DMM_VPN_RMS = (int16_t)(s32tem>>10);
	
    /**********************VRMS**********************************/
	s32tem = ADC_Info.DMM_VP_RMS - ADC_Info.DMM_VN_RMS;
	s32tem = (s32tem*gEEP_data.DMM_V_K) + gEEP_data.DMM_V_B;

    s32tem = (s32tem>>10);
    ADC_Info.DMM_VRMS = (ADC_Info.DMM_VRMS*3 + s32tem)>>2;
	
	/******************************************************************/
   
	s32tem = (Q10_Vol_K*(Vrms_ADC_Filter>>12))>>10;
 	s32tem = (s32tem*gEEP_data.DMM_V_K) + gEEP_data.DMM_V_B;
	ADC_Info.DMM_VRMS2 = (int16_t)(s32tem>>10);
}



