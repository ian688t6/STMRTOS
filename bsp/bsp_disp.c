#include "basic.h"
#include "board.h"
#include "rtos.h"
#include "lcd_core.h"
#include "bsp_disp.h"

#define	LCD_CS_SET  GPIOC->BSRR=1<<9    //Ƭѡ�˿�  		PC9
#define	LCD_RS_SET	GPIOC->BSRR=1<<8    //����/���� 		PC8	   
#define	LCD_WR_SET	GPIOC->BSRR=1<<7    //д����			PC7
#define	LCD_RD_SET	GPIOC->BSRR=1<<6    //������			PC6
								    
#define	LCD_CS_CLR  GPIOC->BRR=1<<9     //Ƭѡ�˿�  		PC9
#define	LCD_RS_CLR	GPIOC->BRR=1<<8     //����/����			PC8	   
#define	LCD_WR_CLR	GPIOC->BRR=1<<7     //д����			PC7
#define	LCD_RD_CLR	GPIOC->BRR=1<<6     //������			PC6   

//PB0~15,��Ϊ������
#define DATAOUT(x) GPIOB->ODR=x; //�������
#define DATAIN     GPIOB->IDR;   //��������	

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
 	GPIOB->CRL=0X88888888; //PB0-7  ��������
	GPIOB->CRH=0X88888888; //PB8-15 ��������
	GPIOB->ODR=0X0000;     //ȫ�����0

	LCD_RS_SET;
	LCD_CS_CLR;
	//��ȡ����(���Ĵ���ʱ,������Ҫ��2��)
	LCD_RD_CLR;
	LCD_RD_SET;
	LCD_CS_SET; 

	GPIOB->CRL=0X33333333; //PB0-7  �������
	GPIOB->CRH=0X33333333; //PB8-15 �������
	GPIOB->ODR=0XFFFF;    //ȫ�������
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

static void disp_test(void)
{
	int32_t x = 0;
	int32_t y = 0;
	int32_t k = 0;
	board_t *pst_bd = board_get();
	lcd_panel_t *pst_lcd = pst_bd->pf_lcd_panel_get();	
	disp_reg(pst_lcd->st_gram.us_gram);
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
	board_t *pst_bd = board_get();
	lcd_panel_t *pst_lcd = pst_bd->pf_lcd_panel_get();
	
	disp_reg(pst_lcd->us_id_addr);
	disp_rdata();
	disp_rdata();
	us_id = disp_rdata();
	us_id <<= 8;
	us_id |= disp_rdata();
	
	return us_id;
}

void bsp_disp_clear(uint16_t us_color)
{
	int32_t x = 0;
	int32_t y = 0;
	board_t *pst_bd = board_get();
	lcd_panel_t *pst_lcd = pst_bd->pf_lcd_panel_get();
	
	disp_reg(pst_lcd->st_gram.us_gram);
	
	for (y = 0; y < pst_lcd->ui_yres; y ++) {
		for (x = 0; x < pst_lcd->ui_xres; x ++) {
			disp_wdata(0x07E0);
		}
	}
	
	return;
}

void bsp_disp_init(void)
{
	GPIO_InitTypeDef st_gpio_inidef;
	board_t *pst_bd = board_get();
	lcd_panel_t *pst_lcd = pst_bd->pf_lcd_panel_get();
	
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
	
	disp_tx_cmds(pst_lcd->st_cmdinfo.past_cmds, pst_lcd->st_cmdinfo.ui_cmd_count);
//	bsp_disp_clear(0x07E0);
	disp_test();
	return;
}
