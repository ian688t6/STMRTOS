#ifndef __LCD_CORE_H__
#define __LCD_CORE_H__

#define LCD_VERTICAL		(0)
#define LCD_HORIZONTAL		(1)
#define LCD_ORITATION		LCD_HORIZONTAL

#define FLAG_DUMMY_READ		(1 << 0)
#define FLAG_TWICE_READ		(1 << 1)
#define FLAG_BGR2RGB		(1 << 2)

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
	uint16_t		us_wgram;
	uint16_t		us_rgram;
//	uint16_t		us_gram_x;
//	uint16_t		us_gram_y;
} lcd_gram_t;

typedef struct lcd_pos
{
	lcd_cmd_t 	*pst_x;
	lcd_cmd_t 	*pst_y;
}  lcd_pos_t;

typedef struct
{
	const char 		*pc_name;
	uint16_t		us_devid;
	uint16_t		us_id_addr;
	lcd_gram_t		st_gram;
	uint16_t		us_flag;
	uint16_t		us_framerate;
	uint32_t		ui_xres;
	uint32_t		ui_yres;
	lcd_cmdinfo_t	st_cmd_init;
	lcd_pos_t		st_cmd_setpos;
} lcd_panel_t;

#define CMD_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

extern lcd_panel_t *lcd_panel_get(void);

#endif
