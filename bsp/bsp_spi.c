#include "basic.h"
#include "FreeRTOS.h"
#include "bsp_spi.h"

static bsp_spi_s gast_spi[] = 
{
	{
		.ui_spi_base 	= SPI1_BASE,
		.pf_in			= bsp_spi_in,
		.pf_out			= bsp_spi_out,
	},
	
	{
		.ui_spi_base 	= SPI2_BASE,
		.pf_in			= bsp_spi_in,
		.pf_out			= bsp_spi_out,
	},	
};

static uint8_t spi_read_byte(uint32_t ui_spi_base)
{
	uint8_t uc_retry=0;

	while (SPI_I2S_GetFlagStatus((SPI_TypeDef *) ui_spi_base, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		uc_retry ++;
		if(uc_retry > 200)return 0;
	}			  
	SPI_I2S_SendData((SPI_TypeDef *)ui_spi_base, 0xFF); //ͨ������SPIx����һ������
	uc_retry=0;

	while (SPI_I2S_GetFlagStatus((SPI_TypeDef *) ui_spi_base, SPI_I2S_FLAG_RXNE) == RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		uc_retry ++;
		if(uc_retry>200)return 0;
	}	  						    
	return SPI_I2S_ReceiveData((SPI_TypeDef *) ui_spi_base); //����ͨ��SPIx������յ�����
}

static uint8_t spi_write_byte(uint32_t ui_spi_base, uint8_t uc_data)
{
	uint8_t uc_retry=0;

	while (SPI_I2S_GetFlagStatus((SPI_TypeDef *) ui_spi_base, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		uc_retry ++;
		if(uc_retry > 200)return 0;
	}			  
	SPI_I2S_SendData((SPI_TypeDef *)ui_spi_base, uc_data); //ͨ������SPIx����һ������
	uc_retry=0;

	while (SPI_I2S_GetFlagStatus((SPI_TypeDef *) ui_spi_base, SPI_I2S_FLAG_RXNE) == RESET)//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		uc_retry ++;
		if(uc_retry>200)return 0;
	}	  						    
	return SPI_I2S_ReceiveData((SPI_TypeDef *) ui_spi_base); //����ͨ��SPIx������յ�����
}

static void spi_init(bsp_spi_s *pst_spi)
{
	SPI_InitTypeDef 	st_spi_init;
	GPIO_InitTypeDef 	st_gpio_init;
	
	switch (pst_spi->ui_spi_base) {
		case SPI1_BASE:
			RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );	
			st_gpio_init.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
			st_gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
			st_gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &st_gpio_init);
			GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
		break;
		
		case SPI2_BASE:
			RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );
			RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE );
			st_gpio_init.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
			st_gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
			st_gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOB, &st_gpio_init);
			GPIO_SetBits(GPIOB,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
		break;
		
		default:
			
		break;
	}
	
	st_spi_init.SPI_Direction 	= SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	st_spi_init.SPI_Mode 		= SPI_Mode_Master;				//����SPI����ģʽ:����Ϊ��SPI
	st_spi_init.SPI_DataSize 	= SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	st_spi_init.SPI_CPOL 		= SPI_CPOL_High;		//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	st_spi_init.SPI_CPHA 		= SPI_CPHA_2Edge;	//���ݲ����ڵڶ���ʱ����
	st_spi_init.SPI_NSS 		= SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	st_spi_init.SPI_BaudRatePrescaler 	= SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	st_spi_init.SPI_FirstBit 			= SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	st_spi_init.SPI_CRCPolynomial 		= 7;	//CRCֵ����Ķ���ʽ
	SPI_Init((SPI_TypeDef *) pst_spi->ui_spi_base, &st_spi_init);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	SPI_Cmd((SPI_TypeDef *) pst_spi->ui_spi_base, ENABLE); //ʹ��SPI����
	
	return;
}

int32_t bsp_spi_in(void *pv_priv, uint8_t *puc_buf, uint32_t ui_len)
{
	int32_t i = 0;
	bsp_spi_s *pst_spi = (bsp_spi_s *)pv_priv;
	
	for (i = 0; i < ui_len; i ++) {
		puc_buf[0] = spi_read_byte(pst_spi->ui_spi_base);
	}
	
	return ui_len;
}

int32_t bsp_spi_out(void *pv_priv, uint8_t *puc_buf, uint32_t ui_len)
{
	int32_t i = 0;
	bsp_spi_s *pst_spi = (bsp_spi_s *)pv_priv;

	for (i = 0; i < ui_len; i ++) {
		spi_write_byte(pst_spi->ui_spi_base, puc_buf[i]);
	}
	
	return ui_len;
}

bsp_spi_s *bsp_spi_register(uint32_t ui_spi_num)
{
	bsp_spi_s *pst_spi = NULL;
	
	if (ui_spi_num >= (sizeof(gast_spi) / sizeof(bsp_spi_s)))
		return NULL;
	
	pst_spi = &gast_spi[ui_spi_num];
	spi_init(pst_spi);
	
	return pst_spi;
}

