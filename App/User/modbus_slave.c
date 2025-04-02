/*
*********************************************************************************************************
*
*	ģ������ : MODSͨ��ģ��. ��վģʽ
*	�ļ����� : modbus_slave.c
*	��    �� : V1.4
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
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
*	�� �� ��: Modbus_Init
*	����˵��: Modbus ����ע��
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: Modbus_Poll_Task
*	����˵��: �������ݰ�. ���߳��д���
*	��    ��: ��
*	�� �� ֵ: ��
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

	if (g_tModS.RxCount < 4)				/* ���յ�������С��4���ֽھ���Ϊ���� */
	{
		goto err_ret;
	}

	/* ����CRCУ��� */
	crc1 = CRC16_Modbus(g_tModS.pRxBuf, g_tModS.RxCount-2);
	if (crc1 != ((g_tModS.pRxBuf[g_tModS.RxCount-2]<<8)+ (g_tModS.pRxBuf[g_tModS.RxCount-1]<<0)))
	{
		goto err_ret;
	}

	/* վ��ַ (1�ֽڣ� */
	addr = g_tModS.pRxBuf[0];				  /* ��1�ֽ� վ�� */
	if ((addr != g_tModS.addr)&&(addr != 0))  /* �ж��������͵������ַ�Ƿ���� */
	{
		goto err_ret;
	}


	if (g_tModS.pRxBuf[1] == 0)		 			/* �ж��������͵������Ƿ���� */
	{
		goto err_ret;
	}
	
	/* ����Ӧ�ò�Э�� */
	MODS_AnalyzeApp();	

	memset(g_tModS.pRxBuf,0,g_tModS.RxCount);
	g_tModS.RxCount = 0;					/* ��������������������´�֡ͬ�� */
	return;
err_ret:
 	 
 	memset(g_tModS.pRxBuf,0,g_tModS.RxCount);
	g_tModS.RxCount = 0;					/* ��������������������´�֡ͬ�� */
}


