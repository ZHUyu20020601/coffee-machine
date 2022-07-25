/*
ZHUYU
2022/7/25
串口通信
*/

#ifndef __CONNECT_H
#define __CONNECT_H

#include "usart.h"
#include "cJSON.h"


/*串口通信*/
//发送数据
void my_uart1_send_data(uint8_t *tdata,uint16_t tnum);
void my_uart1_send_string(uint8_t *tdata);
//开启中断
void my_uart1_enable_interrupt(void);
//中断回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);


#endif


