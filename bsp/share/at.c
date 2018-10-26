#include "basic.h"
#include "FreeRTOS.h"
#include "rtos.h"
#include "at.h"

#define AT_CMDBUF_SIZE		(128)

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

void at_puts(bsp_uart_s *pst_uart, char *Data, ...)
{
	const char *s;
	int d;   
	char buf[16];
	
	pst_uart->st_fifo.us_len = 0;
	va_list ap;
	va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					pst_uart->pf_out(pst_uart, 0x0d);
					Data ++;
					break;

				case 'n':							          //换行符
					pst_uart->pf_out(pst_uart, 0x0a);
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
						pst_uart->pf_out(pst_uart, *s);
						while( USART_GetFlagStatus((USART_TypeDef *)pst_uart->ui_uart_base, USART_FLAG_TXE) == RESET );
					}
					Data++;
					break;

				case 'd':										//十进制
					d = va_arg(ap, int);
					itoa(d, buf, 10);
					for (s = buf; *s; s++) 
					{
						pst_uart->pf_out(pst_uart, *s);
						while( USART_GetFlagStatus((USART_TypeDef *)pst_uart->ui_uart_base, USART_FLAG_TXE) == RESET );
					}
					Data++;					
					break;
					
				 default:
					Data++;
				    break;
			}		 
		} /* end of else if */
		else  pst_uart->pf_out(pst_uart, *Data ++);
		while( USART_GetFlagStatus((USART_TypeDef *)pst_uart->ui_uart_base, USART_FLAG_TXE) == RESET );
	}
}

int32_t at_gets(bsp_uart_s *pst_uart, uint8_t **ppc_buf)
{
	return pst_uart->pf_in(pst_uart, ppc_buf);
}

int32_t at_test(bsp_uart_s *pst_uart, const char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout)
{
	char ac_cmd[AT_CMDBUF_SIZE] = {0};

	/* Todo: send cmd */		
	if (pc_cmd)
		snprintf(ac_cmd, AT_CMDBUF_SIZE, "AT+%s=?\r\n", pc_cmd);
	else
		strcpy(ac_cmd, "AT\r\n");
	at_puts(pst_uart, ac_cmd);
	rtos_mdelay(ui_timeout);

	/* Todo: recv resp */
	if (ui_timeout) {
		at_gets(pst_uart, puc_resp);
		if ((NULL == puc_resp))
			return -1;	
	}
	return 0;
}

int32_t at_qury(bsp_uart_s *pst_uart, const char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout)
{
	char ac_cmd[AT_CMDBUF_SIZE] = {0};

	/* Todo: send cmd */
	snprintf(ac_cmd, AT_CMDBUF_SIZE, "AT+%s?\r\n", pc_cmd);
	at_puts(pst_uart, ac_cmd);
	
	rtos_mdelay(ui_timeout);	
	
	/* Todo: recv resp */
	if (ui_timeout) {
		at_gets(pst_uart, puc_resp);
		if (NULL == puc_resp)
			return -1;
	}
	
	return 0;
}

int32_t at_conf(bsp_uart_s *pst_uart, const char *pc_cmd, char *pc_args, uint8_t **puc_resp, uint32_t ui_timeout)
{
	char ac_cmd[AT_CMDBUF_SIZE] = {0};

	/* Todo: send cmd */
	snprintf(ac_cmd, AT_CMDBUF_SIZE, "AT+%s=%s\r\n", pc_cmd, pc_args);
	printf("AT CMD:%s", ac_cmd);
	at_puts(pst_uart, ac_cmd);

	rtos_mdelay(ui_timeout);	
	
	/* Todo: recv resp */
	if (ui_timeout) {
		at_gets(pst_uart, puc_resp);
		if (NULL == puc_resp)
			return -1;	
	}

	return 0;
}

int32_t at_exec(bsp_uart_s *pst_uart, const char *pc_cmd, uint8_t **puc_resp, uint32_t ui_timeout)
{
	char ac_cmd[AT_CMDBUF_SIZE] = {0};

	/* Todo: send cmd */	
	snprintf(ac_cmd, AT_CMDBUF_SIZE, "AT+%s\r\n", pc_cmd);
	at_puts(pst_uart, ac_cmd);

	rtos_mdelay(ui_timeout);	

	/* Todo: recv resp */
	if (ui_timeout) {
		at_gets(pst_uart, puc_resp);
		if (NULL == puc_resp)
			return -1;
	}
	
	return 0;
}

