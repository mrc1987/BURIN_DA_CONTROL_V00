#ifndef __APP_SPI_H
#define __APP_SPI_H
#include "main.h"
 


#define	SPI0_CS(n)			 gpio_bit_write(GPIOC,GPIO_PIN_4,(bit_status)(n));
#define	SPI2_CS(n)		     gpio_bit_write(GPIOB,GPIO_PIN_6,(bit_status)(n));

 
 



void SPI_Init(void);
 void SPI_CS(uint32_t SPIx,uint8_t val);
uint8_t SPI_OnlySendByte(uint32_t SPIx,uint8_t byte);
uint8_t SPI_ReceiveByte(uint32_t SPIx);
#endif

