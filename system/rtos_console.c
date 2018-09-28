#include "basic.h"
#include "bsp.h"
#include "FreeRTOS.h"
#include "rtos_console.h"

static rtos_console_s gst_console;

#pragma import(__use_no_semihosting)                            
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       

int _sys_exit(int x) 
{ 
	x = x;
	return 0;
} 

int fputc(int ch, FILE *f)
{
	rtos_console_s *pst_console = &gst_console;
	
	return pst_console->out(pst_console->pv_drv, ch);
}

void rtos_console_init(void)
{
	rtos_console_s *pst_console = &gst_console;
	bsp_uart_s *pst_uart = bsp_uart_fetch(BSP_UART1);
	if (NULL == pst_uart)
		return;
	
	pst_console->pv_drv = pst_uart;
	pst_console->out = pst_uart->pf_out;
	
	return;
}

