#include "basic.h"
#include "FreeRTOS.h"
#include "bsp_uart.h"

static bsp_uart_s gast_uart[] = 
{
	{ USART1_BASE, USART1_IRQn, 115200, bsp_uart_out, NULL },
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
			GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;	//�����������
			GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9

			//USART_RX
			GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_10;//PA10
			GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;//��������
			GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10
		break;
		
		case USART2_BASE:
			
		break;
		
		case USART3_BASE:
			
		break;
		
		default:
		return;
	}
		
	//Usart NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel 						= pst_uart->ui_irq;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 3;				//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 3;				//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
    //USART ��ʼ������
	USART_InitStructure.USART_BaudRate 						= pst_uart->ui_baudrate;//���ڲ�����
	USART_InitStructure.USART_WordLength 					= USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits 						= USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity 						= USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl 			= USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode 							= USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init((USART_TypeDef *)pst_uart->ui_uart_base, &USART_InitStructure); //��ʼ������1
	USART_ITConfig((USART_TypeDef *)pst_uart->ui_uart_base, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd((USART_TypeDef *)pst_uart->ui_uart_base, ENABLE);                    //ʹ�ܴ���1 
}

void USART1_IRQHandler(void)
{	
	return;
}

void bsp_uart_init(void)
{
	int32_t i = 0;
	bsp_uart_s *pst_uart = NULL;
	
	for (i = 0; i < (sizeof(gast_uart) / sizeof(bsp_uart_s)); i ++) {
		pst_uart = &gast_uart[i];
		uart_init(pst_uart);
	}
	
	return;
}

int32_t bsp_uart_out(void *pv_arg, uint8_t uc_ch)
{
	bsp_uart_s *pst_uart = (bsp_uart_s *)pv_arg;
	if (NULL == pst_uart)
		return -1;
	
	while((((USART_TypeDef *)pst_uart->ui_uart_base)->SR&0X40)==0);
		((USART_TypeDef *)pst_uart->ui_uart_base)->DR = (uint8_t) uc_ch;      
	return uc_ch;	
}

int32_t bsp_uart_in(void *pv_arg, uint8_t uc_ch)
{
	return 0;
}

bsp_uart_s *bsp_uart_fetch(uint32_t ui_uart_no)
{
	if (ui_uart_no >= sizeof(gast_uart) / sizeof(bsp_uart_s))
		return NULL;
	
	return &gast_uart[ui_uart_no];
}

