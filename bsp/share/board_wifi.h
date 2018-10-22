#ifndef __BOARD_WIFI_H__
#define __BOARD_WIFI_H__

#include "bsp_uart.h"
typedef struct
{
	void (*init)(bsp_uart_s *pst_uart);
} wifi_mcu_t;

extern wifi_mcu_t *wifi_mcu_get(void);

#endif
