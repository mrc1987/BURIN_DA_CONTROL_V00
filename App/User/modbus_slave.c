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
#include "modbus_slave.h"

 void MODS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen);
 void MODS_SendAckOk(void);
 void MODS_SendAckErr(uint8_t _ucErrCode);

 void MODS_AnalyzeApp(void);

 void MODS_RxTimeOut(void);

 void MODS_01H(void);
 void MODS_02H(void);
 void MODS_03H(void);
 void MODS_04H(void);
 void MODS_05H(void);
 void MODS_06H(void);
 void MODS_10H(void);

 uint8_t MODS_ReadRegValue(uint16_t reg_addr, uint8_t *reg_value);
 uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value);

//static uint8_t g_mods_timeout = 0;
void MODS_ReciveNew(uint8_t _byte);
extern void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);

MODS_T g_tModS;


 
/*
*********************************************************************************************************
*	函 数 名: Modbus_Init
*	功能说明: Modbus 函数注册
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Modbus_Init(uint8_t *pTxData,uint8_t *pRxData,pSend pSendFun)
{
   g_tModS.Modbus_Send = pSendFun;
   g_tModS.pTxBuf = pTxData;
   g_tModS.pRxBuf = pRxData;
}

/*
*********************************************************************************************************
*	函 数 名: Modbus_Poll_Task
*	功能说明: 解析数据包. 在线程中处理
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void Modbus_Poll_Task( )
{
	static uint16_t addr=0;
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
	crc1 = CRC16_Modbus(g_tModS.pRxBuf, g_tModS.RxCount-2);
	if (crc1 != ((g_tModS.pRxBuf[g_tModS.RxCount-2]<<8)+ (g_tModS.pRxBuf[g_tModS.RxCount-1]<<0)))
	{
		goto err_ret;
	}

	/* 站地址 (1字节） */
	addr = g_tModS.pRxBuf[0];				  /* 第1字节 站号 */
	if ((addr != g_tModS.addr)&&(addr != 0))  /* 判断主机发送的命令地址是否符合 */
	{
		goto err_ret;
	}


	if (g_tModS.pRxBuf[1] == 0)		 			/* 判断主机发送的命令是否符合 */
	{
		goto err_ret;
	}
	
	/* 分析应用层协议 */
	MODS_AnalyzeApp();	

	memset(g_tModS.pRxBuf,0,g_tModS.RxCount);
	g_tModS.RxCount = 0;					/* 必须清零计数器，方便下次帧同步 */
	return;
err_ret:
 	 
 	memset(g_tModS.pRxBuf,0,g_tModS.RxCount);
	g_tModS.RxCount = 0;					/* 必须清零计数器，方便下次帧同步 */
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
void MODS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen)
{
	uint16_t crc;
 

 
	crc = CRC16_Modbus(_pBuf, _ucLen);
	_pBuf[_ucLen+0] = crc >> 8;
	_pBuf[_ucLen+1] = crc;

	g_tModS.Modbus_Send(_pBuf, _ucLen+2);

}




/*
*********************************************************************************************************
*	函 数 名: MODS_SendAckErr
*	功能说明: 发送错误应答
*	形    参: _ucErrCode : 错误代码
*	返 回 值: 无
*********************************************************************************************************
*/
 void MODS_SendAckErr(uint8_t _ucErrCode)
{

	g_tModS.pTxBuf[0]  =  g_tModS.pRxBuf[0];	
	g_tModS.pTxBuf[1]  =  g_tModS.pRxBuf[1]|0x80;	
	g_tModS.pTxBuf[2]  =  _ucErrCode;	

	MODS_SendWithCRC(g_tModS.pTxBuf, 3);
}

