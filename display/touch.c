/**************************************************************************/

//Private includes --------------------------------------------------------//
#include "touch.h"
#include "spi.h"
#include "tim.h"
#include "st7735.h"

// Private defines -------------------------------------------------------//

#define 	READ_Y 			0xD0
#define 	READ_X 			0x90
#define 	OFFSET    	25
#define 	INIT_CORD   0

// to calibrate uncomment UART_Printf line in ili9341_touch.c
#define 	ILI9341_TOUCH_MIN_RAW_X 		1500
#define 	ILI9341_TOUCH_MAX_RAW_X 		31000
#define 	ILI9341_TOUCH_MIN_RAW_Y 		3276
#define 	ILI9341_TOUCH_MAX_RAW_Y 		30110

#define		DISPLAY_PANEL_WIDTH 			160 //ширина экрана
#define 	DISPLAY_PANEL_HEIGHT 			128 //длина

//Variables---------------------------------------------------------------//
SPI_HandleTypeDef *touch_hspi = &hspi1;
static uint16_t dispSizeX, dispSizeY; // Размеры дисплея
uint8_t calibration_flag = FALSE;
static const int16_t xCenter[] = {OFFSET, DISPLAY_PANEL_WIDTH-OFFSET, OFFSET, 	DISPLAY_PANEL_WIDTH-OFFSET};
static const int16_t yCenter[] = {OFFSET, OFFSET, DISPLAY_PANEL_HEIGHT-OFFSET,	DISPLAY_PANEL_HEIGHT-OFFSET };
static int16_t xPos[5], yPos[5];
static int16_t axc[2], ayc[2], bxc[2], byc[2];
static int16_t ax, bx, ay, by; // Корректирующие коэффициенты
static uint8_t XPT2046_initilazed = FALSE;

//Private function prototypes  --------------------------------------------//
static uint8_t TouchVerifyCoef (void);
static void TouchSetCoef ( int16_t , int16_t , int16_t , int16_t );

//--------------------------------------------------------------------------------------------------------//
void TouchInit(void)
{
	calibration_flag = FALSE;
	dispSizeX = DISPLAY_PANEL_WIDTH; //Установка размеров дисплея
	dispSizeY = DISPLAY_PANEL_HEIGHT;
	XPT2046_initilazed = TRUE;
}

//----------------------------------------Проверка наличия касания----------------------------------------//
uint8_t isTouch (void)
{
	if (LL_GPIO_IsInputPinSet( TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin ) == RESET )
  {	return TRUE;	}// было касание 
	else
	{	return FALSE; }
} 

//--------------------------------------------------------------------------------------------------------//
static uint8_t TouchVerifyCoef ( void )
{
	//if ((ax == 0) || (ax == 0xFFFF) || (bx == 0xFFFF) || (ay == 0) || (ay == 0xFFFF) || (by == 0xFFFF))
	{	return TRUE; }
	//return FALSE;
} 

