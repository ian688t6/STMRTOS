#ifndef __BSP_TP_H__
#define __BSP_TP_H__

#include "bsp_iic.h"
#include "board_tp.h"

typedef struct
{
	TaskHandle_t		st_handle;
	bsp_iic_t			*pst_iic;
	touch_panel_t		*pst_bd_tp;
	touch_panel_state_t st_sta;
} bsp_tp_t;

extern void bsp_tp_init(void);

#endif
