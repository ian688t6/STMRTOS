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
	
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK); //ѡ���ⲿʱ��  HCLK
	pst_sysclk->uc_fac_us=SystemCoreClock/1000000;	//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
	ui_reload=SystemCoreClock/1000000;				//ÿ���ӵļ������� ��λΪM  
	ui_reload *=1000000/configTICK_RATE_HZ;			//����configTICK_RATE_HZ�趨���ʱ��
													//reloadΪ24λ�Ĵ���,���ֵ:16777216,��72M��,Լ��0.233s����	
	pst_sysclk->us_fac_ms=1000/configTICK_RATE_HZ;				//����OS������ʱ�����ٵ�λ	   

	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD = ui_reload; 						//ÿ1/configTICK_RATE_HZ���ж�һ��	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK    

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
