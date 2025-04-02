/*
*********************************************************************************************************
*
*	模块名称 : MODS通信模块. 从站模式
*	文件名称 : modbus_slave.c
*	版    本 : V1.4
*	说    明 : 头文件
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "main.h"
#include "bsp_modbus.h"
 


static void MODS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen);
static void MODS_SendAckOk(void);
static void MODS_SendAckErr(uint8_t _ucErrCode);

static void MODS_AnalyzeApp(void);

static void MODS_RxTimeOut(void);

static void MODS_01H(void);
static void MODS_02H(void);
static void MODS_03H(void);
static void MODS_04H(void);
static void MODS_05H(void);
static void MODS_06H(void);
static void MODS_10H(void);

static uint8_t MODS_ReadRegValue(uint16_t reg_addr, uint8_t *reg_value);
static uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value);

//static uint8_t g_mods_timeout = 0;
void MODS_ReciveNew(uint8_t _byte);
extern void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);

MODS_T g_tModS;


 
#define SAddr 1
  


/*
*********************************************************************************************************
*	函 数 名: Modbus_Poll_Task
*	功能说明: 解析数据包. 在线程中处理
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Modbus_Poll_Task()
{
	 uint16_t addr=0;
	static uint16_t crc1;
   
	bRS485_Timeout_Flag=0;
	
	if (bRS485_Timeout_Flag != 0) 	
	{
		goto err_ret;
	}

	if (g_tModS.RxCount < 4)				/* 接收到的数据小于4个字节就认为错误 */
	{
		goto err_ret;
	}

	/* 计算CRC校验和 */
	crc1 = CRC16_Modbus(g_tModS.RxBuf, g_tModS.RxCount-2);
	if (crc1 != ((g_tModS.RxBuf[g_tModS.RxCount-2]<<8)+ (g_tModS.RxBuf[g_tModS.RxCount-1]<<0)))
	{
		goto err_ret;
	}

	/* 站地址 (1字节） */
	addr = g_tModS.RxBuf[0];				/* 第1字节 站号 */
	if ((addr != SAddr)&&(addr != 0))	/* 判断主机发送的命令地址是否符合 */
	{
		goto err_ret;
	}


	if (g_tModS.RxBuf[1] == 0)		 			/* 判断主机发送的命令是否符合 */
	{
		goto err_ret;
	}
	
	/* 分析应用层协议 */
	MODS_AnalyzeApp();	

	memset(g_tModS.RxBuf,0,g_tModS.RxCount);
	g_tModS.RxCount = 0;					/* 必须清零计数器，方便下次帧同步 */
	bRS485_Timeout_Flag=0;
	bRS485_RX_Flag=0;	
	
	return;
err_ret:
 	//MODS_SendAckErr(RSP_ERR_VALUE);
 	memset(g_tModS.RxBuf,0,g_tModS.RxCount);
	g_tModS.RxCount = 0;					/* 必须清零计数器，方便下次帧同步 */
	bRS485_Timeout_Flag=0;
	bRS485_RX_Flag=0;
}

 



/*
*********************************************************************************************************
*	函 数 名: MODS_SendWithCRC
*	功能说明: 发送一串数据, 自动追加2字节CRC
*	形    参: _pBuf 数据；
*			  _ucLen 数据长度（不带CRC）
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen)
{
	uint16_t crc;
	uint8_t buf[128+2],i;

	memcpy(buf, _pBuf, _ucLen);
	crc = CRC16_Modbus(_pBuf, _ucLen);
	buf[_ucLen++] = crc >> 8;
	buf[_ucLen++] = crc;


	RS485_SendBuf(buf, _ucLen);
	

}




/*
*********************************************************************************************************
*	函 数 名: MODS_SendAckErr
*	功能说明: 发送错误应答
*	形    参: _ucErrCode : 错误代码
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_SendAckErr(uint8_t _ucErrCode)
{
	uint8_t txbuf[3]={0};

	txbuf[0] = g_tModS.RxBuf[0];					/* 485地址 */
	txbuf[1] = g_tModS.RxBuf[1] | 0x80;				/* 异常的功能码 */
	txbuf[2] = _ucErrCode;							/* 错误代码(01,02,03,04) */

	MODS_SendWithCRC(txbuf, 3);
}

