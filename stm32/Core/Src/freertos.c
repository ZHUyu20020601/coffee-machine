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
	2. 注意osDelay是阻塞的，HAL_DELAY()是非阻塞的
		 在使用osDelay时，其他的task任然可以执行
	3. making进程和error进程都采用查询系统状态getsysemstatus()获取状态
		 在connect.c中的start和emergent_stop调用setsystemstatus()修改状态
		 由management进程（优先级仅次于中断）检测系统状态变化，然后挂起making或error进程
	4. 所有线程在for的无限循环中必须由osDelay，否则将持续占用cpu
	5. 在线程里面HAL_UART_Transmit_DMA对uart3似乎无效
		 
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
#include "string.h"
#include "connect.h"
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
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
//制作进程句柄
osThreadId_t makingTaskHandle;

const osThreadAttr_t makingTask_attributes = {
	.name = "makingTask",
	.stack_size = 128 * 10,
	.priority = (osPriority_t) osPriorityNormal,//third rank
};

//出错进程句柄
osThreadId_t errorTaskHandle;

const osThreadAttr_t errorTask_attributes = {
	.name = "errorTask",
	.stack_size = 128 * 6,
	.priority = (osPriority_t) osPriorityAboveNormal,//second rank

};

//管理进程句柄
osThreadId_t managementHandle;

const osThreadAttr_t managementTask_attributes = {
	.name = "managementTark",
	.stack_size = 128 * 8,
	.priority = (osPriority_t) osPriorityHigh,//highest priority
};



/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
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
	  //HAL_UART_Transmit_DMA(&huart2, msg, strlen((char*)msg));
	  //osDelay(pdMS_TO_TICKS(1000));
	  osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**
* @brief  coffee making process
* @param  argument: Not used
* @retval None
  */

uint8_t cnt = 0;

void startMakingTask(void *argument){
	
	
	cnt = 0;

	for(;;){
		/*COFFEE MAKING PROCESS ADDING AT BELOW*/
		
		/*
		TODO:
		procedures:
		1. add coffee
			1.1. volumn
		2. add milk
		3. add sugar
		4. heat
		5. mix
		6. pour out
		
		*/
		
		
		if( GetSystemStatus() == Making){
			
			printf("making %d%%\n", cnt*5);
			
			printf("coffee dist = %.2f cm\n",get_coffee_dist());
			printf("milk dist = %.2f cm\n",get_milk_dist());
			printf("sugar dist = %.2f cm\n", get_sugar_dist());
			printf("temp = %.1f\n", 0.1 * read_temp());
			
			osDelay(pdMS_TO_TICKS(1000));
			//HAL_Delay(200);
			cnt++;
			if(cnt > 20){
				SetStatusWaiting();//switch status back to Waiting
				cnt = 0;
			}
		}
		
		
		/*COFFEE MAKING PROCESS ADDING AT ABOVE*/
		else{
			osDelay(200);
		}
	}
	
}

/**
* @brief  emergent stop
* @param  argument: Not used
* @retval None
  */
void startErrorTask(void *argument){
	for(;;){
		
		/*EMERGENT STOP PROCESS ADDING BELOW*/
		if(GetSystemStatus() == Error){
			
			printf("emergent stop!\n");	
			osDelay(pdMS_TO_TICKS(100));			
		/*EMERGENT STOP PROCESS ADDING ABOVE*/
		}else{
			osDelay(200);
		}
		
	}
}



uint8_t sysflag = 0;//prevent replicate execute

//ATTENTION:
//CANNOT GO DIRECTLY TO ERROR MODE VIA WAITING MODE, ONLY VIA MAKING
void managementTask(void *argument){
	for(;;){
		//suspend Error, Resume making
		if(osThreadGetState(errorTaskHandle) != osThreadBlocked && GetSystemStatus() == Making && sysflag == 0){ 
				
				osThreadSuspend(errorTaskHandle);
				osThreadResume(makingTaskHandle);
				sysflag = 1;
		}
		if(osThreadGetState(makingTaskHandle) != osThreadBlocked && GetSystemStatus() == Error && sysflag == 1){
				osThreadSuspend(makingTaskHandle);
				osThreadResume(errorTaskHandle);
				sysflag = 0;
		}
		if( GetSystemStatus() == Waiting){
				sysflag = 0;
		}
		// with delay for only 1 tick, this management thread will execute very frequently
		// to switch the two threads immediately
		osDelay(1);
		
	}
}




/* USER CODE END Application */

