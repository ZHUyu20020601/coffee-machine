
#ifndef __SENSOR_H_
#define __SENSOR_H_

#include "main.h"
#include "tim.h"
#include "tempSensor.h"

float get_coffee_dist(void);
float get_milk_dist(void);
float get_sugar_dist(void);

void coffee_trig_send(void);
void milk_trig_send(void);
void sugar_trig_send(void);

uint32_t coffee_time_cap(void);
uint32_t milk_time_cap(void);
uint32_t sugar_time_cap(void);

float distance(uint32_t us);

//tempurature
int ds18b20_readtemperature(onewire *ptr);

#endif
