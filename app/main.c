#include "basic.h"
#include "rtos.h"
#include "FreeRTOS.h"
#include "bsp.h"
#include "task.h"
#include "GUI.h"
#include "GUIDemo.h"

#define START_TASK_PRIO		(1)
#define START_STK_SIZE		(128)

#define LED0_TASK_PRIO		(2)
#define LED0_STK_SIZE		(50)

#define LED1_TASK_PRIO		(3)
#define LED1_STK_SIZE		(50)

#define GUI_TASK_PRIO		(4)
#define GUI_STK_SIZE		(512)

TaskHandle_t StartTask_Handler;
TaskHandle_t LED0Task_Handler;
TaskHandle_t LED1Task_Handler;
TaskHandle_t GUITask_Handler;

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

static void gui_task(void *pv_param)
{
	printf("gui_task ...\r\n");
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	GUI_Init();
	WM_SetCreateFlags(WM_CF_MEMDEV);

	GUI_SetBkColor(GUI_BLUE);
	GUI_SetColor(GUI_YELLOW);
	GUI_Clear();
	GUI_SetFont(&GUI_Font24_ASCII);
	GUI_DispStringAt("Hello World!", 0, 0);
	printf("gui_task 1...\r\n");
	for (;;)
	{
//		GUIDEMO_Main();
		rtos_mdelay(500);
	}
}

static void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();
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
	bsp_uart_init();
	bsp_disp_init();
	rtos_console_init();
	
	led_init();
	
	xTaskCreate((TaskFunction_t)start_task,
				(const char *)"start_task",
				(uint16_t)START_STK_SIZE,
				(void *)NULL,
				(UBaseType_t)START_TASK_PRIO,
				(TaskHandle_t *)&StartTask_Handler);

	vTaskStartScheduler();
}

