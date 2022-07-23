#include "support.h"

// 当前系统状态
static SystemStatus SystemCurrentStatus = Waiting;
SystemStatus GetSystemStatus() { return SystemCurrentStatus; }
void SetStatusMaking() { SystemCurrentStatus = Making; }
void SetStatusError() { SystemCurrentStatus = Error; }
void SetStatusWaiting() 
{
	SystemCurrentStatus = Waiting; 
	ExecTaskQueue();
}

// 系统配置文件相关
static SystemCfg SystemConfig;
uint8_t SetCfg(cfg_property cfg_prop, uint8_t value)
{
	// 先判断是否在Making状态下
	if (SystemCurrentStatus == Making) {
		// 存入一个指令队列中，待运行完成后执行
		PushToQueue(cfg_prop, value);
		return 0;
	}
	else if (SystemCurrentStatus == Waiting) {
		if (cfg_prop == coffee) {
			SystemConfig.coffee = value;
		}
		else if (cfg_prop == sugar) {
			SystemConfig.sugar = value;
		}
		else if (cfg_prop == milk) {
			SystemConfig.milk = value;
		}
		else if (cfg_prop == temp) {
			SystemConfig.temp = value;
		}
		return 1;
	}
}

uint8_t GetCfg(cfg_property cfg_prop) {
	if (cfg_prop == coffee) {
		return SystemConfig.coffee;
	}
	else if (cfg_prop == sugar) {
		return SystemConfig.sugar;
	}
	else if (cfg_prop == milk) {
		return SystemConfig.milk;
	}
	else if (cfg_prop == temp) {
		return SystemConfig.temp;
	}
}

// 3. 配置的储存与读取相关
static Command CommandQueue[QUEUE_LEN];
static uint8_t QueueP = 0;
void PushToQueue(cfg_property prop, uint8_t value)
{
	if (QueueP < QUEUE_LEN) {
		CommandQueue[QueueP].prop = prop;
		CommandQueue[QueueP].value = value;
		QueueP++;
	}
}
void ExecTaskQueue()
{
	for (int p = 0; p < QueueP; p++) {
		Command cmd = CommandQueue[p];
		SetCfg(cmd.prop, cmd.value);
	}
	QueueP = 0;
}


