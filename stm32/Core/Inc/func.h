#ifndef __FUNC_H
#define __FUNC_H

//this file contains all machine functions like adding material and controling heater
#include "main.h"

		/*
		1. 加咖啡
			1.1. 容量
			1.2. 开闸
		2. 加牛奶
		3. 加糖
		4. 加热
		5. 搅拌
		6. 倒出
		*/

void shut_all_relay(void);

//add V ml ingredient
void add_coffee(uint8_t ml);
void add_milk(uint8_t ml);
void add_sugar(uint8_t ml);

void mix_on(void);
void mix_off(void);
void mix_time(uint16_t ms);// millisecond

void heat_on(void);
void heat_off(void);
void heat_temp(void);// control by tempurature
//void heat_time(uint8_t s);// second

void pour_out(void);

#endif
