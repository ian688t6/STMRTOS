#include "basic.h"
#include "FreeRTOS.h"
#include "board.h"
#include "rtos.h"
#include "board_wifi.h"
#include "bsp_uart.h"
#include "bsp_wifi.h"

#define TASK_WIFI_LISTEN_PRIO			(8)
#define TASK_WIFI_LISTEN_STK_SIZE		(128)

static wifi_ctx_s 	gst_ctx;

#define CHECK_ACK(resp) \
		(NULL != strstr((char *)resp, "OK"))	

static char *split_chr(char *s, char c)
{
	char *comma, *p;

	comma = strchr(s, c);
	if (!s || !comma)
		return NULL;

	p = comma;
	*comma = ' ';

	for (; *comma == ' '; comma++);

	for (; (p >= s) && *p == ' '; p--)
		*p = 0;

	return comma;
}

static char *split(char *s)
{
	return split_chr(s, ',');
}		
	
static int32_t wifi_resp(void *pv_arg, wifi_ctx_s *pst_ctx)
{
	xSemaphoreTake(pst_ctx->st_sem , portMAX_DELAY);

	printf("RESP:%s", pst_ctx->ac_resp);
	if (pst_ctx->complete)
		pst_ctx->complete(pst_ctx, pv_arg, (char *)pst_ctx->ac_resp);
	if (strstr(pst_ctx->ac_resp, "ERROR"))
		printf("%s", pst_ctx->ac_resp);
	
	return CHECK_ACK(pst_ctx->ac_resp) ? 0  : -1;
}
		
static int32_t wifi_set_mode(wifi_ctx_s *pst_ctx, char *pc_mode)
{
	uint8_t *puc_resp = NULL;
	
	pst_ctx->pst_mcu->conf("CWMODE", pc_mode, &puc_resp, 0);
	
	return 0;
}

static int32_t wifi_join_ap(wifi_ctx_s *pst_ctx, wifi_ssid_s *pst_ssid)
{
	uint8_t *puc_resp = NULL;
	char *pc_arg = NULL;
	
	pc_arg = (char *)pvPortMalloc(128);
	if (NULL == pc_arg)
		return -1;
	memset(pc_arg, 0x0, 128);
	snprintf(pc_arg, 128, "\"%s\",\"%s\"", pst_ssid->ac_ssidname, pst_ssid->ac_password);
	pst_ctx->pst_mcu->conf("CWJAP", pc_arg, &puc_resp, 0);
	vPortFree(pc_arg);
	
	return 0;
}

static void parse_cifsr(char *pc_cifsr, const char *pc_key, char *pc_buf)
{
	char *pc_tmp = NULL;
	
	pc_tmp = strstr(pc_cifsr, pc_key);
	if (NULL == pc_tmp)
		return;
	
	pc_tmp += strlen(pc_key);
	/* skip token " */
	pc_tmp ++;
	pc_tmp ++;
	while ('"' != *pc_tmp)
		*pc_buf ++ = *pc_tmp ++;
	*pc_buf = '\0';
	
	return;
}

static int32_t wifi_get_ifaddr(wifi_ctx_s *pst_ctx, wifi_ifaddr_s *pst_ifaddr)
{
	uint8_t *puc_resp 	= NULL;
	
	pst_ctx->pst_mcu->exec("CIFSR", &puc_resp, 0);
			
	return 0;
}

static void wifi_get_ifaddr_comp(wifi_ctx_s *pst_ctx, void *pv_arg, char *pc_resp)
{
	wifi_ifaddr_s *pst_ifaddr = (wifi_ifaddr_s *)pv_arg;

	parse_cifsr(pc_resp, "APIP", 	pst_ifaddr->ac_ap_addr);
	parse_cifsr(pc_resp, "APMAC", 	pst_ifaddr->ac_ap_mac);
	parse_cifsr(pc_resp, "STAIP", 	pst_ifaddr->ac_sta_addr);
	parse_cifsr(pc_resp, "STAMAC", 	pst_ifaddr->ac_sta_mac);
	
	return;
}

