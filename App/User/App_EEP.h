#ifndef __APP_EEP_H
#define __APP_EEP_H

 #include "main.h"
 

 void InitEEP(void);
 uint8_t Read_EEP(void);
 uint8_t  EEP_Save(void);
 void  EEP_SaveTask(void);
#endif

