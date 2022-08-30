
#ifndef __SENSOR_H_
#define __SENSOR_H_

#include "main.h"
#include "tim.h"
#include "tempSensor.h"

#define MOD_RX_BUF_MAX 200

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
//int ds18b20_readtemperature(onewire *ptr);

int16_t read_temp(void);

//void MoudleUartRecNew(uint8_t ch);
void UartSendCmd(uint8_t cmd, uint8_t *parg, uint8_t len);
uint8_t UartReadTemp(int16_t *pobj, int16_t *pamb);
void UartSetTempMode(uint8_t mode);
//void UartTempCalibLow(void);
//void UartTempCalibHigh(void);
void UartSetCommType(uint8_t type);

#endif
