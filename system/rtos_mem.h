#ifndef __MALLOC_H
#define __MALLOC_H
#include "stm32f10x.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK MiniSTM32������
//�ڴ���� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////


 
#ifndef NULL
#define NULL 0
#endif

//�ڴ�����趨.
#define MEM_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define MEM_MAX_SIZE			25*1024  						//�������ڴ� 42K
#define MEM_ALLOC_TABLE_SIZE	MEM_MAX_SIZE/MEM_BLOCK_SIZE 	//�ڴ���С
 
		 
//�ڴ���������
struct _m_mallco_dev
{
	void (*init)(void);				//��ʼ��
	u8 (*perused)(void);		  	//�ڴ�ʹ����
	u8 	*membase;					//�ڴ�� 
	u16 *memmap; 					//�ڴ����״̬��
	u8  memrdy; 					//�ڴ�����Ƿ����
};
extern struct _m_mallco_dev mallco_dev;	//��mallco.c���涨��

extern void rtos_memset(void *s,u8 c,u32 count);	//�����ڴ�

extern void rtos_memcpy(void *des,void *src,u32 n);//�����ڴ�     

extern u8 rtos_mem_perused(void);

extern void rtos_mem_init(void);					 //�ڴ�����ʼ������(��/�ڲ�����)
////////////////////////////////////////////////////////////////////////////////
//�û����ú���
extern void rtos_mem_free(void *ptr);  				//�ڴ��ͷ�(�ⲿ����)

extern void *rtos_mem_alloc(u32 size);				//�ڴ����(�ⲿ����)

extern void *rtos_mem_realloc(void *ptr,u32 size);	//���·����ڴ�(�ⲿ����)

#endif













