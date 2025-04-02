/*
*********************************************************************************************************
*
*	ģ������ : ����EEPROM 24xx����ģ��
*	�ļ����� : bsp_eeprom_24xx.c
*	��    �� : V1.0
*	˵    �� : ʵ��24xxϵ��EEPROM�Ķ�д������д��������ҳдģʽ���д��Ч�ʡ�
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*
*********************************************************************************************************
*/
#include "main.h"
#include "bsp_sgm5437.h"


/*
*********************************************************************************************************
*	�� �� ��: SGM5437_SetDAC(uint32_t SPIx,uint8_t CH,uint16_t V)
*	����˵��: ����ο���ѹ=3.3/4095*V
*	��    ��:   
*	�� �� ֵ:  
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
	
	SPI_CS(SPIx,1);	//�����ظ�������
}


/*
*********************************************************************************************************
*	�� �� ��: SGM5437_SynSetDAC(uint32_t SPIx,uint16_t *Data,uint8_t N)
*	����˵��: ͬ������ο���ѹ=3.3/4095*V
*	��    ��:   
*	�� �� ֵ:  
*********************************************************************************************************
*/
void SGM5437_SynSetDAC(uint32_t SPIx,uint16_t *Data,uint8_t N)
{
	 
	uint16_t V;

	SPI_CS(SPIx,0);
	SPI_SendByte(SPIx,0x50);
    SPI_SendByte(SPIx,0x20);//����ͬ��
 
	SPI_CS(SPIx,1);	//�����ظ�������
	SPI_CS(SPIx,0);
    V= *(Data+0);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0x8<<4));//DAC0
	SPI_SendByte(SPIx,V&0XFF);
	
	SPI_CS(SPIx,1);	//�����ظ�������
	SPI_CS(SPIx,0); 
	V= *(Data+1);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0x4<<4));//DAC1
	SPI_SendByte(SPIx,V&0XFF);
	
 	SPI_CS(SPIx,1);	//�����ظ�������
	SPI_CS(SPIx,0);
	V= *(Data+2);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0xC<<4));//DAC2
	SPI_SendByte(SPIx,V&0XFF);
	
	SPI_CS(SPIx,1);	//�����ظ�������
	SPI_CS(SPIx,0); 
	V= *(Data+3);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0x2<<4));//DAC3
	SPI_SendByte(SPIx,V&0XFF);
	
 	SPI_CS(SPIx,1);	//�����ظ�������
	SPI_CS(SPIx,0);
	V= *(Data+4);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0xA<<4));//DAC4
	SPI_SendByte(SPIx,V&0XFF);
	
	SPI_CS(SPIx,1);	//�����ظ�������
	SPI_CS(SPIx,0); 
	V= *(Data+5);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0x6<<4));//DAC5
	SPI_SendByte(SPIx,V&0XFF);
	
 	SPI_CS(SPIx,1);	//�����ظ�������
	SPI_CS(SPIx,0);
	V= *(Data+6);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0xE<<4));//DAC6
	SPI_SendByte(SPIx,V&0XFF);
	
	SPI_CS(SPIx,1);	//�����ظ�������
	SPI_CS(SPIx,0); 
	V= *(Data+7);
	SPI_SendByte(SPIx,((V>>8)&0x0F)|(0x1<<4));//DAC7
	SPI_SendByte(SPIx,V&0XFF);

}


/*
*********************************************************************************************************
*	�� �� ��: SGM5437_SetDAC(uint32_t SPIx,uint8_t CH,uint16_t V)
*	����˵��: 
*	��    ��:   
*	�� �� ֵ:  
*********************************************************************************************************
*/
void SGM5437_Exit_Simultaneous(uint32_t SPIx )
{
//  	SPI_CS(SPIx,0);
//	SPI_SendByte(SPIx,0x50);//����ͬ��
//    SPI_SendByte(SPIx,0x60);
//	
////	SPI_CS(SPIx,0);
////    SPI_SendByte(SPIx,0x50);//����ͬ��
////	SPI_SendByte(SPIx,0);
 
	SPI_CS(SPIx,1);	 //������ͬ������DA
}

 

/*
*********************************************************************************************************
*	�� �� ��: void SGM5437_PowerDown(uint32_t SPIx,uint8_t State )
*	����˵��: 
*	��    ��:   
*	�� �� ֵ:  
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