/*
ZHUYU
2022/7/25
串口通信
*/

#ifndef __CONNECT_H
#define __CONNECT_H

#include "usart.h"
#include "cJSON.h"
#include "sys.h"


/*串口通信*/
//发送数据
void uart1_send_data(uint8_t *tdata,uint16_t tnum);
void uart1_send_string(uint8_t *tdata);
//开启中断
void uart1_start_dma(void);

//处理串口接收到的字符串
void parse_msg(uint8_t* msg);

#endif


