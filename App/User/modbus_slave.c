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
		 case 0x10:							/* д�������Ĵ����������̸�дg_tVar�еĲ�����*/
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
		case REG_DA1_ONOFF: 
		{
			s32Tem =  gEEP_data.DA1_ONOFF;
			break;	
		}
	  case REG_DA1_5V_ONOFF: 
		{
           s32Tem =  gEEP_data.DA1_5V_EN;

		   break;	
		}	
	   case REG_DA1_Out_SoftStartEnable: 
		{
           s32Tem =  gEEP_data.DA1_Out_Mode;

		   break;	
		}	
	   case REG_DA1_PWM_Enable: 
		{
           s32Tem =  gEEP_data.DA1_PWM_EN;

		   break;	
		}	
		case REG_DA1_V: 
		{
		   s32Tem =  gEEP_data.DA1_V;

		   break;	
		}
 
	   case REG_DA1_PWM_FRE: 
		{
       
			s32Tem =  gEEP_data.DA1_FRE;
 			break;	
		}	
		
	   case REG_DA1_PWM_DUTY: 
		{
			s32Tem =  gEEP_data.DA1_Duty;

 		    break;	
		}	
		case REG_DA2_ONOFF: 
		{
			s32Tem =  gEEP_data.DA2_ONOFF;
			break;	
		}
	  case REG_DA2_5V_ONOFF: 
		{
           s32Tem =  gEEP_data.DA2_5V_EN;

		   break;	
		}	
	   case REG_DA2_Out_SoftStartEnable: 
		{
           s32Tem =  gEEP_data.DA2_Out_Mode;

		   break;	
		}	
	   case REG_DA2_PWM_Enable: 
		{
           s32Tem =  gEEP_data.DA2_PWM_EN;

		   break;	
		}	
		case REG_DA2_V: 
		{
		   s32Tem =  gEEP_data.DA2_V;

		   break;	
		}
 
	   case REG_DA2_PWM_FRE: 
		{
       
			s32Tem =  gEEP_data.DA2_FRE;
 			break;	
		}	
		
	   case REG_DA2_PWM_DUTY:
		{
			s32Tem =  gEEP_data.DA2_Duty;

 		    break;	
		}	
		
		
        case REG_V_K: 
		{
	       s32Tem = gEEP_data.DA1_V_K ;

		   break;	
		}	
		
        case REG_V_B: 
		{
			s32Tem = gEEP_data.DA1_V_B ;

 			break;	
		}	
		
        case REG_V2_K: 
		{
			s32Tem = gEEP_data.DA2_V_K ;

 			break;	
		}	

        case REG_V2_B: 
		{
			s32Tem = gEEP_data.DA2_V_B ;

 			break;	
		}	

	  case REG_FAWREVER://����汾
		{
             s32Tem = (uint16_t)FIREWARE_VER ;

 			break;	
		}
		default:
		{
			s32Tem = 0;									/* �Ĵ�����ַ���� */
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
		case REG_DA1_ONOFF: 
		{
			  gEEP_data.DA1_ONOFF = reg_value;
			break;	
		}
	  case REG_DA1_5V_ONOFF: 
		{
            gEEP_data.DA1_5V_EN = reg_value;

		   break;	
		}	
	   case REG_DA1_Out_SoftStartEnable: 
		{
           gEEP_data.DA1_Out_Mode = reg_value;

		   break;	
		}	
	   case REG_DA1_PWM_Enable: 
		{
            gEEP_data.DA1_PWM_EN = reg_value;

		   break;	
		}	
		case REG_DA1_V: 
		{
		    gEEP_data.DA1_V = reg_value;

		   break;	
		}
 
	   case REG_DA1_PWM_FRE: 
		{
       
			 gEEP_data.DA1_FRE = reg_value;
 			break;	
		}	
		
	   case REG_DA1_PWM_DUTY: 
		{
			  gEEP_data.DA1_Duty = reg_value;

 		    break;	
		}	
		case REG_DA2_ONOFF: 
		{
			  gEEP_data.DA2_ONOFF = reg_value;
			break;	
		}
	  case REG_DA2_5V_ONOFF: 
		{
            gEEP_data.DA2_5V_EN = reg_value;

		   break;	
		}	
	   case REG_DA2_Out_SoftStartEnable: 
		{
            gEEP_data.DA2_Out_Mode = reg_value;

		   break;	
		}	
	   case REG_DA2_PWM_Enable: 
		{
            gEEP_data.DA2_PWM_EN = reg_value;

		   break;	
		}	
		case REG_DA2_V: 
		{
		     gEEP_data.DA2_V = reg_value;

		   break;	
		}
 
	   case REG_DA2_PWM_FRE: 
		{
       
			  gEEP_data.DA2_FRE = reg_value;
 			break;	
		}	
		
	   case REG_DA2_PWM_DUTY: 
		{
		    gEEP_data.DA2_Duty = reg_value;

 		    break;	
		}	
       case REG_V_K: 
		{
	       gEEP_data.DA1_V_K  = reg_value ;

		   break;	
		}	
		
        case REG_V_B: 
		{
			 gEEP_data.DA1_V_B   = reg_value;

 			break;	
		}	
		
        case REG_V2_K: 
		{
			gEEP_data.DA2_V_K   = reg_value;

 			break;	
		}	

        case REG_V2_B: 
		{
			gEEP_data.DA2_V_B  = reg_value ;

 			break;	
		}	
        case REG_EEP_SAVE:
		{
			if(reg_value ==0xAA55 ) //����EEP ����
			{
				//Flash_EEP_Write();
				WorkSpace.WorkFlags.bit.EEP_UpdateFlag = 1;
 			    ret = 1;
			}
 			break;	
		}	
		case REG_EEP_INIT:
		{
			if(reg_value ==0xAA55 ) //����EEP ����
			{
				InitEEP();
				//Flash_EEP_Write();
				WorkSpace.WorkFlags.bit.EEP_UpdateFlag = 1;
	 
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
	uint8_t reg_value[140];

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)								/* 03H���������8���ֽ� */
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* ����ֵ����� */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.pRxBuf[2]); 				/* �Ĵ����� */
	num = BEBufToUint16(&g_tModS.pRxBuf[4]);				/* �Ĵ������� */
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

/*
*********************************************************************************************************
*	�� �� ��: MODS_10H
*	����˵��: ����д����Ĵ���.  �����ڸ�дʱ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_10H(void)
{
	/*
		�ӻ���ַΪ11H�����ּĴ�������ʵ��ַΪ0001H���Ĵ����Ľ�����ַΪ0002H���ܹ�����2���Ĵ�����
		���ּĴ���0001H������Ϊ000AH�����ּĴ���0002H������Ϊ0102H��

		��������:
			11 �ӻ���ַ
			10 ������
			00 �Ĵ�����ʼ��ַ���ֽ�
			01 �Ĵ�����ʼ��ַ���ֽ�
			00 �Ĵ����������ֽ�
			02 �Ĵ����������ֽ�
			04 �ֽ���
			00 ����1���ֽ�
			0A ����1���ֽ�
			01 ����2���ֽ�
			02 ����2���ֽ�
			C6 CRCУ����ֽ�
			F0 CRCУ����ֽ�

		�ӻ���Ӧ:
			11 �ӻ���ַ
			06 ������
			00 �Ĵ�����ַ���ֽ�
			01 �Ĵ�����ַ���ֽ�
			00 ����1���ֽ�
			01 ����1���ֽ�
			1B CRCУ����ֽ�
			5A	CRCУ����ֽ�

		����:
			01 10 30 00 00 06 0C  07 DE  00 0A  00 01  00 08  00 0C  00 00     389A    ---- дʱ�� 2014-10-01 08:12:00
			01 10 30 00 00 06 0C  07 DF  00 01  00 1F  00 17  00 3B  00 39     5549    ---- дʱ�� 2015-01-31 23:59:57

	*/
	uint16_t reg_addr;
	uint16_t reg_num;
	uint8_t byte_num;
	uint8_t i;
	uint16_t value;
	
	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount < 11)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;			/* ����ֵ����� */
		goto err_ret;
	}

	reg_addr = BEBufToUint16(&g_tModS.pRxBuf[2]); 	/* �Ĵ����� */
	reg_num = BEBufToUint16(&g_tModS.pRxBuf[4]);		/* �Ĵ������� */
	byte_num = g_tModS.pRxBuf[6];					/* ������������ֽ��� */

	if (byte_num != 2 * reg_num)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;
	}
	
	for (i = 0; i < reg_num; i++)
	{
		value = BEBufToUint16(&g_tModS.pRxBuf[7 + 2 * i]);	/* �Ĵ���ֵ */

		if (MODS_WriteRegValue(reg_addr + i, value) == 1)
		{
			;
		}
		else
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
			break;
		}
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
