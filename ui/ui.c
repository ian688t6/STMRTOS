#include "basic.h"
#include "rtos.h"
#include "FreeRTOS.h"
#include "bsp.h"
#include "task.h"
#include "ui.h"

#define TASK_UI_MTOUCH_PRIO		(5)
#define TASK_UI_MTOUCH_SIZE		(256)

static TaskHandle_t gst_handle;
static SemaphoreHandle_t st_sem;

static void callback_mtouch_sta(void *pv_arg)
{
	uint32_t i = 0;
	GUI_MTOUCH_EVENT st_event;
	GUI_MTOUCH_INPUT st_input;
	ui_mtouch_sta_t *pst_sta = (ui_mtouch_sta_t *)pv_arg;
//	printf("mtouch tcont:%02x %02x\r\n", pst_sta->uc_tcont, pst_sta->uc_tstat);
	for (i = 0; i < pst_sta->uc_tcont; i ++)
	{
		if (pst_sta->uc_tstat & (0x01 << i))
		{
			st_event.TimeStamp = GUI_GetTime();
			st_event.NumPoints = 1;
			st_event.LayerIndex = 0;
			st_input.x = pst_sta->aus_x[i];
			st_input.y = pst_sta->aus_y[i];
//			printf("mtouch x[%d]:%d,y[%d]:%d\r\n", i, pst_sta->aus_x[i], i, pst_sta->aus_y[i]);
			st_input.Id = 0;
			st_input.Flags = GUI_MTOUCH_FLAG_MOVE;
			GUI_MTOUCH_StoreEvent(&st_event, &st_input);
			xSemaphoreGive(st_sem);
		}
	}
	
	return;
}

static void task_mtouch(void *pvParameters)
{
	int i = 0;
	GUI_MTOUCH_EVENT st_event;
	GUI_MTOUCH_INPUT st_input;

	WM_SelectWindow(WM_HBKWIN);
	GUI_Clear();
	GUI_SetColor(GUI_WHITE);
	GUI_SetPenSize(5);
	WM_EnableMemdev(WM_HBKWIN);
	
	for (;;)
	{
//		WM_DisableMemdev(WM_HBKWIN);
		GUI_Exec();
//		printf("task_mtouch --- 1\r\n");
		xSemaphoreTake(st_sem , portMAX_DELAY);
//		printf("task_mtouch --- 2\r\n");
		if (0 == GUI_MTOUCH_GetEvent(&st_event)) {
//			printf("task_mtouch %d...\r\n", st_event.NumPoints);
			for (i = 0; i < st_event.NumPoints; i ++) {
				GUI_MTOUCH_GetTouchInput(&st_event, &st_input, i);
//				printf("mtouch x:%d,y:%d\r\n", (uint32_t)st_input.x, (uint32_t)st_input.y);
				GUI_DrawPoint(st_input.x, st_input.y);
			}
		}
		GUI_Delay(1);
	}
}

void ui_init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);//开启CRC时钟
	GUI_Init();  					//STemWin初始化
	WM_SetCreateFlags(WM_CF_MEMDEV);
	return;
}

void ui_mtouch_enable(void)
{
	vSemaphoreCreateBinary(st_sem);
	xSemaphoreTake(st_sem , portMAX_DELAY) ;
	bsp_tp_init(callback_mtouch_sta);
	GUI_MTOUCH_Enable(1);
	GUI_SetBkColor(GUI_BLUE);

	xTaskCreate((TaskFunction_t)task_mtouch,
			(const char *)"task_mtouch",
			(uint16_t)TASK_UI_MTOUCH_SIZE,
			(void *)NULL,
			(UBaseType_t)TASK_UI_MTOUCH_PRIO,
			(TaskHandle_t *)&gst_handle);
//			
	return;
}
