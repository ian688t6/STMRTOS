#ifndef __BOARD_H__
#define __BOARD_H__

#include "lcd_core.h"
#include "board_gpio.h"
#include "board_tp.h"

typedef struct
{
	const char *pc_name;
	board_gpio_t	st_backlight_gpio;
	lcd_panel_t *(*pf_lcd_panel_get)(void);
	touch_panel_t *(*pf_touch_panel_get)(void);
} board_t;

extern board_t *board_get(void);

#endif
