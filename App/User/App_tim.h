#ifndef __APP_TIM_H
#define __APP_TIM_H
#include "main.h"
 

#define DAC_T1_PWM_FRE      20000U //ADC²É¼¯ÆµÂÊ20K

#define DAC_OUT_OFF    0
#define DAC_OUT_HIGH   1
#define DAC_OUT_PWM    2

 
void DA_SetValue(U8 CH, U16 q10_Duty);
void DA_PWM_OUT(U8 CH, U16 SetFre,U16 q10_Duty);
void DA_ENABLE(U8 CH, U8 State);
void drv_Timer_Init(void);
#endif
