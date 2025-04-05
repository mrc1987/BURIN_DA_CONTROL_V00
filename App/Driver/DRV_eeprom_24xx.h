/*
*********************************************************************************************************
*
*	ģ������ : ����EEPROM 24xx02����ģ��
*	�ļ����� : bsp_eeprom_24xx.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*
*********************************************************************************************************
*/

#ifndef _DRV_EEPROM_24XX_H
#define _DRV_EEPROM_24XX_H

typedef  struct 
{

	uint8_t bootFlag;//0xA5 ����Boot��־ ����ֻ��ת��App  
	uint8_t BootVer; //BOOT �汾 
 
} BootLoader_Info_def;

//#define AT24C02
#define AT24C08
//#define AT24C128
#ifdef AT24C02
	#define EE_MODEL_NAME		"AT24C02"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		8			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				256			/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		1			/* ��ַ�ֽڸ��� */
#endif

#ifdef AT24C08
	#define EE_MODEL_NAME		"AT24C08"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		16			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				1024		/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		1			/* ��ַ�ֽڸ��� */
#endif

#ifdef AT24C128
	#define EE_MODEL_NAME	    "AT24C128"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		64			/* ҳ���С(�ֽ�) */
	#define EE_SIZE			   (16*1024)	/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		2			/* ��ַ�ֽڸ��� */
#endif


#define EEP_Offset 0

#define EEP_Boot_Offset 512


uint8_t ee_CheckOk(void);
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);

#endif

 
