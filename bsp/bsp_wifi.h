#ifndef __BSP_WIFI_H__
#define __BSP_WIFI_H__

#define WIFI_MODE_STA		"1"
#define WIFI_MODE_AP		"2"
#define WIFI_MODE_STA_AP	"3"

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

extern void bsp_wifi_init(void);

extern int32_t bsp_wifi_ioctl(uint32_t ui_ioctl_cmd, void *pv_arg, uint32_t ui_size);
	
#endif
