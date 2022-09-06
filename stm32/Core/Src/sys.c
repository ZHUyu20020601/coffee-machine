#include "includes.h"

extern int DEBUG;

static SystemStatus SystemCurrentStatus = Waiting;
static SystemCfg SystemCurrentCfg = { 0, 0, 0, 0 };
static SystemCfg tempCfg = {0, 0, 0, 0};
SystemCfg environmentCfg = {0, 0, 0, 0};


void InitSystem(void){
	SetStatusWaiting();
	InitCfg(&SystemCurrentCfg);
	InitCfg(&tempCfg);

	InitCfg(&environmentCfg);
	if(!DEBUG)
		HAL_GPIO_WritePin(led_GPIO_Port, led_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(led_GPIO_Port, led_Pin, GPIO_PIN_RESET);
	
}
	

void InitCfg(SystemCfg* cfg){
	cfg->coffee = 0;
	cfg->milk = 0;
	cfg->sugar = 0;
	cfg->temp = 0;
}


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


void SetNextCfg(cfg_property property, uint8_t value){
	if(property == coffee)
		tempCfg.coffee = value;
	if(property == milk)
		tempCfg.milk = value;
	if(property == sugar)
		tempCfg.sugar = value;
	if(property == temp)
		tempCfg.temp = value;
}



char* SetCurrentCfg(void){
	
	//如果处于等待状态，直接读取
	if(Waiting == GetSystemStatus()){
		SystemCurrentCfg = tempCfg;
	}
	else{
		return "ERROR";
	}
	//如果处于工作状态则什么都不做
	return NULL;
}


uint8_t GetCurrentCfg(cfg_property property){
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

uint8_t GetNextCfg(cfg_property property){
	if(property == coffee)
		return GetTempCfg(coffee);
	if(property == milk)
		return GetTempCfg(milk);
	if(property == sugar)
		return GetTempCfg(sugar);
	if(property == temp)
		return GetTempCfg(temp);
	return 0;
	
}

uint8_t GetTempCfg(cfg_property property){
	if(property == coffee)
		return tempCfg.coffee;
	if(property == milk)
		return tempCfg.milk;
	if(property == sugar)
		return tempCfg.sugar;
	if(property == temp)
		return tempCfg.temp;
	return 0;
}


uint8_t GetEnviCfg(cfg_property property){
	if(property == coffee)
		return environmentCfg.coffee;
	if(property == milk)
		return environmentCfg.milk;
	if(property == sugar)
		return environmentCfg.sugar;
	if(property == temp)
		return environmentCfg.temp;
	return 0;
}



