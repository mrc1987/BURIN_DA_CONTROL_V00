 

#ifndef MAIN_H
#define MAIN_H

 //-----------------------ͷ�ļ�����--------------------------------------------
#include "gd32f30x.h"
#include "systick.h"
#include <rtthread.h>
#include <string.h>
 
#include "app_spi.h"
#include "SEGGER_RTT.h"
 
#include "datatype.h"
#include "app_lib.h"

#include "drv_dac.h"
#include "bsp_msg.h"
#include "app_uart.h"
#include "APP_DAC.h"
#include "app_adc.h"
#include "app_eep.h"
#include "app_init.h"
//#include "Drv_lcd.h"

// #include "lcd_init.h"
// #include "lcd.h"
// #include "app_lcd.h"
#include "app_tim.h"

#include "drv_i2c_gpio.h"
#include "DRV_EEPROM_24XX.h"

 
//**********************************************************
// BootLoader Flash ��Ϣ��
//GD32F305 RB 128K  RC=256K

#define  FlashSize      ((uint32_t)0x40000U)  //256K

#define  AppFlashBase   ((uint32_t)0x8000000U )
#define  BootFlashSize  ((uint32_t)0x4000U)  //16K
#define  AppFlashSize   ((uint32_t)(0x40000-BootFlashSize))

#define  FlashPageSize ((uint16_t)0x800U)//ҳ��С 2K

#define  AppFlashEEPBase   ((uint32_t)0x8000000U +0x1F800)

#define  AppPageNum   120  //App ����ҳ��

#define  GetPageNum(N)  ((N-AppFlashBase)/FlashPageSize)

 
//-----------------------�궨��--------------------------------------------
#define ON  1
#define OFF 0

#define True  1
#define False 0
//*************************************************************
#define FIREWARE_VER   0x0001    //����汾



//-----------------------IO�궨��--------------------------------------------
 //����IO 
#define S1_IO_IN1           gpio_input_bit_get(GPIOB,GPIO_PIN_13)
#define S1_IO_IN2           gpio_input_bit_get(GPIOB,GPIO_PIN_14)

#define S2_IO_IN1           gpio_input_bit_get(GPIOB,GPIO_PIN_4)
#define S2_IO_IN2           gpio_input_bit_get(GPIOB,GPIO_PIN_7)
 

 //����IO 
 #define ADD1_IO_IN           gpio_input_bit_get(GPIOA,GPIO_PIN_7)
 #define ADD2_IO_IN           gpio_input_bit_get(GPIOA,GPIO_PIN_6)
 
 #define ADD3_IO_IN           gpio_input_bit_get(GPIOA,GPIO_PIN_5)
 #define ADD4_IO_IN           gpio_input_bit_get(GPIOA,GPIO_PIN_4)
 #define ADD5_IO_IN           gpio_input_bit_get(GPIOA,GPIO_PIN_3)
 

  //����IO 
  #define A6_IO_IN2           gpio_input_bit_get(GPIOB,GPIO_PIN_8)
  #define A5_IO_IN2           gpio_input_bit_get(GPIOB,GPIO_PIN_9)
  
  #define A6_IO_IN1           gpio_input_bit_get(GPIOB,GPIO_PIN_15)
 


//���IO 
#define LED_RUN(n)           gpio_bit_write(GPIOC,GPIO_PIN_13,(bit_status)(n==0?1:0));//9

/***************************************************************/
#define DA1_5V_ONOFF(n)        gpio_bit_write(GPIOB,GPIO_PIN_12,(bit_status)n);
#define DA2_5V_ONOFF(n)        gpio_bit_write(GPIOA,GPIO_PIN_2,(bit_status)n);
 


/***************************************************************/
#define S0(n)        gpio_bit_write(GPIOA,GPIO_PIN_5,(bit_status)n);
#define S1(n)        gpio_bit_write(GPIOB,GPIO_PIN_3,(bit_status)n);
 

/***************************************************************/
#define RS485_RX_ENABLE(n)        gpio_bit_write(GPIOA,GPIO_PIN_11,(bit_status)(n==0?1:0));

 
//------------------------------��������-------------------------------------------------------------
 
 
 


 
 
extern __IO uint32_t SYS_TimeTick;
extern  ST_EEPStruct   gEEP_data; 
 
extern  ST_WorkSpace_def WorkSpace; //�����ռ�����	
 
//------------------------------��������-------------------------------------------------------------
 



 
//------------------------------��������-------------------------------------------------------------
void rt_os_tick_callback(void);
void delay_1ms(uint32_t T);
void delay_us(uint32_t T);

 
extern void APP_TaskInit(void);
  
 
 


void SET_DA1_PWM_Duty(uint16_t SetV);
void SET_DA2_PWM_Duty(uint16_t SetV);


#endif /* MAIN_H */
