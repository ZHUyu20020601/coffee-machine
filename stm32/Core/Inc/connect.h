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
//不要再中断函数中使用！！
void uart1_send_data(uint8_t *tdata,uint16_t tnum);
void uart1_send_string(uint8_t *tdata);

void uart3_send_string(uint8_t *tdata);


//开启中断
void uart1_start_dma(void);
void uart2_start_dma(void);
void uart3_start_dma(void);


/*业务函数*/
//处理串口接收到的字符串
//分析命令类型，转到具体处理函数（下列1.2.3.4）
void parse_msg(uint8_t* msg);

//1.参数设定
void set_cfg(char* variable, uint8_t value, uint8_t id);

//2.参数查询
void req_cfg(char* variable, uint8_t id);

//3.开机
void start(uint8_t id);

//4.关机
void emergent_stop(uint8_t id);




/*回调函数*/
//执行命令后响应
void response_ok(uint8_t id);
void response_busy(uint8_t id);
void response_error(uint8_t id, char* msg);
void response_request(uint8_t id, char* variable, uint8_t value);
void response_status(uint8_t id);



#endif