static void conn_tcp(wifi_ctx_s *pst_ctx, char *pc_addr, uint32_t ui_port)
{
	uint8_t *puc_resp = NULL;
	char* pc_arg = NULL;
	
	pc_arg = (char *)pvPortMalloc(128);
	if (NULL == pc_arg)
		return ;
	memset(pc_arg, 0x0, 128);
	snprintf(pc_arg, 128, "\"TCP\",\"%s\",%d", pc_addr, ui_port);
	pst_ctx->pst_mcu->conf("CIPSTART", pc_arg, &puc_resp, 0);
	vPortFree(pc_arg);
	
	return;
}

static void conn_udp(wifi_ctx_s *pst_ctx, uint32_t ui_mux, uint32_t ui_id, 
	char *pc_addr, uint32_t ui_rmt_port, uint32_t ui_loc_port, uint32_t ui_mode)
{
	uint8_t *puc_resp = NULL;
	char* pc_arg = NULL;
	
	pc_arg = (char *)pvPortMalloc(128);
	if (NULL == pc_arg)
		return ;
	memset(pc_arg, 0x0, 128);

	if (ui_mux)
		snprintf(pc_arg, 128, "%d,\"UDP\",\"%s\",%d,%d,%d", 
			ui_id, pc_addr, ui_rmt_port, ui_loc_port, ui_mode);
	else
		snprintf(pc_arg, 128, "\"UDP\",\"%s\",%d,%d,%d", 
			pc_addr, ui_rmt_port, ui_loc_port, ui_mode);

	pst_ctx->pst_mcu->conf("CIPSTART", pc_arg, &puc_resp, 0);
	vPortFree(pc_arg);
	
	return;
}

static int32_t wifi_connect(wifi_ctx_s *pst_ctx, wifi_conn_s *pst_conn)
{
	int32_t i_ret = 0;
	uint8_t *puc_resp = NULL;
	
	/* Todo: config mux conn */
	i_ret = pst_conn->ui_mux == 1 ? 
		pst_ctx->pst_mcu->conf("CIPMUX", "1", &puc_resp, 0) : pst_ctx->pst_mcu->conf("CIPMUX", "0", &puc_resp, 0);
	i_ret = wifi_resp(pst_conn, pst_ctx);
	if (0 != i_ret)
	{
		printf("ERROR: wifi connect link is builed\r\n");
		return i_ret;
	}
	
	switch (pst_conn->ac_type[0])
	{
		case 'T':
			/* Todo: tcp connect */
			conn_tcp(pst_ctx, pst_conn->ac_addr, pst_conn->ui_rmt_port);
		break;
		
		case 'U':
			/* Todo: udp connect */
			conn_udp(pst_ctx, pst_conn->ui_mux, pst_conn->ui_id,  
				pst_conn->ac_addr, pst_conn->ui_rmt_port, 
				pst_conn->ui_loc_port, pst_conn->ui_mode);
		break;
		
		default:
			return -1;
	}
	
	return i_ret;
}

static int32_t wifi_set_txmode(wifi_ctx_s *pst_ctx, uint32_t ui_mode)
{
	int32_t i_ret = 0;
	uint8_t *puc_resp = NULL;
	
	i_ret = ui_mode == 0 ? 
		pst_ctx->pst_mcu->conf("CIPMODE", "0", &puc_resp, 0) : 
		pst_ctx->pst_mcu->conf("CIPMODE", "1", &puc_resp, 0);
	
	return i_ret;
}

static int32_t wifi_begin_passthrough(wifi_ctx_s *pst_ctx)
{
	uint8_t *puc_resp = NULL;
	
	pst_ctx->pst_mcu->exec("CIPSEND", &puc_resp, 0);
	
	return 0;
}

static int32_t wifi_tx_passthrough(wifi_ctx_s *pst_ctx, char *pc_data)
{
	pst_ctx->pst_mcu->puts(pc_data);
	return 0;	
}

static int32_t wifi_end_passthrough(wifi_ctx_s *pst_ctx)
{
	pst_ctx->pst_mcu->puts("+++");
	return 0;
}

