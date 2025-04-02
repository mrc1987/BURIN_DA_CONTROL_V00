#ifndef __APP_CAN_H
#define __APP_CAN_H
#include "main.h"
 
extern ByteBitArea PS_Module_CanStatus;//模块CAN接收状态
extern ByteBitArea PC_CanStatus;//上位机接收状态


typedef  struct 
{
        
	uint32_t DestAddr:6;   //目标节点地址
	uint32_t R1:1;         //备用
	uint32_t ACK:1;        //响应
	uint32_t SourceAddr:6; //源节点地址
	uint32_t R2:1;         //备用
	uint32_t Subpolo:1;    //分包
	uint32_t SubCnt:2;     //分包计数
	uint32_t R3:6;         //保留
	uint32_t FuncID:5;     //功能码
  
} _CANExID;


typedef  union 
{
        
  struct 
  {
        
	uint32_t DestAddr:6;   //目标节点地址
	uint32_t R1:1;         //备用
	uint32_t ACK:1;        //响应
	uint32_t SourceAddr:6; //源节点地址
	uint32_t R2:1;         //备用
	uint32_t Subpolo:1;    //分包
	uint32_t SubCnt:2;     //分包计数
	uint32_t R3:6;         //保留
	uint32_t FuncID:5;     //功能码
  
  }BIT ;
 uint32_t u32CAN_ExID;
  
} _CANExID_U;


typedef  enum 
{
  MasterRead=4,//主控读
  MasterWrite=5, //主控写
  SlaverRead=6,   //从控读
  SlaverWrite=7, //从控写

  SlaverAutoUp=9, //从控自动发
  CustCMD=24, //自定义
  BootCMD=28, //boot 指令
} _FunCode;


//======================================================================
#define PC_CAN_Err         PC_CanStatus.Bit.b0  //CAN错误
#define PC_CAN_Rx_flag     PC_CanStatus.Bit.b1  //接收标志
#define PC_CAN_Tx_flag     PC_CanStatus.Bit.b2  //发送标志
#define PC_CAN_Full_flag   PC_CanStatus.Bit.b3  //缓冲区满标志

#define PSModule_CAN_Err         PC_CanStatus.Bit.b0  //CAN错误
#define PSModule_CAN_Rx_flag     PC_CanStatus.Bit.b1  //接收标志
#define PSModule_CAN_Tx_flag     PC_CanStatus.Bit.b2  //发送标志
#define PSModule_CAN_Full_flag   PC_CanStatus.Bit.b3  //缓冲区满标志
//======================================================================

 void app_caninit (void);

void CAN0_RX0_IRQHandler(void);
void CAN1_RX0_IRQHandler(void);
#endif

