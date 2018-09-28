#ifndef __RTOS_SYS_H__
#define __RTOS_SYS_H__

typedef struct
{
	uint8_t 	uc_fac_us;
	uint16_t	us_fac_ms;
} rtos_sysclk_s;

extern void rtos_sys_clkget(rtos_sysclk_s *pst_sysclk);

extern void rtos_sys_init(void);

#endif
