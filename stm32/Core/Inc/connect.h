/*
ZHUYU
2022/7/25
����ͨ��
*/

#ifndef __CONNECT_H
#define __CONNECT_H

#include "usart.h"
#include "cJSON.h"


/*����ͨ��*/
//��������
void my_uart1_send_data(uint8_t *tdata,uint16_t tnum);
void my_uart1_send_string(uint8_t *tdata);
//�����ж�
void my_uart1_enable_interrupt(void);
//�жϻص�
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);


#endif


