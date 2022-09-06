#ifndef __INCLUDES__
#define __INCLUDES__

/*
 arrange all the include headfiles in this file
*/

/*-----std-----*/
#include "stdio.h"
#include "stdlib.h"

/*-----user-----*/
#include "sys.h"
#include "func.h"
#include "sensor.h"
#include "tempSensor.h"
#include "connect.h"
#include "Kalman_Filter.h"

/*-----rtos-----*/
#include "FreeRtos.h"
#include "cmsis_os.h"
#include "task.h"

/*-----other-----*/
#include "main.h"
#include "string.h"
#include "cJSON.h"
#include "math.h"

/*private defination*/
//在os中延时采用的单位是tick，从毫秒到ticks采用下面的宏定义
#define  pdMS_TO_TICKS( xTimeInMs )     ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )

#endif

