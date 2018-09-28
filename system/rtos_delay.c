#include "FreeRTOS.h"
#include "task.h"
#include "rtos_sys.h"
#include "stm32f10x.h"

void rtos_udelay(uint32_t ui_nus)
{
	uint32_t ui_ticks;
	uint32_t ui_told, ui_tnow, ui_tcnt = 0;
	uint32_t ui_reload = SysTick->LOAD;
	rtos_sysclk_s st_clk;

	if (0 == ui_nus)
		return;
	
	rtos_sys_clkget(&st_clk);
	ui_ticks = ui_nus * st_clk.uc_fac_us;
	ui_told = SysTick->VAL;
	
	while (1)
	{
		ui_tnow = SysTick->VAL;
		if (ui_tnow != ui_told)
		{
			if (ui_tnow < ui_told)
			{
				ui_tcnt += ui_told - ui_tnow;
			}
			else
			{
				ui_tcnt += ui_reload - ui_tnow + ui_told;
			}
			ui_told = ui_tnow;
			if (ui_tcnt >= ui_ticks)
				break;
		}
	}
	
	return;
}

void rtos_mdelay(uint32_t ui_nms)
{
	rtos_sysclk_s st_clk;
	
	if (0 == ui_nms)
		return;
	
	rtos_sys_clkget(&st_clk);	
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//系统已经运行
	{		
		if(ui_nms >= st_clk.us_fac_ms)						//延时的时间大于OS的最少时间周期 
		{ 
   			vTaskDelay(ui_nms / st_clk.us_fac_ms);	 		//FreeRTOS延时
		}
		ui_nms %= st_clk.us_fac_ms;						//OS已经无法提供这么小的延时了,采用普通方式延时    
	}
	rtos_udelay((uint32_t)(ui_nms*1000));				//普通方式延时

	return;
}

void rtos_xdelay(uint32_t ui_nms)
{
	u32 i;

	if (0 == ui_nms)
		return;
	
	for(i = 0; i < ui_nms; i ++) 
		rtos_udelay(1000);
}

