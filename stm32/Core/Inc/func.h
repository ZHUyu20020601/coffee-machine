#ifndef __FUNC_H
#define __FUNC_H

//this file contains all machine functions like adding material and controling heater
#include "main.h"

void shut_all_relay(void);

//add V ml ingredient
void add_coffee(uint8_t ml);
void add_milk(uint8_t ml);
void add_sugar(uint8_t ml);

void mix_on(void);
void mix_off(void);
//void mix_time(uint16_t ms);// millisecond

void heat_on(void);
void heat_off(void);
void heat_temp(void);// control by tempurature
//void heat_time(uint8_t s);// second

void pour_out(void);

void switch_led(void);

#endif
