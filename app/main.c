#include "basic.h"
#include "rtos.h"
#include "FreeRTOS.h"
#include "bsp.h"
#include "task.h"

#define START_TASK_PRIO		(1)
#define START_STK_SIZE		(128)

#define LED0_TASK_PRIO		(2)
#define LED0_STK_SIZE		(50)

#define LED1_TASK_PRIO		(3)
#define LED1_STK_SIZE		(50)

TaskHandle_t StartTask_Handler;
TaskHandle_t LED0Task_Handler;
TaskHandle_t LED1Task_Handler;

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

