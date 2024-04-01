/**************************************************************************/
/*! 
    @file     ST7735.h
*/
/**************************************************************************/

#ifndef __ST7735_H__
#define __ST7735_H__

#ifdef __cplusplus
extern "C" {
#endif

// Private includes ----------------------------------------------------------//
#include "main.h" 

// Private defines -----------------------------------------------------------//
#define ST7735_PANEL_WIDTH  160 //ширина экрана
#define ST7735_PANEL_HEIGHT 128 //длина

// System control functions
#define NOP      				(0x00)
#define SWRESET   			(0x01)
#define RDDID     			(0x04)
#define RDDST     			(0x09)
#define RDDPM     			(0x0A)
#define RDDMADCTL 			(0x0B)
#define RDDCOLMOD 			(0x0C)
#define RDDIM    				(0x0D)
#define RDDSM     			(0x0E)
#define SLPIN     			(0x10)
#define SLPOUT    			(0x11)
#define PTLON    			 	(0x12)
#define NORON     			(0x13)
#define INVOFF    			(0x20)
#define INVON     			(0x21)
#define SGAMMASET   		(0x26)
#define DISPOFF   			(0x28)
#define DISPON    			(0x29)
#define CASET    			 	(0x2A)
#define RASET     			(0x2B)
#define RAMWR     			(0x2C)
#define RAMRD     			(0x2E)
#define PTLAR     			(0x30)
#define TEOFF    			 	(0x34)
#define TEON      			(0x35)
#define MADCTL    			(0x36)
#define IDMOFF    			(0x38)
#define IDMON    			 	(0x39)
#define COLMODE    			(0x3A)
#define RDID1     			(0xDA)
#define RDID2     			(0xDB)
#define RDID3     			(0xDC)

// Panel control functions
#define FRMCTR1  		(0xB1)
#define FRMCTR2   	(0xB2)
#define FRMCTR3   	(0xB3)
#define INVCTR    	(0xB4)
#define DISSET5   	(0xB6)
#define SOURCE			(0xB7)
#define PWCTR1    	(0xC0)
#define PWCTR2   		(0xC1)
#define PWCTR3    	(0xC2)
#define PWCTR4    	(0xC3)
#define PWCTR5   		(0xC4)
#define VMCTR1   		(0xC5)
#define VMOFCTR   	(0xC7)
#define WRID2     	(0xD1)
#define WRID3     	(0xD2)
#define PWCTR6    	(0xFC)
#define NVFCTR1   	(0xD9)
#define NVFCTR2  		(0xDE)
#define NVFCTR3   	(0xDF)

#define GMCTRP1   	(0xE0)
#define GMCTRN1   	(0xE1)
#define EXTCTRL   	(0xF0)
#define GAMMABIT   	(0xF2)
#define VCOM4L    	(0xFF)

// 16 битные цвета
#define BLACK     	0x0000
#define BLUE     		0x001F
#define RED      	 	0xF800
#define GREEN    		0x07E0
#define CYAN      	0x07FF
#define MAGENTA   	0xF81F
#define YELLOW   		0xFFE0
#define WHITE    		0xFFFF
#define BRED     	 	0XF81F
#define GRED 				0XFFE0
#define GBLUE			0X07FF
#define BROWN 		0XBC40 
#define BRRED 		0XFC07
#define GRAY  		0X8430 
#define DARKBLUE    0X01CF	
#define LIGHTBLUE   0X7D7C	 
#define GRAYBLUE    0X5458 
#define LIGHTGREEN  0X841F 
#define LGRAY 			0XC618 
#define LGRAYBLUE   0XA651 
#define LBBLUE      0X2B12 

//8 битные цвета
#define BLACK8 		0x00
#define BLUE8  		0x1F
#define GREEN8 		0x7E
#define WHITE8 		0xFF
#define RED8  		0xF8
#define YELLOW8 	0xFE
#define GREY8 		0xEE

// Private constants --------------------------------------------------------//


// typedef ------------------------------------------------------------------//
enum LCD_orient
{
	ROTATE_0 = 0,
	ROTATE_1,
	ROTATE_2,
	ROTATE_3
};

// Function prototypes ------------------------------------------------------//
void lcdInit(void);
void ClearLcdMemory(void);
void st7735SetAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void LCD_Refresh(void);
void LCD_FillRGB(uint16_t );
void LCD_SetFont(const uint8_t * , uint32_t );
void lcdDrawPixel(uint16_t , uint16_t , uint16_t );
uint32_t LCD_FastShowChar(uint32_t x, uint32_t y, uint8_t num);
uint32_t LCD_GetCharWidth(uint32_t y, uint8_t num);
void LCD_ShowString(uint16_t x, uint16_t y, char *p);
void LCD_ShowStringSize(uint16_t x, uint16_t y, char *p, uint32_t size);
void LCD_DrawBitmap(uint16_t , uint16_t , uint8_t *);
void LCD_DrawBitmapDMA(uint16_t , uint16_t , uint8_t *);
void LCD_DrawBMP(const char* buf, int x0, int y0, int w, int h);
void lcdDrawHLine(uint16_t x0, uint16_t x1, uint16_t y, uint16_t color);
void lcdDrawVLine(uint16_t x, uint16_t y0, uint16_t y1, uint16_t color);
void LcdDrawRectangle(uint16_t x0,uint16_t x1,uint16_t y0,uint16_t y1,uint16_t color);
void LcdDrawGraphSimple(uint32_t *buf, uint32_t color);
void LcdDrawGraph(uint32_t *bufLow, uint32_t *bufMiddle, uint32_t *bufHigh);
void LcdDrawUvGraph(uint32_t Low,uint32_t Middle, uint32_t High);
void LcdDrawASGraph(uint32_t left,uint32_t right);
void LcdDrawMgGraph(int *buf, int low, int high);

// Macros--------------------------------------------------------------------//
#define LCD_RST(x) ((x)? (LL_GPIO_SetOutputPin(LCD_RESET_GPIO_Port, LCD_RESET_Pin)) : (LL_GPIO_ResetOutputPin(LCD_RESET_GPIO_Port, LCD_RESET_Pin)));  
#define LCD_DC(x) ((x)? (LL_GPIO_SetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin)) : (LL_GPIO_ResetOutputPin(LCD_DC_GPIO_Port, LCD_DC_Pin)));  
#define LCD_CS(x) ((x)? (LL_GPIO_ResetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin)) : (LL_GPIO_SetOutputPin(LCD_CS_GPIO_Port, LCD_CS_Pin)));  

// Variables ----------------------------------------------------------------//
extern const unsigned char * GlobalFont;
extern uint32_t Paint_Color;
extern uint32_t Back_Color;
extern uint8_t LCD_buffer[];
//extern const unsigned char Arial_26x28[224*112];		//
//extern const unsigned char Arial_31x33[224*132];		// 20pt
//extern const unsigned char Arial_36x37[224*185];		// 24pt
//extern const unsigned char Arial_44x46[224*276];  	// 30pt

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
