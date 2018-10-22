#include "basic.h"
#include "board_lcd.h"

static uint8_t gauc_lcdon_cmd00[] = 
{
	0xE2, 0x1D, 0x02, 0x04,
};

static uint8_t gauc_lcdon_cmd01[] = 
{
	0xE0, 0x01,
};

static uint8_t gauc_lcdon_cmd02[] = 
{
	0xE0, 0x03,
};

static uint8_t gauc_lcdon_cmd03[] = 
{
	0x01,
};

static uint8_t gauc_lcdon_cmd04[] = 
{
	0xE6, 0x2F, 0xFF, 0xFF,
};

static uint8_t gauc_lcdon_cmd05[] = 
{
	0xB0, 0x20, 0x00, 
	/* horizontal panel size SSD_HOR_RESOLUTION	800	*/
	0x03, 0x1F, 
	/* vertical panel size SSD_VER_RESOLUTION 480 */
	0x01, 0xDF,
	
	0x00,
};

/* Set front porch and back porch  HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH */
static uint8_t gauc_lcdon_cmd06[] = 
{
	/* SSD_HOR_BACK_PORCH 46 */
	/* SSD_HOR_FRONT_PORCH 210 */
	0xB4, 0x04, 0x1F,

	/* SSD_HOR_BACK_PORCH */	
	0x00, 0x2D,
	
	/* HOR_PULSE_WIDTH 1 */
	0x00,
	
	0x00,
	0x00,
	0x00,
};

/* Set the vertical blanking interval between last scan line and next LFRAME pulse */
/* VER_PULSE_WIDTH		1  */
/* VER_BACK_PORCH		23 */
/* VER_FRONT_PORCH		22 */
static uint8_t gauc_lcdon_cmd07[] = 
{
	0xB6,
	/* SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH */
	0x02, 0x0C,
	/* VER_BACK_PORCH */
	0x00, 0x16,
	/* VER_FRONT_PORCH 22 */
	0x15,
	
	0x00,
	0x00,
};

static uint8_t gauc_lcdon_cmd08[] = 
{
	0xF0, 0x03,
};

static uint8_t gauc_lcdon_cmd09[] = 
{
	0x29,
};

static uint8_t gauc_lcdon_cmd0a[] = 
{
	0xD0, 0x00,
};

static uint8_t gauc_lcdon_cmd0b[] = 
{
	0xBE, 0x05, 0xFE, 0x01, 0x00, 0x00, 0x00,
};

static uint8_t gauc_lcdon_cmd0c[] = 
{
	0xB8, 0x03, 0x01,
};

static uint8_t gauc_lcdon_cmd0d[] = 
{
	0xBA, 0x01,
};

static uint8_t gauc_lcdon_cmd0e[] = 
{
	0xBE, 0x05, 0xFF, 0x01, 0xFF, 0x00, 0x00,
};

static uint8_t gauc_lcdon_cmd0f[] = 
{
	0x36, 0x00,
};

static uint8_t gauc_lcdon_cmd10[] = 
{
	0x2A, 0x00, 0x00, 0x03, 0x1F,
};

static uint8_t gauc_lcdon_cmd11[] = 
{
	0x2B, 0x00, 0x00, 0x01, 0xDF,   
};

static lcd_cmd_t gast_lcdon_cmds[] = 
{
	{ 1,	CMD_ARRAY_SIZE(gauc_lcdon_cmd00), gauc_lcdon_cmd00 },
	{ 10,	CMD_ARRAY_SIZE(gauc_lcdon_cmd01), gauc_lcdon_cmd01 },
	{ 12,	CMD_ARRAY_SIZE(gauc_lcdon_cmd02), gauc_lcdon_cmd02 },
	{ 10,	CMD_ARRAY_SIZE(gauc_lcdon_cmd03), gauc_lcdon_cmd03 },
	{ 0,	CMD_ARRAY_SIZE(gauc_lcdon_cmd04), gauc_lcdon_cmd04 },
	{ 0,	CMD_ARRAY_SIZE(gauc_lcdon_cmd05), gauc_lcdon_cmd05 },
	{ 0,	CMD_ARRAY_SIZE(gauc_lcdon_cmd06), gauc_lcdon_cmd06 },
	{ 0,	CMD_ARRAY_SIZE(gauc_lcdon_cmd07), gauc_lcdon_cmd07 },
	{ 0,	CMD_ARRAY_SIZE(gauc_lcdon_cmd08), gauc_lcdon_cmd08 },
	{ 0,	CMD_ARRAY_SIZE(gauc_lcdon_cmd09), gauc_lcdon_cmd09 },
	{ 0,	CMD_ARRAY_SIZE(gauc_lcdon_cmd0a), gauc_lcdon_cmd0a },
	{ 0,	CMD_ARRAY_SIZE(gauc_lcdon_cmd0b), gauc_lcdon_cmd0b },
	{ 0,	CMD_ARRAY_SIZE(gauc_lcdon_cmd0c), gauc_lcdon_cmd0c },
	{ 0,	CMD_ARRAY_SIZE(gauc_lcdon_cmd0d), gauc_lcdon_cmd0d },
	{ 0,	CMD_ARRAY_SIZE(gauc_lcdon_cmd0e), gauc_lcdon_cmd0e },
	{ 0,	CMD_ARRAY_SIZE(gauc_lcdon_cmd0f), gauc_lcdon_cmd0f },
	{ 0,	CMD_ARRAY_SIZE(gauc_lcdon_cmd10), gauc_lcdon_cmd10 },
	{ 0,	CMD_ARRAY_SIZE(gauc_lcdon_cmd11), gauc_lcdon_cmd11 },
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

static lcd_panel_t gst_lcd_tft1963 = 
{
	.pc_name	= "lcd_tft1963",
	.us_devid	= 0x1963,
	.us_id_addr = 0xD3,
	.st_gram = {
		.us_wgram = 0x2C,
		.us_rgram = 0x2E,
	},
	.us_framerate = 60,
	.us_flag = 0,
	.ui_xres = 800,
	.ui_yres = 480,
	
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
	return &gst_lcd_tft1963;
}
