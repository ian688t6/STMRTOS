#include "FreeRTOS.h"
#include "task.h"
#include "rtos_sys.h"
#include "stm32f10x.h"

static rtos_sysclk_s gst_sysclk = {
	0, 0
};

static void sysclk_init(void)
{
	rtos_sysclk_s *pst_sysclk = &gst_sysclk;
	uint32_t ui_reload;
	
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK); //选择外部时钟  HCLK
	pst_sysclk->uc_fac_us=SystemCoreClock/1000000;	//不论是否使用OS,fac_us都需要使用
	ui_reload=SystemCoreClock/1000000;				//每秒钟的计数次数 单位为M  
	ui_reload *=1000000/configTICK_RATE_HZ;			//根据configTICK_RATE_HZ设定溢出时间
													//reload为24位寄存器,最大值:16777216,在72M下,约合0.233s左右	
	pst_sysclk->us_fac_ms=1000/configTICK_RATE_HZ;				//代表OS可以延时的最少单位	   

	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD = ui_reload; 						//每1/configTICK_RATE_HZ秒中断一次	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK    

	return;
}

void rtos_sys_clkget(rtos_sysclk_s *pst_sysclk)
{
	if (NULL == pst_sysclk)
		return;
	
	*pst_sysclk = gst_sysclk;
	
	return;
}

void rtos_sys_init(void)
{
	sysclk_init();
	return;
}
