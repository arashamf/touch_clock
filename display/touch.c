/**************************************************************************/

//Private includes --------------------------------------------------------//
#include "touch.h"
#include "spi.h"
#include "tim.h"
#include "st7735.h"

// Private defines -------------------------------------------------------//
#define 	TRUE 		1
#define 	FALSE 	0

#define 	READ_Y 		0xD0
#define 	READ_X 		0x90

// to calibrate uncomment UART_Printf line in ili9341_touch.c
#define 	ILI9341_TOUCH_MIN_RAW_X 		1500
#define 	ILI9341_TOUCH_MAX_RAW_X 		31000
#define 	ILI9341_TOUCH_MIN_RAW_Y 		3276
#define 	ILI9341_TOUCH_MAX_RAW_Y 		30110

// change depending on screen orientation
#define 	ILI9341_TOUCH_SCALE_X 			240
#define 	ILI9341_TOUCH_SCALE_Y 			320

//Variables---------------------------------------------------------------//
SPI_HandleTypeDef *touch_hspi = &hspi1;
static int diffX, diffY; // Корректирующие коэффициенты
static uint16_t dispSizeX, dispSizeY; // Размеры дисплея
static const int16_t xCenter[] = { 35, DISPLAY_PANEL_WIDTH-35, 35, 	DISPLAY_PANEL_WIDTH-35 	};
static const int16_t yCenter[] = { 35, 35, DISPLAY_PANEL_HEIGHT-35,	DISPLAY_PANEL_HEIGHT-35 };
static int16_t xPos[5], yPos[5];
extern char sBuffer[256];
static int16_t axc[2], ayc[2], bxc[2], byc[2];
static int16_t ax, bx, ay, by;

//Private function prototypes  --------------------------------------------//
static uint16_t TPReadX ( void );
static uint16_t TPReadY ( void );
static uint8_t TouchVerifyCoef (void);
uint8_t isTouch (void); // было касание или нет

//-----------------------------------------------------------------------//
void TouchInit(void)
{
	diffX = diffY = 0;
  dispSizeX = dispSizeY = 0;
}

//-----------------------------------------------------------------------//
static uint16_t TPReadX ( void)
{
	uint16_t x;
	T_CS(ON);
	x = T_SPI_Write (touch_hspi, READ_X );		// 10010100 - Read Y, 12bit mode
																						//  001   - A2..A0 - X+
																						//	0     - 12 bit
																						// 	1     = SER/nDFR = 1
	T_CS(OFF);
	return x;
} 

//-----------------------------------------------------------------------//
static uint16_t TPReadY ( void )
{
	uint16_t y;
	T_CS(ON);
	y = T_SPI_Write (touch_hspi, READ_Y);			// 11010100 - Read Y, 12bit mode
																						//  101       - A2..A0 - X+
																						// 0 - 12 bit, 1 = SER/nDFR = 1  
	T_CS(OFF);
	return y;				
} 

//-----------------------------------------------------------------------//
uint8_t TouchReadXY (uint16_t *px, uint16_t *py, uint8_t isReadCorrected)
{
	uint8_t flag;	uint16_t x, y;

	flag = isTouch ();
	if (flag)
	{ 
		y = 4095 - TPReadX(); // касание обнаружено
		x = TPReadY();
		if ((isReadCorrected) && (!TouchVerifyCoef()))
		{
			*px = (x / ax) + bx;
			*py = (y / ay) + by;
		} 
		else // без коррекции
		{ 
			*px = x;
			*py = y;
		} 
	} 
	return flag;
} 

//-----------------------------------------------------------------------//
// Задание размеров дисплея
void TouchSetScreenSize ( uint16_t sizeX, uint16_t sizeY )
{
	dispSizeX = sizeX;
	dispSizeY = sizeY;
} 

//-----------------------------------------------------------------------//
// было касание или нет 
uint8_t isTouch (void)
{
	if (LL_GPIO_IsInputPinSet( TOUCH_IRQ_GPIO_Port, TOUCH_IRQ_Pin ) == RESET )
  {	return TRUE;	}// было касание 
	else
	{	return FALSE; }
} 

//-----------------------------------------------------------------------//
// Задать калибровочные коэффициенты
void TouchSetCoef ( int16_t _ax, int16_t _bx, int16_t _ay, int16_t _by )
{
	ax = _ax;
	bx = _bx;
	ay = _ay;
	by = _by;
} 

//-----------------------------------------------------------------------//
static uint8_t TouchVerifyCoef ( void )
{
	if ((ax == 0) || (ax == 0xFFFF) || (bx == 0xFFFF) || (ay == 0) || (ay == 0xFFFF) || (by == 0xFFFF))
	{	return TRUE; }
	return FALSE;
} 

