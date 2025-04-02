#include "main.h"

PS_EEPStruct EEP_Data;

BootLoader_Info_def   EEP_BootData;


/*
*********************************************************************************************************
*	�� �� ��: MODS_SendWithCRC
*	����˵��: ����һ������, �Զ�׷��2�ֽ�CRC
*	��    ��: _pBuf ���ݣ�
*			  _ucLen ���ݳ��ȣ�����CRC��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void EEPDataWithCRC(uint8_t *_pBuf, uint8_t _ucLen)
{
	uint16_t crc;
	uint8_t buf[128+2];

	memcpy(buf, _pBuf, _ucLen);
	crc = CRC16_Modbus(_pBuf, _ucLen);
	buf[_ucLen++] = crc >> 8;
	buf[_ucLen++] = crc;



	

}
/**************************************************************************************
* FunctionName   : void  Read_EEP()
* Description    : ��ȡEEP������
* EntryParameter : None
* ReturnValue    : 1 �ɹ�  0 ʧ��
**************************************************************************************/

uint8_t Read_EEP()
{
 
  uint8_t  uLen,i;
  
 uLen=sizeof(PS_EEPStruct);

	
 if(ee_ReadBytes((uint8_t*)&EEP_Data,EEP_Offset,uLen) )
    {   
	  if(EEP_Data.EEPFlag == 0xAA55)
       {
          for(i=0;i<12;i++)
		  {
		    if((EEP_Data.DAC_K[i] >12000)||(EEP_Data.DAC_K[i] < 8000)) EEP_Data.DAC_K[i]  =10000;
            if((EEP_Data.DAC_B[i] < -5000)||(EEP_Data.DAC_B[i] > 5000)) EEP_Data.DAC_B[i]  = 0;
		  }
	      bEEP_ERROR=0;
		 return 1;
	  }
	  else
	  {
 
		  EEP_Data.EEPFlag=0XAA55;
         for(i=0;i<12;i++)
		  {
		     EEP_Data.DAC_K[i]  =10000;
             EEP_Data.DAC_B[i]  = 0;
			
		  }

		  if(ee_WriteBytes((uint8_t*)&EEP_Data,EEP_Offset,uLen))
		  {
			   bEEP_ERROR=0;  
			   return 1;
		  }
		  else
		  {
             bEEP_ERROR=1;
		  }
 
          return 0;
	  }

  }
  else
  {
         for(i=0;i<12;i++)
		{
		     EEP_Data.DAC_K[i]  =10000;
             EEP_Data.DAC_B[i]  = 0;
			
		}

		EEP_Data.EEPFlag=0XAA55;
	   if(ee_WriteBytes((uint8_t*)&EEP_Data,EEP_Offset,uLen))
		  {
			  bEEP_ERROR=0;  
			  return 1;
		  }
		  else
		  {
             bEEP_ERROR=1;
		  }
 
	  return 0;
  }

}

/**************************************************************************************
* FunctionName   : EEP_Save
* Description    : ��������
* EntryParameter : None
* ReturnValue    : 0 ����ʧ��  1 ����ɹ�
**************************************************************************************/

uint8_t  EEP_Save()
{
    uint8_t  uLen;
	uLen=sizeof(PS_EEPStruct);
   if(ee_WriteBytes((uint8_t*)&EEP_Data,EEP_Offset,uLen))
     {
		 bEEP_ERROR=0;  
		 return 1;
	 }
   else
	 {
        bEEP_ERROR=1;
     }
 
  return 0;
}

/**************************************************************************************
* FunctionName   : EEP_RegSave
* Description    : ����ָ���Ĵ�����������
* EntryParameter : None
* ReturnValue    : 0 ����ʧ��  1 ����ɹ�
**************************************************************************************/

uint8_t  EEP_RegSave(uint8_t RegAddr,uint16_t RegValue)
{
   if(ee_WriteBytes((uint8_t*)&RegValue,RegAddr,2))
     {
		 bEEP_ERROR=0;  
		 return 1;
	 }
   else
	 {
        bEEP_ERROR=1;
     }
 
  return 0;
}

/**************************************************************************************
* FunctionName   : EEP_RegSave
* Description    : ����ָ���Ĵ�����������
* EntryParameter : None
* ReturnValue    : 0 ����ʧ��  1 ����ɹ�
**************************************************************************************/

uint8_t  EEP_DAC_KBWrite(uint8_t RegAddr,uint16_t K,int16_t B)
{
   if(ee_WriteBytes((uint8_t*)&K,2*RegAddr+EEP_DAC_K_Offset,2)==0)
     {
		 bEEP_ERROR=0;  
		 return 0;
	 }

	 
	 if(ee_WriteBytes((uint8_t*)&B,2*RegAddr+EEP_DAC_B_Offset,2) == 0)
     {
		 bEEP_ERROR=0;  
		 return 0;
	 }

 
     return 1;
}

/**************************************************************************************
* FunctionName   : EEP_BootEntry()
* Description    : ��������
* EntryParameter : None
* ReturnValue    : 0 ����ʧ��  1 ����ɹ�
**************************************************************************************/

uint8_t  EEP_BootEntry()
{
    uint8_t  uLen;
	EEP_BootData.bootFlag=0xA5;//����BootLoader
	uLen=sizeof(EEP_BootData);
   if(ee_WriteBytes((uint8_t*)&EEP_BootData,EEP_Boot_Offset,uLen))
     {
		 return 1;
	 }

     return 0;
}


