/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

#define LED_GREEN(x) ((x)? (LL_GPIO_SetOutputPin(LED_GREEN_GPIO_Port, LED_GREEN_Pin)) : (LL_GPIO_ResetOutputPin(LED_GREEN_GPIO_Port, LED_GREEN_Pin)))
#define LED_RED(x) ((x)? (LL_GPIO_SetOutputPin(LED_RED_GPIO_Port, LED_RED_Pin)) : (LL_GPIO_ResetOutputPin(LED_RED_GPIO_Port, LED_RED_Pin)))

#define TOOGLE_GREEN_LED() (LED_GREEN(!(LL_GPIO_IsOutputPinSet(LED_GREEN_GPIO_Port, LED_GREEN_Pin))))
#define TOOGLE_RED_LED() (LED_RED(!(LL_GPIO_IsOutputPinSet(LED_RED_GPIO_Port, LED_RED_Pin))))
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

