/*
*********************************************************************************************************
*
*	ģ������ : MCU DAC����ģ��
*	�ļ����� : DRV_DAC.c
*	��    �� : V1.0
*	˵    �� :  
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		
*
*
*********************************************************************************************************
*/
 

#include "DRV_DAC.h"
#include "gd32f30x.h"



/**************************************************************************
*�������� dac_config
*������   DAC��ʼ��
*���أ�
*˵����
*
*****************************************************************************/
void dac_config(void)
{

    rcu_periph_clock_enable(RCU_DAC);
	
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_DAC);
	gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5);
    dac_deinit();
    /* configure the DAC0 */
    dac_trigger_disable(DAC0);
    dac_wave_mode_config(DAC0, DAC_WAVE_DISABLE);
    dac_output_buffer_enable(DAC0);
    
    /* configure the DAC1 */
    dac_trigger_disable(DAC1);
    dac_wave_mode_config(DAC1, DAC_WAVE_DISABLE);
    dac_output_buffer_enable(DAC1);
    
    /* enable DAC concurrent mode and set data */
    dac_concurrent_enable();
    dac_concurrent_data_set(DAC_ALIGN_12B_R, 466, 3103);//Ĭ�����2.5V DAC0=0.2V DAC1=0.2V  
	//1862=20A  2327=25A  28A=2607  30A=2973  466=5A  19A=2700  10A=931  29.5A=2746
}

/**************************************************************************
*�������� DAC_SET_VOL
*������   ���õ�ѹDAC
*���أ�
*˵����
*
*****************************************************************************/
void DAC_SET_VOL(uint16_t V)
{
 
  dac_data_set(DAC1,DAC_ALIGN_12B_R, V);
}

/**************************************************************************
*�������� DAC_SET_CURR
*������   ���õ���DAC
*���أ�
*˵����
*
*****************************************************************************/
void DAC_SET_CURR(uint16_t I)
{
	
  dac_data_set(DAC0,DAC_ALIGN_12B_R, I);
}


