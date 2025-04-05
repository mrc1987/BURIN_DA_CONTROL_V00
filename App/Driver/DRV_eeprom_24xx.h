/*
*********************************************************************************************************
*
*	模块名称 : 串行EEPROM 24xx02驱动模块
*	文件名称 : bsp_eeprom_24xx.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	修改记录 :
*		版本号  日期       作者    说明
*
*********************************************************************************************************
*/

#ifndef _DRV_EEPROM_24XX_H
#define _DRV_EEPROM_24XX_H

typedef  struct 
{

	uint8_t bootFlag;//0xA5 进入Boot标志 其他只跳转到App  
	uint8_t BootVer; //BOOT 版本 
 
} BootLoader_Info_def;

//#define AT24C02
#define AT24C08
//#define AT24C128
#ifdef AT24C02
	#define EE_MODEL_NAME		"AT24C02"
	#define EE_DEV_ADDR			0xA0		/* 设备地址 */
	#define EE_PAGE_SIZE		8			/* 页面大小(字节) */
	#define EE_SIZE				256			/* 总容量(字节) */
	#define EE_ADDR_BYTES		1			/* 地址字节个数 */
#endif

#ifdef AT24C08
	#define EE_MODEL_NAME		"AT24C08"
	#define EE_DEV_ADDR			0xA0		/* 设备地址 */
	#define EE_PAGE_SIZE		16			/* 页面大小(字节) */
	#define EE_SIZE				1024		/* 总容量(字节) */
	#define EE_ADDR_BYTES		1			/* 地址字节个数 */
#endif

#ifdef AT24C128
	#define EE_MODEL_NAME	    "AT24C128"
	#define EE_DEV_ADDR			0xA0		/* 设备地址 */
	#define EE_PAGE_SIZE		64			/* 页面大小(字节) */
	#define EE_SIZE			   (16*1024)	/* 总容量(字节) */
	#define EE_ADDR_BYTES		2			/* 地址字节个数 */
#endif


#define EEP_Offset 0

#define EEP_Boot_Offset 512


uint8_t ee_CheckOk(void);
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);

#endif

 