//Калибровка----------------------------------------------------------------------------------------------//
void TouchCalibrate (void)
{
	int16_t x, y;

	// Если калибровочные коэффициенты уже установлены - выход
/*	if ( !TouchVerifyCoef ( ) )
		return;*/

	//Левый верхний угол
	ClearLcdMemory();
	lcdDrawHLine(INIT_CORD, INIT_CORD+(2*OFFSET), OFFSET, BLACK8); 
	lcdDrawVLine(OFFSET, INIT_CORD, INIT_CORD+(2*OFFSET), BLACK8);
	LCD_ShowString (30, 70, "calibration");
	LCD_ShowString(45, 90, "press");
	LCD_Refresh();
	while (1)
	{
		if (ILI9341_TouchGetCoordinates ( &x, &y) == TRUE)
		{
			if (x < 4090 && y < 4090)
			{
				xPos[0] = x;
				yPos[0] = y;
				break;
			} 
		}
	} 	
	ClearLcdMemory();
	snprintf (LCD_str_buffer, sizeof (LCD_str_buffer), "x=%d, y=%d", xPos[0], yPos[0]);
	LCD_ShowString(50, 70, LCD_str_buffer);
	LCD_ShowString(50, 100, "release");	
	LCD_Refresh();
//	HAL_Delay(3000);
	while (isTouch() == TRUE) {} // ждать отпускания
		
	//Правый верхний угол
	ClearLcdMemory(); 	// right top corner draw
	lcdDrawHLine (dispSizeX-(2*OFFSET), dispSizeX+INIT_CORD, OFFSET, BLACK8);
	lcdDrawVLine (dispSizeX-OFFSET,	INIT_CORD, INIT_CORD+(2*OFFSET), BLACK8);
	LCD_ShowString (30, 70, "calibration");
	LCD_ShowString(45, 90, "press");
	LCD_Refresh();		
	while (1)
	{
		if (ILI9341_TouchGetCoordinates ( &x, &y) == TRUE)
		{
			if (x < 4090 && y < 4090)
			{
				xPos[1] = x;
				yPos[1] = y;
				break;
			}
		} 
	} 	
	ClearLcdMemory(); 
	snprintf (LCD_str_buffer, sizeof (LCD_str_buffer), "x=%d, y=%d", xPos[1], yPos[1]);
	LCD_ShowString(50, 70, LCD_str_buffer);
	LCD_ShowString(50, 100, "release");	
	LCD_Refresh();
//	HAL_Delay(3000);
	while (isTouch() == TRUE) {} // ждать отпускания

	ClearLcdMemory(); 	// Левый нижний угол
	lcdDrawHLine(INIT_CORD, INIT_CORD+(2*OFFSET), dispSizeY-OFFSET, BLACK8);	
	lcdDrawVLine (INIT_CORD+OFFSET, dispSizeY-(2*OFFSET), dispSizeY, BLACK8);	
	LCD_ShowString (30, 10, "calibration");
	LCD_ShowString(45, 30, "press");
	LCD_Refresh();
	while (1)
	{
		if (ILI9341_TouchGetCoordinates ( &x, &y) == TRUE)
		{
			if (x < 4090 && y < 4090)
			{
				xPos[2] = x;
				yPos[2] = y;
				break;
			}
		} 
	} 	
	ClearLcdMemory(); 
	snprintf (LCD_str_buffer, sizeof (LCD_str_buffer), "x=%d, y=%d", xPos[2], yPos[2]);
	LCD_ShowString(50, 70, LCD_str_buffer);
	LCD_ShowString(50, 100, "release");	
	LCD_Refresh();
//	HAL_Delay(3000);
	while (isTouch() == TRUE) {} // ждать отпускания

	ClearLcdMemory(); // Правый нижний угол
	lcdDrawHLine(dispSizeX-(2*OFFSET), dispSizeX+INIT_CORD, dispSizeY-OFFSET, BLACK8);
	lcdDrawVLine (dispSizeX-OFFSET, dispSizeY-INIT_CORD, dispSizeY-(2*OFFSET),  BLACK8);	
	LCD_ShowString (30, 10, "calibration");
	LCD_ShowString(45, 30, "press");
	LCD_Refresh();
	while (1)
	{
		if (ILI9341_TouchGetCoordinates ( &x, &y) == TRUE)
		{
			if (x < 4090 && y < 4090)
			{
				xPos[3] = x;
				yPos[3] = y;
				break;
			}
		} 
	} 	
	ClearLcdMemory(); 
	snprintf (LCD_str_buffer, sizeof (LCD_str_buffer), "x=%d, y=%d", xPos[3], yPos[3]);
	LCD_ShowString(50, 70, LCD_str_buffer);
	LCD_ShowString(50, 100, "release");	
	LCD_Refresh();
//	HAL_Delay(3000);
	while (isTouch() == TRUE) {} // ждать отпускания

	// Расчёт коэффициентов
	axc[0] = (xPos[3] - xPos[0])/(xCenter[3] - xCenter[0]);
	bxc[0] = xCenter[0] - xPos[0]/axc[0];
	ayc[0] = (yPos[3] - yPos[0])/(yCenter[3] - yCenter[0]);
	byc[0] = yCenter[0] - yPos[0]/ayc[0];

	/*axc[1] = (xPos[2] - xPos[1])/(xCenter[2] - xCenter[1]);
	bxc[1] = xCenter[1] - xPos[1]/axc[1];
	ayc[1] = (yPos[2] - yPos[1])/(yCenter[2] - yCenter[1]);
	byc[1] = yCenter[1] - yPos[1]/ayc[1];*/

	// Сохранить коэффициенты
	TouchSetCoef ( axc[0], bxc[0], ayc[0], byc[0] );

	ClearLcdMemory(); 
	LCD_ShowString(5, 30, "Finish calibration");
	snprintf (LCD_str_buffer, sizeof (LCD_str_buffer), "ax=%d, bx=%d", ax, bx);
	LCD_ShowString(5, 50, LCD_str_buffer);
	snprintf (LCD_str_buffer, sizeof (LCD_str_buffer), "ay=%d, by=%d", ay, by);
	LCD_ShowString(5, 70, LCD_str_buffer);
	LCD_Refresh();
	calibration_flag = TRUE;
} 

