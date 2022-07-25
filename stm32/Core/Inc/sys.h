
#ifndef __SYS_H
#define __SYS_H

#include "main.h"
#include "usart.h"

/*״̬��*/
typedef enum { Waiting, Making, Error } SystemStatus;
typedef enum {coffee, sugar, milk, temp} cfg_property;
typedef struct {
	uint8_t coffee;	// ����ԭ���ļ�����
	uint8_t sugar;	// ��ˮ�ļ�����
	uint8_t milk;	// ţ�̵ļ�����
	uint8_t temp;	// �趨�ļ����¶�
} SystemCfg;



/*״̬����*/

//��ȡ��ǰ״̬
SystemStatus GetSystemStatus(void);
//����ϵͳ��ǰ״̬
void SetStatusMaking(void);
void SetStatusError(void);
void SetStatusWaiting(void);
//���ü���
void SetCfg(cfg_property, uint8_t);
uint8_t GetCfg(cfg_property);

/*���ߺ���*/
//void HAL_Delay_us(uint32_t us);
//void reset_printf(int uart);

#endif

