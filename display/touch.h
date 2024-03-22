/**************************************************************************/

#ifndef __TOUCH_H__
#define __TOUCH_H__

// Private includes -----------------------------------------------------------//
#include "main.h" 

// Function prototypes --------------------------------------------------------//
void TouchInit(void);
void TouchCalibrate (void);
uint8_t ILI9341_TouchGetCoordinates(uint16_t *, uint16_t *) ;
// Private macro -------------------------------------------------------------//

// Private defines -----------------------------------------------------------//
#define T_CS(x) ((x)? (LL_GPIO_ResetOutputPin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin)) : (LL_GPIO_SetOutputPin(TOUCH_CS_GPIO_Port, TOUCH_CS_Pin)));  

#define 	TRUE 		1
#define 	FALSE 	0
// Private constants --------------------------------------------------------//

#endif
