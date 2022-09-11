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
	

	
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "includes.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* 
* 在这里写些提醒！
  1. osDelay(pdMS_TO_TICKS(ms));可以延时毫秒?
	2. 注意osDelay是阻塞的，HAL_DELAY()是非阻塞的
		 在使用osDelay时，其他的task任然可以执行
	3. making进程和error进程都采用查询系统状态getsysemstatus()获取状态
		 在connect.c中的start和emergent_stop调用setsystemstatus()修改状态
		 由management进程（优先级仅次于中断）检测系统状态变化，然后挂起making或error进程
	4. 所有线程在for的无限循环中必须由osDelay，否则将持续占用cpu
	5. 在线程里尽量采用非阻塞式的发送方式，即使用HAL_UART_Transmit而非HAL_UART_Transmit_DMA
		 
*/

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

extern onewire tempSensor;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern uint8_t send_buf[200];
extern int DEBUG;
extern SystemCfg environmentCfg;


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

osThreadId_t sendHandle;

const osThreadAttr_t sendTask_attributes = {
	.name = "sendTark",
	.stack_size = 128 * 8,
	.priority = (osPriority_t) osPriorityNormal,//highest priority
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
void sendTask(void* argument);

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
	sendHandle = osThreadNew(sendTask, "sender", &sendTask_attributes);

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
	  environmentCfg.coffee = dist_to_vol(get_coffee_dist());
	  environmentCfg.milk = dist_to_vol(get_milk_dist());
	  environmentCfg.sugar = dist_to_vol(get_sugar_dist());
	  environmentCfg.temp = read_temp();
	  osDelay(20);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**
* @brief  coffee making process
* @param  argument: Not used
* @retval None

* TODO:
	procedures:
	1. add coffee
		1.1. volumn
	2. add milk
	3. add sugar
	4. heat
	5. mix
	6. pour out
		
*/

void startMakingTask(void *argument){
	
	for(;;){		
		if( GetSystemStatus() == Making){
			
	/*COFFEE MAKING PROCESS ADDING AT BELOW*/
				
			switch_led();
			add_coffee(GetCurrentCfg(coffee));
			
			
			switch_led();
			add_milk(GetCurrentCfg(milk));
			
			switch_led();
			add_sugar(GetCurrentCfg(sugar));
			
			switch_led();
			//heater and mixer are synchronized
			mix_on();
			heat_temp();
			mix_off();

			switch_led();
			pour_out();

			
			shut_all_relay();
			
			

	/*COFFEE MAKING PROCESS ADDING AT ABOVE*/
			SetStatusWaiting();
			osDelay(pdMS_TO_TICKS(500));			
			
		}
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
		
		if(GetSystemStatus() == Error){
			
		/*EMERGENT STOP PROCESS ADDING BELOW*/
			shut_all_relay();
		/*EMERGENT STOP PROCESS ADDING ABOVE*/
			
			
			osDelay(pdMS_TO_TICKS(1000));			
		}else{
			osDelay(200);
		}
		
	}
}



//FUNCTION:
//make sure that making and error task won't execute at the same time
void managementTask(void *argument){
	for(;;){
		//suspend Error, Resume making
		if(osThreadGetState(errorTaskHandle) != osThreadBlocked && GetSystemStatus() == Making ){ 
			osThreadSuspend(errorTaskHandle);
			osThreadResume(makingTaskHandle);
		}
		if(osThreadGetState(makingTaskHandle) != osThreadBlocked && GetSystemStatus() == Error ){
			osThreadSuspend(makingTaskHandle);
			osThreadResume(errorTaskHandle);
		}

		// with delay for only 1 tick, this management thread will execute very frequently
		// to switch the two threads immediately
		osDelay(1);
		
	}
}

void sendTask(void* argument){
	for(;;){
		//detect msg exist
		if(strlen((char*)send_buf) > 0){
			
			if(DEBUG){
				HAL_UART_Transmit(&huart1, send_buf, strlen((char*)send_buf), portMAX_DELAY);
			}
			else{
				HAL_UART_Transmit(&huart3, send_buf, strlen((char*)send_buf), portMAX_DELAY);
				//make a copy to usb
				HAL_UART_Transmit(&huart1, send_buf, strlen((char*)send_buf), portMAX_DELAY);
			}
			
			//clear send-buf
			memset(send_buf, 0, 200);
			
		}
		osDelay(20);
	}
}



/* USER CODE END Application */

