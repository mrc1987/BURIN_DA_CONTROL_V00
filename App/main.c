#include "gd32f30x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"


/**************************************************************************
*�������� main
*���������������
*���أ�
*˵����
*
*****************************************************************************/
int main(void)
{
  uint16_t i,j;
   APP_TaskInit();//��ʼ����������
   fwdgt_config(1000, FWDGT_PSC_DIV128);
   fwdgt_write_enable();
 

	while(1)
	{
        fwdgt_counter_reload();
	    rt_thread_delay(250); 
	    LED_RUN(ON) 	
		rt_thread_delay(250); 
		LED_RUN(OFF) 	
        fwdgt_counter_reload();

	}
}
 
