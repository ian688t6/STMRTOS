#include "basic.h"
#include "board.h"
#include "rtos.h"
#include "board_lcd.h"
#include "bsp_disp.h"

#define	LCD_CS_SET  GPIOC->BSRR=1<<9    //片选端口  		PC9
#define	LCD_RS_SET	GPIOC->BSRR=1<<8    //数据/命令 		PC8	   
#define	LCD_WR_SET	GPIOC->BSRR=1<<7    //写数据			PC7
#define	LCD_RD_SET	GPIOC->BSRR=1<<6    //读数据			PC6
								    
#define	LCD_CS_CLR  GPIOC->BRR=1<<9     //片选端口  		PC9
#define	LCD_RS_CLR	GPIOC->BRR=1<<8     //数据/命令			PC8	   
#define	LCD_WR_CLR	GPIOC->BRR=1<<7     //写数据			PC7
#define	LCD_RD_CLR	GPIOC->BRR=1<<6     //读数据			PC6   

//PB0~15,作为数据线
#define DATAOUT(x) GPIOB->ODR=x; //数据输出
#define DATAIN     GPIOB->IDR;   //数据输入	

static lcd_panel_t *gpst_drv;
static bsp_disp_t gst_disp;

static uint16_t disp_bgr2rgb(uint16_t c)
{
	u16  r,g,b,rgb;   
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
}

static void disp_reg(uint16_t us_reg)
{
	LCD_RS_CLR;
 	LCD_CS_CLR; 
	DATAOUT(us_reg); 
	LCD_WR_CLR; 
	LCD_WR_SET; 
 	LCD_CS_SET;
	
	return ;
}

static void disp_wdata(uint16_t us_data)
{
	LCD_RS_SET;
	LCD_CS_CLR;
	DATAOUT(us_data);
	LCD_WR_CLR;
	LCD_WR_SET;
	LCD_CS_SET;
	return ;
}

static uint16_t disp_rdata(void)
{
	uint16_t t;
 	GPIOB->CRL=0X88888888; //PB0-7  上拉输入
	GPIOB->CRH=0X88888888; //PB8-15 上拉输入
	GPIOB->ODR=0X0000;     //全部输出0

	LCD_RS_SET;
	LCD_CS_CLR;
	//读取数据(读寄存器时,并不需要读2次)
	LCD_RD_CLR;
	LCD_RD_SET;
	LCD_CS_SET; 

	GPIOB->CRL=0X33333333; //PB0-7  上拉输出
	GPIOB->CRH=0X33333333; //PB8-15 上拉输出
	GPIOB->ODR=0XFFFF;    //全部输出高
	return t;  
}

static void disp_tx(lcd_cmd_t *pst_cmd)
{
	int i = 0;
	
	for (i = 0;  i < pst_cmd->ui_size; i ++) {
		if (0 == i) {
			disp_reg(pst_cmd->puc_payload[i]);
			continue;
		}
		disp_wdata(pst_cmd->puc_payload[i]);
	}
	
	return;
}

static void disp_setpos(uint16_t us_x, uint16_t us_y)
{
	lcd_panel_t *pst_lcd = gpst_drv;	
	
//	printf("set pos x:%d y:%d \r\n", us_x, us_y);
	
	switch (pst_lcd->st_cmd_setpos.pst_x->ui_size)
	{
		case 1:
		disp_reg(pst_lcd->st_cmd_setpos.pst_x->puc_payload[0]);
		disp_wdata(us_x >> 8);
		disp_wdata(us_x & 0xff);
		disp_wdata((pst_lcd->ui_xres - 1) >> 8);		
		disp_wdata((pst_lcd->ui_xres - 1) & 0xff);
		break;
		
		case 2:
		disp_reg(pst_lcd->st_cmd_setpos.pst_x->puc_payload[0]);
		disp_wdata(us_x >> 8);
		disp_reg(pst_lcd->st_cmd_setpos.pst_x->puc_payload[1]);
		disp_wdata(us_x & 0xff);	
		break;
		
		default:
		break;
	}
	
	switch (pst_lcd->st_cmd_setpos.pst_y->ui_size)
	{
		case 1:
		disp_reg(pst_lcd->st_cmd_setpos.pst_y->puc_payload[0]);
		disp_wdata(us_y >> 8);
		disp_wdata(us_y & 0xff);			
		disp_wdata((pst_lcd->ui_yres - 1) >> 8);		
		disp_wdata((pst_lcd->ui_yres - 1) & 0xff);
		break;
		
		case 2:
		disp_reg(pst_lcd->st_cmd_setpos.pst_y->puc_payload[0]);
		disp_wdata(us_y >> 8);
		disp_reg(pst_lcd->st_cmd_setpos.pst_y->puc_payload[1]);
		disp_wdata(us_y & 0xff);				
		break;
		
		default:
		break;
	}
	
	return;
}

