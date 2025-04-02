#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "main.h"

#define USE_HORIZONTAL 3  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#define LCD_W 240
#define LCD_H 240


//#define SPI_SCK_0   gpio_bit_write(GPIOB,GPIO_PIN_13,(bit_status)0);
//#define SPI_SCK_1   gpio_bit_write(GPIOB,GPIO_PIN_13,(bit_status)1);

//#define SPI_SDA_0   gpio_bit_write(GPIOB,GPIO_PIN_15,(bit_status)0);
//#define SPI_SDA_1   gpio_bit_write(GPIOB,GPIO_PIN_15,(bit_status)1);

// 
//#define SPI_DC_0    gpio_bit_write(GPIOB,GPIO_PIN_14,(bit_status)0);
//#define SPI_DC_1    gpio_bit_write(GPIOB,GPIO_PIN_14,(bit_status)1);

//#define SPI_CS_0   gpio_bit_write(GPIOB,GPIO_PIN_12,(bit_status)0);
//#define SPI_CS_1   gpio_bit_write(GPIOB,GPIO_PIN_12,(bit_status)1);
//-----------------LCD端口定义---------------- 

#define LCD_SCLK_Clr() gpio_bit_write(GPIOB,GPIO_PIN_13,(bit_status)0)//SCL=SCLK
#define LCD_SCLK_Set() gpio_bit_write(GPIOB,GPIO_PIN_13,(bit_status)1)

#define LCD_MOSI_Clr() gpio_bit_write(GPIOB,GPIO_PIN_15,(bit_status)0)//SDA=MOSI
#define LCD_MOSI_Set() gpio_bit_write(GPIOB,GPIO_PIN_15,(bit_status)1)

//#define LCD_RES_Clr()  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_RESET)//RES
//#define LCD_RES_Set()  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_SET)

#define LCD_DC_Clr()   gpio_bit_write(GPIOB,GPIO_PIN_14,(bit_status)0)//DC
#define LCD_DC_Set()   gpio_bit_write(GPIOB,GPIO_PIN_14,(bit_status)1)

#define LCD_CS_Clr()  gpio_bit_write(GPIOB,GPIO_PIN_12,(bit_status)0)//CS
#define LCD_CS_Set()  gpio_bit_write(GPIOB,GPIO_PIN_12,(bit_status)1)

//#define LCD_BLK_Clr()  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6, GPIO_PIN_RESET)//BLK
//#define LCD_BLK_Set()  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6, GPIO_PIN_SET)


void LCD_GPIO_Init(void);//初始化GPIO
void LCD_Writ_Bus(uint8_t dat);//模拟SPI时序
void LCD_WR_DATA8(uint8_t dat);//写入一个字节
void LCD_WR_DATA(uint16_t dat);//写入两个字节
void LCD_WR_REG(uint8_t dat);//写入一个指令
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);//设置坐标函数
void LCD_Init(void);//LCD初始化
#endif




