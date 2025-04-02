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
*	�� �� ��: Modbus_Poll_Task
*	����˵��: �������ݰ�. ���߳��д���
*	��    ��: ��
*	�� �� ֵ: ��
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

	if (g_tModS.RxCount < 4)				/* ���յ�������С��4���ֽھ���Ϊ���� */
	{
		goto err_ret;
	}

	/* ����CRCУ��� */
	crc1 = CRC16_Modbus(g_tModS.RxBuf, g_tModS.RxCount-2);
	if (crc1 != ((g_tModS.RxBuf[g_tModS.RxCount-2]<<8)+ (g_tModS.RxBuf[g_tModS.RxCount-1]<<0)))
	{
		goto err_ret;
	}

	/* վ��ַ (1�ֽڣ� */
	addr = g_tModS.RxBuf[0];				/* ��1�ֽ� վ�� */
	if ((addr != SAddr)&&(addr != 0))	/* �ж��������͵������ַ�Ƿ���� */
	{
		goto err_ret;
	}


	if (g_tModS.RxBuf[1] == 0)		 			/* �ж��������͵������Ƿ���� */
	{
		goto err_ret;
	}
	
	/* ����Ӧ�ò�Э�� */
	MODS_AnalyzeApp();	

	memset(g_tModS.RxBuf,0,g_tModS.RxCount);
	g_tModS.RxCount = 0;					/* ��������������������´�֡ͬ�� */
	bRS485_Timeout_Flag=0;
	bRS485_RX_Flag=0;	
	
	return;
