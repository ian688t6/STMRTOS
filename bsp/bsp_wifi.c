#include "basic.h"
#include "FreeRTOS.h"
#include "board.h"
#include "rtos.h"
#include "board_wifi.h"
#include "bsp_uart.h"
#include "bsp_wifi.h"

#define CHECK_ACK(resp) \
		(NULL != strstr((char *)resp, "OK"))

void bsp_wifi_init(void)
{
	int32_t i_ret = 0;
	
	uint8_t *puc_resp = NULL;
	board_t *pst_bd = board_get();
	bsp_uart_s *pst_uart = bsp_uart_register(BSP_UART2);
	wifi_mcu_t *pst_mcu = pst_bd->pf_wifi_mcu_get();
	
	i_ret = pst_mcu->init(pst_uart);
	if (0 != i_ret)
	{
		printf("esp8266 init failed!\r\n");
		return;
	}
	
	pst_mcu->test(NULL, &puc_resp, 500);
	if (!CHECK_ACK(puc_resp)) {
		printf("AT test failed!\r\n");
	}
	
	return;
}
