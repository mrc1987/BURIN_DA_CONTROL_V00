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


typedef  volatile unsigned char VU8;
typedef  volatile signed char VS8;
typedef  volatile unsigned short VU16;
typedef  volatile signed short VS16;
typedef  volatile unsigned long VU32;
typedef  volatile signed long VS32;
typedef  volatile signed long long VS64;
typedef  volatile unsigned long long VU64;
typedef  volatile float   VF32;

typedef  unsigned char U8;
typedef  signed char   S8;
typedef  unsigned short U16;
typedef  signed short S16;
typedef  unsigned long U32;
typedef  signed long     S32;
typedef  signed long long S64;
typedef  unsigned long long U64;
typedef  float              F32;


 
//============8位结构定义=======================================
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
 

 
//============16位结构定义=======================================
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
 

 
 
//============32位结构定义=======================================

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





//============电源工作标志定义===============================
typedef  union 
{
   struct 
    {
		U32 DA1_SoftStart_Flag: 1;      
		U32 DA2_SoftStart_Flag: 1;    

		U32 DA1_SoftStart_EN: 1;      
		U32 DA2_SoftStart_EN: 1;    


		U32 DA1_CNTL_ONOFF: 1;       // 控制DA输出开关
		U32 DA1_CNTL_5V_ONOFF: 1;    // 控制5V输出开关 
		U32 DA1_CNTL_PWM_EN: 1;      // 控制DA输出PWM开关 
		U32 DA1_CNTL_Rise_Mode: 1;    // 缓慢和直接输出DA   

		U32 DA2_CNTL_ONOFF: 1;        // 控制DA输出开关 
		U32 DA2_CNTL_5V_ONOFF: 1;     // 控制5V输出开关
		U32 DA2_CNTL_PWM_EN: 1;       // 控制DA输出PWM开关 
	    U32 DA2_CNTL_Rise_Mode: 1;    // 缓慢和直接输出DA   

		U32 DA_AutoMode: 1;    // 程序手动/自动切换
		U32 EEP_UpdateFlag: 1;    // EEP 更新标志
 
	}bit;
  
  U32 all;              //电源标志
}Un_WorkFlag_Def;


//================Flash EEP 数据结构============================
typedef  struct 
{
  uint16_t  EEPFlag:8; 

  uint16_t  DA1_Out_Mode:1; //缓慢和直接输出DA 
  uint16_t  DA1_PWM_EN:1;   //输出PWM开关 
  uint16_t  DA1_5V_EN:1;    //5V 开关 
  uint16_t  DA1_ONOFF:1;    //5V 开关 

  uint16_t  DA2_Out_Mode:1; //缓慢和直接输出DA 
  uint16_t  DA2_PWM_EN:1;   //输出PWM开关 
  uint16_t  DA2_5V_EN:1;    //输出PWM开关 
  uint16_t  DA2_ONOFF:1;    //5V 开关 

  uint16_t DA1_V;    // DA1电压值
  uint16_t DA1_FRE;  // DA1频率
  uint16_t DA1_Duty; // DA1_PWM占空比值


  uint16_t DA2_V;    // DA2电压值
  uint16_t DA2_FRE;  // DA2频率
  uint16_t DA2_Duty; // DA2_PWM占空比值




  uint16_t DA1_V_K;    
  int16_t  DA1_V_B;    
	
  uint16_t DA2_V_K;    
  int16_t  DA2_V_B;  
 	
} ST_EEPStruct;


 

//============地址标志定义===============================
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
	}bit;
  
  uint8_t all;              
}UN_ADDR_Flag_Def;


//============DA设置标志定义===============================
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
		uint8_t R0: 1; 
		uint8_t R1: 1;            
	}BIT;
  
  uint8_t all;              
}UN_DA_SetFlag_Def;

 		
//typedef struct 
//{
//  uint16_t DA1_V; //DA1电压值
//  uint16_t DA1_FRE; //DA1电流值
//  uint16_t DA1_Duty; //DA1电流值

//  uint16_t DA2_V; //DA1电压值
//  uint16_t DA2_FRE; //DA1电流值
//  uint16_t DA2_Duty; //DA1电流值

//}ST_WorkSpace_def;


 
typedef struct 
{
	UN_DA_SetFlag_Def DA1_SetFlags; //DA设置标志	
	UN_DA_SetFlag_Def DA2_SetFlags; //DA设置标志	
	UN_ADDR_Flag_Def  ADDR_Flags; //地址标志	
	Un_WorkFlag_Def   WorkFlags; //工作标志	
    uint8_t modbus_addr; //modbus地址

	S16 DA1_V; //当前 DA1电压值 x100 
	S16 DA2_V; //当前 DA1电压值 x100 
}ST_WorkSpace_def;


 

#endif /* __DATATYPE_H */
