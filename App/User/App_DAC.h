#ifndef __APP_DAC_H
#define __APP_DAC_H

 #include "main.h"
 


 #define DAC1_CH    0
 #define DAC2_CH    1

 #define OFF    0
 #define ON     1

 #define DA_OUT_PWM_Mode    1
 #define DA_OUT_LEVEL_Mode  0

 #define DA_SLOW_RISE_Mode    1
 #define DA_FAST_RISE_Mode    0

void DAC_Process(void);
#endif

