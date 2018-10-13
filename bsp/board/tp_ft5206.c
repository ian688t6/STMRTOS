#include "basic.h"
#include "rtos.h"
#include "basic.h"
#include "board_tp.h"

/* ft5206 register */
#define REG_DEVICE_MODE			(0x00)
#define REG_TD_STATUS       	(0x02)
#define REG_TP1	 				(0X03)
#define REG_TP2 				(0X09)
#define REG_TP3 				(0X0F)
#define REG_TP4 				(0X15)
#define REG_TP5 				(0X1B) 
#define	REG_ID_G_LIB_VERSION	(0xA1)
#define REG_ID_G_MODE 			(0xA4)
#define REG_ID_G_THGROUP		(0x80)
#define REG_ID_G_PERIODACTIVE	(0x88)

/* ft5206 pin */
#define TP_RESET_PIN			PCout(13)

static uint8_t tp_init(bsp_iic_t *pst_iic)
{
	uint8_t uc_regval 	= 0;
	uint16_t us_version = 0;
	GPIO_InitTypeDef  st_gpio_init;	
	
	printf("ft5206 tp init \r\n");
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能PC端口时钟
	st_gpio_init.GPIO_Pin 	= GPIO_Pin_13;			// PC13端口配置
	st_gpio_init.GPIO_Mode 	= GPIO_Mode_Out_PP;	//推挽输出
	st_gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &st_gpio_init);
	GPIO_SetBits(GPIOC,GPIO_Pin_13);					//PC13输出1
	
	st_gpio_init.GPIO_Pin=GPIO_Pin_1;				//PC1端口配置
	st_gpio_init.GPIO_Mode=GPIO_Mode_IPU;			//上拉输入
	GPIO_Init(GPIOC, &st_gpio_init);				//PC1上拉输入
	GPIO_SetBits(GPIOC,GPIO_Pin_1);						//上拉	
 
	TP_RESET_PIN = 0;				//复位
	rtos_mdelay(20);
 	TP_RESET_PIN = 1;				//释放复位		    
	rtos_mdelay(50);  	
		
	/* Device mode: Normal */
	uc_regval = 0x00;
	pst_iic->write(REG_DEVICE_MODE, &uc_regval, sizeof(uc_regval));
	/* ID_G_ MODE: Polling mode */
	uc_regval = 0x00;
	pst_iic->write(REG_ID_G_MODE, &uc_regval, sizeof(uc_regval));
	/* ID_G_THGROUP */
	uc_regval = 22;
	pst_iic->write(REG_ID_G_THGROUP, &uc_regval, sizeof(uc_regval));
	/* ID_G_PERIODACTIVE */
	uc_regval = 12;
	pst_iic->write(REG_ID_G_PERIODACTIVE, &uc_regval, sizeof(uc_regval));

	/* Read Version */
	pst_iic->read(REG_ID_G_LIB_VERSION, (uint8_t *)&us_version, sizeof(us_version));
	printf("FT5206 IC ID_G_LIB_VERSION: %04x\r\n", us_version);
	
	return 0;
}

static uint8_t tp_scan(bsp_iic_t *pst_iic, touch_panel_state_t *pst_sta)
{
	int32_t i = 0;
	uint8_t	uc_temp		= 0xFF;
	uint8_t uc_tcont 	= 0;
	uint8_t auc_buf[4] 	= {0};
	uint8_t auc_tp_regs[TOUCH_PANEL_MAX_TCONT] = { REG_TP1, REG_TP2, REG_TP3, REG_TP4, REG_TP5 };
	
	/* Todo: read tp status , how many points detect */
	pst_iic->read(REG_TD_STATUS, &uc_tcont, sizeof(uint8_t));
	uc_tcont = uc_tcont & 0x0F;

	if (pst_sta->uc_tcont < uc_tcont)
		return 0;
	
	if (0 == uc_tcont) 
	{
		/* Todo: TP Pen Release */
		if (pst_sta->uc_tstat & 0x1F)
		{
			pst_sta->uc_tstat = 0;
			printf("pen release x[%d]:%d,y[%d]:%d\r\n", i, pst_sta->aus_x[i], i, pst_sta->aus_y[i]);
		}
		return 0;
	}
	
	/* Todo: TP Pen Down */
	uc_temp <<= uc_tcont;
	pst_sta->uc_tstat = ~uc_temp;
//	printf("tstat: 0x%02x\r\n", pst_sta->uc_tstat);
	
	for (i = 0; i < pst_sta->uc_tcont; i ++)
	{
		if (pst_sta->uc_tstat & (0x01 << i))
		{
			pst_iic->read(auc_tp_regs[i], auc_buf, sizeof(auc_buf));
			pst_sta->aus_y[i] = ((uint16_t)(auc_buf[0] & 0X0F)<<8) | auc_buf[1];
			pst_sta->aus_x[i] = ((uint16_t)(auc_buf[2] & 0X0F)<<8) | auc_buf[3];
		}
		
		if((auc_buf[0] & 0XF0)!=0X80)
		{
			pst_sta->aus_x[i] = 0;
			pst_sta->aus_y[i] = 0;		
		}
		
		printf("pen down x[%d]:%d,y[%d]:%d\r\n", i, pst_sta->aus_x[i], i, pst_sta->aus_y[i]);
	}
	
	return 0;
}

static touch_panel_t gst_tp =
{
	.uc_dev = 0X70,
	.init = tp_init,
	.scan = tp_scan,
};

touch_panel_t *touch_panel_get(void)
{
	return &gst_tp;
}


