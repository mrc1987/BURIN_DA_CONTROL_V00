#include "main.h"

ST_EEPStruct       gEEP_data;             //EEP 保存数据

BootLoader_Info_def   EEP_BootData;


 /**************************************************************************
*函数名： void InitEEP()
*描述：  
*返回：
*说明：
*
*****************************************************************************/
 
 void InitEEP()
 {
    gEEP_data.EEPFlag =0xA5;
	 
	gEEP_data.DA1_Out_Mode = 0; //缓慢和直接输出DA
	gEEP_data.DA1_PWM_EN   = 0;   //输出PWM开关	
	gEEP_data.DA1_5V_EN    = 0;    //5V 开关
	gEEP_data.DA1_ONOFF    = 0;    //5V 开关

	gEEP_data.DA2_Out_Mode = 0; //缓慢和直接输出DA
	gEEP_data.DA2_PWM_EN   = 0;   //输出PWM开关	
	gEEP_data.DA2_5V_EN    = 0;    //5V 开关
	gEEP_data.DA2_ONOFF    = 0;    //5V 开关


    gEEP_data.DA1_V = 5*1024;
	gEEP_data.DA2_V = 5*1024;

    gEEP_data.DA1_Duty = 50;
	gEEP_data.DA1_FRE  = 1000;
	 
	gEEP_data.DA2_Duty = 50;
	gEEP_data.DA2_FRE  = 1000;
	 
	 
	gEEP_data.DA1_V_K = 0;
	gEEP_data.DA1_V_B = 1<<10;
	 
 
	gEEP_data.DA2_V_K = 0;
	gEEP_data.DA2_V_B = 1<<10;

	 
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
* Description    : 读取EEP中数据
* EntryParameter : None
* ReturnValue    : 1 成功  0 失败
**************************************************************************************/

uint8_t Read_EEP()
{
 
  uint8_t  uLen,i;
  
 uLen=sizeof(ST_EEPStruct);

	
 if(ee_ReadBytes((uint8_t*)&gEEP_data,EEP_Offset,uLen) )
    {   
	  if(gEEP_data.EEPFlag == 0xA5)
       {
 
		 return 1;
	  }
	  else
	  {
 
          InitEEP();
		  if(ee_WriteBytes((uint8_t*)&gEEP_data,EEP_Offset,uLen))
		  {
 
			   return 1;
		  }
 
	  }

  }
  else
  {
       InitEEP();
	   if(ee_WriteBytes((uint8_t*)&gEEP_data,EEP_Offset,uLen))
		  {
 
			  return 1;
		  }
 
 

  }
  return 0;
}

/**************************************************************************************
* FunctionName   : EEP_Save
* Description    : 保存数据
* EntryParameter : None
* ReturnValue    : 0 保存失败  1 保存成功
**************************************************************************************/

uint8_t  EEP_Save()
{
    uint8_t  uLen;
	uLen=sizeof(ST_EEPStruct);
   if(ee_WriteBytes((uint8_t*)&gEEP_data,EEP_Offset,uLen))
     {
 
		 return 1;
	 }
 
 
  return 0;
}

/**************************************************************************************
* FunctionName   : EEP_SaveTask
* Description    : 保存数据
* EntryParameter : None
* ReturnValue    : 0 保存失败  1 保存成功
**************************************************************************************/

void  EEP_SaveTask()
{
    uint8_t  uLen;
	uint8_t  wLen;
    static uint8_t SaveAddrIndex = 0;
	uLen=sizeof(ST_EEPStruct);
	if(WorkSpace.WorkFlags.bit.EEP_UpdateFlag)
	{
		if(SaveAddrIndex < uLen)
		{
			if(SaveAddrIndex + EE_PAGE_SIZE > uLen)
			{
				wLen  = uLen - SaveAddrIndex;
			}
			else
			{
				wLen = EE_PAGE_SIZE;
			}
		  ee_WriteBytes((uint8_t*)&gEEP_data + SaveAddrIndex,EEP_Offset + SaveAddrIndex,wLen);
		  SaveAddrIndex += wLen;
		}
		else
		{
			SaveAddrIndex = 0;
			WorkSpace.WorkFlags.bit.EEP_UpdateFlag = 0;
		}
 
	}
	else
	{
		SaveAddrIndex =0;
	}
 
}


/**************************************************************************************
* FunctionName   : EEP_RegSave
* Description    : 保存指定寄存器数据数据
* EntryParameter : None
* ReturnValue    : 0 保存失败  1 保存成功
**************************************************************************************/

uint8_t  EEP_RegSave(uint8_t RegAddr,uint16_t RegValue)
{
   if(ee_WriteBytes((uint8_t*)&RegValue,RegAddr,2))
     {
 
		 return 1;
	 }
 
 
  return 0;
}

// /**************************************************************************************
// * FunctionName   : EEP_RegSave
// * Description    : 保存指定寄存器数据数据
// * EntryParameter : None
// * ReturnValue    : 0 保存失败  1 保存成功
// **************************************************************************************/

// uint8_t  EEP_DAC_KBWrite(uint8_t RegAddr,uint16_t K,int16_t B)
// {
//    if(ee_WriteBytes((uint8_t*)&K,2*RegAddr+EEP_DAC_K_Offset,2)==0)
//      {
 
// 		 return 0;
// 	 }

	 
// 	 if(ee_WriteBytes((uint8_t*)&B,2*RegAddr+EEP_DAC_B_Offset,2) == 0)
//      {
	 
// 		 return 0;
// 	 }

 
//      return 1;
// }

/**************************************************************************************
* FunctionName   : EEP_BootEntry()
* Description    : 保存数据
* EntryParameter : None
* ReturnValue    : 0 保存失败  1 保存成功
**************************************************************************************/

uint8_t  EEP_BootEntry()
{
    uint8_t  uLen;
	EEP_BootData.bootFlag=0xA5;//进入BootLoader
	uLen=sizeof(EEP_BootData);
   if(ee_WriteBytes((uint8_t*)&EEP_BootData,EEP_Boot_Offset,uLen))
     {
		 return 1;
	 }

     return 0;
}


