#ifndef __APP_DAC_H
#define __APP_DAC_H

 #include "main.h"
 

extern   uint16_t u16SetIout_Ref; //设置输出电流
extern   uint16_t u16SetVout_Ref; //设置输出电压

void DAC_Process(void);
#endif

