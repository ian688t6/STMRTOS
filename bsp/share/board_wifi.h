#ifndef __BOARD_WIFI_H__
#define __BOARD_WIFI_H__

#include "bsp_uart.h"
typedef struct
{
	bsp_uart_s *pst_uart;
	void (*init)(bsp_uart_s *pst_uart);
	int32_t (*test)(char *pc_cmd);
	int32_t (*qury)(char *pc_cmd);
	int32_t (*conf)(char *pc_cmd, char *pc_args);
	int32_t (*exec)(char *pc_cmd);
} wifi_mcu_t;

extern wifi_mcu_t *wifi_mcu_get(void);

#endif
