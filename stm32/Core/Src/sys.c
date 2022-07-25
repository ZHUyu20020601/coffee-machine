#include "sys.h"
#include "usart.h"

static SystemStatus SystemCurrentStatus = Waiting;
static SystemCfg SystemCurrentCfg = { 0, 0, 0, 0 };


SystemStatus GetSystemStatus(){
	return SystemCurrentStatus;
}


void SetStatusMaking(){
	SystemCurrentStatus = Making;
}
void SetStatusError(){
	SystemCurrentStatus = Error;
}
void SetStatusWaiting(){
	SystemCurrentStatus = Waiting;
}

void SetCfg(cfg_property property, uint8_t value){
	if(property == coffee)
		SystemCurrentCfg.coffee = value;
	if(property == milk)
		SystemCurrentCfg.milk = value;
	if(property == sugar)
		SystemCurrentCfg.sugar = value;
	if(property == temp)
		SystemCurrentCfg.temp = value;
}


uint8_t GetCfg(cfg_property property){
	if(property == coffee)
		return SystemCurrentCfg.coffee;
	if(property == milk)
		return SystemCurrentCfg.milk;
	if(property == sugar)
		return SystemCurrentCfg.sugar;
	if(property == temp)
		return SystemCurrentCfg.temp;
	return 0;
}
