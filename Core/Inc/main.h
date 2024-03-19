/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TOUCH_CS_Pin LL_GPIO_PIN_0
#define TOUCH_CS_GPIO_Port GPIOA
#define TOUCH_IRQ_Pin LL_GPIO_PIN_1
#define TOUCH_IRQ_GPIO_Port GPIOB
#define LED_GREEN_Pin LL_GPIO_PIN_2
#define LED_GREEN_GPIO_Port GPIOB
#define LED_RED_Pin LL_GPIO_PIN_12
#define LED_RED_GPIO_Port GPIOB
#define LCD_DC_Pin LL_GPIO_PIN_3
#define LCD_DC_GPIO_Port GPIOB
#define LCD_RESET_Pin LL_GPIO_PIN_4
#define LCD_RESET_GPIO_Port GPIOB
#define LCD_CS_Pin LL_GPIO_PIN_5
#define LCD_CS_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define 	ON 		1
#define 	OFF 	0

extern char LCD_str_buffer[16];
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
