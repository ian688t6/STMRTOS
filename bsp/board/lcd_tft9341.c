#include "basic.h"
#include "lcd_core.h"

static uint8_t gauc_lcdon_cmd0[] = 
{
	0xCF, 0x00, 0xC1, 0x30,
};

static uint8_t gauc_lcdon_cmd1[] = 
{
	0xED, 0x64, 0x03, 0x12, 0x81,
};

static uint8_t gauc_lcdon_cmd2[] = 
{
	0xE8, 0x85, 0x10, 0x7A,
};

static uint8_t gauc_lcdon_cmd3[] = 
{
	0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,
};

static uint8_t gauc_lcdon_cmd4[] = 
{
	0xF7, 0x20,
};

static uint8_t gauc_lcdon_cmd5[] = 
{
	0xEA, 0x00, 0x00,
};

static uint8_t gauc_lcdon_cmd6[] = 
{
	0xC0, 0x1B,
};

static uint8_t gauc_lcdon_cmd7[] = 
{
	0xC1, 0x01,
};

static uint8_t gauc_lcdon_cmd8[] = 
{
	0xC5, 0x30, 0x30,
};

static uint8_t gauc_lcdon_cmd9[] = 
{
	0xC7, 0xB7,
};

#if (LCD_ORITATION == 0)
/* LCD_VERTICAL */
static uint8_t gauc_lcdon_cmd10[] = 
{
	0x36, 0x08,
};
#elif (LCD_ORITATION == 1)	
/* LCD_HORIZONTAL */
static uint8_t gauc_lcdon_cmd10[] = 
{
	0x36, 0xa8,
};
#endif

static uint8_t gauc_lcdon_cmd11[] = 
{
	0x3A, 0x55,
};

static uint8_t gauc_lcdon_cmd12[] = 
{
	0xB1, 0x00, 0x1A,
};

static uint8_t gauc_lcdon_cmd13[] = 
{
	0xB6, 0x0A, 0xA2,
};

static uint8_t gauc_lcdon_cmd14[] = 
{
	0xF2, 0x00,
};

static uint8_t gauc_lcdon_cmd15[] = 
{
	0x26, 0x01,
};

static uint8_t gauc_lcdon_cmd16[] = 
{
	0xE0, 0x0F, 0x2A, 0x28, 0x08, 0x0E, 0x08, 0x54, 0xA9, 0x43, 0x0A, 0x0F, 0x00, 0x00, 0x00, 0x00,
};

static uint8_t gauc_lcdon_cmd17[] = 
{
	0xE1, 0x00, 0x15, 0x17, 0x07, 0x11, 0x06, 0x2B, 0x56, 0x3C, 0x05, 0x10, 0x0F, 0x3F, 0x3F, 0x0F,
};
#if (LCD_ORITATION == 0)
/* LCD_VERTICAL */
static uint8_t gauc_lcdon_cmd18[] = 
{
	0x2B, 0x00, 0x00, 0x01, 0x3F,
};

static uint8_t gauc_lcdon_cmd19[] = 
{
	0x2A, 0x00, 0x00, 0x00, 0xEF,
};
#elif (LCD_ORITATION == 1)
/* LCD_HORIZONTAL */
static uint8_t gauc_lcdon_cmd18[] = 
{
	0x2B, 0x00, 0x00, 0x00, 0xEF,
};

static uint8_t gauc_lcdon_cmd19[] = 
{
	0x2A, 0x00, 0x00, 0x01, 0x3F,
};
#endif

static uint8_t gauc_lcdon_cmd20[] = 
{
	0x11,
};

static uint8_t gauc_lcdon_cmd21[] = 
{
	0x29,
};

static lcd_cmd_t gast_lcdon_cmds[] = 
{
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd0),  	gauc_lcdon_cmd0 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd1), 	gauc_lcdon_cmd1 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd2), 	gauc_lcdon_cmd2 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd3), 	gauc_lcdon_cmd3 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd4), 	gauc_lcdon_cmd4 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd5), 	gauc_lcdon_cmd5 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd6), 	gauc_lcdon_cmd6 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd7), 	gauc_lcdon_cmd7 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd8), 	gauc_lcdon_cmd8 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd9), 	gauc_lcdon_cmd9 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd10), 	gauc_lcdon_cmd10 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd11), 	gauc_lcdon_cmd11 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd12), 	gauc_lcdon_cmd12 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd13), 	gauc_lcdon_cmd13 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd14), 	gauc_lcdon_cmd14 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd15), 	gauc_lcdon_cmd15 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd16), 	gauc_lcdon_cmd16 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd17),	gauc_lcdon_cmd17 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd18), 	gauc_lcdon_cmd18 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd19), 	gauc_lcdon_cmd19 },
	{ 120, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd20), 	gauc_lcdon_cmd20 },
	{ 0, 	CMD_ARRAY_SIZE(gauc_lcdon_cmd21), 	gauc_lcdon_cmd21 },	
};

static uint8_t gauc_lcd_setpos_x_cmd[] = 
{
	0x2A,
};

static lcd_cmd_t gst_lcd_setpos_x_cmd =
{
	.ui_delay = 0,
	.ui_size = CMD_ARRAY_SIZE(gauc_lcd_setpos_x_cmd),
	.puc_payload = gauc_lcd_setpos_x_cmd,
};

static uint8_t gauc_lcd_setpos_y_cmd[] =
{
	0x2B,
};

static lcd_cmd_t gst_lcd_setpos_y_cmd =
{
	.ui_delay = 0,
	.ui_size = CMD_ARRAY_SIZE(gauc_lcd_setpos_y_cmd),
	.puc_payload = gauc_lcd_setpos_y_cmd,
};

static lcd_panel_t gst_lcd_tft9341 = 
{
	.pc_name = "lcd_tft9341",
	.us_devid = 0x9341,
	.us_id_addr	= 0xD3,
	.st_gram = {
		.us_wgram = 0x2C,
		.us_rgram = 0X2E,
	},
	.us_framerate = 60,
#if (LCD_ORITATION == 0)
	/* LCD_VERTICAL */
	.ui_xres = 240,
	.ui_yres = 320,
#elif (LCD_ORITATION == 1)
	/* LCD_HORIZONTAL */
	.ui_xres = 320,
	.ui_yres = 240,
#endif	
	.st_cmd_init = {
		.past_cmds = gast_lcdon_cmds,
		.ui_cmd_count = CMD_ARRAY_SIZE(gast_lcdon_cmds),
	},
	.st_cmd_setpos = {
		.pst_x = &gst_lcd_setpos_x_cmd,
		.pst_y = &gst_lcd_setpos_y_cmd,
	},
};

lcd_panel_t *lcd_panel_get(void)
{
	return &gst_lcd_tft9341;
}
