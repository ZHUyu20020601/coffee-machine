/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void HAL_Delay_us(uint32_t us);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define coffee_trigger_Pin GPIO_PIN_0
#define coffee_trigger_GPIO_Port GPIOC
#define coffee_echo_Pin GPIO_PIN_1
#define coffee_echo_GPIO_Port GPIOC
#define milk_trigger_Pin GPIO_PIN_1
#define milk_trigger_GPIO_Port GPIOA
#define temp_transmit_Pin GPIO_PIN_2
#define temp_transmit_GPIO_Port GPIOA
#define temp_receive_Pin GPIO_PIN_3
#define temp_receive_GPIO_Port GPIOA
#define milk_echo_Pin GPIO_PIN_4
#define milk_echo_GPIO_Port GPIOA
#define sugar_trigger_Pin GPIO_PIN_5
#define sugar_trigger_GPIO_Port GPIOA
#define sugar_echo_Pin GPIO_PIN_6
#define sugar_echo_GPIO_Port GPIOA
#define pour_relay_Pin GPIO_PIN_5
#define pour_relay_GPIO_Port GPIOC
#define coffee_relay_Pin GPIO_PIN_0
#define coffee_relay_GPIO_Port GPIOB
#define milk_relay_Pin GPIO_PIN_1
#define milk_relay_GPIO_Port GPIOB
#define sugar_relay_Pin GPIO_PIN_2
#define sugar_relay_GPIO_Port GPIOB
#define esp_transmit_Pin GPIO_PIN_10
#define esp_transmit_GPIO_Port GPIOB
#define esp_receive_Pin GPIO_PIN_11
#define esp_receive_GPIO_Port GPIOB
#define led_Pin GPIO_PIN_8
#define led_GPIO_Port GPIOA
#define mixer_relay_Pin GPIO_PIN_3
#define mixer_relay_GPIO_Port GPIOB
#define heater_relay_Pin GPIO_PIN_4
#define heater_relay_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
