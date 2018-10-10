#ifndef __BOARD_TP_H__
#define __BOARD_TP_H__

typedef struct
{
	uint8_t 	uc_dev;
	uint8_t		(*init)(void);
	uint8_t		(*scan)(void);
} touch_panel_t;

extern touch_panel_t *touch_panel_get(void);

#endif
