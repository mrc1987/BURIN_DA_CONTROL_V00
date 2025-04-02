/*
*********************************************************************************************************
*
*	模块名称 : 串行EEPROM 24xx驱动模块
*	文件名称 : bsp_eeprom_24xx.c
*	版    本 : V1.0
*	说    明 : 实现24xx系列EEPROM的读写操作。写操作采用页写模式提高写入效率。
*
*	修改记录 :
*		版本号  日期        作者     说明
*
*********************************************************************************************************
*/
#include "main.h"
#include "bsp_sgm5437.h"


/*
*********************************************************************************************************
*	函 数 名: SGM5437_SetDAC(uint32_t SPIx,uint8_t CH,uint16_t V)
*	功能说明: 输出参考电压=3.3/4095*V
*	形    参:   
*	返 回 值:  
*********************************************************************************************************
*/
void SGM5437_SetDAC(uint32_t SPIx,uint8_t CH,uint16_t V)
{
	SPI_CS(SPIx,0);
     if(V>4095)V=4095;
	switch(CH)
	{
		case 1: SPI_SendByte(SPIx,((V>>8)&0x0F)|(0x8<<4));break;
		case 2: SPI_SendByte(SPIx,((V>>8)&0x0F)|(0x4<<4));break;
		case 3: SPI_SendByte(SPIx,((V>>8)&0x0F)|(0xC<<4));break;
		case 4: SPI_SendByte(SPIx,((V>>8)&0x0F)|(0x2<<4));break;
		case 5: SPI_SendByte(SPIx,((V>>8)&0x0F)|(0xA<<4));break;
		case 6: SPI_SendByte(SPIx,((V>>8)&0x0F)|(0x6<<4));break;
		case 7: SPI_SendByte(SPIx,((V>>8)&0x0F)|(0xE<<4));break;
		case 8: SPI_SendByte(SPIx,((V>>8)&0x0F)|(0x1<<4));break;
	}
	SPI_SendByte(SPIx,V&0XFF);
	
	SPI_CS(SPIx,1);	//上升沿更新数据
}


/*
*********************************************************************************************************
*	函 数 名: SGM5437_SynSetDAC(uint32_t SPIx,uint16_t *Data,uint8_t N)
*	功能说明: 同步输出参考电压=3.3/4095*V
*	形    参:   
*	返 回 值:  
*********************************************************************************************************
*/
void SGM5437_SynSetDAC(uint32_t SPIx,uint16_t *Data,uint8_t N)
{
	 
	uint16_t V;

	SPI_CS(SPIx,0);
	SPI_SendByte(SPIx,0x50);
    SPI_SendByte(SPIx,0x20);//开启同步
 
	SPI_CS(SPIx,1);	//上升沿更新数据
	SPI_CS(SPIx,0);
    V= *(Data+0);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0x8<<4));//DAC0
	SPI_SendByte(SPIx,V&0XFF);
	
	SPI_CS(SPIx,1);	//上升沿更新数据
	SPI_CS(SPIx,0); 
	V= *(Data+1);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0x4<<4));//DAC1
	SPI_SendByte(SPIx,V&0XFF);
	
 	SPI_CS(SPIx,1);	//上升沿更新数据
	SPI_CS(SPIx,0);
	V= *(Data+2);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0xC<<4));//DAC2
	SPI_SendByte(SPIx,V&0XFF);
	
	SPI_CS(SPIx,1);	//上升沿更新数据
	SPI_CS(SPIx,0); 
	V= *(Data+3);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0x2<<4));//DAC3
	SPI_SendByte(SPIx,V&0XFF);
	
 	SPI_CS(SPIx,1);	//上升沿更新数据
	SPI_CS(SPIx,0);
	V= *(Data+4);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0xA<<4));//DAC4
	SPI_SendByte(SPIx,V&0XFF);
	
	SPI_CS(SPIx,1);	//上升沿更新数据
	SPI_CS(SPIx,0); 
	V= *(Data+5);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0x6<<4));//DAC5
	SPI_SendByte(SPIx,V&0XFF);
	
 	SPI_CS(SPIx,1);	//上升沿更新数据
	SPI_CS(SPIx,0);
	V= *(Data+6);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0xE<<4));//DAC6
	SPI_SendByte(SPIx,V&0XFF);
	
	SPI_CS(SPIx,1);	//上升沿更新数据
	SPI_CS(SPIx,0); 
	V= *(Data+7);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0x1<<4));//DAC7
	SPI_SendByte(SPIx,V&0XFF);

}


/*
*********************************************************************************************************
*	函 数 名: SGM5437_SetDAC(uint32_t SPIx,uint8_t CH,uint16_t V)
*	功能说明: 
*	形    参:   
*	返 回 值:  
*********************************************************************************************************
*/
void SGM5437_Exit_Simultaneous(uint32_t SPIx )
{
//  	SPI_CS(SPIx,0);
//	SPI_SendByte(SPIx,0x50);//开启同步
//    SPI_SendByte(SPIx,0x60);
//	
////	SPI_CS(SPIx,0);
////    SPI_SendByte(SPIx,0x50);//开启同步
////	SPI_SendByte(SPIx,0);
 
	SPI_CS(SPIx,1);	 //上升沿同步更新DA
}

 

/*
*********************************************************************************************************
*	函 数 名: void SGM5437_PowerDown(uint32_t SPIx,uint8_t State )
*	功能说明: 
*	形    参:   
*	返 回 值:  
*********************************************************************************************************
*/
void SGM5437_PowerDown(uint32_t SPIx,uint8_t State )
{
	SPI_CS(SPIx,0);
     if(State==0)
	 {
        SPI_SendByte(SPIx,0x9<<4);
     	SPI_SendByte(SPIx,0);
	 }
	 else
	 {
       SPI_SendByte(SPIx,(0x9<<4)&0X0F);
       SPI_SendByte(SPIx,0xFF);		 
	 }
	
	SPI_CS(SPIx,1);	 
}