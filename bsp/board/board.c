#include "basic.h"
#include "board.h"

static board_t gst_board_mini_stm32 = 
{
	.pc_name = "MiniStm32",
	.st_backlight_gpio = {
		.ui_group 	= GPIOC_BASE,
		.ui_gpio	= GPIO_Pin_10,
	},
	.pf_lcd_panel_get 	= lcd_panel_get,
	.pf_touch_panel_get = touch_panel_get,
	.pf_wifi_mcu_get	= wifi_mcu_get,
};

board_t *board_get(void)
{
	return &gst_board_mini_stm32;
}
