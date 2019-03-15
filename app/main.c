#include "basic.h"
#include "rtos.h"
#include "FreeRTOS.h"
#include "bsp.h"
#include "task.h"
#include "ui.h"
#include "GUIDemo.h"

#define START_TASK_PRIO		(1)
#define START_STK_SIZE		(256)

#define LED0_TASK_PRIO		(2)
#define LED0_STK_SIZE		(50)

#define LED1_TASK_PRIO		(3)
#define LED1_STK_SIZE		(50)

#define GUI_TASK_PRIO		(4)
#define GUI_STK_SIZE		(128)

#define NETWORK_TASK_PRIO	(7)
#define NETWORK_STK_SIZE	(128)


TaskHandle_t StartTask_Handler;
TaskHandle_t LED0Task_Handler;
TaskHandle_t LED1Task_Handler;
TaskHandle_t GUITask_Handler;
TaskHandle_t NETWORKTask_Handler;

static void led_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_8);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_SetBits(GPIOD,GPIO_Pin_2);
	
	return;
}

static void led0_task(void *pvParameters)
{
	while (1)
	{
		printf("led0_task...\r\n");
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);
		rtos_mdelay(500);
		GPIO_SetBits(GPIOA,GPIO_Pin_8);	
		rtos_mdelay(500);
	}
}

static void led1_task(void *pvParameters)
{
	while (1)
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_2);
		rtos_mdelay(200);
		GPIO_ResetBits(GPIOD,GPIO_Pin_2);
		rtos_mdelay(800);		
	}
}
#define SCREEN_DIV          6 // 2^6 = 64
#define FACTOR_EMWIN        4
#define FACTOR_DESC        11
#define FACTOR_ANY_COMP    22
#define FACTOR_VERSION     31
#define FACTOR_LOGO        38
#define FACTOR_WWW         56

static void network_task(void *pv_param)
{
	wifi_ssid_s 	*pst_ssid 	= NULL;
	wifi_ifaddr_s 	*pst_ifaddr = NULL;
	wifi_conn_s		*pst_conn	= NULL;
	wifi_link_status_s *pst_link = NULL;
	uint32_t 		ui_txmode	= 0;
	int32_t			i_ret = 0;
	/* Todo: set wifi workmode: sta + ap */
	bsp_wifi_ioctl(IOCTL_WIFI_SET_MODE, WIFI_MODE_STA_AP, strlen(WIFI_MODE_STA_AP) + 1);
	
	
	/* Todo: join wifi ap */
	pst_ssid = (wifi_ssid_s *)pvPortMalloc(sizeof(wifi_ssid_s));
	if (NULL == pst_ssid)
	{
		return;
	}
	strncpy(pst_ssid->ac_ssidname, "ruifeng1", sizeof(pst_ssid->ac_ssidname));
	strncpy(pst_ssid->ac_password, "88888888", sizeof(pst_ssid->ac_password));
	do {
		i_ret = bsp_wifi_ioctl(IOCTL_WIFI_JOIN_AP, pst_ssid, sizeof(wifi_ssid_s));
	} while (0 != i_ret);
	vPortFree(pst_ssid);
	printf("connect ap success!\r\n");
	
	
	/* Todo: query wifi ifaddr*/
	pst_ifaddr = (wifi_ifaddr_s *)pvPortMalloc(sizeof(wifi_ifaddr_s));
	if (NULL == pst_ifaddr)
	{
		return;
	}
	bsp_wifi_ioctl(IOCTL_WIFI_GET_IFADDR, pst_ifaddr, sizeof(wifi_ifaddr_s));
	printf("APaddr: %s APmac: %s STAaddr: %s STAmac: %s\r\n", 
			pst_ifaddr->ac_ap_addr, pst_ifaddr->ac_ap_mac,
			pst_ifaddr->ac_sta_addr, pst_ifaddr->ac_sta_mac);
	vPortFree(pst_ifaddr);

	/* Todo: wifi connect */
	pst_conn = (wifi_conn_s *)pvPortMalloc(sizeof(wifi_conn_s));
	if (NULL == pst_conn)
	{
		return;
	}
	pst_conn->ui_mux = 0;
	strncpy(pst_conn->ac_type, "TCP", sizeof(pst_conn->ac_type));
	strncpy(pst_conn->ac_addr, "192.168.1.141", sizeof(pst_conn->ac_addr));
	pst_conn->ui_rmt_port = 8099;
	i_ret = bsp_wifi_ioctl(IOCTL_WIFI_CONNECT, pst_conn, sizeof(wifi_conn_s));
	if (0 != i_ret)
	{
		printf("wifi connect failed!\r\n");
	}
	vPortFree(pst_conn);
	
	/* Todo: wifi set txmode */
	bsp_wifi_ioctl(IOCTL_WIFI_SET_TXMODE, &ui_txmode, sizeof(int32_t));
	
	for (;;)
	{
		printf("network task ...\r\n");
//		bsp_wifi_ioctl(IOCTL_WIFI_TX, "hello ian", strlen("hello ian"));
		pst_link = (wifi_link_status_s *)pvPortMalloc(sizeof(wifi_link_status_s));
		if (NULL == pst_link)
		{
			return;
		}
		memset(pst_link, 0x0, sizeof(wifi_link_status_s));
		bsp_wifi_ioctl(IOCTL_WIFI_GET_LINKSTATUS, pst_link, sizeof(wifi_link_status_s));
		printf("linkstatus status: %d conntype: %d id: %d addr: %s rmtport: %d locport: %d tetype: %d\r\n", 
			pst_link->ui_status, pst_link->ui_conntype, pst_link->ui_id,
			pst_link->ac_addr, pst_link->ui_rmt_port, pst_link->ui_loc_port, pst_link->ui_tetype);
		vPortFree(pst_link);
		rtos_mdelay(1000);
	}
}

