
/*
ZHUYU
2022/7/25

���ļ��������ϵͳ�Ĺ���״̬���趨���洢�͵��ÿ��ȼ�����ȵ�Ԥ��
��ϵͳ����ʱ����InitSystem()��ʼ�����в��������buf��
��SystemState,SystemCurrenCfg,tempCurrent������Ϊstatic�����ļ���
��Ϊֻ����
���Ҫ�趨���ϲ�����ֻ��Ҫ�ȵ���SetNextCfg(...)�趨���в�����Ȼ��
����AddBuffer()��tempCfgѹ�뻺���м���
�����趨�ļ��ϲ�������ֱ��Ӧ�ã������Ⱦ���buf����ѭFIFO��ȡ
��Ҫ��ȡbuf��SystemCurrentCfg�У�����SetCurrentCfg()

e.g.

InitSystem(); //����
SetNextCfg(coffee,40); //�趨����
SetNextCfg(milk,20);
SetNextCfg(sugar,10);
SetNextCfg(temp,30);
AddBuffer(); //���뻺��
SetCurrentCfg(); //��ȡ�����趨
SetStatusMaking(); //���빤��״̬


*/

#ifndef __SYS_H
#define __SYS_H

#include "main.h"


/*״̬��*/
typedef enum { Waiting, Making, Error } SystemStatus;

typedef enum {coffee, sugar, milk, temp} cfg_property;

typedef struct {
	uint8_t coffee;	// ����ԭ���ļ�����
	uint8_t sugar;	// ��ˮ�ļ�����
	uint8_t milk;	// ţ�̵ļ�����
	uint8_t temp;	// �趨�ļ����¶�
} SystemCfg;

typedef struct {
	SystemCfg buffer[5]; //�洢5�����ȵ�Ԥ��
	int rear; //�����һ����������Ŀ�λ
}	SystemCfgBuf;


/*״̬����*/

//��ʼ������״̬
void InitSystem(void);
void InitCfg(SystemCfg* cfg);

//��ȡ��ǰ״̬
SystemStatus GetSystemStatus(void);

//����ϵͳ��ǰ״̬
void SetStatusMaking(void);
void SetStatusError(void);
void SetStatusWaiting(void);

//���ü������ݵ�tempCfg��
void SetNextCfg(cfg_property, uint8_t);

//�ȴ�tempCfg������Ϻ����buffer
char* AddBuffer(void);
int buf_empty(void);

//��ȡbufԤ�裬�趨��һ�����ȵĲ���
char* SetCurrentCfg(void);
uint8_t GetCurrentCfg(cfg_property);

/*���ߺ���*/
//void HAL_Delay_us(uint32_t us);
//void reset_printf(int uart);

#endif