//Калибровка------------------------------------------------------------//
void TouchCalibrate (void)
{
	uint16_t x, y;

	// Если калибровочные коэффициенты уже установлены - выход
	if ( !TouchVerifyCoef ( ) )
		return;

	LCD_SetFont (GlobalFont, RED8); 	// left top corner draw
	ClearLcdMemory();
	LCD_ShowString (50, 90, "calibration");
	lcdDrawHLine(10, 10+50, 10+25, 0x0000); 
	lcdDrawVLine(10+25, 10, 10+50, 0x0000);
	LCD_ShowString(50, 100, "push");
	LCD_Refresh();
	while (1)
	{
		while (!isTouch()) {} // ожидание нажатия
		TouchReadXY ( &x, &y, FALSE);
		if (x < 4090 && y < 4090)
		{
			xPos[0] = x;
			yPos[0] = y;
			break;
		} 
	} 
	ClearLcdMemory();
	LCD_ShowString(50, 100, "release");	
	LCD_Refresh();
	while (isTouch()) {} // ждать отпускания
//	LCD_ShowString( 50, 100, "          ");
		
	ClearLcdMemory(); 	// right top corner draw
	LCD_ShowString (50, 90, "calibration");
	lcdDrawHLine (DISPLAY_PANEL_WIDTH-10-50, DISPLAY_PANEL_WIDTH-10-50+50, 10+25, 0x0000);
	lcdDrawVLine (10+50, DISPLAY_PANEL_WIDTH-10-25, DISPLAY_PANEL_WIDTH-10-25, 0x0000);
	LCD_ShowString(50, 100, "push");
	LCD_Refresh();		
	while (1)
	{
		// ждать нажатия
		while ( !isTouch ( ) );
		TouchReadXY ( &x, &y, FALSE);
		if (x < 4090 && y < 4090)
		{
			xPos[1] = x;
			yPos[1] = y;
			break;
		} 
	} 	
	ClearLcdMemory(); 
	LCD_ShowString(50, 100, "release");	
	LCD_Refresh();
	while (isTouch()) {} // ждать отпускания
//	LCD_ShowString( 50, 100, "          ");


	ClearLcdMemory(); 	// left down corner draw
	LCD_ShowString  (50, 90, "calibration");
	lcdDrawVLine (DISPLAY_PANEL_HEIGHT-10-25, 10, 10+50, 0x0000 );	// hor
	lcdDrawHLine( DISPLAY_PANEL_HEIGHT-10-50, DISPLAY_PANEL_HEIGHT-10-50+50, 10+25,  0x0000 );	// vert
	LCD_ShowString(50, 100, "push");
	LCD_Refresh();
	while (1)
	{
		// ждать нажатия
		while ( !isTouch ( ) );
		TouchReadXY ( &x, &y, FALSE);
		if (x < 4090 && y < 4090)
		{
			xPos[2] = x;
			yPos[2] = y;
			break;
		} 
	} 
	
	ClearLcdMemory(); 
	LCD_ShowString(50, 100, "release");	
	LCD_Refresh();
	while (isTouch()) {} // ждать отпускания
//	LCD_ShowString( 50, 100, "          ");

	
	ClearLcdMemory(); // Правый нижний
	LCD_ShowString  (50, 90, "calibration");
	lcdDrawHLine( DISPLAY_PANEL_HEIGHT-10-50, DISPLAY_PANEL_WIDTH-10-50+50, DISPLAY_PANEL_HEIGHT-10-25,  0x0000 );
	lcdDrawVLine (DISPLAY_PANEL_HEIGHT-10-25, DISPLAY_PANEL_HEIGHT-10-50, DISPLAY_PANEL_HEIGHT-10-50+50, 0x0000 );	
	LCD_ShowString(50, 100, "push");
	LCD_Refresh();
	while (1)
	{
		// ждать нажатия
		while ( !isTouch ( ) );
		TouchReadXY ( &x, &y, FALSE);
		if (x < 4090 && y < 4090)
		{
			xPos[3] = x;
			yPos[3] = y;
			break;
		} 
	} 
	
	ClearLcdMemory(); 
	LCD_ShowString(50, 100, "release");	
	LCD_Refresh();
	while (isTouch()) {} // ждать отпускания
	//LCD_ShowString( 50, 100, "          ");

	// Расчёт коэффициентов
	axc[0] = (xPos[3] - xPos[0])/(xCenter[3] - xCenter[0]);
	bxc[0] = xCenter[0] - xPos[0]/axc[0];
	ayc[0] = (yPos[3] - yPos[0])/(yCenter[3] - yCenter[0]);
	byc[0] = yCenter[0] - yPos[0]/ayc[0];

	axc[1] = (xPos[2] - xPos[1])/(xCenter[2] - xCenter[1]);
	bxc[1] = xCenter[1] - xPos[1]/axc[1];
	ayc[1] = (yPos[2] - yPos[1])/(yCenter[2] - yCenter[1]);
	byc[1] = yCenter[1] - yPos[1]/ayc[1];

	// Сохранить коэффициенты
	TouchSetCoef ( axc[0], bxc[0], ayc[0], byc[0] );

	ClearLcdMemory(); 
	LCD_ShowString(20, 90, "Finish calibration");
	LCD_Refresh();
	delay_us (10000); //10 ms
} 

//----------------------------------------------------------------------//
uint8_t ILI9341_TouchGetCoordinates(uint16_t * x, uint16_t * y) 
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
	for(uint8_t i = 0; i < 10; i++) 
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

	*x = ((raw_x - ILI9341_TOUCH_MIN_RAW_X) * ILI9341_TOUCH_SCALE_X) /	(ILI9341_TOUCH_MAX_RAW_X - ILI9341_TOUCH_MIN_RAW_X);
	*y = ((raw_y - ILI9341_TOUCH_MIN_RAW_Y) * ILI9341_TOUCH_SCALE_Y) /	(ILI9341_TOUCH_MAX_RAW_Y - ILI9341_TOUCH_MIN_RAW_Y);
	return TRUE;
}