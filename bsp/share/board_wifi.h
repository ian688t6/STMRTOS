#ifndef __BOARD_WIFI_H__
#define __BOARD_WIFI_H__

#include "bsp_uart.h"
typedef struct
{
	bsp_uart_s *pst_uart;
	int32_t (*init)(bsp_uart_s *pst_uart);
	int32_t (*test)(char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout);
	int32_t (*qury)(char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout);
	int32_t (*conf)(char *pc_cmd, char *pc_args, uint8_t **puc_resp, uint32_t ui_timeout);
	int32_t (*exec)(char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout);
} wifi_mcu_t;

extern wifi_mcu_t *wifi_mcu_get(void);

#endif
