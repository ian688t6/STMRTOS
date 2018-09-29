#ifndef __BSP_DISP_H__
#define __BSP_DISP_H__

extern void bsp_disp_init(void);

extern void bsp_disp_draw_point(uint16_t us_x, uint16_t us_y, uint16_t us_color);

extern void bsp_disp_draw_line(uint16_t us_x1, uint16_t us_y1, uint16_t us_x2, uint16_t us_y2);

extern void bsp_disp_draw_rect(uint16_t us_x1, uint16_t us_y1, uint16_t us_x2, uint16_t us_y2);

extern uint16_t bsp_disp_id(void);

#endif