static void wifi_tx_comp(wifi_ctx_s *pst_ctx, void *pv_data, char *pc_resp)
{
	int32_t i_ret = 0;
	
	if (!strstr(pc_resp, ">"))
		return;
	
	pst_ctx->pst_mcu->puts((char *)pv_data);
	i_ret = wifi_resp(pv_data, pst_ctx);
	if (0 != i_ret)
	{
		printf("wifi tx comp failed!\r\n");
		return;
	}
	
	return;
}

static int32_t wifi_tx(wifi_ctx_s *pst_ctx, char *pc_data)
{
	int32_t i_len = 0;
	uint8_t *puc_resp = NULL;
	char ac_arg[32] = {0};
	
	if (NULL == pc_data)
		return -1;
	
	i_len = strlen(pc_data);
	if (i_len == 0)
		return -1;
	
	snprintf(ac_arg, sizeof(ac_arg), "%d", i_len);
	pst_ctx->pst_mcu->conf("CIPSEND", ac_arg, &puc_resp, 0);
		
	return 0;
}

static void wifi_link_status_comp(wifi_ctx_s *pst_ctx, void *pv_data, char *pc_resp)
{
	char *pc_tmp = NULL;
	char *pc_str = NULL;
	wifi_link_status_s *pst_status = (wifi_link_status_s *)pv_data;
	
	/* Todo: parse wifi link status response */
	pc_tmp = strstr(pc_resp, "STATUS:");
	if (NULL == pc_tmp)
		return;
	pc_tmp += strlen("STATUS:");
	pst_status->ui_status = strtoul(pc_tmp, 0, 0);
	
	pc_tmp = strstr(pc_resp, "+CIPSTATUS:");
	if (NULL == pc_tmp)
		return;
	pc_tmp += strlen("+CIPSTATUS:");
	/* Todo: connect id */
	pc_str = split(pc_tmp);
	pst_status->ui_id = strtoul(pc_tmp, 0, 0);
	
	/* Todo: connect type tcp or udp */
	pc_tmp = split(pc_str);
	pc_str ++;
	switch (*pc_str)
	{
		case 'T':
			pst_status->ui_conntype = WIFI_CONNTYPE_TCP;
		break;
		
		case 'U':
			pst_status->ui_conntype = WIFI_CONNTYPE_UDP;
		break;
		
		default:
		break;
	}
	
	/* Todo: ip addr */
	pc_str = split(pc_tmp);
	pc_tmp ++;
	strncpy(pst_status->ac_addr, pc_tmp, sizeof(pst_status->ac_addr));
	pst_status->ac_addr[strlen(pc_tmp) - 1] = '\0';
		
	/* Todo: remote port */
	pc_tmp = split(pc_str);
	pst_status->ui_rmt_port = strtoul(pc_str, 0, 0);
	
	/* Todo: local port */
	pc_str = split(pc_tmp);
	pst_status->ui_loc_port = strtoul(pc_tmp, 0, 0);
	
	/* Todo: tetype */
	pc_tmp = split(pc_str);
	pst_status->ui_tetype = strtoul(pc_str, 0, 0);
	
	return;
}	

static int32_t wifi_link_status(wifi_ctx_s *pst_ctx, wifi_link_status_s *pst_status)
{
	uint8_t *puc_resp = NULL;
	pst_ctx->pst_mcu->exec("CIPSTATUS", &puc_resp, 0);
	return 0;
}

static void task_wifi_resp(void *pv_param)
{
	uint8_t *puc_resp = NULL;
	wifi_ctx_s *pst_ctx = (wifi_ctx_s *)pv_param;
	
	for (;;)
	{
		pst_ctx->pst_mcu->gets(&puc_resp);
//		printf("wifi_resp %s\r\n", (char *)puc_resp);
		if (strstr((char *)puc_resp, "OK") || strstr((char *)puc_resp, "ERROR") || 
			strstr((char *)puc_resp, "FAIL")) {
			
			memcpy(pst_ctx->ac_resp, puc_resp, sizeof(pst_ctx->ac_resp));
			pst_ctx->pst_mcu->pst_uart->st_fifo.us_len = 0;
			pst_ctx->pst_mcu->pst_uart->st_fifo.auc_buf[pst_ctx->pst_mcu->pst_uart->st_fifo.us_len] = '\0';
			xSemaphoreGive(pst_ctx->st_sem);
		}
		rtos_mdelay(500);
	}
}

