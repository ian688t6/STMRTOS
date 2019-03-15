#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "FreeRTOS.h"
#include "semphr.h"

typedef enum
{
	BSP_UART1 = 0,
	BSP_UART2,
	BSP_UART3,
	BSP_UART4,
	BSP_UART5,
} uart_num_e;

#define BSP_UART_FIFO_LEN		(256)

typedef struct
{
	SemaphoreHandle_t 	st_sem;
	uint8_t 			auc_buf[BSP_UART_FIFO_LEN];
	__IO uint16_t		us_len;
} uart_fifo;

typedef int32_t (*uart_out)(void *pv_priv, uint8_t uc_ch);
typedef int32_t (*uart_in)(void *pv_priv, uint8_t **ppuc_ch);

typedef struct
{
	uint32_t	ui_uart_base;
	uint32_t	ui_irq;
	uint32_t	ui_baudrate;
	uart_out	pf_out;
	uart_in		pf_in;
	uart_fifo	st_fifo;
} bsp_uart_s;

//extern void bsp_uart_init(void);

extern bsp_uart_s *bsp_uart_register(uint32_t ui_uart_no);

extern int32_t bsp_uart_out(void *pst_arg, uint8_t uc_ch);

extern int32_t bsp_uart_in(void *pst_arg, uint8_t **ppuc_ch);

#endif
