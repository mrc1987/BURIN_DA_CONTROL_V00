#ifndef __APP_UART_H
#define __APP_UART_H
#include "main.h"
 
extern WordBitArea  RS485_Status;

#define bRS485_Timeout_Flag  RS485_Status.Bit.b0   
#define bRS485_RX_Flag       RS485_Status.Bit.b1   
#define bRS485_TxBusy        RS485_Status.Bit.b2  
#define bRS485_Timeout_Flag  RS485_Status.Bit.b0   
#define bRS485_Timeout_Flag  RS485_Status.Bit.b0   


void PC_UART_thread_entry(void *parameter);
#endif

