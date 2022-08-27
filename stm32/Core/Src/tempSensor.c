
#include "tempSensor.h"
#include "main.h"


void onewire_setmode(onewire *ptr, unsigned char isout){
	GPIO_InitTypeDef gpio;
	gpio.Pin = ptr->pin;
	gpio.Mode = isout ? GPIO_MODE_OUTPUT_PP : GPIO_MODE_INPUT;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(ptr->gpiox, &gpio);
} 


void onewire_writebyte(onewire *ptr, unsigned char data){
	unsigned char i;
	for(i = 8;i > 0;i--){
		onewire_setmode(ptr, 1);
		//HAL_GPIO_WritePin(ptr->gpiox, ptr->pin, GPIO_PIN_RESET);//write bit 0
		//delay_us(2);
		//if(data & 1){
		//	HAL_GPIO_WritePin(ptr->gpiox, ptr->pin, GPIO_PIN_SET);//write bit 1
		//}
		//else{
		//	HAL_GPIO_WritePin(ptr->gpiox, ptr->pin, GPIO_PIN_RESET);
		//}
		//delay_us(60);
		//HAL_GPIO_WritePin(ptr->gpiox, ptr->pin, GPIO_PIN_SET);
		if(data & 1){//write 1
			HAL_GPIO_WritePin(ptr->gpiox, ptr->pin, GPIO_PIN_RESET);
			HAL_Delay_us(6);
			HAL_GPIO_WritePin(ptr->gpiox, ptr->pin, GPIO_PIN_SET);
			HAL_Delay_us(64);
		}
		else{//write 0
			HAL_GPIO_WritePin(ptr->gpiox, ptr->pin, GPIO_PIN_RESET);
			HAL_Delay_us(60);
			HAL_GPIO_WritePin(ptr->gpiox, ptr->pin, GPIO_PIN_SET);
			HAL_Delay_us(10);
		}
		data >>= 1;
	}
}

unsigned char onewire_readbyte(onewire *ptr){
	unsigned char result = 0, i;
	for(i = 8;i > 0;i--){
		onewire_setmode(ptr, 1);
		HAL_GPIO_WritePin(ptr->gpiox, ptr->pin, GPIO_PIN_RESET);
		HAL_Delay_us(6);
		HAL_GPIO_WritePin(ptr->gpiox, ptr->pin, GPIO_PIN_SET);
		HAL_Delay_us(9);
		result >>= 1;
		onewire_setmode(ptr, 0);//in
		if(HAL_GPIO_ReadPin(ptr->gpiox, ptr->pin)){
			result |= 0x80;
		}
		HAL_Delay_us(55);
	}
	return result;
}

unsigned char onewire_reset(onewire *ptr){
	unsigned char flag = 0;
	onewire_setmode(ptr, 1);
	HAL_GPIO_WritePin(ptr->gpiox, ptr->pin, GPIO_PIN_RESET);//pull onewire low
	HAL_Delay_us(480);
	HAL_GPIO_WritePin(ptr->gpiox, ptr->pin, GPIO_PIN_SET);//release the bus
	HAL_Delay_us(70);
	onewire_setmode(ptr, 0);
	flag = HAL_GPIO_ReadPin(ptr->gpiox, ptr->pin);
	HAL_Delay_us(410);
	return flag;
}
	
