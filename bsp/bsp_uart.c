#include "basic.h"
#include "FreeRTOS.h"
#include "bsp_uart.h"

static bsp_uart_s gast_uart[] = 
{
	{ 
			.ui_uart_base 	= USART1_BASE, 
			.ui_irq 		= USART1_IRQn, 
			.ui_baudrate	= 115200,
			.pf_out 		= bsp_uart_out, 
			.pf_in			= NULL 
	},

	{ 
			.ui_uart_base	= USART2_BASE, 
			.ui_irq			= USART2_IRQn, 
			.ui_baudrate 	= 115200,
			.pf_out 		= bsp_uart_out, 
			.pf_in			= bsp_uart_in, 
	},
	
	{ 
			.ui_uart_base	= USART3_BASE, 
			.ui_irq			= USART3_IRQn, 
			.ui_baudrate 	= 115200,
			.pf_out 		= bsp_uart_out, 
			.pf_in			= bsp_uart_in, 
	},
	
	{ 
			.ui_uart_base	= UART4_BASE, 
			.ui_irq			= UART4_IRQn, 
			.ui_baudrate 	= 115200,
			.pf_out 		= bsp_uart_out, 
			.pf_in			= bsp_uart_in, 
	},
	
	{ 
			.ui_uart_base	= UART5_BASE, 
			.ui_irq			= UART5_IRQn, 
			.ui_baudrate 	= 115200,
			.pf_out 		= bsp_uart_out, 
			.pf_in			= bsp_uart_in, 
	},	
};

static void uart_init(bsp_uart_s *pst_uart)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	USART_InitTypeDef 	USART_InitStructure;
	NVIC_InitTypeDef 	NVIC_InitStructure;

	switch (pst_uart->ui_uart_base)
	{
		case USART1_BASE:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	
			//USART_TX
			GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9; //PA.9
			GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;	//复用推挽输出
			GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

			//USART_RX
			GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_10;//PA10
			GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;//浮空输入
			GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10
		break;
		
		case USART2_BASE:
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
			USART_DeInit(USART2);

			GPIO_InitStructure.GPIO_Pin	 	= GPIO_Pin_2;
			GPIO_InitStructure.GPIO_Speed	= GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_AF_PP;
			GPIO_Init(GPIOA, &GPIO_InitStructure);

			GPIO_InitStructure.GPIO_Pin		= GPIO_Pin_3;
			GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_IN_FLOATING;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
		break;
	
		case USART3_BASE:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
			/* 配置UART3 TX GPIO PB10 */
			GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_10; 		//PB.10
			GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;	//复用推挽输出
			GPIO_Init(GPIOB, &GPIO_InitStructure);			//初始化GPIOC.10
			
			/* 配置UART3 RX GPIO PB11 */
			GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_11; 		//PB.11
			GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING; //浮空输入
			GPIO_Init(GPIOB, &GPIO_InitStructure);			//初始化GPIOB.11
		break;

		case UART4_BASE:
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
			/* 配置UART4 TX GPIO PC10 */
			GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_10; 		//PC.10
			GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;	//复用推挽输出
			GPIO_Init(GPIOC, &GPIO_InitStructure);			//初始化GPIOC.10
		
			/* 配置UART4 RX GPIO PC11 */
			GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_11; 		//PC.11
			GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING; //浮空输入
			GPIO_Init(GPIOC, &GPIO_InitStructure);			//初始化GPIOC.11
		break;
		
		case UART5_BASE:
			/* 使能UART5 */
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC, ENABLE);
		
			/* 配置UART5 TX GPIO PC10 */
			GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_12; 		//PC.12
			GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
			GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;	//复用推挽输出
			GPIO_Init(GPIOC, &GPIO_InitStructure);			//初始化GPIOC.12

			/* 配置UART5 RX GPIO PD2 */
			GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_2; 		//PD.2
			GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING; //浮空输入
			GPIO_Init(GPIOD, &GPIO_InitStructure);			//初始化GPIOD.2
		break;
		
		default:
		return;
	}
		
	//Usart NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel 						= pst_uart->ui_irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 3;				//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 3;				//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
    //USART 初始化设置
	USART_InitStructure.USART_BaudRate 						= pst_uart->ui_baudrate;//串口波特率
	USART_InitStructure.USART_WordLength 					= USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits 						= USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity 						= USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl 			= USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode 							= USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init((USART_TypeDef *)pst_uart->ui_uart_base, &USART_InitStructure); //初始化串口1
	USART_ITConfig((USART_TypeDef *)pst_uart->ui_uart_base, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd((USART_TypeDef *)pst_uart->ui_uart_base, ENABLE);                    //使能串口1 
	USART_GetFlagStatus((USART_TypeDef *)pst_uart->ui_uart_base, USART_FLAG_TC);

	memset(&pst_uart->st_fifo, 0x0, sizeof(uart_fifo));
//	vSemaphoreCreateBinary(pst_uart->st_fifo.st_sem);
//	xSemaphoreTake(pst_uart->st_fifo.st_sem , portMAX_DELAY) ;
}