//---------------------------------Сохранение калибровочных коэффициентов---------------------------------//ъ
static void TouchSetCoef ( int16_t _ax, int16_t _bx, int16_t _ay, int16_t _by )
{
	ax = _ax;
	bx = _bx;
	ay = _ay;
	by = _by;
} 


//--------------------------------------------------------------------------------------------------------//
uint8_t ILI9341_TouchGetCoordinates(int16_t * x, int16_t * y) 
{
	static const uint8_t cmd_read_x[] = { READ_X };
	static const uint8_t cmd_read_y[] = { READ_Y };
	static const uint8_t zeroes_tx[] = { 0x00, 0x00 };
	uint8_t y_raw[2]; 		
	uint8_t x_raw[2];
	uint32_t raw_x = 0;
	uint32_t raw_y = 0;
	uint32_t avg_x = 0;
	uint32_t avg_y = 0;
	uint8_t nsamples = 0;
		
	T_CS(ON);
	for(uint8_t i = 0; i < 10; i++) //получение 10 подряд координат
	{
		if((isTouch()) == FALSE)
		{	break;	}
		nsamples++; //увеличение на единицу
		
		HAL_SPI_Transmit(touch_hspi, (uint8_t*)cmd_read_y, sizeof(cmd_read_y), HAL_MAX_DELAY);
		HAL_SPI_TransmitReceive(touch_hspi, (uint8_t*)zeroes_tx, y_raw, sizeof(y_raw), HAL_MAX_DELAY);
		
		HAL_SPI_Transmit(touch_hspi, (uint8_t*)cmd_read_x, sizeof(cmd_read_x), HAL_MAX_DELAY);
		HAL_SPI_TransmitReceive(touch_hspi, (uint8_t*)zeroes_tx, x_raw, sizeof(x_raw), HAL_MAX_DELAY);

		avg_x += (((uint16_t)x_raw[0]) << 8) | ((uint16_t)x_raw[1]);
		avg_y += (((uint16_t)y_raw[0]) << 8) | ((uint16_t)y_raw[1]);
	}
	T_CS(OFF);	

  if(nsamples < 10)
	{	return FALSE; }

	raw_x = (avg_x/10);
	if(raw_x < ILI9341_TOUCH_MIN_RAW_X)
	{	raw_x = ILI9341_TOUCH_MIN_RAW_X; }
  if(raw_x > ILI9341_TOUCH_MAX_RAW_X)
	{	raw_x = ILI9341_TOUCH_MAX_RAW_X; }

	raw_y = (avg_y/10);
	if(raw_y < ILI9341_TOUCH_MIN_RAW_X)
	{	raw_y = ILI9341_TOUCH_MIN_RAW_Y;	}
	if(raw_y > ILI9341_TOUCH_MAX_RAW_Y)
	{	raw_y = ILI9341_TOUCH_MAX_RAW_Y;	}
   
  // UART_Printf("raw_x = %d, raw_y = %d\r\n", x, y);  // Uncomment this line to calibrate touchscreen:

	*x = ((raw_x - ILI9341_TOUCH_MIN_RAW_X) * DISPLAY_PANEL_WIDTH) /	(ILI9341_TOUCH_MAX_RAW_X - ILI9341_TOUCH_MIN_RAW_X);
	*y = ((raw_y - ILI9341_TOUCH_MIN_RAW_Y) * DISPLAY_PANEL_HEIGHT) /	(ILI9341_TOUCH_MAX_RAW_Y - ILI9341_TOUCH_MIN_RAW_Y);
	
	if (calibration_flag == TRUE)
	{
		*x = (*x/ax) + bx;
		*y = (*y/ay) + by;
	}
	return TRUE;
}

//-------------------------------------------------------------------------------------------------------//