err_ret:
 	//MODS_SendAckErr(RSP_ERR_VALUE);
 	memset(g_tModS.RxBuf,0,g_tModS.RxCount);
	g_tModS.RxCount = 0;					/* ��������������������´�֡ͬ�� */
	bRS485_Timeout_Flag=0;
	bRS485_RX_Flag=0;
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
*	�� �� ��: MODS_SendAckErr
*	����˵��: ���ʹ���Ӧ��
*	��    ��: _ucErrCode : �������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_SendAckErr(uint8_t _ucErrCode)
{
	uint8_t txbuf[3]={0};

	txbuf[0] = g_tModS.RxBuf[0];					/* 485��ַ */
	txbuf[1] = g_tModS.RxBuf[1] | 0x80;				/* �쳣�Ĺ����� */
	txbuf[2] = _ucErrCode;							/* �������(01,02,03,04) */

	MODS_SendWithCRC(txbuf, 3);
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_SendAckOk
*	����˵��: ������ȷ��Ӧ��.
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: MODS_AnalyzeApp
*	����˵��: ����Ӧ�ò�Э��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_AnalyzeApp(void)
{
	switch (g_tModS.RxBuf[1])				/* ��2���ֽ� ������ */
	{

		case 0x04:		
		case 0x03:							/* ��ȡ���ּĴ���*/
		 {
			MODS_03H();
			break;
		 }
		case 0x06:							/* д��������Ĵ���*/
		  {   
			MODS_06H();	
 			break;
		  }
		default:
		{
			g_tModS.RspCode = RSP_ERR_CMD;
			MODS_SendAckErr(g_tModS.RspCode);	/* ��������������� */
			break;
		}
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
static uint8_t MODS_ReadRegValue(uint16_t reg_addr, uint8_t *reg_value)
{
	uint16_t value;
	
	switch (reg_addr)									/* �жϼĴ�����ַ */
	{
        case 1://����汾
		  {
			value =	Power_MeasData.SoftVer;	
			break;
		  }
 
        case 2://�����ѹ
		  {
			value =Power_MeasData.Vout;	
			break;
		  }
         case 3://�������
		  {
			value =	Power_MeasData.Iout;
			break;
		  }
		 case 4://���Ƶ��
		  {
			 
			value =	 Power_MeasData.Fre_Out;	
			break;
		  }
	    case 5://�¶�1
		  {
			  
			value =  Power_MeasData.T1;	
			break;
		  }
		 case 6://�¶�2
		  {
			value =  Power_MeasData.T2;		
			break;
		  }
	    case 7://�����ѹ
		  {
			 
			value =	 Power_MeasData.Vin;	
			break;
		  }
		 case 8://��Դ״̬
		  {
			   
			value =	PS_Status.Value;	
			break;
		  }
	     case 9://��Դ�ź�״̬
		  {
			value = PS_IO_Status.Value;	
			break;
		  }
		 case 10://�ֶ���������ֵ
		  {
			
			value = Power_MeasData.ExSetIout;	
			break;
		  }
		case 11://�ź���������
		  {
			 
			value =	Power_MeasData.ExSetPeriond;	
			break;
		  }
		 case 14://�����������ֵ
		  {
			
			value =	  Ps_SetPara.SetIout;	
			break;
		  }
		 case 15://�������Ƶ��
		  {
			   
			value =	 Ps_SetPara.SetFre;	
			break;
		  }
	     case 16://�����������
		  {
			  
			value =	 Ps_SetPara.SetDuty;	
			break;
		  }
		 case 17://���������ѹ
		  {
			   
			value =	Ps_SetPara.SetVout;	
			break;
		  }
	    case 18://�����������ֵ
		  {
			 
			value =	 EEP_Data.Iout_Limt;	
			break;
		  }
		 case 19://Զ������־
		  {
			  value =	bRemoteLock?0x0A:00;	
			break;
		  }
	    case 20://�����ź�ģ����K
		  {
			  
			value =	EEP_Data.ExVref_K;	
			break;
		  }
		case 21://�����ź�ģ����B
		  {
			value = EEP_Data.ExVref_B;	
			break;
		  }
		case 22://�������У׼  Kֵ
		  {
			 
			value =  EEP_Data.Iout_K;	
			break;
		  }
		case 23://���������У׼ Bֵ
		  {
			value =	  EEP_Data.Iout_B;	
			break;
		  }
	    case 24://�����ѹУ׼  Kֵ[8000,12000]
		  {
			  
			value =	 EEP_Data.Vout_K;	
			break;
		  }
	   case 25://�����ѹУ׼  Bֵ 
		  {
			value =  EEP_Data.Vout_B;	
			break;
		  }
	  case 26://��������У׼K
		  {
			  
			value =EEP_Data.Set_CC_K;	
			break;
		  }
	  case 27://��������У׼B
		  {
			value =	EEP_Data.Set_CC_B;	
			break;
		  }
	 case 28://���õ�ѹУ׼  Kֵ
		  {
			 
			value =	 EEP_Data.Set_Vout_K;	
			break;
		  }
	 case 29://���õ�ѹУ׼  Bֵ
		  {
			value =	 EEP_Data.Set_Vout_B;	
			break;
		  }
		default:
			return 0;									/* �����쳣������ 0 */
	}

	reg_value[0] = value >> 8;
	reg_value[1] = value;

	return 1;											/* ��ȡ�ɹ� */
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
static uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value)
{
	int16_t i16_Value;
	switch (reg_addr)							/* �жϼĴ�����ַ */
	{	
			
		 case 14://�����������ֵ
		  {
			if(reg_value <2000) Ps_SetPara.SetIout=reg_value;	
			 else return 0;
			break;
		  }
		 case 15://�������Ƶ��
		  {
			   
			if((reg_value <= 20000) && (reg_value >= 50))  Ps_SetPara.SetFre = reg_value;
            else Ps_SetPara.SetFre = 0;			  
			break;
		  }
	     case 16://�����������
		  {
			  
			  
			if((reg_value <= 5000)  )   Ps_SetPara.SetDuty = reg_value;
            else return 0;				  
			break;
		  }
		 case 17://���������ѹ
		  {
			
			if((reg_value <= 300) && (reg_value >= 150)) 	Ps_SetPara.SetVout = reg_value;
            else return 0;					  
 
			break;
		  }
	    case 18://�����������ֵ
		  {
			if(reg_value <= 1400) 	 EEP_Data.Iout_Limt = reg_value;
            else return 0;		 
              EEP_RegSave(EEP_DataOffset+10*2,reg_value);;
			break;
		  }
		 case 19://Զ������־
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
	    case 20://�����ź�ģ����K
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
		case 21://�����ź�ģ����B
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
		case 22://�������У׼  Kֵ
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
		case 23://���������У׼ Bֵ
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
	    case 24://�����ѹУ׼  Kֵ[8000,12000]
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
	   case 25://�����ѹУ׼  Bֵ 
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
	  case 26://��������У׼K
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
	  case 27://��������У׼B
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
	 case 28://���õ�ѹУ׼  Kֵ
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
	 case 29://���õ�ѹУ׼  Bֵ
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
			return 0;		/* �����쳣������ 0 */
	}

	return 1;		/* ��ȡ�ɹ� */
}

/*
*********************************************************************************************************
*	�� �� ��: MODS_03H
*	����˵��: ��ȡ���ּĴ��� ��һ���������ּĴ�����ȡ�õ�ǰ�Ķ�����ֵ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODS_03H(void)
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

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 				/* �Ĵ����� */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);					/* �Ĵ������� */
	if(num == 0){g_tModS.RspCode = RSP_ERR_VALUE;goto err_ret;}
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
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0];
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1];
		g_tModS.TxBuf[g_tModS.TxCount++] = num * 2;			/* �����ֽ��� */
       
		for (i = 0; i < num; i++)
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i];
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i+1];
		}
		
		MODS_SendWithCRC(g_tModS.TxBuf, g_tModS.TxCount);	/* ������ȷӦ�� */
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
static void MODS_05H(void)
{
	uint16_t reg;
	uint16_t value;

	g_tModS.RspCode = RSP_OK;
	
	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}

	reg   = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* �Ĵ����� */
	value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* ���� */
	
	if (value != 0xFF00 && value != 0x0000)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}
	

		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */

err_ret:
	if (g_tModS.RspCode == RSP_OK)				/* ��ȷӦ�� */
	{
		MODS_SendAckOk();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);		/* ��������������� */
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
static void MODS_06H(void)
{

	uint16_t reg;
	uint16_t value;

	g_tModS.RspCode = RSP_OK;

	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* ����ֵ����� */
		goto err_ret;
	}

	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* �Ĵ����� */
	value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* �Ĵ���ֵ */

	if (MODS_WriteRegValue(reg, value) == 1)	/* �ú������д���ֵ����Ĵ��� */
	{
		;
	}
	else
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* �Ĵ�����ַ���� */
	}

err_ret:
	if (g_tModS.RspCode == RSP_OK)				/* ��ȷӦ�� */
	{
		MODS_SendAckOk();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);		/* ��������������� */
	}
}


/*****************************  *********************************/
