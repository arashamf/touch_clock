/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//-------------------------------------------------------------------------------------------//
/*void i2c_read_array (uint8_t slave_address, uint8_t registr_adr, uint8_t * data, uint8_t numb_byte)
{
	uint8_t count = 0;
	uint8_t tmp_buf[2] ;
	tmp_buf[0] = (uint8_t)slave_address>>8;
	tmp_buf[1] = (uint8_t)((0x00FF & slave_address) | FLAG_WRITE);
	
  LL_I2C_DisableBitPOS(I2C1);
  LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
  LL_I2C_GenerateStartCondition(I2C1);
  while(!LL_I2C_IsActiveFlag_SB(I2C1)){};   //read state
  (void) I2C1->SR1;
		
  LL_I2C_TransmitData8(I2C1, tmp_buf[0]);
  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
  LL_I2C_TransmitData8(I2C1, tmp_buf[1]);
  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
		
	while(!LL_I2C_IsActiveFlag_ADDR(I2C1)){};
  LL_I2C_ClearFlag_ADDR(I2C1);	
		
	LL_I2C_TransmitData8(I2C1, registr_adr);
  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};	
		
  LL_I2C_GenerateStartCondition(I2C1);
  while(!LL_I2C_IsActiveFlag_SB(I2C1)){};
  (void) I2C1->SR1;
		
	LL_I2C_TransmitData8(I2C1, tmp_buf[0]);
  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
  LL_I2C_TransmitData8(I2C1, tmp_buf[1]);
  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
		
  while (!LL_I2C_IsActiveFlag_ADDR(I2C1)){};
  LL_I2C_ClearFlag_ADDR(I2C1);
		
  for(count=0; count<numb_byte; count++)
  {
    if(count < (numb_byte-1))
    {
      while(!LL_I2C_IsActiveFlag_RXNE(I2C1)){};
      *(data+count) = LL_I2C_ReceiveData8(I2C1);
    }
    else
    {
      LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
      LL_I2C_GenerateStopCondition(I2C1);
      while(!LL_I2C_IsActiveFlag_RXNE(I2C1)){};
      *(data+count) = LL_I2C_ReceiveData8(I2C1);
    }
  }		
}

//--------------------------------------------------------------------------------------//
void i2c_write_buffer_16bit_registr (uint8_t slave_address, uint16_t registr_adr, uint8_t * data_buffer, uint8_t numb_byte)
{
	uint8_t tmp_buf[2] ;
	tmp_buf[0] = (uint8_t)(registr_adr>>8);
	tmp_buf[1] = (uint8_t)(0x00FF & registr_adr);
	uint8_t count = 0;
	
	LL_I2C_DisableBitPOS(I2C1);
  LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
	
  LL_I2C_GenerateStartCondition(I2C1);
  while(!LL_I2C_IsActiveFlag_SB(I2C1)){};   //read state
  (void) I2C1->SR1;
		
  LL_I2C_TransmitData8(I2C1, slave_address | FLAG_WRITE);
  while(!LL_I2C_IsActiveFlag_ADDR(I2C1)){};
  LL_I2C_ClearFlag_ADDR(I2C1);
		
  LL_I2C_TransmitData8(I2C1, (uint8_t) tmp_buf[0]);
  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
		
  LL_I2C_TransmitData8(I2C1, (uint8_t) tmp_buf[1]);
  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
		
  for(count = 0; count<numb_byte; count++)
  {
    LL_I2C_TransmitData8(I2C1, *(data_buffer + count));
    while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
  }
  LL_I2C_GenerateStopCondition(I2C1); //условие STOP
}

//-------------------------------------------------------------------------------------//
void i2c_write_buffer (uint8_t slave_address, uint8_t * data_buffer, uint8_t numb_byte)
{
	uint8_t count = 0;
	
	LL_I2C_DisableBitPOS(I2C1);
  LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
	
  LL_I2C_GenerateStartCondition(I2C1);
  while(!LL_I2C_IsActiveFlag_SB(I2C1)){};   //read state
  (void) I2C1->SR1;
		
  LL_I2C_TransmitData8(I2C1, slave_address | FLAG_WRITE);
  while(!LL_I2C_IsActiveFlag_ADDR(I2C1)){};
  LL_I2C_ClearFlag_ADDR(I2C1);
		
	for(count = 0; count < numb_byte; count++)
  {
    LL_I2C_TransmitData8(I2C1, *(data_buffer + count));
    while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
  }
  LL_I2C_GenerateStopCondition(I2C1); //условие STOP
}*/
/* USER CODE END 1 */
