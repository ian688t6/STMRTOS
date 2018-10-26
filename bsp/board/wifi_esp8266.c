#include "basic.h"
#include "rtos.h"
#include "basic.h"
#include "board_wifi.h"
#include "at.h"

static wifi_mcu_t gst_mcu;

static void esp8266_puts(char *Data, ...)
{
	wifi_mcu_t *pst_mcu = &gst_mcu;
	at_puts(pst_mcu->pst_uart, Data);
	return;
}

static int32_t esp8266_gets(uint8_t **ppc_buf)
{
	wifi_mcu_t *pst_mcu = &gst_mcu;
	return at_gets(pst_mcu->pst_uart, ppc_buf);
}

static int32_t esp8266_init(bsp_uart_s *pst_uart)
{
	uint8_t *puc_resp = NULL;
	wifi_mcu_t *pst_mcu = &gst_mcu;
	pst_mcu->pst_uart = pst_uart;
	
	esp8266_puts("ATE0\r\n");
	rtos_xdelay(3000);
	esp8266_gets(&puc_resp);
	if (strstr((char *)puc_resp, "OK"))
		return 0;
	
	return -1;
}

static int32_t esp8266_test(const char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout)
{
	wifi_mcu_t *pst_mcu = &gst_mcu;
	return at_test(pst_mcu->pst_uart, pc_cmd, puc_resp, ui_timeout);
}

static int32_t esp8266_qury(const char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout)
{
	wifi_mcu_t *pst_mcu = &gst_mcu;
	return at_qury(pst_mcu->pst_uart, pc_cmd, puc_resp, ui_timeout);
}

static int32_t esp8266_conf(const char *pc_cmd, char *pc_args, uint8_t **puc_resp, uint32_t ui_timeout)
{
	wifi_mcu_t *pst_mcu = &gst_mcu;	
	return at_conf(pst_mcu->pst_uart, pc_cmd, pc_args, puc_resp, ui_timeout);
}

static int32_t esp8266_exec(const char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout)
{
	wifi_mcu_t *pst_mcu = &gst_mcu;
	return at_exec(pst_mcu->pst_uart, pc_cmd, puc_resp,  ui_timeout);
}

wifi_mcu_t *wifi_mcu_get(void)
{
	wifi_mcu_t *pst_mcu = &gst_mcu;
	
	pst_mcu->init = esp8266_init;
	pst_mcu->test = esp8266_test;
	pst_mcu->qury = esp8266_qury;
	pst_mcu->conf = esp8266_conf;
	pst_mcu->exec = esp8266_exec;
	pst_mcu->puts = esp8266_puts;
	pst_mcu->gets = esp8266_gets;
	
	return &gst_mcu;
}

