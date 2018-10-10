#ifndef __BSP_IIC_H__
#define __BSP_IIC_H__

#include <FreeRTOS.h>
#include <semphr.h>
#include "bsp_gpio.h"

#define IIC_ACK		1
#define IIC_NAK		0	

//IO方向设置
#define GPIO_SDA_IN()  {GPIOC->CRL&=0XFFFF0FFF;GPIOC->CRL|=8<<4*3;}
#define GPIO_SDA_OUT() {GPIOC->CRL&=0XFFFF0FFF;GPIOC->CRL|=3<<4*3;}

//IO操作函数	 
#define GPIO_IIC_SCL    PCout(0) 			//SCL     
#define GPIO_IIC_SDA    PCout(3) 			//SDA	 
#define GPIO_READ_SDA   PCin(3)  			//输入SDA 

typedef struct
{
	uint8_t uc_dev;
	SemaphoreHandle_t st_lock;
	int32_t (*write)(uint8_t uc_reg, uint8_t *puc_buf, uint32_t ui_len);
	int32_t (*read) (uint8_t uc_reg, uint8_t *puc_buf, uint32_t ui_len);
} bsp_iic_t;

extern void bsp_iic_init(void);

extern bsp_iic_t *bsp_iic_register(uint8_t uc_devid);

extern int32_t bsp_iic_read(uint8_t uc_reg, uint8_t *puc_data, uint32_t ui_len);

extern int32_t bsp_iic_write(uint8_t uc_reg, uint8_t *puc_data, uint32_t ui_len);

#endif
