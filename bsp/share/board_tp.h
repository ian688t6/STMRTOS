#ifndef __BOARD_TP_H__
#define __BOARD_TP_H__

typedef struct
{
	uint8_t 	uc_dev;
} touch_panel_t;

extern touch_panel_t *touch_panel_get(void);

#endif
