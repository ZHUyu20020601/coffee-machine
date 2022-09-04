#include "sys.h"
#include "stdio.h"

extern int DEBUG;

static SystemStatus SystemCurrentStatus = Waiting;
static SystemCfg SystemCurrentCfg = { 0, 0, 0, 0 };
static SystemCfg tempCfg = {0, 0, 0, 0};
SystemCfg environmentCfg = {0, 0, 0, 0};
//static SystemCfgBuf buf;


void InitSystem(void){
	int i;
	SetStatusWaiting();
	InitCfg(&SystemCurrentCfg);
	InitCfg(&tempCfg);
	/*
	for(i = 0; i < 5; i++)
		InitCfg(&(buf.buffer[i]));
	buf.rear = 0;
	*/
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

/*
char* AddBuffer(void){
	if(buf.rear < 5){
		buf.buffer[buf.rear] = tempCfg;
		buf.rear++;
	}else{
		return "buf full!";
	}
	return NULL;
}
*/

char* SetCurrentCfg(void){
	int i = 0;
	//如果处于等待状态，直接读取
	if(Waiting == GetSystemStatus()){
		SystemCurrentCfg = tempCfg;
	}
	/*
	if(Waiting == GetSystemStatus()){
		if(buf.rear == 0)
			return "buf empty!";
		else{
			SystemCurrentCfg = buf.buffer[0];
			for(;i < buf.rear ; i++){
				buf.buffer[i] = buf.buffer[i+1];
			}
			buf.rear--;
		}
	}
	*/
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
		//return buf.buffer[0].coffee;
		return GetTempCfg(coffee);
	if(property == milk)
		//return buf.buffer[0].milk;
		return GetTempCfg(milk);
	if(property == sugar)
		//return buf.buffer[0].sugar;
		return GetTempCfg(sugar);
	if(property == temp)
		//return buf.buffer[0].temp;
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

/*
int buf_empty(void){
	if(buf.rear == 0)
		return 1;
	return 0;
}

*/