static void disp_tx_cmds(lcd_cmd_t *pst_cmds, uint32_t ui_count)
{
	int32_t i = 0;
	lcd_cmd_t *pst_cmd = pst_cmds;
	
	for (i = 0; i < ui_count; i ++) {
		disp_tx(pst_cmd);
		rtos_mdelay(pst_cmd->ui_delay);
		pst_cmd ++;
	}
		
	return;
}

static void disp_set_pixel(uint16_t us_x, uint16_t us_y, uint16_t us_color)
{
	bsp_disp_draw_point(us_x, us_y, us_color);
	return;
}

static uint16_t disp_get_pixel(uint16_t us_x, uint16_t us_y)
{
	uint16_t us_ret;
	uint16_t us_r, us_g, us_b;
	lcd_panel_t *pst_lcd = gpst_drv;
	
	if ((us_x >= pst_lcd->ui_xres) || (us_y >= pst_lcd->ui_yres))
		return 0;
	
	disp_setpos(us_x, us_y);
	disp_reg(pst_lcd->st_gram.us_rgram);

	GPIOB->CRL=0X88888888;
	GPIOB->CRH=0X88888888;
	GPIOB->ODR=0XFFFF;

	LCD_RS_SET;
	LCD_CS_CLR;	    

	//读取数据(读GRAM时,第一次为假读)	
	LCD_RD_CLR;		   
	rtos_udelay(1);										//延时		
 	us_ret = DATAIN; 										//实际坐标颜色 
	LCD_RD_SET;
	
	if (pst_lcd->us_flag & FLAG_DUMMY_READ)
	{
		LCD_RD_CLR;		   
		rtos_udelay(1);										//延时		
		us_r = DATAIN;  										//实际坐标颜色 
		LCD_RD_SET; 
		if (pst_lcd->us_flag & FLAG_TWICE_READ)
		{
			LCD_RD_CLR;					   
			rtos_udelay(1);										//延时
			us_b = DATAIN;//读取蓝色值  	  
			LCD_RD_SET;
			us_g = us_r&0XFF;//对于9341,第一次读取的是RG的值,R在前,G在后,各占8位
			us_g <<= 8;
			us_ret = (((us_r>>11)<<11)|((us_g>>10)<<5)|(us_b>>11));
		}
		else
		{
			LCD_RD_CLR;					   
			LCD_RD_SET;
			us_r = DATAIN;//6804第二次读取的才是真实值 
			us_ret = us_r;
		}
	}
	
	LCD_CS_SET;
	GPIOB->CRL=0X33333333; 			//PB0-7  上拉输出
	GPIOB->CRH=0X33333333; 			//PB8-15 上拉输出
	GPIOB->ODR=0XFFFF;    			//全部输出高  
	
	return !(pst_lcd->us_flag & FLAG_BGR2RGB) ? us_ret: disp_bgr2rgb(us_ret);					//1963直接读就可以 
}

static void disp_fill_color(uint16_t us_x1, uint16_t us_y1, uint16_t us_x2, uint16_t us_y2, uint16_t us_color)
{
	u16 i,j;
	u16 xlen=0;
	lcd_panel_t *pst_lcd = gpst_drv;
	
	xlen = us_x2 - us_x1 + 1;	 
	for(i = us_y1; i <= us_y2; i++)
	{
		disp_setpos(us_x1,i);      				//?????? 
		disp_reg(pst_lcd->st_gram.us_wgram);
		for(j = 0; j < xlen; j ++)
			disp_wdata(us_color);	//?????? 	    
	}
	
	return;
}

void bsp_disp_test(void)
{
	int32_t x = 0;
	int32_t y = 0;
	int32_t k = 0;
	board_t *pst_bd = board_get();
	lcd_panel_t *pst_lcd = pst_bd->pf_lcd_panel_get();	
	disp_reg(pst_lcd->st_gram.us_wgram);
	for (y = 0; y < pst_lcd->ui_yres; y += 16, k ++) {
		for (x = y * pst_lcd->ui_xres; x < y * pst_lcd->ui_xres + pst_lcd->ui_xres * 16; x ++) {
			switch (k % 16)
			{
				case 0:
					disp_wdata(0x07E0);	
				break;
				
				case 1:
					disp_wdata(0x001F);
				break;
				
				case 2:
					disp_wdata(0XF81F);
				break;
				
				case 3:
					disp_wdata(0XFFE0);
				break;
				
				case 4:
					disp_wdata(0X07FF);
				break;
				
				case 5:
					disp_wdata(0xF800);
				break;
				
				case 6:
					disp_wdata(0xF81F);
				break;
				
				case 7:
					disp_wdata(0XBC40);
					break;
				
				case 8:
					disp_wdata(0XFC07);
				break;
				
				case 9:
					disp_wdata(0X8430);
				break;
				
				case 10:
					disp_wdata(0X01CF);
				break;
				
				case 11:
					disp_wdata(0XC618);
				break;
				
				case 12:
					disp_wdata(0XA651);
				break;
				
				case 13:
					disp_wdata(0X2B12);
				break;
				
				case 14:
					disp_wdata(0x0000);
				break;
			
				case 15:
					disp_wdata(0xFFFF);
				break;
			}	
		}
	}
}

