// Includes -----------------------------------------------------------------------------------------//
#include "rtc.h"
#include "i2c.h"

// DEFINES ------------------------------------------------------------------------------------------//

// TYPEDEFS ----------------------------------------------------------------------------------------//

// STATIC PROTOTYPES--------------------------------------------------------------------------------//
static uint8_t RTC_ConvertToDec(uint8_t );
static uint8_t RTC_ConvertToBinDec(uint8_t );

// STATIC VARIABLES---------------------------------------------------------------------------------//
//массив с количеством дней в предыдущем месяце, [0][12] - не високосный, [1][12] - високосный 
const unsigned int pred_sum [2] [12] =	{ 
																					{0,31,59,90,120,151,181,212,243,273,304,334}, 
																					{0,31,60,91,121,152,182,213,244,274,305,335} 
																				};

//FUNCTIONS-----------------------------------------------------------------------------------------------//
uint8_t GetTime (uint8_t RTC_adress, uint8_t registr_adress, uint8_t sizebuf, uint8_t * RTC_buffer)
{

	if (HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)RTC_adress, &registr_adress, 1, (uint32_t)0xFFFF)!= HAL_OK)
	{	return  HAL_ERROR;	}
	
	while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){} //ожидание готовности приёма		
		
	if(HAL_I2C_Master_Receive (&hi2c1, (uint16_t) RTC_adress, (uint8_t*)RTC_buffer, (uint16_t) sizebuf, (uint32_t)0xFFFF)!=HAL_OK) //запись показателей времени	
	{	return  HAL_ERROR;	}
	
	for (uint8_t count = 0; count < sizebuf; count++)
	{
		*RTC_buffer = RTC_ConvertToDec(*RTC_buffer); //перевод числа из двоично-десятичного представления  в обычное
		RTC_buffer++;
	}
	
	return  HAL_OK;
}

//---------------------------------------------------------------------------------------------------------//
uint8_t GetTemp (uint8_t  RTC_adress, uint8_t registr_adress,  uint8_t * temp_buffer)
{
	
	if (HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)RTC_adress, &registr_adress, 1, (uint32_t)0xFFFF)!= HAL_OK)
	{	return  HAL_ERROR;	}
	
	while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){} //ожидание готовности приёма
		
	if (HAL_I2C_Master_Receive (&hi2c1, (uint16_t) RTC_adress, temp_buffer, 1, (uint32_t)0xFFFF)!=HAL_OK)
	{	return  HAL_ERROR;	}
	
	return  HAL_OK;
}

//---------------------------------------------------------------------------------------------------------//
uint8_t SetTime (uint8_t RTC_adress, uint8_t registr_adress, char *time)
{
	//формирование сообщения для RTC	
	uint8_t I2C_RTC_buffer [4] = {0}; //массив c отформатированными данными времени для передачи по i2c
	//uint8_t * ptr_RTC_buffer = I2C_RTC_buffer;
	signed char ptr = 1;
	char buf_ch;
	
	for (signed char count = 0; count < 6; count++) 
	{
		buf_ch = *(time+count);
		if ((buf_ch >= 0x30) && (buf_ch <= 0x39))
		{
			if (!(count%2)) //при i=0, 2, 4
			{
				I2C_RTC_buffer [ptr] += 10 * (buf_ch - 48); //десятый разряд секунды, минуты, часы (дня, месяца, года)
			}
			else
			{
				I2C_RTC_buffer [ptr] += buf_ch - 48; //единичный разряд секунды, минуты, часы (дня, месяца, года)
				I2C_RTC_buffer [ptr] = RTC_ConvertToBinDec(I2C_RTC_buffer [ptr]); //перевод в двоично-десятичное представление
				ptr++;
			}
		}
		else
		{	return  HAL_ERROR;	}
	}
	I2C_RTC_buffer [0] = registr_adress; //в первый элемент массива - адрес регистра RTC	
	
	if (HAL_I2C_Master_Transmit(&hi2c1, (uint16_t) RTC_adress, (uint8_t*)I2C_RTC_buffer, 4, (uint32_t)0xFFFF)!= HAL_OK) //отправка данных
	{	return  HAL_ERROR;	}
	
	return  HAL_OK;
}

//-------------------ф-я перевода числа из двоично-десятичного представления  в обычное-------------------//
static uint8_t RTC_ConvertToDec(uint8_t digit)
{
	uint8_t ch = ((digit>>4)*10 + (0x0F & digit));
	return ch;
}

//--------------------ф-я перевода числа из обычного в двоично-десятичного представления--------------------//
static uint8_t RTC_ConvertToBinDec(uint8_t digit)
{
	uint8_t ch = ((digit/10) << 4) + (digit%10);
	return ch;
}


//---------------------------------------------------------------------------------------------------------//
uint8_t read_reg_RTC (uint8_t adress, uint8_t sizebuf)
{
	uint8_t reg_adr = 0x2; //адрес регистра RTC 
//	uint8_t sizebuf = 0x3; //количество байт для чтения
	uint8_t I2C_RTC_buffer [sizebuf];
	
	if(HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)adress, &reg_adr, 1, (uint32_t)0xFFFF)!= HAL_OK) //передача адреса RTC DS3231
  {	return  HAL_ERROR;	}
	
	while(HAL_I2C_GetState(&hi2c1)!=HAL_I2C_STATE_READY){}
		
	if (HAL_I2C_Master_Receive (&hi2c1, (uint16_t) adress, (uint8_t*)I2C_RTC_buffer, (uint16_t) sizebuf, (uint32_t)0xFFFF)!=HAL_OK)
	{	return  HAL_ERROR;	}
	
	for (uint8_t count = 0; count < sizebuf; count++) //перевод числа из двоично-десятичного представления  в обычное
	{	I2C_RTC_buffer[count] = RTC_ConvertToDec(I2C_RTC_buffer[count]);	}
	
	return  HAL_OK;
}

//---------------------------------------------------------------------------------------------------------//
void edit_RTC_data (uint8_t adress,  char * time)
{	

}

//---------------------------------------------------------------------------------------------------------//
uint8_t get_file_title (void)
{
	uint8_t Date [3]; 
	uint8_t file_title = 0;
	
	GetTime (RTC_ADDRESS,  FIRST_REGISTR_DATE, 3, Date); //чтение регистров 0х4-0х6 (дата: дд/мм/гг)
	file_title = ((pred_sum [0][*(Date+1)-1] )+ *Date);
	
	return file_title;
}

//-----------------------------перевод данных времени из числовой в символьной-----------------------------//
void convert_time (uint8_t time_size, char * mod_time_data, uint8_t * time_data)
{
		
	for (int8_t count = (time_size-1); count >= 0; count--)
	{
		*mod_time_data++ = (*(time_data + count) / 10) + 0x30; //сначала часы
		*mod_time_data++ = (*(time_data + count) % 10) + 0x30;
		
		if (count > 0)
		{	*mod_time_data++ = ':'; }
	}
}

//---------------------------------------------------------------------------------------------------------//