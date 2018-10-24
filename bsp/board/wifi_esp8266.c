#include "basic.h"
#include "rtos.h"
#include "basic.h"
#include "board_wifi.h"

#define ESP_CMDBUF_SIZE		(128)

static wifi_mcu_t gst_mcu;

static char *itoa( int value, char *string, int radix )
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

static void esp8266_puts(char *Data, ...)
{
	const char *s;
	int d;   
	char buf[16];
	wifi_mcu_t *pst_mcu = &gst_mcu;
	
	va_list ap;
	va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					pst_mcu->pst_uart->pf_out(pst_mcu->pst_uart, 0x0d);
					Data ++;
					break;

				case 'n':							          //换行符
					pst_mcu->pst_uart->pf_out(pst_mcu->pst_uart, 0x0a);
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
					for ( ; *s; s++) 
					{
						pst_mcu->pst_uart->pf_out(pst_mcu->pst_uart, *s);
						while( USART_GetFlagStatus((USART_TypeDef *)pst_mcu->pst_uart->ui_uart_base, USART_FLAG_TXE) == RESET );
					}
					Data++;
					break;

				case 'd':										//十进制
					d = va_arg(ap, int);
					itoa(d, buf, 10);
					for (s = buf; *s; s++) 
					{
						pst_mcu->pst_uart->pf_out(pst_mcu->pst_uart, *s);
						while( USART_GetFlagStatus((USART_TypeDef *)pst_mcu->pst_uart->ui_uart_base, USART_FLAG_TXE) == RESET );
					}
					Data++;					
					break;
					
				 default:
					Data++;
				    break;
			}		 
		} /* end of else if */
		else  pst_mcu->pst_uart->pf_out(pst_mcu->pst_uart, *Data ++);
		while( USART_GetFlagStatus((USART_TypeDef *)pst_mcu->pst_uart->ui_uart_base, USART_FLAG_TXE) == RESET );
	}
}

static int32_t esp8266_gets(uint8_t **ppc_buf)
{
	wifi_mcu_t *pst_mcu = &gst_mcu;
	return pst_mcu->pst_uart->pf_in(pst_mcu->pst_uart, ppc_buf);
}

static int32_t esp8266_init(bsp_uart_s *pst_uart)
{
	uint8_t *puc_resp = NULL;
	wifi_mcu_t *pst_mcu = &gst_mcu;
	pst_mcu->pst_uart = pst_uart;
	
	esp8266_puts("ATE0\r\n");
	rtos_xdelay(3000);
	esp8266_gets(&puc_resp);
	if (strstr((char *)puc_resp, "OK"))
		return 0;
	
	return -1;
}

static int32_t esp8266_test(const char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout)
{
	char ac_cmd[ESP_CMDBUF_SIZE] = {0};
	wifi_mcu_t *pst_mcu = &gst_mcu;
	
	pst_mcu->pst_uart->st_fifo.us_len = 0;
	/* Todo: send cmd */		
	if (pc_cmd)
		snprintf(ac_cmd, ESP_CMDBUF_SIZE, "AT+%s=?\r\n", pc_cmd);
	else
		strcpy(ac_cmd, "AT\r\n");
	esp8266_puts(ac_cmd);
	rtos_mdelay(ui_timeout);
	/* Todo: recv resp */
	esp8266_gets(puc_resp);
	if ((NULL == puc_resp))
		return -1;
	return 0;
}

static int32_t esp8266_qury(const char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout)
{
	char ac_cmd[ESP_CMDBUF_SIZE] = {0};
	wifi_mcu_t *pst_mcu = &gst_mcu;
	
	pst_mcu->pst_uart->st_fifo.us_len = 0;

	/* Todo: send cmd */
	snprintf(ac_cmd, ESP_CMDBUF_SIZE, "AT+%s?\r\n", pc_cmd);
	esp8266_puts(ac_cmd);
	
	rtos_mdelay(ui_timeout);	
	
	/* Todo: recv resp */
	esp8266_gets(puc_resp);
	if (NULL == puc_resp)
		return -1;
	
	return 0;
}

static int32_t esp8266_conf(const char *pc_cmd, char *pc_args, uint8_t **puc_resp, uint32_t ui_timeout)
{
	char ac_cmd[ESP_CMDBUF_SIZE] = {0};
	wifi_mcu_t *pst_mcu = &gst_mcu;
	
	pst_mcu->pst_uart->st_fifo.us_len = 0;

	/* Todo: send cmd */
	snprintf(ac_cmd, ESP_CMDBUF_SIZE, "AT+%s=%s\r\n", pc_cmd, pc_args);
	esp8266_puts(ac_cmd);

	rtos_mdelay(ui_timeout);	
	
	/* Todo: recv resp */
	esp8266_gets(puc_resp);
	if (NULL == puc_resp)
		return -1;

	return 0;
}

static int32_t esp8266_exec(const char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout)
{
	char ac_cmd[ESP_CMDBUF_SIZE] = {0};
	wifi_mcu_t *pst_mcu = &gst_mcu;
	
	pst_mcu->pst_uart->st_fifo.us_len = 0;

	/* Todo: send cmd */	
	snprintf(ac_cmd, ESP_CMDBUF_SIZE, "AT+%s\r\n", pc_cmd);
	esp8266_puts(ac_cmd);

	rtos_mdelay(ui_timeout);	

	/* Todo: recv resp */
	esp8266_gets(puc_resp);
	if (NULL == puc_resp)
		return -1;

	return 0;
}

wifi_mcu_t *wifi_mcu_get(void)
{
	wifi_mcu_t *pst_mcu = &gst_mcu;
	
	pst_mcu->init = esp8266_init;
	pst_mcu->test = esp8266_test;
	pst_mcu->qury = esp8266_qury;
	pst_mcu->conf = esp8266_conf;
	pst_mcu->exec = esp8266_exec;
	
	return &gst_mcu;
}

