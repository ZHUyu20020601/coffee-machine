
/*
ZHUYU
2022/7/25

该文件负责调控系统的工作状态，设定、存储和调用咖啡加料配比的预设
在系统启动时调用InitSystem()初始化所有参数，清空buf。
将SystemState,SystemCurrenCfg,tempCurrent均定义为static，在文件外
视为只读。
如果要设定加料参数，只需要先调用SetNextCfg(...)设定所有参数，然后
调用AddBuffer()将tempCfg压入缓存中即可
所有设定的加料参数不会直接应用，而是先经过buf，遵循FIFO读取
若要读取buf到SystemCurrentCfg中，调用SetCurrentCfg()

e.g.

InitSystem(); //启动
SetNextCfg(coffee,40); //设定加料
SetNextCfg(milk,20);
SetNextCfg(sugar,10);
SetNextCfg(temp,30);
//AddBuffer(); //加入缓存
//谨慎起见最好使用 
char* msg = AddBuffer();
if(msg != NULL)
	response_error(id, msg);
else
	response_ok(id);
SetCurrentCfg(); //读取缓存设定
SetStatusMaking(); //进入工作状态


*/

#ifndef __SYS_H
#define __SYS_H

#include "main.h"


/*状态机*/
typedef enum { Waiting, Making, Error } SystemStatus;

typedef enum {coffee, sugar, milk, temp} cfg_property;

typedef struct {
	uint8_t coffee;	// 咖啡原浆的加入量
	uint8_t sugar;	// 糖水的加入量
	uint8_t milk;	// 牛奶的加入量
	uint8_t temp;	// 设定的加热温度
} SystemCfg;

typedef struct {
	SystemCfg buffer[5]; //存储5杯咖啡的预设
	int rear; //标记下一个可以填入的空位
}	SystemCfgBuf;


/*状态函数*/

//初始化所有状态
void InitSystem(void);
void InitCfg(SystemCfg* cfg);

//读取当前状态
SystemStatus GetSystemStatus(void);

//设置系统当前状态
void SetStatusMaking(void);
void SetStatusError(void);
void SetStatusWaiting(void);

//设置加料数据到tempCfg中
void SetNextCfg(cfg_property, uint8_t);

//等待tempCfg设置完毕后存入buffer
char* AddBuffer(void);
int buf_empty(void);

//读取buf预设，设定下一杯咖啡的参数
char* SetCurrentCfg(void);
uint8_t GetCurrentCfg(cfg_property);
uint8_t GetNextCfg(cfg_property);
uint8_t GetTempCfg(cfg_property);
uint8_t GetEnviCfg(cfg_property);

/*工具函数*/
//void HAL_Delay_us(uint32_t us);
//void reset_printf(int uart);

#endif

