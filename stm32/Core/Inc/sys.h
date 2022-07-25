
#ifndef __SYS_H
#define __SYS_H

#include "main.h"
#include "usart.h"

/*状态机*/
typedef enum { Waiting, Making, Error } SystemStatus;
typedef enum {coffee, sugar, milk, temp} cfg_property;
typedef struct {
	uint8_t coffee;	// 咖啡原浆的加入量
	uint8_t sugar;	// 糖水的加入量
	uint8_t milk;	// 牛奶的加入量
	uint8_t temp;	// 设定的加热温度
} SystemCfg;



/*状态函数*/

//读取当前状态
SystemStatus GetSystemStatus(void);
//设置系统当前状态
void SetStatusMaking(void);
void SetStatusError(void);
void SetStatusWaiting(void);
//设置加料
void SetCfg(cfg_property, uint8_t);
uint8_t GetCfg(cfg_property);

/*工具函数*/
//void HAL_Delay_us(uint32_t us);
//void reset_printf(int uart);

#endif

