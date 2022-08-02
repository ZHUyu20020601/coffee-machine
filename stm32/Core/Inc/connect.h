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
//��Ҫ���жϺ�����ʹ�ã���
void uart1_send_data(uint8_t *tdata,uint16_t tnum);
void uart1_send_string(uint8_t *tdata);


//�����ж�
void uart1_start_dma(void);



/*ҵ����*/
//�����ڽ��յ����ַ���
//�����������ͣ�ת�����崦����������1.2.3.4��
void parse_msg(uint8_t* msg);

//1.�����趨
void set_cfg(char* variable, uint8_t value, uint8_t id);

//2.������ѯ
void req_cfg(char* variable, uint8_t id);

//3.����
void start(uint8_t id);

//4.�ػ�
void emergent_stop(uint8_t id);




/*�ص�����*/
//ִ���������Ӧ
void response_ok(uint8_t id);
void response_busy(uint8_t id);
void response_error(uint8_t id, char* msg);
void response_request(uint8_t id, char* variable, uint8_t value);
void response_status(uint8_t id);



#endif


