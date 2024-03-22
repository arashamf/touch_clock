/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "st7735.h"
#include "Font.h"
#include "touch.h"
#include "rtc.h"
#include "lvgl.h"
#include "LCDController.h"
//#include "lv_spinner.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define LVGL_mode 
#define demo_mode 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char LCD_str_buffer[16];
uint16_t T_Xcord; 
uint16_t T_Ycord;

uint8_t RTC_data [6];
//char demo_time[] = {'0', '0', '0', '5', '1', '2'};
lv_obj_t * led_red;
lv_obj_t * led_green;

//lv_span_t *  clock_spans;

uint8_t sec = 12; 
uint8_t min = 12;
uint8_t hour = 12;

char time [10];

lv_span_t *  clock_span;
lv_obj_t * button;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI2_Init();
  MX_TIM9_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	tim_delay_init ();
	delay_us(5000);
	
	LED_GREEN(OFF);
	LED_RED(ON);
	
	TouchInit();
	
	#ifdef demo_mode
	lcdInit();
	ClearLcdMemory();
	LCD_ShowString (2,2, "Test OK...");
	LCD_Refresh();
	
	LED_RED(ON);
	LED_GREEN(OFF);
	
	TouchCalibrate ();
	
	HAL_Delay(500);
	#endif
	
	#ifdef LVGL_mode
	lv_init();
  lv_port_disp_init();	
	
	// Change the active screen's background color
	lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(LV_PALETTE_INDIGO), LV_PART_MAIN);
	lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0xffffff), LV_PART_MAIN);

	//Create a spinner. Объект Spinner представляет собой вращающуюся дугу внутри круга.
	lv_obj_t * spinner = lv_spinner_create(lv_scr_act(), 1000, 32);
	lv_obj_set_size(spinner, 48, 48);
	lv_obj_align(spinner,  LV_ALIGN_TOP_MID, 0, 5);
	
	
	button = button_init (); // Create a button	
	led_red  = lv_led_create(lv_scr_act());	// Create a red led	
	led_green  = lv_led_create(lv_scr_act()); //Create a green led
	lv_2leds_init (led_red, led_green); //init leds
	
	//Создание объекта span group. Span - это объект, который используется для отображения форматированного текста. 
	//В отличие от объекта label, spangroup может отображать текст, стилизованный под другие шрифты, цвета и размеры, в объект spangroup.
	clock_span = lv_span_init (); 
	
	//SetTime (RTC_ADDRESS, FIRST_REGISTR_TIME, demo_time); //установка времени
	
	if (GetTime (RTC_ADDRESS, FIRST_REGISTR_TIME, 3, RTC_data) == HAL_OK) //получение данных времени  ч/м/с
	{	
		//snprintf (time, sizeof (time), "%2u:%2u:%2u", RTC_data[2],  RTC_data[1],  RTC_data[0]);	
		convert_time (3, time, RTC_data);
	}
	else
	{	snprintf (time, sizeof (time), "error");	}
	lv_span_set_text_static(clock_span, time); //Ввода статического текста. Он не будет сохранен в span, поэтому переменная 'text' должна быть "активной", пока существует span.	
	#endif
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		#ifdef LVGL_mode
		for (uint8_t count=0; count < 100; count++)
		{
			lv_timer_handler();
			HAL_Delay(10);
		}
		
		TOOGLE_GREEN_LED();
		TOOGLE_RED_LED();
		lv_led_toggle(led_red);
		lv_led_toggle(led_green);
		
		if ((sec++)>59)
		{
			sec=0;
			min++;
			if ((min++)>59)
			{
				min=0;
				hour++;
			}
			if (hour > 23)
			{	hour = 0;	}
		}
		if (GetTime (RTC_ADDRESS, FIRST_REGISTR_TIME, 3, RTC_data) == HAL_OK)
		{	
			convert_time (3, time, RTC_data);
		//	snprintf (time, sizeof (time), "%2u:%2u:%2u", RTC_data[2],  RTC_data[1],  RTC_data[0]);	
		}
		else
		{	snprintf (time, sizeof (time), "error");	}
		lv_span_set_text_static(clock_span, time);
		#endif
		
		#ifdef demo_mode
	//	TOOGLE_GREEN_LED();
	//	TOOGLE_RED_LED();
		if (ILI9341_TouchGetCoordinates(&T_Xcord, &T_Ycord) == TRUE)
		{
			ClearLcdMemory();
			snprintf (LCD_str_buffer, sizeof (LCD_str_buffer), "X=%u, Y=%u", T_Xcord, T_Ycord);		
			LCD_ShowString (2, 5, LCD_str_buffer);
			LCD_Refresh();
			HAL_Delay(50);
		}
		HAL_Delay(5);
		#endif
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