/*
*********************************************************************************************************
*	函 数 名: MODS_SendAckOk
*	功能说明: 发送正确的应答.
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
 void MODS_SendAckOk(void)
{
	uint8_t txbuf[32];
	uint8_t i;

	for (i = 0; i < g_tModS.RxCount; i++)
	{
		txbuf[i] = g_tModS.pRxBuf[i];
	}
	g_tModS.Modbus_Send(txbuf, g_tModS.RxCount);
}

/*
*********************************************************************************************************
*	函 数 名: MODS_AnalyzeApp
*	功能说明: 分析应用层协议
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MODS_AnalyzeApp(void)
{
	uint8_t RecAddr=0;
	RecAddr=g_tModS.pRxBuf[0];
	switch (g_tModS.pRxBuf[1])				/* 第2个字节 功能码 */
	{
	
		case 0x03:	
		  {			/* 读取保持寄存器*/
			if(RecAddr != 0) //广播命令 不响应
			 {
			   MODS_03H();
			 }
			 break;
		  }
		case 0x05:							/* 强制单线圈*/
		  {
			MODS_05H();
			break;
		   }
		case 0x06:							/* 写单个保存寄存器（此例程改写g_tVar中的参数）*/
		 {
			MODS_06H();	
			break;
		 }  
		default:
			if(RecAddr != 0) //广播命令 不响应
			{
			  g_tModS.RspCode = RSP_ERR_CMD;
			  MODS_SendAckErr(g_tModS.RspCode);	/* 告诉主机命令错误 */
			}
			break;
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
uint8_t MODS_ReadRegValue(uint16_t reg_addr, uint8_t *pReg_value)
{
	uint8_t  ret;
	int32_t s32Tem;
	
	s32Tem = 0;
	ret = 1;
	/*******************************************/
	switch (reg_addr)									/* 判断寄存器地址 */
	{
		case REG_DMM_VRMS: 
		{
           s32Tem = ADC_Info.DMM_VRMS;

		   break;	
		}	
	  case REG_DMM_VP_RMS: 
		{
           s32Tem = ADC_Info.DMM_VP_RMS;

		   break;	
		}	
	   case REG_DMM_VN_RMS: 
		{
           s32Tem = ADC_Info.DMM_VN_RMS;

		   break;	
		}	
	   case REG_DMM_VPN_RMS: 
		{
           s32Tem = ADC_Info.DMM_VRMS2;

		   break;	
		}	
		case REG_DMM_RLY: 
		{
           s32Tem = DMM_Flags.Value;

		   break;	
		}	
	   case REG_RLY: 
		{
            s32Tem = RLY_Flags.Value;
	
 			break;	
		}	
		
	   case REG_IN: 
		{
            s32Tem = IN_Flags.Value;

 		    break;	
		}	
		
		
        case REG_V_K: 
		{
	       s32Tem = gEEP_data.DMM_V_K ;

		   break;	
		}	
		
        case REG_V_B: 
		{
			s32Tem = gEEP_data.DMM_V_B ;

 			break;	
		}	
		
        case REG_V2_K: 
		{
			s32Tem = gEEP_data.DMM_V2_K ;

 			break;	
		}	

        case REG_V2_B: 
		{
			s32Tem = gEEP_data.DMM_V2_B ;

 			break;	
		}	

	  case REG_FAWREVER://软件版本
		{
             s32Tem = (uint16_t)FIREWARE_VER ;

 			break;	
		}

	
	}

	*(pReg_value+0) = s32Tem >> 8;
	*(pReg_value+1) = s32Tem;

	return ret;											/* 读取成功 */
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
 uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value)
{
	uint8_t  ret ;
	int16_t i16Tem;
	ret = 1;
	i16Tem = 0;
	switch (reg_addr)							/* 判断寄存器地址 */
	{	
		case REG_DMM_RLY: 
		{
            DMM_Flags.Value = reg_value;

		   break;	
		}	
	   case REG_RLY: 
		{
             RLY_Flags.Value = reg_value;

 			break;	
		}	
       case REG_V_K: 
		{
	       gEEP_data.DMM_V_K  = reg_value ;

		   break;	
		}	
		
        case REG_V_B: 
		{
			 gEEP_data.DMM_V_B   = reg_value;

 			break;	
		}	
		
        case REG_V2_K: 
		{
			gEEP_data.DMM_V2_K   = reg_value;

 			break;	
		}	

        case REG_V2_B: 
		{
			gEEP_data.DMM_V2_B  = reg_value ;

 			break;	
		}	
        case REG_EEP_SAVE:
		{
			if(reg_value ==0xAA55 ) //保存EEP 数据
			{
				Flash_EEP_Write();
 			    ret = 1;
			}
 			break;	
		}	

	}

	return ret;
}

/*
*********************************************************************************************************
*	函 数 名: MODS_03H
*	功能说明: 读取保持寄存器 在一个或多个保持寄存器中取得当前的二进制值
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
 void MODS_03H(void)
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

	reg = BEBufToUint16(&g_tModS.pRxBuf[2]); 				/* 寄存器号 */
	num = BEBufToUint16(&g_tModS.pRxBuf[4]);					/* 寄存器个数 */
	if(num == 0)
     {  
	     g_tModS.RspCode = RSP_ERR_VALUE;
	     goto err_ret;
	 }
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
		g_tModS.pTxBuf[g_tModS.TxCount++] = g_tModS.pRxBuf[0];
		g_tModS.pTxBuf[g_tModS.TxCount++] = g_tModS.pRxBuf[1];
		g_tModS.pTxBuf[g_tModS.TxCount++] = num * 2;			/* 返回字节数 */
       
		for (i = 0; i < num; i++)
		{
			g_tModS.pTxBuf[g_tModS.TxCount++] = reg_value[2*i];
			g_tModS.pTxBuf[g_tModS.TxCount++] = reg_value[2*i+1];
		}
		
		MODS_SendWithCRC(g_tModS.pTxBuf, g_tModS.TxCount);	/* 发送正确应答 */
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
 void MODS_05H(void)
{
	uint16_t reg;
	uint16_t value;
    uint16_t index;
	
	g_tModS.RspCode = RSP_OK;
	
	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* 数据值域错误 */
		goto err_ret;
	}

	reg   = BEBufToUint16(&g_tModS.pRxBuf[2]); 	/* 寄存器号 */
	value = BEBufToUint16(&g_tModS.pRxBuf[4]);	/* 数据 */
	
	switch (reg)							/* 判断寄存器地址 */
	{	

	    case REG_RLY1: 
	    case REG_RLY2: 
		case REG_RLY3: 
		case REG_RLY4: 
	    case REG_RLY5: 
	    case REG_RLY6: 
		case REG_RLY7: 
		case REG_RLY8:
		{
			index = reg-REG_RLY1;
			if(value == 0xFF00)
			{
               RLY_Flags.Value |= (1<<index);
			}
			else if(value == 0)
			{
			   RLY_Flags.Value &= ~(1<<index);	
			}
		   break;	
		}	
	    case REG_DMM1: 
	    case REG_DMM2: 
		case REG_DMM3: 
		case REG_DMM4: 
	    case REG_DMM5: 
	    case REG_DMM6: 
		case REG_DMM7: 
		case REG_DMM8:
		{
  
		   index = reg-REG_DMM1;
			if(value == 0xFF00)
			{
               DMM_Flags.Value = (1<<index);
			}
			else if(value == 0)
			{
			   DMM_Flags.Value = 0;	
			}
 		   break;	
		}		
 
		default:
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址错误 */
			break;
		}
	}

	
 

 
err_ret:
	if(g_tModS.pRxBuf[0] != 0) //广播命令 不响应
	{
		if (g_tModS.RspCode == RSP_OK)				/* 正确应答 */
		{
			MODS_SendAckOk();
		}
		else
		{
			MODS_SendAckErr(g_tModS.RspCode);		/* 告诉主机命令错误 */
		}
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
 void MODS_06H(void)
{

	uint16_t reg;
	uint16_t value;

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* 数据值域错误 */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.pRxBuf[2]); 	/* 寄存器号 */
	value = BEBufToUint16(&g_tModS.pRxBuf[4]);	/* 寄存器值 */

	if (MODS_WriteRegValue(reg, value) == 1)	/* 该函数会把写入的值存入寄存器 */
	{
		;
	}
	else
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址错误 */
	}

err_ret:
	if(g_tModS.pRxBuf[0] != 0)//广播命令 不响应
	{
		if (g_tModS.RspCode == RSP_OK)				/* 正确应答 */
		{
			MODS_SendAckOk();
		}
		else
		{
			MODS_SendAckErr(g_tModS.RspCode);		/* 告诉主机命令错误 */
		}
   }
}


/*****************************  *********************************/
