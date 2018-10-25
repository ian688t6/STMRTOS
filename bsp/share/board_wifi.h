#ifndef __BOARD_WIFI_H__
#define __BOARD_WIFI_H__

#include "bsp_uart.h"
typedef struct
{
	bsp_uart_s *pst_uart;
	int32_t (*init)(bsp_uart_s *pst_uart);
	int32_t (*test)(const char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout);
	int32_t (*qury)(const char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout);
	int32_t (*conf)(const char *pc_cmd, char *pc_args, uint8_t **puc_resp, uint32_t ui_timeout);
	int32_t (*exec)(const char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout);
	void (*puts)(char *pc_data, ...);
	int32_t (*gets)(uint8_t **ppc_buf);
} wifi_mcu_t;

extern wifi_mcu_t *wifi_mcu_get(void);

#endif