/*
*********************************************************************************************************
*	�� �� ��: MODS_SendWithCRC
*	����˵��: ����һ������, �Զ�׷��2�ֽ�CRC
*	��    ��: _pBuf ���ݣ�
*			  _ucLen ���ݳ��ȣ�����CRC��
*	�� �� ֵ: ��
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
*	�� �� ��: MODS_SendAckErr
*	����˵��: ���ʹ���Ӧ��
*	��    ��: _ucErrCode : �������
*	�� �� ֵ: ��
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
*	�� �� ��: MODS_SendAckOk
*	����˵��: ������ȷ��Ӧ��.
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: MODS_AnalyzeApp
*	����˵��: ����Ӧ�ò�Э��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODS_AnalyzeApp(void)
{
	uint8_t RecAddr=0;
	RecAddr=g_tModS.pRxBuf[0];
	switch (g_tModS.pRxBuf[1])				/* ��2���ֽ� ������ */
	{
	
		case 0x03:	
		  {			/* ��ȡ���ּĴ���*/
			if(RecAddr != 0) //�㲥���� ����Ӧ
			 {
			   MODS_03H();
			 }
			 break;
		  }
		case 0x05:							/* ǿ�Ƶ���Ȧ*/
		  {
			MODS_05H();
			break;
		   }
		case 0x06:							/* д��������Ĵ����������̸�дg_tVar�еĲ�����*/
		 {
			MODS_06H();	
			break;
		 }  
		default:
			if(RecAddr != 0) //�㲥���� ����Ӧ
			{
			  g_tModS.RspCode = RSP_ERR_CMD;
			  MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
			}
			break;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_ReadRegValue
*	����˵��: ��ȡ���ּĴ�����ֵ
*	��    ��: reg_addr �Ĵ�����ַ
*			  reg_value ��żĴ������
*	�� �� ֵ: 1��ʾOK 0��ʾ����
*********************************************************************************************************
*/
uint8_t MODS_ReadRegValue(uint16_t reg_addr, uint8_t *pReg_value)
{
	uint8_t  ret;
	int32_t s32Tem;
	
	s32Tem = 0;
	ret = 1;
	/*******************************************/
	switch (reg_addr)									/* �жϼĴ�����ַ */
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

	  case REG_FAWREVER://����汾
		{
             s32Tem = (uint16_t)FIREWARE_VER ;

 			break;	
		}

	
	}

	*(pReg_value+0) = s32Tem >> 8;
	*(pReg_value+1) = s32Tem;

	return ret;											/* ��ȡ�ɹ� */
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_WriteRegValue
*	����˵��: ��ȡ���ּĴ�����ֵ
*	��    ��: reg_addr �Ĵ�����ַ
*			  reg_value �Ĵ���ֵ
*	�� �� ֵ: 1��ʾOK 0��ʾ����
*********************************************************************************************************
*/
 uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value)
{
	uint8_t  ret ;
	int16_t i16Tem;
	ret = 1;
	i16Tem = 0;
	switch (reg_addr)							/* �жϼĴ�����ַ */
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
			if(reg_value ==0xAA55 ) //����EEP ����
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
*	�� �� ��: MODS_03H
*	����˵��: ��ȡ���ּĴ��� ��һ���������ּĴ�����ȡ�õ�ǰ�Ķ�����ֵ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
 void MODS_03H(void)
{
	uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint8_t reg_value[64];

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)								/* 03H���������8���ֽ� */
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* ����ֵ����� */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.pRxBuf[2]); 				/* �Ĵ����� */
	num = BEBufToUint16(&g_tModS.pRxBuf[4]);					/* �Ĵ������� */
	if(num == 0)
     {  
	     g_tModS.RspCode = RSP_ERR_VALUE;
	     goto err_ret;
	 }
	if (num > (sizeof(reg_value) / 2))
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* ����ֵ����� */
		goto err_ret;
	}

	for (i = 0; i < num; i++)
	{
		if (MODS_ReadRegValue(reg, &reg_value[2 * i]) == 0)	/* �����Ĵ���ֵ����reg_value */
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;				/* �Ĵ�����ַ���� */
			break;
		}
		reg++;
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)							/* ��ȷӦ�� */
	{
		g_tModS.TxCount = 0;
		g_tModS.pTxBuf[g_tModS.TxCount++] = g_tModS.pRxBuf[0];
		g_tModS.pTxBuf[g_tModS.TxCount++] = g_tModS.pRxBuf[1];
		g_tModS.pTxBuf[g_tModS.TxCount++] = num * 2;			/* �����ֽ��� */
       
		for (i = 0; i < num; i++)
		{
			g_tModS.pTxBuf[g_tModS.TxCount++] = reg_value[2*i];
			g_tModS.pTxBuf[g_tModS.TxCount++] = reg_value[2*i+1];
		}
		
		MODS_SendWithCRC(g_tModS.pTxBuf, g_tModS.TxCount);	/* ������ȷӦ�� */
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);					/* ���ʹ���Ӧ�� */
	}
}


/*
*********************************************************************************************************
*	�� �� ��: MODS_05H
*	����˵��: ǿ�Ƶ���Ȧ����ӦD01/D02/D03��
*	��    ��: ��
*	�� �� ֵ: ��
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
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}

	reg   = BEBufToUint16(&g_tModS.pRxBuf[2]); 	/* �Ĵ����� */
	value = BEBufToUint16(&g_tModS.pRxBuf[4]);	/* ���� */
	
	switch (reg)							/* �жϼĴ�����ַ */
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
			g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
			break;
		}
	}

	
 

 
err_ret:
	if(g_tModS.pRxBuf[0] != 0) //�㲥���� ����Ӧ
	{
		if (g_tModS.RspCode == RSP_OK)				/* ��ȷӦ�� */
		{
			MODS_SendAckOk();
		}
		else
		{
			MODS_SendAckErr(g_tModS.RspCode);		/* ��������������� */
		}
    }
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_06H
*	����˵��: д�����Ĵ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
 void MODS_06H(void)
{

	uint16_t reg;
	uint16_t value;

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.pRxBuf[2]); 	/* �Ĵ����� */
	value = BEBufToUint16(&g_tModS.pRxBuf[4]);	/* �Ĵ���ֵ */

	if (MODS_WriteRegValue(reg, value) == 1)	/* �ú������д���ֵ����Ĵ��� */
	{
		;
	}
	else
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
	}

err_ret:
	if(g_tModS.pRxBuf[0] != 0)//�㲥���� ����Ӧ
	{
		if (g_tModS.RspCode == RSP_OK)				/* ��ȷӦ�� */
		{
			MODS_SendAckOk();
		}
		else
		{
			MODS_SendAckErr(g_tModS.RspCode);		/* ��������������� */
		}
   }
}


/*****************************  *********************************/
