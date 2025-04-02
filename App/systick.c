 

#include "gd32f30x.h"
#include "systick.h"

volatile static uint32_t systime_tick=0;

/**************************************************************************************
* FunctionName   : systick_config(void)
* Description    :  系数节拍时钟配置
* EntryParameter : None
* ReturnValue    : None
**************************************************************************************/
void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U))
	{
        /* capture error */
        while (1){}
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

