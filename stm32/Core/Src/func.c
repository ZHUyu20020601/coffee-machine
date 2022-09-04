#include "func.h"
#include "sensor.h"
#include "stdio.h"

//set const bottom area
const static int area = 25; //cm^2
extern int DEBUG;

float dist_to_vol(float dist){
	return (dist - 15.080) / -0.013  ;
}

float vol_to_dist(float vol){
	return vol * (-0.013 ) + 15.080;
}

void shut_all_relay(void){
	HAL_GPIO_WritePin(coffee_relay_GPIO_Port, coffee_relay_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(milk_relay_GPIO_Port, milk_relay_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(sugar_relay_GPIO_Port, sugar_relay_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(heater_relay_GPIO_Port, heater_relay_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(mixer_relay_GPIO_Port, mixer_relay_Pin, GPIO_PIN_RESET);
}

void add_coffee(uint8_t ml){

	float h_now = get_coffee_dist();
	float vol_now = dist_to_vol(h_now);
	float h_end = vol_to_dist(vol_now - ml / 1.12);
	//open valve
	//low electric level valid
	HAL_GPIO_WritePin(coffee_relay_GPIO_Port, coffee_relay_Pin, GPIO_PIN_RESET);
	while(h_now < h_end){
		h_now = get_coffee_dist();
	}
	HAL_GPIO_WritePin(coffee_relay_GPIO_Port, coffee_relay_Pin, GPIO_PIN_SET);
	
	if(DEBUG){
		//printf("add %d ml coffee\r\n", ml);
	}
}

void add_milk(uint8_t ml){

	float h_now = get_milk_dist();
	float vol_now = dist_to_vol(h_now);
	float h_end = vol_to_dist(vol_now - ml / 1.12);
	//open valve
	//low electric level valid
	HAL_GPIO_WritePin(milk_relay_GPIO_Port, milk_relay_Pin, GPIO_PIN_RESET);
	while(h_now < h_end){
		h_now = get_milk_dist();
	}
	HAL_GPIO_WritePin(milk_relay_GPIO_Port, milk_relay_Pin, GPIO_PIN_SET);
	
	if(DEBUG){
		//printf("add %d ml milk\r\n", ml);
	}
}

void add_sugar(uint8_t ml){
	
	//ml /= 1.5;
	float h_now = get_sugar_dist();
	float vol_now = dist_to_vol(h_now);
	float h_end = vol_to_dist(vol_now - ml / 1.12);
	//open valve
	//low electric level valid
	HAL_GPIO_WritePin(sugar_relay_GPIO_Port, sugar_relay_Pin, GPIO_PIN_RESET);
	while(h_now < h_end){
		h_now = get_sugar_dist();
	}
	HAL_GPIO_WritePin(sugar_relay_GPIO_Port, sugar_relay_Pin, GPIO_PIN_SET);
	
	if(DEBUG){
		//printf("add %d ml sugar\r\n", ml);
	}
}

void mix_on(void){
	HAL_GPIO_WritePin(mixer_relay_GPIO_Port, mixer_relay_Pin, GPIO_PIN_RESET);
}

void mix_off(void){
	HAL_GPIO_WritePin(mixer_relay_GPIO_Port, mixer_relay_Pin, GPIO_PIN_SET);
}


void mix_time(uint16_t ms){
	mix_on();
	HAL_Delay(ms);
	mix_off();
}

void heat_on(void){
	HAL_GPIO_WritePin(heater_relay_GPIO_Port, heater_relay_Pin, GPIO_PIN_RESET);
}

void heat_off(void){
	HAL_GPIO_WritePin(heater_relay_GPIO_Port, heater_relay_Pin, GPIO_PIN_SET);
}


void heat_time(uint8_t s){
	heat_on();
	uint32_t ms = s * 1000;
	HAL_Delay(ms);
	heat_off();
}

void pour_out(void){
	HAL_GPIO_WritePin(pour_relay_GPIO_Port,pour_relay_Pin, GPIO_PIN_RESET);
	HAL_Delay(10000);
	HAL_GPIO_WritePin(pour_relay_GPIO_Port,pour_relay_Pin, GPIO_PIN_SET);
	
}

