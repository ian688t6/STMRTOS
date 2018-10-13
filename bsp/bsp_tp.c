#include "basic.h"
#include "board.h"
#include "rtos.h"
#include "board_tp.h"
#include "bsp_iic.h"
#include "bsp_tp.h"

#define TASK_TP_SCAN_PRIO		(5)
#define TASK_TP_SCAN_SIZE		(256)

static bsp_tp_t gst_tp;

static void task_tp_scan(void *pvParameters)
{
	bsp_tp_t *pst_tp = (bsp_tp_t *)pvParameters;
	
	pst_tp->st_sta.uc_tcont = 1;
	for (;;)
	{
		printf("tp scan ...\r\n");
		pst_tp->pst_bd_tp->scan(pst_tp->pst_iic, &pst_tp->st_sta);
		rtos_mdelay(50);
	}	
}

void bsp_tp_init(void)
{
	bsp_tp_t		*pst_tp		= &gst_tp;
	board_t 		*pst_bd 	= board_get();
	
	pst_tp->pst_bd_tp 	= pst_bd->pf_touch_panel_get();
	pst_tp->pst_iic 	= bsp_iic_register(pst_tp->pst_bd_tp->uc_dev);
	
	pst_tp->pst_bd_tp->init(pst_tp->pst_iic);
	xTaskCreate((TaskFunction_t)task_tp_scan,
				(const char *)"task_tp_scan",
				(uint16_t)TASK_TP_SCAN_SIZE,
				(void *)pst_tp,
				(UBaseType_t)TASK_TP_SCAN_PRIO,
				(TaskHandle_t *)&pst_tp->st_handle);	
	return;
}
