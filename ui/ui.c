#include "basic.h"
#include "rtos.h"
#include "FreeRTOS.h"
#include "bsp.h"
#include "task.h"
#include "ui.h"

void ui_init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//����CRCʱ��
	GUI_Init();  					//STemWin��ʼ��
	WM_SetCreateFlags(WM_CF_MEMDEV);

	return;
}
