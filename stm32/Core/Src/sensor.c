
#include "sensor.h"
#include "cmsis_os.h"


extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

extern TIM_HandleTypeDef htim6;

const static int average_span = 5;


float get_coffee_dist(void){
	int i;
	float dist = 0;
	for(i = 0 ; i < average_span ; ++i){
		coffee_trig_send();
		dist += distance(coffee_time_cap());
		HAL_Delay(20);//wait 20ms
		//osDelay(20);
	}
	return dist / 5;
}

float get_milk_dist(void){
	int i;
	float dist = 0;
	for(i = 0 ; i < average_span ; ++i){
		milk_trig_send();
		dist += distance(milk_time_cap());
		HAL_Delay(20);//wait 20ms
	}
	return dist / 5;
}

float get_sugar_dist(void){
	int i;
	float dist = 0;
	for(i = 0 ; i < average_span ; ++i){
		sugar_trig_send();
		dist += distance(sugar_time_cap());
		HAL_Delay(20);//wait 20ms
	}
	return dist / 5;
}
	

void coffee_trig_send(void){
	//set high level to send signal
	HAL_GPIO_WritePin(coffee_trigger_GPIO_Port, coffee_trigger_Pin, GPIO_PIN_RESET);
	HAL_Delay_us(2);
	HAL_GPIO_WritePin(coffee_trigger_GPIO_Port, coffee_trigger_Pin, GPIO_PIN_SET);
	//keep sending
	HAL_Delay_us(20);
	//stop
	HAL_GPIO_WritePin(coffee_trigger_GPIO_Port, coffee_trigger_Pin, GPIO_PIN_RESET);
}

void milk_trig_send(void){
	HAL_GPIO_WritePin(milk_trigger_GPIO_Port, milk_trigger_Pin, GPIO_PIN_RESET);
	HAL_Delay_us(2);
	HAL_GPIO_WritePin(milk_trigger_GPIO_Port, milk_trigger_Pin, GPIO_PIN_SET);
	HAL_Delay_us(20);
	HAL_GPIO_WritePin(milk_trigger_GPIO_Port, milk_trigger_Pin, GPIO_PIN_RESET);
}

void sugar_trig_send(void){
	HAL_GPIO_WritePin(sugar_trigger_GPIO_Port, sugar_trigger_Pin, GPIO_PIN_RESET);
	HAL_Delay_us(2);
	HAL_GPIO_WritePin(sugar_trigger_GPIO_Port, sugar_trigger_Pin, GPIO_PIN_SET);
	HAL_Delay_us(20);
	HAL_GPIO_WritePin(sugar_trigger_GPIO_Port, sugar_trigger_Pin, GPIO_PIN_RESET);
}

//get time span must within 65536us
uint32_t coffee_time_cap(void){
	
	uint32_t tim;
	
	//wait for callback
	while(HAL_GPIO_ReadPin(coffee_echo_GPIO_Port, coffee_echo_Pin) == GPIO_PIN_RESET);
	__HAL_TIM_SetCounter(&htim6,0);

	HAL_TIM_Base_Start(&htim6);

	while(HAL_GPIO_ReadPin(coffee_echo_GPIO_Port, coffee_echo_Pin) == GPIO_PIN_SET);
	HAL_TIM_Base_Stop(&htim6);
	
	tim = __HAL_TIM_GetCounter(&htim6);
	return tim;
	
	
}

uint32_t milk_time_cap(void){
	uint32_t tim;
	//wait for callback
	while(HAL_GPIO_ReadPin(milk_echo_GPIO_Port, milk_echo_Pin) == GPIO_PIN_RESET);
	__HAL_TIM_SetCounter(&htim6,0);
	HAL_TIM_Base_Start(&htim6);
	while(HAL_GPIO_ReadPin(milk_echo_GPIO_Port, milk_echo_Pin) == GPIO_PIN_SET);
	HAL_TIM_Base_Stop(&htim6);
	tim = __HAL_TIM_GetCounter(&htim6);
	return tim;
}

uint32_t sugar_time_cap(void){
	uint32_t tim;
	//wait for callback
	while(HAL_GPIO_ReadPin(sugar_echo_GPIO_Port, sugar_echo_Pin) == GPIO_PIN_RESET);
	__HAL_TIM_SetCounter(&htim6,0);
	HAL_TIM_Base_Start(&htim6);
	while(HAL_GPIO_ReadPin(sugar_echo_GPIO_Port, sugar_echo_Pin) == GPIO_PIN_SET);
	HAL_TIM_Base_Stop(&htim6);
	tim = __HAL_TIM_GetCounter(&htim6);
	return tim;
}

float distance(uint32_t us){
	float dist;
	//dist = (340 * us) / 10000 / 2; // cm
	dist = us * 0.017;
	return dist;
}



int ds18b20_readtemperature(onewire *ptr){
	unsigned char lo, hi;
	int x = 0;
	onewire_reset(ptr);
	onewire_writebyte(ptr, 0xcc);//read serial
	onewire_writebyte(ptr, 0x44);//enable temperature
	HAL_Delay(800);
	onewire_reset(ptr);
	onewire_writebyte(ptr, 0xcc);
	onewire_writebyte(ptr, 0xbe);
	lo = onewire_readbyte(ptr);
	hi = onewire_readbyte(ptr);
	x = lo | (hi << 8);
	x = (int)(x * 0.0625 * 100 + 0.5);
	return x;
}



