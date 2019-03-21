#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__

#include "FreeRTOS.h"

typedef enum
{
	BSP_SPI1 = 0,
	BSP_SPI2,
} spi_num_e;

typedef int32_t (*spi_out)(void *pv_priv, uint8_t *puc_buf, uint32_t ui_len);
typedef int32_t (*spi_in)(void *pv_priv,  uint8_t *puc_buf, uint32_t ui_len);

typedef struct
{
	uint32_t	ui_spi_base;
	spi_in		pf_in;
	spi_out		pf_out;
} bsp_spi_s;

extern bsp_spi_s *bsp_spi_register(uint32_t ui_spi_num);

extern int32_t bsp_spi_in(void *pv_priv, uint8_t *puc_buf, uint32_t ui_len);

extern int32_t bsp_spi_out(void *pv_priv, uint8_t *puc_buf, uint32_t ui_len);

#endif
