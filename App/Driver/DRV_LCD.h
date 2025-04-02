/*
*********************************************************************************************************
*
*	模块名称 : LCD驱动模块
*	文件名称 : LCD.h
*	版    本 : V1.0
*	说    明 : 头文件。
*
*
*********************************************************************************************************
*/

#ifndef _DRV_LCD_H
#define _DRV_LCD_H

#include "main.h"

#define LCD_CMD_RST            0x01  //复位

#define LCD_CMD_INV_OFF        0x20  //反向显示关
#define LCD_CMD_INV_ON         0x21  //反向显示开

#define LCD_CMD_DISPOFF        0x28      //显示关
#define LCD_CMD_DISPON         0x29      //显示开


#define LCD_CMD_CASET      0x2A      //Column address set
#define LCD_CMD_RAMWR      0x2C      //Memory write

#define LCD_CMD_MADCTL      0x36      //Memory data access contro
#define LCD_CMD_COLMOD      0x3A      //Interface pixel format

#define LCD_CMD_PORCTRL    0xB2     ///< Porch Setting
#define LCD_CMD_GCTRL      0xB7     ///< Gate Control

#define LCD_CMD_VCOMS      0xBB     ///< VCOM Setting
#define LCD_CMD_LCMCTRL    0xC0     ///< LCM Control

#define LCD_CMD_VDVVRHEN   0xC2     ///< VDV and VRH Command Enable
#define LCD_CMD_VRHS       0xC3     

#define LCD_CMD_VDVS       0xC4      
#define LCD_CMD_FRCTRL2    0xC6     ///< Frame Rate Control in Normal Mode

#define LCD_CMD_PWCTRL1    0xD0     ///< Power Control 1
#define LCD_CMD_FRCTRL2    0xC6     ///< Frame Rate Control in Normal Mode
 


#define LCD_CMD_SLPOUT         1
/***********************************************/
void lcd_clear(uint16_t color);
void lcd_init(void);
#endif