static void gui_task(void *pv_param)
{
//	char acVersion[30] = "Version of STemWin: ";
//	int  xCenter, xSize, ySize;
	ui_init();
	ui_mtouch_enable();
	printf("gui_task ...\r\n");

//	
//	xSize   = LCD_GetXSize();
//	ySize   = LCD_GetYSize();
//	xCenter = xSize >> 1;
//	GUI_SetTextMode(GUI_TM_TRANS);
//  //
//  // emWin
//  //
//	GUI_SetFont(&GUI_Font24_ASCII);
//	GUI_DispStringHCenterAt("STemWin", xCenter, (FACTOR_EMWIN * ySize) >> SCREEN_DIV);

	printf("gui_task 1...\r\n");
	for (;;)
	{
//			printf("gui_task 2...\r\n");
////	GUI_SetBkColor(GUI_BLUE);
////	GUI_SetColor(GUI_WHITE);
////	GUI_Clear();
//		GUIDEMO_Main();
		rtos_mdelay(500);
	}
}

static void start_task(void *pvParameters)
{	
	taskENTER_CRITICAL();
	bsp_wifi_init();
	
	xTaskCreate((TaskFunction_t)led0_task,
				(const char *)"led0_task",
				(uint16_t)LED0_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)LED0_TASK_PRIO,
				(TaskHandle_t *)&LED0Task_Handler);

	xTaskCreate((TaskFunction_t)led1_task,
				(const char *)"led1_task",
				(uint16_t)LED1_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)LED1_TASK_PRIO,
				(TaskHandle_t *)&LED1Task_Handler);
#if 1
	xTaskCreate((TaskFunction_t)gui_task,
				(const char *)"gui_task",
				(uint16_t)GUI_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)GUI_TASK_PRIO,
				(TaskHandle_t *)&GUITask_Handler);
#endif

#if 1				
	xTaskCreate((TaskFunction_t)network_task,
				(const char *)"network_task",
				(uint16_t)NETWORK_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)NETWORK_TASK_PRIO,
				(TaskHandle_t *)&NETWORKTask_Handler);
#endif				
	vTaskDelete(StartTask_Handler);
	taskEXIT_CRITICAL();
}

void Delay(u32 count)
{
	u32 i=0;
	for(;i<count;i++);
}

int main(void)
{	
	rtos_sys_init();
	rtos_mem_init();
	rtos_console_init();

	bsp_disp_init();
	bsp_iic_init();

	led_init();
	
	xTaskCreate((TaskFunction_t)start_task,
				(const char *)"start_task",
				(uint16_t)START_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)START_TASK_PRIO,
				(TaskHandle_t *)&StartTask_Handler);

	vTaskStartScheduler();
}

