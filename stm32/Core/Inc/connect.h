/*
ZHUYU
2022/7/25
����ͨ��
*/

#ifndef __CONNECT_H
#define __CONNECT_H

#include "usart.h"
#include "cJSON.h"
#include "sys.h"


/*����ͨ��*/
//��������
void uart1_send_data(uint8_t *tdata,uint16_t tnum);
void uart1_send_string(uint8_t *tdata);
//�����ж�
void uart1_start_dma(void);

//�����ڽ��յ����ַ���
void parse_msg(uint8_t* msg);

#endif


