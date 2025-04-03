#include "APP_DAC.h"
#include "DRV_DAC.h"
#include "main.h"

#define  q10_DA_FACTOR   68L    //1/15V*1024 = 
 

uint16_t u16SetIout_Ref;          //�����������
uint16_t u16SetVout_Ref;          //���������ѹ

int16_t pre_POW_ERR;
 

/*
*********************************************************************************************************
*	�� �� ��: Set_DAC_OUT
*	����˵��: ��������ο�
*	��    ��: DAC_OUT : ���õ������ѹ  
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void Set_DAC_OUT(U8 DAC_CH,uint16_t DAC_OUT)
{
    uint32_t u32Tem;
    U16 q10_Duty;
    u32Tem = DAC_OUT*q10_DA_FACTOR;
    q10_Duty = (u32Tem>>10);
	  DA_SetValue(DAC_CH,   q10_Duty);
 
}


 


 

/*
*********************************************************************************************************
*	�� �� ��: DAC_Process
*	����˵��: DAC���ô���
*	��    ��: 
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void DAC_Process(void)
{

    S32 s32tem = 0;
    static U8 Run_Step1 = 0;
    static U32 DA1_Value = 0;
    static U32 DA2_Value = 0;

    static U16 DA1_ON_DelayTime = 0;
    static U16 DA2_ON_DelayTime = 0;

 
  /********************DAC1-OUT****************/
  s32tem = WorkSpace.DA1_V*gEEP_data.DA1_V_K + gEEP_data.DA1_V_B;
  s32tem = s32tem>>10;
 


 
   /*******************DAC1-Softstart************************* */
  if(WorkSpace.WorkFlags.BIT.DA1_CNTL_Rise_Mode == DA_SLOW_RISE_Mode)
  {
     if( WorkSpace.WorkFlags.BIT.DA1_SoftStart_Flag == 0)
     {
       if(s32tem > DA1_Value)
       {
          DA1_Value += 2;
    
       }
       else
       {
          DA1_Value = s32tem;
          WorkSpace.WorkFlags.BIT.DA1_SoftStart_Flag = 1;
       }
     }
     else
     {
       Set_DAC_OUT(DAC1_CH ,s32tem);
     }
  }
  else  
  {
     Set_DAC_OUT(DAC1_CH ,s32tem);
  }


    /********************DAC2-OUT****************/
    s32tem = WorkSpace.DA2_V*gEEP_data.DA2_V_K + gEEP_data.DA2_V_B;
    s32tem = s32tem>>10;
   /*******************DAC2-Softstart************************* */
   if(WorkSpace.WorkFlags.BIT.DA1_CNTL_Rise_Mode == DA_SLOW_RISE_Mode)
   {
      if( WorkSpace.WorkFlags.BIT.DA1_SoftStart_Flag == 0)
      {
        if(WorkSpace.WorkFlags.BIT.DA1_SoftStart_EN )
        {
            if(s32tem > DA1_Value) 
            {
              DA1_Value += 2;
            }
            else
            {
              DA1_Value = s32tem;
              WorkSpace.WorkFlags.BIT.DA1_SoftStart_Flag = 1;
            }
        }
        else
         {
           DA1_Value = 0;
         }
      }
      else
      {
        Set_DAC_OUT(DAC1_CH ,s32tem);
      }
   }
   else  
   {
      Set_DAC_OUT(DAC1_CH ,s32tem);
   }
 
 
   /********************DAC1-EN****************/
   if(WorkSpace.WorkFlags.BIT.DA1_CNTL_ONOFF)
   {
      if(++DA1_ON_DelayTime > 200)
      {
        DA1_ON_DelayTime = 500;
        if( WorkSpace.WorkFlags.BIT.DA1_CNTL_PWM_EN)
        {
          DA_ENABLE(DAC1_CH ,DAC_OUT_PWM);
        }
        else if ( !WorkSpace.WorkFlags.BIT.DA1_CNTL_PWM_EN)
        {
          DA_ENABLE(DAC1_CH ,DAC_OUT_HIGH);
        }
        WorkSpace.WorkFlags.BIT.DA1_SoftStart_EN = 1;
     }
     else
     {
       DA_ENABLE(DAC1_CH ,DAC_OUT_OFF);
     }
 
   }
   else
   {
      WorkSpace.WorkFlags.BIT.DA1_SoftStart_Flag = 0;
      WorkSpace.WorkFlags.BIT.DA1_SoftStart_EN = 0;
      DA1_Value = 0;
      DA1_ON_DelayTime = 0;
      DA_ENABLE(DAC1_CH ,DAC_OUT_OFF);
   }
    /********************DAC2-EN****************/

    if(WorkSpace.WorkFlags.BIT.DA2_CNTL_ONOFF)
    {
      
      if(++DA2_ON_DelayTime > 200)
      {
        DA2_ON_DelayTime = 500;
        if( WorkSpace.WorkFlags.BIT.DA2_CNTL_PWM_EN)
        {
          DA_ENABLE(DAC2_CH ,DAC_OUT_PWM);
        }
        else if ( !WorkSpace.WorkFlags.BIT.DA2_CNTL_PWM_EN)
        {
          DA_ENABLE(DAC2_CH ,DAC_OUT_HIGH);
        }
        WorkSpace.WorkFlags.BIT.DA2_SoftStart_EN = 1;
     }
     else
     {
       DA_ENABLE(DAC2_CH ,DAC_OUT_OFF);
     }
  
    }
    else
    {
       WorkSpace.WorkFlags.BIT.DA2_SoftStart_Flag = 0;
       WorkSpace.WorkFlags.BIT.DA2_SoftStart_EN = 0;
       DA2_Value = 0;
       DA_ENABLE(DAC2_CH ,DAC_OUT_OFF);
    }

    /******************DAC-PWM ***************************/
     DA_PWM_OUT(DAC1_CH ,gEEP_data.DA1_FRE,gEEP_data.DA1_Duty);
     DA_PWM_OUT(DAC2_CH ,gEEP_data.DA2_FRE,gEEP_data.DA2_Duty);

    /******************DA1-5V ONOFF*************************/
    if (WorkSpace.WorkFlags.BIT.DA1_5V_ONOFF)
    {
      DA1_5V_ONOFF(ON);
    }
    else
    {
      DA1_5V_ONOFF(OFF);
    } 
 
   /******************DA2-5V ONOFF*************************/
    if (WorkSpace.WorkFlags.BIT.DA2_5V_ONOFF)
    {
      DA2_5V_ONOFF(ON);
    }
    else
    {
      DA2_5V_ONOFF(OFF);
    } 
    
	
}


