#include "basic.h"
#include "board.h"
#include "rtos.h"
#include "bsp_iic.h"

static bsp_iic_t gst_dev;

static void iic_start(void)
{
	return;
}

static void iic_stop(void)
{
	return;
}

static int32_t iic_ack(uint8_t uc_ack)
{
	return 0;
}

static uint8_t iic_read(void)
{
	uint8_t uc_data = 0;
	
	return uc_data;
}

static void iic_write(uint8_t uc_data)
{
	return;
}

int32_t bsp_iic_read(uint8_t uc_reg, uint8_t *puc_data, uint32_t ui_len)
{
	uint32_t i = 0;
	bsp_iic_t *pst_dev = &gst_dev;
	
	/* Todo: iic lock */
	
	
	do {
		/* Todo: iic start */
		iic_start();
		iic_write(pst_dev->uc_dev);
		if (0 != iic_ack(IIC_ACK)) break;
		iic_write(uc_reg & 0xFF);
		if (0 != iic_ack(IIC_ACK)) break;
		iic_start();
		iic_write(pst_dev->uc_dev | 0x01);
		if (0 != iic_ack(IIC_ACK)) break;
		
		for (i = 0; i < ui_len; i ++)
		{
			puc_data[i] = iic_read();
		}
		/* Todo: iic stop */		
		iic_stop();
	} while (0);
	
	/* Todo: iic_unlock */
	
	return 0;
}

int32_t bsp_iic_write(uint8_t uc_reg, uint8_t *puc_data, uint32_t ui_len)
{
	uint32_t i = 0;
	bsp_iic_t *pst_dev = &gst_dev;

	/* Todo: iic lock */
	
	do {
		/* Todo: iic start */
		iic_start();
		iic_write(pst_dev->uc_dev);
		if (0 != iic_ack(IIC_ACK)) break;
		iic_write(uc_reg & 0xFF);
		if (0 != iic_ack(IIC_ACK)) break;
		
		for (i = 0; i < ui_len; i ++)
		{
			iic_write(puc_data[i]);
			if (0 != iic_ack(IIC_ACK)) break;
		}

		/* Todo: iic stop */
		iic_stop();
	} while(0);
	
	/* Todo: iic_unlock */

	return 0;
}

void bsp_iic_init(void)
{
	return;
}

bsp_iic_t *bsp_iic_register(uint8_t uc_devid)
{
	bsp_iic_t *pst_dev = &gst_dev;
	
	pst_dev->uc_dev 	= uc_devid;
	pst_dev->read 		= bsp_iic_read;
	pst_dev->write 		= bsp_iic_write;
	
	return pst_dev;
}
