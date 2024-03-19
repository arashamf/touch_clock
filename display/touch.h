/**************************************************************************/

#ifndef __TOUCH_H__
#define __TOUCH_H__

// Private includes -----------------------------------------------------------//
#include "main.h" 

// Function prototypes --------------------------------------------------------//
void TouchInit(void);
uint8_t TouchReadXY (uint16_t *px, uint16_t *py, uint8_t isReadCorrected);
void TouchSetScreenSize ( uint16_t sizeX, uint16_t sizeY );
uint8_t isTouch (void);
void TouchSetCoef (int16_t _ax, int16_t _bx, int16_t _ay, int16_t _by);
void TouchCalibrate (void);
uint8_t ILI9341_TouchGetCoordinates(uint16_t *, uint16_t *) ;
// Private macro -------------------------------------------------------------//

// Private defines -----------------------------------------------------------//
#define T_CS(x) ((x)? (LL_GPIO_ResetOutputPin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin)) : (LL_GPIO_SetOutputPin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin)));  

#define		DISPLAY_PANEL_WIDTH 			160 //ширина экрана
#define 	DISPLAY_PANEL_HEIGHT 			128 //длина
// Private constants --------------------------------------------------------//

#endif
