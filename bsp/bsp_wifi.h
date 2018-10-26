#ifndef __BSP_WIFI_H__
#define __BSP_WIFI_H__

#include "FreeRTOS.h"
#include "board_wifi.h"

#define WIFI_MODE_STA		"1"
#define WIFI_MODE_AP		"2"
#define WIFI_MODE_STA_AP	"3"

enum
{
	WIFI_CONNTYPE_TCP,
	WIFI_CONNTYPE_UDP,
};

enum
{
	IOCTL_WIFI_SET_MODE 			= 1,
	IOCTL_WIFI_JOIN_AP				= 2,
	IOCTL_WIFI_GET_IFADDR 			= 3,
	IOCTL_WIFI_CONNECT				= 4,
	IOCTL_WIFI_SET_TXMODE			= 5,
	IOCTL_WIFI_BEGIN_PASSTHROUGH 	= 6,
	IOCTL_WIFI_END_PASSTHROUGH		= 7,
	IOCTL_WIFI_TX_PASSTHROUGH		= 8,
	IOCTL_WIFI_TX					= 9,
	IOCTL_WIFI_GET_LINKSTATUS		= 10,
};

enum
{
	WIFI_LINKSTATUS_GOTIP			= 2,
	WIFI_LINKSTATUS_CONNECT			= 3,
	WIFI_LINKSTATUS_DISCONNECT		= 4,
	WIFI_LINKSTATUS_LOSS			= 5,
};

typedef struct
{
	char ac_ssidname[64];
	char ac_password[64];
} wifi_ssid_s;

typedef struct
{
	char ac_ap_addr[16];
	char ac_ap_mac[20];
	char ac_sta_addr[16];
	char ac_sta_mac[20];
} wifi_ifaddr_s;

typedef struct
{
	uint32_t 	ui_mux;
	uint32_t	ui_id;
	char 		ac_type[4];
	char		ac_addr[32];
	uint32_t	ui_rmt_port;
	uint32_t	ui_loc_port;
	uint32_t	ui_mode;
} wifi_conn_s;

typedef struct
{
	uint32_t	ui_id;
	uint32_t	ui_status;
	uint32_t	ui_conntype;
	char 		ac_addr[32];
	uint32_t	ui_rmt_port;
	uint32_t	ui_loc_port;
	uint32_t	ui_tetype;
} wifi_link_status_s;

typedef struct wifi_ctx
{
	TaskHandle_t 		st_handle;
	SemaphoreHandle_t 	st_sem;
	wifi_mcu_t			*pst_mcu;
	uint32_t			ui_cmd;
	char 				ac_resp[256];
	void (*complete)(struct wifi_ctx *pst_ctx, void *pv_arg, char *pc_resp);
} wifi_ctx_s;

extern void bsp_wifi_init(void);

extern int32_t bsp_wifi_ioctl(uint32_t ui_ioctl_cmd, void *pv_arg, uint32_t ui_size);
	
#endif
