#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"	

#define FIRST_RTC_REGISTR_TIME 		0x00
#define FIRST_RTC_REGISTR_DATE 		0x04
#define RTC_REGISTR_TEMP 					0x11	
#define RTC_ADDRESS 							0xD0 //i2c-адрес мк RTC со сдвигом влево на один разряд 

uint8_t GetTime (uint8_t , uint8_t , uint8_t , uint8_t * );
uint8_t GetTemp (uint8_t  , uint8_t ,  uint8_t * );
uint8_t SetTime (uint8_t , uint8_t , char *);
uint8_t RTC_ConvertToDec(uint8_t );
uint8_t RTC_ConvertToBinDec(uint8_t );
void edit_RTC_data (uint8_t , char * );
uint8_t read_reg_RTC (uint8_t , uint8_t );
uint8_t get_file_title (void);	
void convert_time (unsigned char , unsigned char * , unsigned char * );	
#ifdef __cplusplus
}
#endif

#endif /* RTC_H */
