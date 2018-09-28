#ifndef __RTOS_CONSOLE_H__
#define __RTOS_CONSOLE_H__

typedef int32_t (*console_out)(void *pv_drv, uint8_t uc_ch);
typedef int32_t (*console_in)(void *pv_drv, uint8_t uc_ch);

typedef struct
{
	void *pv_drv;
	console_in	in;
	console_out	out;
} rtos_console_s;

extern void rtos_console_init(void);

#endif
