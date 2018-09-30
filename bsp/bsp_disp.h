#ifndef __BSP_DISP_H__
#define __BSP_DISP_H__


typedef void (*callback_set_pixel)(uint16_t us_x, uint16_t us_y, uint16_t us_color);
typedef uint16_t (*callback_get_pixel)(uint16_t us_x, uint16_t us_y);
typedef void (*callback_fill_color)(uint16_t us_x1, uint16_t us_y1, uint16_t us_x2, uint16_t us_y2, uint16_t us_color);

typedef struct
{
	uint32_t				ui_xres;
	uint32_t				ui_yres;
	callback_set_pixel		set_pixel;
	callback_get_pixel		get_pixel;
	callback_fill_color		fill_color;
} bsp_disp_t;

extern void bsp_disp_register(bsp_disp_t **ppst_bsp_disp);

extern void bsp_disp_init(void);

extern void bsp_disp_draw_point(uint16_t us_x, uint16_t us_y, uint16_t us_color);

extern void bsp_disp_draw_line(uint16_t us_x1, uint16_t us_y1, uint16_t us_x2, uint16_t us_y2);

extern void bsp_disp_draw_rect(uint16_t us_x1, uint16_t us_y1, uint16_t us_x2, uint16_t us_y2);

extern uint16_t bsp_disp_id(void);

extern void bsp_disp_test(void);

#endif
