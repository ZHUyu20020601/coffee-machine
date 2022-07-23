#ifndef SUPPORT_H
#define SUPPORT_H

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

// 1. 系统的状态相关
typedef enum { Waiting, Making, Error } SystemStatus;
SystemStatus GetSystemStatus();
void SetStatusMaking();
void SetStatusError();
void SetStatusWaiting();

// 2. 系统的配置变量相关
typedef enum {coffee, sugar, milk, temp} cfg_property;
typedef struct {
	uint8_t coffee;
	uint8_t sugar;
	uint8_t milk;
	uint8_t temp;
} SystemCfg;
uint8_t SetCfg(cfg_property, uint8_t);
uint8_t GetCfg(cfg_property);

// 3. 配置的储存与执行相关
#define QUEUE_LEN 100
typedef struct {
	cfg_property prop;
	uint8_t value;
} Command;
void PushToQueue(cfg_property, uint8_t);
void ExecTaskQueue();

#endif // !SUPPORT_H
