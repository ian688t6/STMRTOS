#include "basic.h"
#include "FreeRTOS.h"
#include "board.h"
#include "rtos.h"
#include "board_wifi.h"
#include "bsp_uart.h"
#include "bsp_wifi.h"

#define CHECK_ACK(resp) \
		(NULL != strstr((char *)resp, "OK"))

static wifi_mcu_t *pmcu = NULL;		
		
static int32_t wifi_set_mode(char *pc_mode)
{
	uint8_t *puc_resp = NULL;
	
	pmcu->conf("CWMODE", pc_mode, &puc_resp, 500);
	if (!CHECK_ACK(puc_resp)) {
		printf("AT+CWMODE failed!\r\n");
		return -1;
	}
	printf("AT+CWMODE OK!\r\n");
	return 0;
}

static int32_t wifi_join_ap(wifi_ssid_s *pst_ssid)
{
	uint8_t *puc_resp 	= NULL;
	char ac_arg[128] 	= {0};
	
	snprintf(ac_arg, sizeof(ac_arg), "\"%s\",\"%s\"", pst_ssid->ac_ssidname, pst_ssid->ac_password);
	
	do {
		pmcu->conf("CWJAP", ac_arg, &puc_resp, 3000);
		printf("AT+CWJAP=%s RESP:%s", ac_arg, (char *)puc_resp);
	} while (!strstr((char *)puc_resp, "WIFI GOT IP"));
	
	return 0;
}

static void parse_cifsr(char *pc_cifsr, const char *pc_key, char *pc_buf)
{
	char *pc_tmp = NULL;
	
	pc_tmp = strstr(pc_cifsr, pc_key);
	pc_tmp += strlen(pc_key);
	/* skip token " */
	pc_tmp ++;
	pc_tmp ++;
	while ('"' != *pc_tmp)
		*pc_buf ++ = *pc_tmp ++;
	*pc_buf = '\0';
	
	return;
}

static int32_t wifi_get_ifaddr(wifi_ifaddr_s *pst_ifaddr)
{
	uint8_t *puc_resp 	= NULL;
	
	do {
		pmcu->exec("CIFSR", &puc_resp, 3000);
//		printf("fifolen: %d, ifaddr: %s", pmcu->pst_uart->st_fifo.us_len, (char *)puc_resp);
	} while (!strstr((char *)puc_resp, "APIP") 	&& 
			!strstr((char *)puc_resp, "APMAC") 	&& 
			!strstr((char *)puc_resp, "STAIP")	&&
			!strstr((char *)puc_resp, "STAMAC"));
			printf("puc_resp: %s", (char *)puc_resp);	
	/* Todo: parse response */
	parse_cifsr((char *)puc_resp, "APIP", 	pst_ifaddr->ac_ap_addr);
	parse_cifsr((char *)puc_resp, "APMAC", 	pst_ifaddr->ac_ap_mac);
	parse_cifsr((char *)puc_resp, "STAIP", 	pst_ifaddr->ac_sta_addr);
	parse_cifsr((char *)puc_resp, "STAMAC", pst_ifaddr->ac_sta_mac);
			
	return 0;
}

static void conn_tcp(char *pc_addr, uint32_t ui_port)
{
	uint8_t *puc_resp = NULL;
	char ac_arg[128] = {0};
	
	snprintf(ac_arg, sizeof(ac_arg), "\"TCP\",\"%s\",%d", pc_addr, ui_port);
	pmcu->conf("CIPSTART", ac_arg, &puc_resp, 3000);
	if (!CHECK_ACK(puc_resp)) {
		printf("AT+CIPSTART TCP failed! %s\r\n", (char *)puc_resp);
		return;
	}
	
	return;
}

static void conn_udp(uint32_t ui_mux, uint32_t ui_id, 
	char *pc_addr, uint32_t ui_rmt_port, uint32_t ui_loc_port, uint32_t ui_mode)
{
	uint8_t *puc_resp = NULL;
	char ac_arg[128] = {0};

	if (ui_mux)
		snprintf(ac_arg, sizeof(ac_arg), "%d,\"UDP\",\"%s\",%d,%d,%d", 
			ui_id, pc_addr, ui_rmt_port, ui_loc_port, ui_mode);
	else
		snprintf(ac_arg, sizeof(ac_arg), "\"UDP\",\"%s\",%d,%d,%d", 
			pc_addr, ui_rmt_port, ui_loc_port, ui_mode);

	pmcu->conf("CIPSTART", ac_arg, &puc_resp, 3000);
	if (!CHECK_ACK(puc_resp)) {
		printf("AT+CIPSTART UDP failed! %s\r\n", (char *)puc_resp);
		return;
	}
	
	return;
}

static int32_t wifi_connect(wifi_conn_s *pst_conn)
{
	int32_t i_ret = 0;
	uint8_t *puc_resp = NULL;
	
	/* Todo: config mux conn */
	i_ret = pst_conn->ui_mux == 1 ? 
		pmcu->conf("CIPMUX", "1", &puc_resp, 500) : pmcu->conf("CIPMUX", "0", &puc_resp, 500);
	if (!CHECK_ACK(puc_resp) && !strstr((char *)puc_resp, "Link is builded"))
	{
		return -1;
	}
	
	switch (pst_conn->ac_type[0])
	{
		case 'T':
			/* Todo: tcp connect */
			conn_tcp(pst_conn->ac_addr, pst_conn->ui_rmt_port);
		break;
		
		case 'U':
			/* Todo: udp connect */
			conn_udp(pst_conn->ui_mux, pst_conn->ui_id,  
				pst_conn->ac_addr, pst_conn->ui_rmt_port, 
				pst_conn->ui_loc_port, pst_conn->ui_mode);
		break;
		
		default:
			return -1;
	}
	
	return i_ret;
}
		
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
		//return;
	}
	
	pst_mcu->test(NULL, &puc_resp, 1500);
	if (!CHECK_ACK(puc_resp)) {
		printf("AT test failed %s!\r\n", (char *)puc_resp);
	}
	
	pmcu = pst_mcu;
	
	return;
}

int32_t bsp_wifi_ioctl(uint32_t ui_ioctl_cmd, void *pv_arg, uint32_t ui_size)
{
	int32_t i_ret = 0;
	
	switch (ui_ioctl_cmd)
	{
		case IOCTL_WIFI_SET_MODE:
			i_ret = wifi_set_mode((char *)pv_arg);
		break;
		
		case IOCTL_WIFI_JOIN_AP:
			i_ret = wifi_join_ap((wifi_ssid_s *)pv_arg);
		break;
		
		case IOCTL_WIFI_GET_IFADDR:
			i_ret = wifi_get_ifaddr((wifi_ifaddr_s *)pv_arg);
		break;
		
		case IOCTL_WIFI_CONNECT:
			i_ret = wifi_connect((wifi_conn_s *)pv_arg);
		break;
		
		default:
			
		break;
	}
	
	return i_ret;
}
