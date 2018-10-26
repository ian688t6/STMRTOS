#ifndef __AT_H__
#define __AT_H__

#include "bsp_uart.h"

extern void at_puts(bsp_uart_s *pst_uart, char *Data, ...);

extern int32_t at_gets(bsp_uart_s *pst_uart, uint8_t **ppc_buf);

extern int32_t at_test(bsp_uart_s *pst_uart, const char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout);

extern int32_t at_qury(bsp_uart_s *pst_uart, const char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout);

extern int32_t at_conf(bsp_uart_s *pst_uart, const char *pc_cmd, char *pc_args, uint8_t **puc_resp, uint32_t ui_timeout);

extern int32_t at_exec(bsp_uart_s *pst_uart, const char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout);

#endif