/*
*********************************************************************************************************
*	函 数 名: MODS_SendAckOk
*	功能说明: 发送正确的应答.
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_SendAckOk(void)
{
	uint8_t txbuf[32];
	uint8_t i;

	for (i = 0; i < g_tModS.RxCount; i++)
	{
		txbuf[i] = g_tModS.RxBuf[i];
	}
	RS485_SendBuf(txbuf, g_tModS.RxCount);
}

/*
*********************************************************************************************************
*	函 数 名: MODS_AnalyzeApp
*	功能说明: 分析应用层协议
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_AnalyzeApp(void)
{
	switch (g_tModS.RxBuf[1])				/* 第2个字节 功能码 */
	{

		case 0x04:		
		case 0x03:							/* 读取保持寄存器*/
		 {
			MODS_03H();
			break;
		 }
		case 0x06:							/* 写单个保存寄存器*/
		  {   
			MODS_06H();	
 			break;
		  }
		default:
		{
			g_tModS.RspCode = RSP_ERR_CMD;
			MODS_SendAckErr(g_tModS.RspCode);	/* 告诉主机命令错误 */
			break;
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: MODS_ReadRegValue
*	功能说明: 读取保持寄存器的值
*	形    参: reg_addr 寄存器地址
*			  reg_value 存放寄存器结果
*	返 回 值: 1表示OK 0表示错误
*********************************************************************************************************
*/
static uint8_t MODS_ReadRegValue(uint16_t reg_addr, uint8_t *reg_value)
{
	uint16_t value;
	
	switch (reg_addr)									/* 判断寄存器地址 */
	{
        case 1://软件版本
		  {
			value =	Power_MeasData.SoftVer;	
			break;
		  }
 
        case 2://输出电压
		  {
			value =Power_MeasData.Vout;	
			break;
		  }
         case 3://输出电流
		  {
			value =	Power_MeasData.Iout;
			break;
		  }
		 case 4://输出频率
		  {
			 
			value =	 Power_MeasData.Fre_Out;	
			break;
		  }
	    case 5://温度1
		  {
			  
			value =  Power_MeasData.T1;	
			break;
		  }
		 case 6://温度2
		  {
			value =  Power_MeasData.T2;		
			break;
		  }
	    case 7://输出电压
		  {
			 
			value =	 Power_MeasData.Vin;	
			break;
		  }
		 case 8://电源状态
		  {
			   
			value =	PS_Status.Value;	
			break;
		  }
	     case 9://电源信号状态
		  {
			value = PS_IO_Status.Value;	
			break;
		  }
		 case 10://手动设置限流值
		  {
			
			value = Power_MeasData.ExSetIout;	
			break;
		  }
		case 11://信号输入周期
		  {
			 
			value =	Power_MeasData.ExSetPeriond;	
			break;
		  }
		 case 14://设置输出限流值
		  {
			
			value =	  Ps_SetPara.SetIout;	
			break;
		  }
		 case 15://设置输出频率
		  {
			   
			value =	 Ps_SetPara.SetFre;	
			break;
		  }
	     case 16://设置输出脉宽
		  {
			  
			value =	 Ps_SetPara.SetDuty;	
			break;
		  }
		 case 17://设置输出电压
		  {
			   
			value =	Ps_SetPara.SetVout;	
			break;
		  }
	    case 18://设置输出过流值
		  {
			 
			value =	 EEP_Data.Iout_Limt;	
			break;
		  }
		 case 19://远程锁标志
		  {
			  value =	bRemoteLock?0x0A:00;	
			break;
		  }
	    case 20://输入信号模拟量K
		  {
			  
			value =	EEP_Data.ExVref_K;	
			break;
		  }
		case 21://输入信号模拟量B
		  {
			value = EEP_Data.ExVref_B;	
			break;
		  }
		case 22://输出电流校准  K值
		  {
			 
			value =  EEP_Data.Iout_K;	
			break;
		  }
		case 23://输出电流流校准 B值
		  {
			value =	  EEP_Data.Iout_B;	
			break;
		  }
	    case 24://输出电压校准  K值[8000,12000]
		  {
			  
			value =	 EEP_Data.Vout_K;	
			break;
		  }
	   case 25://输出电压校准  B值 
		  {
			value =  EEP_Data.Vout_B;	
			break;
		  }
	  case 26://设置限流校准K
		  {
			  
			value =EEP_Data.Set_CC_K;	
			break;
		  }
	  case 27://设置限流校准B
		  {
			value =	EEP_Data.Set_CC_B;	
			break;
		  }
	 case 28://设置电压校准  K值
		  {
			 
			value =	 EEP_Data.Set_Vout_K;	
			break;
		  }
	 case 29://设置电压校准  B值
		  {
			value =	 EEP_Data.Set_Vout_B;	
			break;
		  }
		default:
			return 0;									/* 参数异常，返回 0 */
	}

	reg_value[0] = value >> 8;
	reg_value[1] = value;

	return 1;											/* 读取成功 */
}

/*
*********************************************************************************************************
*	函 数 名: MODS_WriteRegValue
*	功能说明: 读取保持寄存器的值
*	形    参: reg_addr 寄存器地址
*			  reg_value 寄存器值
*	返 回 值: 1表示OK 0表示错误
*********************************************************************************************************
*/
static uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value)
{
	int16_t i16_Value;
	switch (reg_addr)							/* 判断寄存器地址 */
	{	
			
		 case 14://设置输出限流值
		  {
			if(reg_value <2000) Ps_SetPara.SetIout=reg_value;	
			 else return 0;
			break;
		  }
		 case 15://设置输出频率
		  {
			   
			if((reg_value <= 20000) && (reg_value >= 50))  Ps_SetPara.SetFre = reg_value;
            else Ps_SetPara.SetFre = 0;			  
			break;
		  }
	     case 16://设置输出脉宽
		  {
			  
			  
			if((reg_value <= 5000)  )   Ps_SetPara.SetDuty = reg_value;
            else return 0;				  
			break;
		  }
		 case 17://设置输出电压
		  {
			
			if((reg_value <= 300) && (reg_value >= 150)) 	Ps_SetPara.SetVout = reg_value;
            else return 0;					  
 
			break;
		  }
	    case 18://设置输出过流值
		  {
			if(reg_value <= 1400) 	 EEP_Data.Iout_Limt = reg_value;
            else return 0;		 
              EEP_RegSave(EEP_DataOffset+10*2,reg_value);;
			break;
		  }
		 case 19://远程锁标志
		  {
			  if(reg_value==0xA)
			  {
			    bRemoteLock=1;	
			  }
			  else
			  {
				bRemoteLock=0;	
			  }
			break;
		  }
	    case 20://输入信号模拟量K
		  {
			if((reg_value >= 8000 ) && ( reg_value <= 12000))
			{
              EEP_Data.ExVref_K=reg_value;
			  EEP_RegSave(EEP_DataOffset+0*2,reg_value);
			}
			else
			{
			  return 0;		
			}
			break;
		  }
		case 21://输入信号模拟量B
		  {
			  i16_Value= (int16_t)reg_value;
			if((i16_Value >= -5000 ) && ( i16_Value <= 5000))
			{
              EEP_Data.ExVref_B=i16_Value;
			  EEP_RegSave(EEP_DataOffset+1*2,reg_value);;
			}
			else
			{
			  return 0;		
			}
 	
			break;
		  }
		case 22://输出电流校准  K值
		  {
			if((reg_value >= 8000 ) && ( reg_value <= 12000))
			{
              EEP_Data.Iout_K=reg_value;
			  EEP_RegSave(EEP_DataOffset+2*2,reg_value);
			}
			else
			{
			  return 0;		
			}
 
			break;
		  }
		case 23://输出电流流校准 B值
		  {
		    i16_Value= (int16_t)reg_value;
			if((i16_Value >= -5000 ) && ( i16_Value <= 5000))
			{
              EEP_Data.Iout_B=i16_Value;
			  EEP_RegSave(EEP_DataOffset+3*2,reg_value);
			}
			else
			{
			  return 0;		
			}
 
			break;
		  }
	    case 24://输出电压校准  K值[8000,12000]
		  {
			if((reg_value >= 8000 ) && ( reg_value <= 12000))
			{
               EEP_Data.Vout_K=reg_value;
			   EEP_RegSave(EEP_DataOffset+4*2,reg_value);
			}
			else
			{
			  return 0;		
			}			  
	 	
			break;
		  }
	   case 25://输出电压校准  B值 
		  {
		    i16_Value= (int16_t)reg_value;
			if((i16_Value >= -5000 ) && ( i16_Value <= 5000))
			{
              EEP_Data.Vout_B=i16_Value;
			  EEP_RegSave(EEP_DataOffset+5*2,reg_value);
			}
			else
			{
			  return 0;		
			}
	
			break;
		  }
	  case 26://设置限流校准K
		  {
			if((reg_value >= 8000 ) && ( reg_value <= 12000))
			{
               EEP_Data.Set_CC_K=reg_value;
			  EEP_RegSave(EEP_DataOffset+6*2,reg_value);
			}
			else
			{
			  return 0;		
			}				  
 	
			break;
		  }
	  case 27://设置限流校准B
		  {
			  
			i16_Value= (int16_t)reg_value;
			if((i16_Value >= -5000 ) && ( i16_Value <= 5000))
			{
              EEP_Data.Set_CC_B=i16_Value;
			  EEP_RegSave(EEP_DataOffset+7*2,reg_value);
			}
			else
			{
			  return 0;		
			}
			break;
		  }
	 case 28://设置电压校准  K值
		  {
			if((reg_value >= 8000 ) && ( reg_value <= 12000))
			{
              EEP_Data.Set_Vout_K=reg_value;
			  EEP_RegSave(EEP_DataOffset+8*2,reg_value);
			}
			else
			{
			  return 0;		
			}			 
			break;
		  }
	 case 29://设置电压校准  B值
		  {
			i16_Value= (int16_t)reg_value;
			if((i16_Value >= -5000 ) && ( i16_Value <= 5000))
			{
              EEP_Data.Set_Vout_B=i16_Value;
			  EEP_RegSave(EEP_DataOffset+9*2,reg_value);
			}
			else
			{
			  return 0;		
			}

			break;
		  }
		default:
			return 0;		/* 参数异常，返回 0 */
	}

	return 1;		/* 读取成功 */
}

/*
*********************************************************************************************************
*	函 数 名: MODS_03H
*	功能说明: 读取保持寄存器 在一个或多个保持寄存器中取得当前的二进制值
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_03H(void)
{
	uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint8_t reg_value[64];

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)								/* 03H命令必须是8个字节 */
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* 数据值域错误 */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 				/* 寄存器号 */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);					/* 寄存器个数 */
	if(num == 0){g_tModS.RspCode = RSP_ERR_VALUE;goto err_ret;}
	if (num > (sizeof(reg_value) / 2))
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* 数据值域错误 */
		goto err_ret;
	}

	for (i = 0; i < num; i++)
	{
		if (MODS_ReadRegValue(reg, &reg_value[2 * i]) == 0)	/* 读出寄存器值放入reg_value */
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;				/* 寄存器地址错误 */
			break;
		}
		reg++;
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)							/* 正确应答 */
	{
		g_tModS.TxCount = 0;
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
		g_tModS.TxBuf[g_tModS.TxCount++] = num * 2;			/* 返回字节数 */
       
		for (i = 0; i < num; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i];
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i+1];
		}
		
		MODS_SendWithCRC(g_tModS.TxBuf, g_tModS.TxCount);	/* 发送正确应答 */
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);					/* 发送错误应答 */
	}
}


/*
*********************************************************************************************************
*	函 数 名: MODS_05H
*	功能说明: 强制单线圈（对应D01/D02/D03）
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_05H(void)
{
	uint16_t reg;
	uint16_t value;

	g_tModS.RspCode = RSP_OK;
	
	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* 数据值域错误 */
		goto err_ret;
	}

	reg   = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* 寄存器号 */
	value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* 数据 */
	
	if (value != 0xFF00 && value != 0x0000)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* 数据值域错误 */
		goto err_ret;
	}
	

		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址错误 */

err_ret:
	if (g_tModS.RspCode == RSP_OK)				/* 正确应答 */
	{
		MODS_SendAckOk();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);		/* 告诉主机命令错误 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: MODS_06H
*	功能说明: 写单个寄存器
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_06H(void)
{

	uint16_t reg;
	uint16_t value;

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* 数据值域错误 */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* 寄存器号 */
	value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* 寄存器值 */

	if (MODS_WriteRegValue(reg, value) == 1)	/* 该函数会把写入的值存入寄存器 */
	{
		;
	}
	else
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址错误 */
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)				/* 正确应答 */
	{
		MODS_SendAckOk();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);		/* 告诉主机命令错误 */
	}
}


/*****************************  *********************************/
