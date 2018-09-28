#ifndef __LCD_CORE_H__
#define __LCD_CORE_H__

typedef struct lcd_cmd 
{
	/* payload data length */
	uint32_t ui_delay;
	uint32_t ui_size;
	/* payload */
	uint8_t *puc_payload;
} lcd_cmd_t;

typedef struct lcd_cmdinfo 
{
	lcd_cmd_t 	*past_cmds;
	uint32_t 	ui_cmd_count;
} lcd_cmdinfo_t;

typedef struct lcd_gram
{
	uint16_t		us_gram;
	uint16_t		us_gram_x;
	uint16_t		us_gram_y;
} lcd_gram_t;

typedef struct
{
	const char 		*pc_name;
	uint16_t		us_devid;
	uint16_t		us_id_addr;
	lcd_gram_t		st_gram;
	uint16_t		us_framerate;
	uint32_t		ui_xres;
	uint32_t		ui_yres;
	lcd_cmdinfo_t	st_cmdinfo;
} lcd_panel_t;

#define CMD_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

extern lcd_panel_t *lcd_panel_get(void);

#endif
