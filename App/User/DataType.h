/******************************************************************************
* @file     APP_datatype.h
* @author   MRC
* @version  V0.0.0
* @date     2020-12-14
* @brief    
******************************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DATATYPE_H
#define __DATATYPE_H

 #include "main.h"


 
//============8λ�ṹ����=======================================
typedef  union 
 {
	struct {
		uint8_t b0: 1;
		uint8_t b1: 1;
		uint8_t b2: 1;
		uint8_t b3: 1;
		uint8_t b4: 1;
		uint8_t b5: 1;
		uint8_t b6: 1;
		uint8_t b7: 1; 
	} Bit;
	uint8_t Value;
}ByteBitArea;
 

 
//============16λ�ṹ����=======================================
typedef  union 
 {
	struct {
		uint16_t b0: 1;
		uint16_t b1: 1;
		uint16_t b2: 1;
		uint16_t b3: 1;
		uint16_t b4: 1;
		uint16_t b5: 1;
		uint16_t b6: 1;
		uint16_t b7: 1; 
		

		uint16_t b8: 1;
		uint16_t b9: 1;
		uint16_t b10: 1;
		uint16_t b11: 1;
		uint16_t b12: 1;
		uint16_t b13: 1;
		uint16_t b14: 1;
		uint16_t b15: 1; 		
		
	} Bit;
	uint16_t Value;
}WordBitArea;
 

 
 
//============32λ�ṹ����=======================================

typedef  union 
 {
	struct {
		uint32_t b0: 1;
		uint32_t b1: 1;
		uint32_t b2: 1;
		uint32_t b3: 1;
		uint32_t b4: 1;
		uint32_t b5: 1;
		uint32_t b6: 1;
		uint32_t b7: 1;

		uint32_t b8: 1;
		uint32_t b9: 1;
		uint32_t b10: 1;
		uint32_t b11: 1;
		uint32_t b12: 1;
		uint32_t b13: 1;
		uint32_t b14: 1;
		uint32_t b15: 1;
		
		uint32_t b16: 1;
		uint32_t b17: 1;
		uint32_t b18: 1;
		uint32_t b19: 1;
		uint32_t b20: 1;
		uint32_t b21: 1;
		uint32_t b22: 1;
		uint32_t b23: 1;
		
		uint32_t b24: 1;
		uint32_t b25: 1;
		uint32_t b26: 1;
		uint32_t b27: 1;
		uint32_t b28: 1;
		uint32_t b29: 1;
		uint32_t b30: 1;
		uint32_t b31: 1;
		
	} Bit;
	uint32_t Status;
}U32BitArea;





//============��Դ������־����===============================
typedef  union 
{
   struct 
    {
		uint16_t DA1_ONOFF: 1; // ͨ��1�ֶ�����5V�������
     	uint16_t DA1_5V_ONOFF: 1;    //ͨ��1�ֶ�����DA�������
  		uint16_t DA1_PWM_EN: 1;  //ͨ��1�ֶ�����DA���PWM���� 
		uint16_t DA1_OUT_Mode: 1;  //������ֱ�����DA   

		uint16_t DA2_ONOFF: 1; // ͨ��1�ֶ�����5V�������
		uint16_t DA2_5V_ONOFF: 1;    //ͨ��1�ֶ�����DA�������
		uint16_t DA2_PWM_EN: 1;      //ͨ��1�ֶ�����DA���PWM���� 
	    uint16_t DA2_OUT_Mode: 1;    //������ֱ�����DA   

		uint16_t DA_AutoMode: 1;    //�����ֶ�/�Զ��л�

	}BIT;
  
  uint16_t all;              //��Դ��־
}Un_WorkFlag_Def;


//================Flash EEP ���ݽṹ============================
typedef  struct 
{
  uint16_t  EEPFlag:8; 

  uint16_t  DA1_Out_Mode:2; //������ֱ�����DA 
  uint16_t  DA1_PWM_EN:2;   //���PWM���� 
  uint16_t  DA2_Out_Mode:2; //������ֱ�����DA 
  uint16_t  DA2_PWM_EN:2;  //���PWM���� 

  uint16_t DA1_V;    // DA1��ѹֵ
  uint16_t DA1_FRE;  // DA1Ƶ��
  uint16_t DA1_Duty; // DA1_PWMռ�ձ�ֵ


  uint16_t DA2_V;    // DA2��ѹֵ
  uint16_t DA2_FRE;  // DA2Ƶ��
  uint16_t DA2_Duty; // DA2_PWMռ�ձ�ֵ




  uint16_t DA1_V_K;    
  int16_t  DA1_V_B;    
	
  uint16_t DA2_V_K;    
  int16_t  DA2_V_B;  
 	
} ST_EEPStruct;


 

//============��ַ��־����===============================
typedef  union 
{
   struct 
    {
	    uint8_t ADDR0: 1;      
		uint8_t ADDR1: 1;      
		uint8_t ADDR2: 1;   
		uint8_t ADDR3: 1; 
		uint8_t ADDR4: 1;       
	    uint8_t R:3;               
	}BIT;
  
  uint8_t all;              
}UN_ADDR_Flag_Def;


//============DA���ñ�־����===============================
typedef  union 
{
   struct 
    {
	    uint8_t DA0: 1;      
		uint8_t DA1: 1;      
		uint8_t DA2: 1;   
		uint8_t DA3: 1; 
		uint8_t DA4: 1;       
		uint8_t DA5: 1;   
		uint8_t DA6: 1; 
		uint8_t DA7: 1;            
	}BIT;
  
  uint8_t all;              
}UN_DA_SetFlag_Def;

 		
typedef struct 
{
  uint16_t DA1_V; //DA1��ѹֵ
  uint16_t DA1_FRE; //DA1����ֵ
  uint16_t DA1_Duty; //DA1����ֵ

  uint16_t DA2_V; //DA1��ѹֵ
  uint16_t DA2_FRE; //DA1����ֵ
  uint16_t DA2_Duty; //DA1����ֵ

}ST_WorkSpace_def;


 
typedef struct 
{
	UN_DA_SetFlag_Def DA1_SetFlags; //DA���ñ�־	
	UN_DA_SetFlag_Def DA2_SetFlags; //DA���ñ�־	
	UN_ADDR_Flag_Def  ADDR_Flags; //��ַ��־	
	Un_WorkFlag_Def   WorkFlags; //������־	
    uint8_t modbus_addr; //modbus��ַ

	uint16_t DA1_V; //��ǰ DA1��ѹֵ x100 
	uint16_t DA2_V; //��ǰ DA1��ѹֵ x100 
}ST_WorkSpace_def;


 

#endif /* __DATATYPE_H */
