#include "basic.h"
#include "board.h"
#include "rtos.h"
#include "bsp_iic.h"

#define IIC_LOCK(lock) 		xSemaphoreTakeRecursive(lock, portMAX_DELAY)
#define IIC_UNLOCK(lock)	xSemaphoreGiveRecursive(lock)

static bsp_iic_t gst_dev;

static void iic_start(void)
{
	GPIO_SDA_OUT();     //sda线输出
	GPIO_IIC_SDA=1;	  	  
	GPIO_IIC_SCL=1;
	
	rtos_udelay(10);
 	GPIO_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	
	rtos_udelay(10);
	GPIO_IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 

	return;
}

static void iic_stop(void)
{
	GPIO_SDA_OUT();//sda线输出
	GPIO_IIC_SCL=0;
	GPIO_IIC_SDA=0;
	rtos_udelay(10);
	GPIO_IIC_SCL=1;
	rtos_udelay(10);
	GPIO_IIC_SDA=1;//STOP:when CLK is high DATA change form low to high 
	
	return;
}

static int32_t iic_wait_ack(void)
{
	uint8_t uc_err_time = 0;
	
	GPIO_SDA_IN();      //SDA设置为输入  
	GPIO_IIC_SDA = 1;
	rtos_udelay(1);	   
	
	GPIO_IIC_SCL = 1;
	rtos_udelay(1);	 
	
	while(GPIO_READ_SDA)
	{
		uc_err_time ++;
		if(uc_err_time > 250)
		{
			iic_stop();
			return -1;
		} 
	}
	
	GPIO_IIC_SCL=0;//时钟输出0 	   
	
	return 0;  
}


static int32_t iic_ack(void)
{
	GPIO_IIC_SCL = 0;
	GPIO_SDA_OUT();
	GPIO_IIC_SDA = 0;
	rtos_udelay(10);
	GPIO_IIC_SCL = 1;
	rtos_udelay(10);
	GPIO_IIC_SCL = 0;
	
	return 0;
}

static int32_t iic_nak(void)
{
	GPIO_IIC_SCL=0;
	GPIO_SDA_OUT();
	GPIO_IIC_SDA=1;
	rtos_udelay(10);
	GPIO_IIC_SCL=1;
	rtos_udelay(10);
	GPIO_IIC_SCL=0;
	
	return 0;
}

static uint8_t iic_read(uint32_t ui_ack)
{
	uint8_t i = 0;
	uint8_t uc_data = 0;
	
	GPIO_SDA_IN();
	for (i = 0; i < 8; i ++)
	{
	    GPIO_IIC_SCL = 0; 	    	   
		rtos_udelay(30);
		GPIO_IIC_SCL = 1;  
		uc_data <<= 1;
		uc_data |= GPIO_READ_SDA !=0 ? 0x01 : 0x00;
		//if(GPIO_READ_SDA) uc_data++; 
	}
	
	if (!ui_ack)
		iic_nak();	//发送nACK
	else 
		iic_ack();  //发送ACK
	
	return uc_data;
}

static void iic_write(uint8_t uc_data)
{
	uint8_t i = 0;
	
	GPIO_SDA_OUT(); 	    
    GPIO_IIC_SCL = 0;//拉低时钟开始数据传输
    
	for(i = 0; i < 8; i ++)
    {              
        GPIO_IIC_SDA = (uc_data & 0x80)>>7;
        uc_data <<= 1; 	      

		GPIO_IIC_SCL = 1;
		rtos_udelay(10);

		GPIO_IIC_SCL = 0;	
		rtos_udelay(10);
    }
	
	return;
}

int32_t bsp_iic_read(uint8_t uc_reg, uint8_t *puc_data, uint32_t ui_len)
{
	uint32_t i = 0;
	bsp_iic_t *pst_dev = &gst_dev;
	
	/* Todo: iic lock */
	IIC_LOCK(pst_dev->st_lock);

	do {
		/* Todo: iic start */
		iic_start();
		iic_write(pst_dev->uc_dev);
		if (0 != iic_wait_ack()) break;
		iic_write(uc_reg & 0xFF);
		if (0 != iic_wait_ack()) break;
		iic_start();
		iic_write(pst_dev->uc_dev | 0x01);
		if (0 != iic_wait_ack()) break;
		
		for (i = 0; i < ui_len; i ++)
		{
			puc_data[i] = iic_read(i == (ui_len-1) ? IIC_NAK : IIC_ACK);
		}
		/* Todo: iic stop */		
		iic_stop();
	} while (0);
	
	/* Todo: iic_unlock */
	IIC_UNLOCK(pst_dev->st_lock);
	
	return 0;
}

int32_t bsp_iic_write(uint8_t uc_reg, uint8_t *puc_data, uint32_t ui_len)
{
	uint32_t i = 0;
	bsp_iic_t *pst_dev = &gst_dev;

	/* Todo: iic lock */
	IIC_LOCK(pst_dev->st_lock);
	
	do {
		/* Todo: iic start */
		iic_start();
		iic_write(pst_dev->uc_dev);
		if (0 != iic_wait_ack()) break;
		iic_write(uc_reg & 0xFF);
		if (0 != iic_wait_ack()) break;
		
		for (i = 0; i < ui_len; i ++)
		{
			iic_write(puc_data[i]);
			if (0 != iic_wait_ack()) break;
		}

		/* Todo: iic stop */
		iic_stop();
	} while(0);
	
	/* Todo: iic_unlock */
	IIC_UNLOCK(pst_dev->st_lock);
	
	return 0;
}

void bsp_iic_init(void)
{
	bsp_iic_t *pst_dev = &gst_dev;
//	GPIO_InitTypeDef st_gpio_init;

// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 	// enable PORTC Clock
//	st_gpio_init.GPIO_Pin 	= GPIO_Pin_0 | GPIO_Pin_3;		// PC0,PC3
//	st_gpio_init.GPIO_Mode 	= GPIO_Mode_Out_PP;				// Mode out pp
//	st_gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC, &st_gpio_init);
//	GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_3);
	
	RCC->APB2ENR|=1<<4;		//先使能外设IO PORTC时钟    
	GPIOC->CRL&=0XFFFF0FF0;	//PC0,PC3 推挽输出
	GPIOC->CRL|=0X00003003;	   
	GPIOC->ODR|=1<<1;	    //PC0 输出高	 
	GPIOC->ODR|=1<<3;	    //PC3 输出高
	pst_dev->st_lock = xSemaphoreCreateRecursiveMutex();
	
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
