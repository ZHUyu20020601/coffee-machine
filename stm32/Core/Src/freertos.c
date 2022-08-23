/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
	
/* 
* 在这里写些提醒！
  1. osDelay(pdMS_TO_TICKS(ms));可以延时毫秒?
	2. 注意osDelay是阻塞的，HAL_DELAY()是非阻塞的。
		 在使用osDelay时，其他的task任然可以执行
	3. making进程和error进程都采用查询系统状态getsysemstatus()获取状态
		 在connect.c中的start和emergent_stop调用setsystemstatus()修改状态
		 由management进程（优先级仅次于中断）检测系统状态变化，然后挂起making或error进程
	4. 所有线程在for的无限循环中必须由osDelay，否则将持续占用cpu
		 
*/
	
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "sys.h"
#include "stdio.h"
#include "sensor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//在os中延时采用的单位是tick，从毫秒到ticks采用下面的宏定义，因为系统中已经定义过，可以直接使用
//#define  pdMS_TO_TICKS( xTimeInMs )     ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

extern onewire tempSensor;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
//制作进程句柄
osThreadId_t makingTaskHandle;

const osThreadAttr_t makingTask_attributes = {
	.name = "makingTask",
	.stack_size = 128 * 10,
	.priority = (osPriority_t) osPriorityNormal,//第三
};

//出错进程句柄
osThreadId_t errorTaskHandle;

const osThreadAttr_t errorTask_attributes = {
	.name = "errorTask",
	.stack_size = 128 * 6,
	.priority = (osPriority_t) osPriorityAboveNormal,//第二高

};

//管理进程句柄
osThreadId_t managementHandle;

const osThreadAttr_t managementTask_attributes = {
	.name = "managementTark",
	.stack_size = 128 * 8,
	.priority = (osPriority_t) osPriorityHigh,//设置优先级最高
};



/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void startMakingTask(void *argument);
void startErrorTask(void *argument);
void managementTask(void *argument);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	makingTaskHandle = osThreadNew(startMakingTask, "making", &makingTask_attributes);
	errorTaskHandle = osThreadNew(startErrorTask, "error", &errorTask_attributes);
	managementHandle = osThreadNew(managementTask, "manage", &managementTask_attributes);

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
		//printf("default\n");
		osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**
* @brief  咖啡机制作进程，在里面调用func中的函数
* @param  argument: Not used
* @retval None
  */

uint8_t cnt = 0;//计数变量

void startMakingTask(void *argument){
	
	
	cnt = 0;

	for(;;){
		/*在下方加入制作咖啡机的进程代码*/
		
		/*
		TODO:
		将下面的代码（if中的内容）替换为实际的业务逻辑代码
		1. 加咖啡
			1.1. 容量
		2. 加牛奶
		3. 加糖
		4. 加热
		5. 搅拌
		6. 倒出
		
		ATTENTION:
		1. Error进程会将此进程挂起
		2. 此进程挂起后恢复，会直接回到上一次被挂起的点继续执行
		
		*/
		
		
		if( GetSystemStatus() == Making){
			
			printf("making %d%%\n", cnt*5);
			
			printf("temp = %.2f\n", ds18b20_readtemperature(&tempSensor) / 100.0);
			printf("sugar dist = %.2f\n", get_sugar_dist());
			printf("temp again = %.2f\n", ds18b20_readtemperature(&tempSensor) / 100.0);
			
			//osDelay(pdMS_TO_TICKS(2000));
			//HAL_Delay(200);
			cnt++;
			if(cnt > 20){
				SetStatusWaiting();//重新将状态切回等待
				cnt = 0;
			}
		}
		
		
		/*在上方加入制作咖啡机的进程代码*/
		else{
			osDelay(200);
		}
	}
	
}

/**
* @brief  紧急停止
* @param  argument: Not used
* @retval None
  */
void startErrorTask(void *argument){
	for(;;){
		
		/*在下方加入咖啡机停机的代码*/
		if(GetSystemStatus() == Error){
			
			printf("emergent stop!\n");			
			
		}
		/*在上方加入紧急停机的进程代码*/
		
		osDelay(200);
		
	}
}


uint8_t sysflag = 0;//防止重复执行

void managementTask(void *argument){
	for(;;){
		if(osThreadGetState(errorTaskHandle) != osThreadBlocked && GetSystemStatus() == Making && sysflag == 0){ //执行，挂起error 恢复making
				
				osThreadSuspend(errorTaskHandle);
				osThreadResume(makingTaskHandle);
				sysflag = 1;
				//printf("111\n");
		}
		if(osThreadGetState(makingTaskHandle) != osThreadBlocked && GetSystemStatus() == Error && sysflag == 1){
				//printf("222\n");
				osThreadSuspend(makingTaskHandle);
				osThreadResume(errorTaskHandle);
				sysflag = 0;
		}
		if( GetSystemStatus() == Waiting){
				sysflag = 0;
		}
		osDelay(1);//单次执行循环速度快
		//必须要有delay，否则会始终占用cpu，阻塞其他函数
	}
}




/* USER CODE END Application */