void bsp_wifi_init(void)
{
	int32_t i_ret = 0;
	wifi_ctx_s *pst_ctx = &gst_ctx;
	uint8_t *puc_resp 	= NULL;
	board_t *pst_bd 	= board_get();
	bsp_uart_s *pst_uart	= bsp_uart_register(BSP_UART2);
	pst_ctx->pst_mcu 		= pst_bd->pf_wifi_mcu_get();
	
	i_ret = pst_ctx->pst_mcu->init(pst_uart);
	if (0 != i_ret)
	{
		printf("esp8266 init failed!\r\n");
		//return;
	}
	
	vSemaphoreCreateBinary(pst_ctx->st_sem);
	xSemaphoreTake(pst_ctx->st_sem , portMAX_DELAY) ;
	xTaskCreate((TaskFunction_t)task_wifi_resp,
			(const char *)"task_wifi_resp",
			(uint16_t)TASK_WIFI_LISTEN_STK_SIZE,
			(void *)pst_ctx,
			(UBaseType_t)TASK_WIFI_LISTEN_PRIO,
			(TaskHandle_t *)&pst_ctx->st_handle);
	 						
	pst_ctx->pst_mcu->test(NULL, &puc_resp, 500);
	printf("at test get resp %s", (char *)puc_resp);
	if (!CHECK_ACK(puc_resp)) {
		printf("AT test failed %s!\r\n", (char *)puc_resp);
	}
	
	return;
}

int32_t bsp_wifi_ioctl(uint32_t ui_ioctl_cmd, void *pv_arg, uint32_t ui_size)
{
	int32_t i_ret = 0;
	wifi_ctx_s *pst_ctx = &gst_ctx;
	pst_ctx->ui_cmd 	= ui_ioctl_cmd;
	pst_ctx->complete 	= NULL;
	
	switch (ui_ioctl_cmd)
	{
		case IOCTL_WIFI_SET_MODE:
			i_ret = wifi_set_mode(pst_ctx, (char *)pv_arg);
		break;
		
		case IOCTL_WIFI_JOIN_AP:
			i_ret = wifi_join_ap(pst_ctx, (wifi_ssid_s *)pv_arg);
		break;
		
		case IOCTL_WIFI_GET_IFADDR:
			pst_ctx->complete = wifi_get_ifaddr_comp;
			i_ret = wifi_get_ifaddr(pst_ctx, (wifi_ifaddr_s *)pv_arg);
		break;
		
		case IOCTL_WIFI_CONNECT:
			i_ret = wifi_connect(pst_ctx, (wifi_conn_s *)pv_arg);
		break;
		
		case IOCTL_WIFI_SET_TXMODE:
			i_ret = wifi_set_txmode(pst_ctx, *(uint32_t *)pv_arg);
		break;
		
		case IOCTL_WIFI_BEGIN_PASSTHROUGH:
			i_ret = wifi_begin_passthrough(pst_ctx);
		break;

		case IOCTL_WIFI_TX_PASSTHROUGH:
			i_ret = wifi_tx_passthrough(pst_ctx, (char *)pv_arg);
		break;
		
		case IOCTL_WIFI_END_PASSTHROUGH:
			i_ret = wifi_end_passthrough(pst_ctx);
		break;
		
		case IOCTL_WIFI_TX:
			pst_ctx->complete = wifi_tx_comp;
			i_ret = wifi_tx(pst_ctx, (char *)pv_arg);
		break;
		
		case IOCTL_WIFI_GET_LINKSTATUS:
			pst_ctx->complete = wifi_link_status_comp;
			i_ret = wifi_link_status(pst_ctx, (wifi_link_status_s *)pv_arg);
		break;
		
		default:
			
		break;
	}
	
	if (0 != i_ret)
	{
		printf("IOCTL WIFI ERROR CMD: %d\r\n", pst_ctx->ui_cmd);
		return -1;
	}
	i_ret = wifi_resp(pv_arg, pst_ctx);
	
	return i_ret;
}