uint16_t bsp_disp_id(void)
{
	uint16_t us_id;
	lcd_panel_t *pst_lcd = gpst_drv;
	
	disp_reg(pst_lcd->us_id_addr);
	disp_rdata();
	disp_rdata();
	us_id = disp_rdata();
	us_id <<= 8;
	us_id |= disp_rdata();
	
	return us_id;
}

void bsp_disp_draw_point(uint16_t us_x, uint16_t us_y, uint16_t us_color)
{
	lcd_panel_t *pst_lcd = gpst_drv;

	disp_setpos(us_x, us_y);
	disp_reg(pst_lcd->st_gram.us_wgram);
	disp_wdata(us_color);
	
	return;
}

void bsp_disp_draw_line(uint16_t us_x1, uint16_t us_y1, uint16_t us_x2, uint16_t us_y2)
{
	uint16_t t;
	int32_t i_distance 	= 0;
	int32_t i_row 		= us_x1;
	int32_t i_col 		= us_y1; 
	int32_t i_xerr = 0, i_yerr = 0;
	int32_t i_incx = 0, i_incy = 0;
	int32_t i_delta_x = us_x2 - us_x1;
	int32_t i_delta_y = us_y2 - us_y1;
	
	i_delta_x = i_delta_x >= 0 ? i_delta_x : -i_delta_x;
	i_delta_y = i_delta_y >= 0 ? i_delta_y : -i_delta_y;
	i_incx = i_delta_x == 0 ? 0 : i_delta_x / (us_x2 - us_x1);
	i_incy = i_delta_y == 0 ? 0 : i_delta_y / (us_y2 - us_y1);
	i_distance = i_delta_x > i_delta_y ? i_delta_x : i_delta_y;

	for (t = 0; t <= i_distance + 1; t ++)
	{  
		bsp_disp_draw_point(i_row, i_col, 0x0000);
		i_xerr += i_delta_x ; 
		i_yerr += i_delta_y ; 
		if(i_xerr > i_distance) 
		{ 
			i_xerr -= i_distance; 
			i_row += i_incx; 
		}
		
		if(i_yerr > i_distance) 
		{ 
			i_yerr -= i_distance; 
			i_col += i_incy; 
		} 
	}
	
	return;
}

void bsp_disp_draw_rect(uint16_t us_x1, uint16_t us_y1, uint16_t us_x2, uint16_t us_y2)
{
	bsp_disp_draw_line(us_x1, us_y1, us_x2, us_y1);
	bsp_disp_draw_line(us_x1, us_y1, us_x1, us_y2);
	bsp_disp_draw_line(us_x1, us_y2, us_x2, us_y2);
	bsp_disp_draw_line(us_x2, us_y1, us_x2, us_y2);
	
	return;
}

void bsp_disp_clear(uint16_t us_color)
{
	int32_t x = 0;
	int32_t y = 0;
	lcd_panel_t *pst_lcd = gpst_drv;
	
	disp_reg(pst_lcd->st_gram.us_wgram);
	
	for (y = 0; y < pst_lcd->ui_yres; y ++) {
		for (x = 0; x < pst_lcd->ui_xres; x ++) {
			disp_wdata(0x07E0);
		}
	}
	
	return;
}

void bsp_disp_register(bsp_disp_t **ppst_bsp_disp)
{
	bsp_disp_t *pst_disp = &gst_disp;
	
	pst_disp->set_pixel 	= disp_set_pixel;
	pst_disp->get_pixel 	= disp_get_pixel;
	pst_disp->fill_color 	= disp_fill_color;
	*ppst_bsp_disp = pst_disp;
	
	return;
}

void bsp_disp_init(void)
{
	GPIO_InitTypeDef st_gpio_inidef;
	board_t *pst_bd = board_get();
	lcd_panel_t *pst_lcd = pst_bd->pf_lcd_panel_get();
	bsp_disp_t *pst_disp = &gst_disp;

	gpst_drv = pst_lcd;
	pst_disp->ui_xres = pst_lcd->ui_xres;
	pst_disp->ui_yres = pst_lcd->ui_yres;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	
	st_gpio_inidef.GPIO_Pin 	= GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6;
	st_gpio_inidef.GPIO_Mode 	= GPIO_Mode_Out_PP;
	st_gpio_inidef.GPIO_Speed 	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &st_gpio_inidef); //GPIOC
	GPIO_SetBits(GPIOC,GPIO_Pin_10|GPIO_Pin_9|GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6);
	
	st_gpio_inidef.GPIO_Pin 	= GPIO_Pin_All;
	GPIO_Init(GPIOB, &st_gpio_inidef); //GPIOB
	GPIO_SetBits(GPIOB,GPIO_Pin_All);
	
	/* Todo: delay 50ms */
	rtos_mdelay(50);
	
	disp_tx_cmds(pst_lcd->st_cmd_init.past_cmds, pst_lcd->st_cmd_init.ui_cmd_count);
//	bsp_disp_test();
	return;
}
