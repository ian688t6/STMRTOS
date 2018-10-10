#ifndef __BSP_IIC_H__
#define __BSP_IIC_H__

#define IIC_ACK		0
#define IIC_NACK	1	

typedef struct
{
	uint8_t 	uc_dev;
	int32_t (*write)(uint8_t uc_reg, uint8_t *puc_buf, uint32_t ui_len);
	int32_t (*read) (uint8_t uc_reg, uint8_t *puc_buf, uint32_t ui_len);
} bsp_iic_t;

extern void bsp_iic_init(void);

extern bsp_iic_t *bsp_iic_register(uint8_t uc_devid);

extern int32_t bsp_iic_read(uint8_t uc_reg, uint8_t *puc_data, uint32_t ui_len);

extern int32_t bsp_iic_write(uint8_t uc_reg, uint8_t *puc_data, uint32_t ui_len);

#endif
