#ifndef __BOARD_H__
#define __BOARD_H__

#include "board_lcd.h"
#include "board_gpio.h"
#include "board_tp.h"
#include "board_wifi.h"

typedef struct
{
	const char *pc_name;
	board_gpio_t	st_backlight_gpio;
	lcd_panel_t *(*pf_lcd_panel_get)(void);
	touch_panel_t *(*pf_touch_panel_get)(void);
	wifi_mcu_t *(*pf_wifi_mcu_get)(void);
} board_t;

extern board_t *board_get(void);

#endif
