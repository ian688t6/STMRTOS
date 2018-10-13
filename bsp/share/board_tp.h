#ifndef __BOARD_TP_H__
#define __BOARD_TP_H__

#include "bsp_iic.h"
#include "bsp_gpio.h"

#define TOUCH_PANEL_MAX_TCONT	(5)

typedef struct
{
	uint8_t		uc_tcont;
	uint8_t		uc_tstat;
	uint16_t	aus_x[TOUCH_PANEL_MAX_TCONT];
	uint16_t	aus_y[TOUCH_PANEL_MAX_TCONT];
} touch_panel_state_t;

typedef struct
{
	uint8_t 	uc_dev;
	uint8_t		(*init)(bsp_iic_t *pst_iic);
	uint8_t		(*scan)(bsp_iic_t *pst_iic, touch_panel_state_t *pst_sta);
} touch_panel_t;

extern touch_panel_t *touch_panel_get(void);

#endif