void USART1_IRQHandler(void)
{	
	uint8_t uc_ch;
	bsp_uart_s *pst_uart = &gast_uart[BSP_UART1];
	if (RESET != USART_GetITStatus(USART1, USART_IT_RXNE))
	{
		//		USART_ClearITPendingBit(USART2,USART_IT_RXNE); //清除中断标志.
		uc_ch = USART_ReceiveData(USART1);
		pst_uart->st_fifo.auc_buf[pst_uart->st_fifo.us_len ++] = uc_ch;
		pst_uart->st_fifo.us_len = (pst_uart->st_fifo.us_len % (BSP_UART_FIFO_LEN - 1));
	}

	if (SET == USART_GetITStatus(USART1, USART_IT_IDLE)) 
	{
		USART_ClearFlag(USART1, USART_FLAG_ORE);
		USART_ReceiveData(USART1);
		uc_ch = USART_ReceiveData(USART1);
	}

	return;
}

void USART2_IRQHandler(void)
{
//	uint8_t uc_ch;
	bsp_uart_s *pst_uart = &gast_uart[BSP_UART2];
	
	if (RESET != USART_GetITStatus(USART2, USART_IT_RXNE))
	{
//		USART_ClearITPendingBit(USART2,USART_IT_RXNE); //清除中断标志.		
//		printf("+");
		pst_uart->st_fifo.auc_buf[pst_uart->st_fifo.us_len ++] = USART_ReceiveData(USART2);
		pst_uart->st_fifo.us_len = pst_uart->st_fifo.us_len % (BSP_UART_FIFO_LEN - 1);
	}

//	if ( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )                                         //数据帧接收完毕
//	{
//		USART_ClearFlag(USART3,USART_FLAG_ORE);  //读SR 
//		USART_ReceiveData(USART3); //读DR 
//		uc_ch = USART_ReceiveData( USART3 );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
//  }
	
	return;
}

void USART3_IRQHandler(void)
{
//	uint8_t uc_ch;
	bsp_uart_s *pst_uart = &gast_uart[BSP_UART3];
	
	if (RESET != USART_GetITStatus(USART3, USART_IT_RXNE))
	{
//		USART_ClearITPendingBit(USART2,USART_IT_RXNE); //清除中断标志.		
//		printf("+");
		pst_uart->st_fifo.auc_buf[pst_uart->st_fifo.us_len ++] = USART_ReceiveData(USART3);
		pst_uart->st_fifo.us_len = pst_uart->st_fifo.us_len % (BSP_UART_FIFO_LEN - 1);
	}

//	if ( USART_GetITStatus( USART2, USART_IT_IDLE ) == SET )                                         //数据帧接收完毕
//	{
//		USART_ClearFlag(USART3,USART_FLAG_ORE);  //读SR 
//		USART_ReceiveData(USART3); //读DR 
//		uc_ch = USART_ReceiveData( USART3 );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)
//  }
	
	return;
}

void UART4_IRQHandler(void)
{
	bsp_uart_s *pst_uart = &gast_uart[BSP_UART4];
	
	if (RESET != USART_GetITStatus(UART4, USART_IT_RXNE))
	{
//		USART_ClearITPendingBit(USART2,USART_IT_RXNE); //清除中断标志.		
//		printf("+");
		pst_uart->st_fifo.auc_buf[pst_uart->st_fifo.us_len ++] = USART_ReceiveData(UART4);
		pst_uart->st_fifo.us_len = pst_uart->st_fifo.us_len % (BSP_UART_FIFO_LEN - 1);
	}

	return;
}


void UART5_IRQHandler(void)
{
	bsp_uart_s *pst_uart = &gast_uart[BSP_UART5];
	
	if (RESET != USART_GetITStatus(UART5, USART_IT_RXNE))
	{
//		USART_ClearITPendingBit(USART2,USART_IT_RXNE); //清除中断标志.		
//		printf("+");
		pst_uart->st_fifo.auc_buf[pst_uart->st_fifo.us_len ++] = USART_ReceiveData(UART5);
		pst_uart->st_fifo.us_len = pst_uart->st_fifo.us_len % (BSP_UART_FIFO_LEN - 1);
	}

	return;
}

//void bsp_uart_init(void)
//{
//	int32_t i = 0;
//	bsp_uart_s *pst_uart = NULL;
//	
//	for (i = 0; i < (sizeof(gast_uart) / sizeof(bsp_uart_s)); i ++) {
//		pst_uart = &gast_uart[i];
//		uart_init(pst_uart);
//	}
//	
//	return;
//}

int32_t bsp_uart_out(void *pv_arg, uint8_t uc_ch)
{
	bsp_uart_s *pst_uart = (bsp_uart_s *)pv_arg;
	if (NULL == pst_uart)
		return -1;
	
	while((((USART_TypeDef *)pst_uart->ui_uart_base)->SR&0X40)==0);
		((USART_TypeDef *)pst_uart->ui_uart_base)->DR = (uint8_t) uc_ch;      
	return uc_ch;
}

int32_t bsp_uart_in(void *pv_arg, uint8_t **ppuc_buf)
{
	bsp_uart_s *pst_uart = (bsp_uart_s *)pv_arg;
	if (NULL == pst_uart)
		return 0;
		
	pst_uart->st_fifo.auc_buf[pst_uart->st_fifo.us_len] = '\0';
	*ppuc_buf = pst_uart->st_fifo.auc_buf;
		
	return pst_uart->st_fifo.us_len;
}

bsp_uart_s *bsp_uart_register(uint32_t ui_uart_no)
{
	bsp_uart_s *pst_uart = NULL;
	
	if (ui_uart_no >= sizeof(gast_uart) / sizeof(bsp_uart_s))
		return NULL;
	
	pst_uart = &gast_uart[ui_uart_no];
	uart_init(pst_uart);
	
	return pst_uart;
}

