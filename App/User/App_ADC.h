#ifndef __APP_ADC_H
#define __APP_ADC_H
#include "main.h"
 
 #define ADC_BUF_SIZE 64
 
 //ADC 采集数据结构
 typedef struct
 {
	uint16_t index;
    uint16_t len; 
    
	int16_t  VDIFF_ADC_BUF[ADC_BUF_SIZE];
	uint16_t VP_ADC_BUF[ADC_BUF_SIZE];
	uint16_t VN_ADC_BUF[ADC_BUF_SIZE];
	uint16_t VPN_ADC_BUF[ADC_BUF_SIZE];
	 
	int16_t VP_ADC_Fliter; 
	int16_t VN_ADC_Fliter; 
	int16_t VPN_ADC_Filter;
	int16_t Vdiff_ADC_Filter;	 
	 
	int16_t DMM_VP_RMS; 
	int16_t DMM_VN_RMS; 
	int16_t DMM_VPN_RMS;
	 
	int16_t DMM_VRMS; 
	 
	int16_t DMM_VRMS2; 

 } ADC_INFO_DEF;
  
 extern ADC_INFO_DEF ADC_Info;//ADC 采集信息
#endif

