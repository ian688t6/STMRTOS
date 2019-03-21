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

	while (SPI_I2S_GetFlagStatus((SPI_TypeDef *) ui_spi_base, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		uc_retry ++;
		if(uc_retry > 200)return 0;
	}			  
	SPI_I2S_SendData((SPI_TypeDef *)ui_spi_base, 0xFF); //通过外设SPIx发送一个数据
	uc_retry=0;

	while (SPI_I2S_GetFlagStatus((SPI_TypeDef *) ui_spi_base, SPI_I2S_FLAG_RXNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		uc_retry ++;
		if(uc_retry>200)return 0;
	}	  						    
	return SPI_I2S_ReceiveData((SPI_TypeDef *) ui_spi_base); //返回通过SPIx最近接收的数据
}

static uint8_t spi_write_byte(uint32_t ui_spi_base, uint8_t uc_data)
{
	uint8_t uc_retry=0;

	while (SPI_I2S_GetFlagStatus((SPI_TypeDef *) ui_spi_base, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		uc_retry ++;
		if(uc_retry > 200)return 0;
	}			  
	SPI_I2S_SendData((SPI_TypeDef *)ui_spi_base, uc_data); //通过外设SPIx发送一个数据
	uc_retry=0;

	while (SPI_I2S_GetFlagStatus((SPI_TypeDef *) ui_spi_base, SPI_I2S_FLAG_RXNE) == RESET)//检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		uc_retry ++;
		if(uc_retry>200)return 0;
	}	  						    
	return SPI_I2S_ReceiveData((SPI_TypeDef *) ui_spi_base); //返回通过SPIx最近接收的数据
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
	
	st_spi_init.SPI_Direction 	= SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	st_spi_init.SPI_Mode 		= SPI_Mode_Master;				//设置SPI工作模式:设置为主SPI
	st_spi_init.SPI_DataSize 	= SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	st_spi_init.SPI_CPOL 		= SPI_CPOL_High;		//选择了串行时钟的稳态:时钟悬空高
	st_spi_init.SPI_CPHA 		= SPI_CPHA_2Edge;	//数据捕获于第二个时钟沿
	st_spi_init.SPI_NSS 		= SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	st_spi_init.SPI_BaudRatePrescaler 	= SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
	st_spi_init.SPI_FirstBit 			= SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	st_spi_init.SPI_CRCPolynomial 		= 7;	//CRC值计算的多项式
	SPI_Init((SPI_TypeDef *) pst_spi->ui_spi_base, &st_spi_init);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	SPI_Cmd((SPI_TypeDef *) pst_spi->ui_spi_base, ENABLE); //使能SPI外设
	
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

