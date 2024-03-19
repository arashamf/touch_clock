/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
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
/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */
#define CPU_CLOCK_VALUE (96000000UL)
TIM_TypeDef * TIM_DEL = TIM9;
/* USER CODE END 0 */

/* TIM9 init function */
void MX_TIM9_Init(void)
{

  /* USER CODE BEGIN TIM9_Init 0 */

  /* USER CODE END TIM9_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM9);

  /* USER CODE BEGIN TIM9_Init 1 */

  /* USER CODE END TIM9_Init 1 */
  TIM_InitStruct.Prescaler = 0;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 65535;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM9, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM9);
  LL_TIM_SetClockSource(TIM9, LL_TIM_CLOCKSOURCE_INTERNAL);
  /* USER CODE BEGIN TIM9_Init 2 */

  /* USER CODE END TIM9_Init 2 */

}

/* USER CODE BEGIN 1 */
//-----------------------------------------------------------------------------------------------------//
void tim_delay_init (void)
{
	LL_TIM_InitTypeDef TIM_InitStruct = {0};

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM9);   // Peripheral clock enable 

  TIM_InitStruct.Prescaler = (uint16_t)((CPU_CLOCK_VALUE/1000000)-1); //предделитель 96ћ√ц/1ћ√ц=96
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 0xFF;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM_DEL, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM_DEL);
	LL_TIM_SetClockSource(TIM_DEL, LL_TIM_CLOCKSOURCE_INTERNAL);

}

//-----------------------------------------------------------------------------------------------------//
void delay_us(uint16_t delay)
{
  LL_TIM_SetAutoReload(TIM_DEL, delay); //
	LL_TIM_ClearFlag_UPDATE(TIM_DEL); //сброс флага обновлени€ таймера
	LL_TIM_SetCounter(TIM_DEL, 0); //сброс счЄтного регистра
	LL_TIM_EnableCounter(TIM_DEL); //включение таймера
	while (LL_TIM_IsActiveFlag_UPDATE(TIM_DEL) == 0) {} //ожидание установки флага обновлени€ таймера 
	LL_TIM_DisableCounter(TIM_DEL); //выключение таймера		
}

/* USER CODE END 1 */
