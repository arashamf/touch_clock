#include "rtc.h"
#include "i2c.h"

//������ � ����������� ���� � ���������� ������, [0][12] - �� ����������, [1][12] - ���������� 
const unsigned int pred_sum [2] [12] =	{ 
																					{0,31,59,90,120,151,181,212,243,273,304,334}, 
																					{0,31,60,91,121,152,182,213,244,274,305,335} 
																				};

//----------------------------------------------------------------------------------------------------------//
uint8_t GetTime (uint8_t RTC_adress, uint8_t registr_adress, uint8_t sizebuf, uint8_t * RTC_buffer)
{

	if (HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)RTC_adress, &registr_adress, 1, (uint32_t)0xFFFF)!= HAL_OK)
	{	return  HAL_ERROR;	}
	
	while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){} //�������� ���������� �����		
		
	if(HAL_I2C_Master_Receive (&hi2c1, (uint16_t) RTC_adress, (uint8_t*)RTC_buffer, (uint16_t) sizebuf, (uint32_t)0xFFFF)!=HAL_OK) //������ ����������� �������	
	{	return  HAL_ERROR;	}
	
	for (uint8_t count = 0; count < sizebuf; count++)
	{
		*RTC_buffer = RTC_ConvertToDec(*RTC_buffer); //������� ����� �� �������-����������� �������������  � �������
		RTC_buffer++;
	}
	
	return  HAL_OK;
}

//----------------------------------------------------------------------------------------------------------//
uint8_t GetTemp (uint8_t  RTC_adress, uint8_t registr_adress,  uint8_t * temp_buffer)
{
	
	if (HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)RTC_adress, &registr_adress, 1, (uint32_t)0xFFFF)!= HAL_OK)
	{	return  HAL_ERROR;	}
	
	while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){} //�������� ���������� �����
		
	if (HAL_I2C_Master_Receive (&hi2c1, (uint16_t) RTC_adress, temp_buffer, 1, (uint32_t)0xFFFF)!=HAL_OK)
	{	return  HAL_ERROR;	}
	
	return  HAL_OK;
}

//----------------------------------------------------------------------------------------------------------//
uint8_t SetTime (uint8_t RTC_adress, uint8_t registr_adress, char *time)
{
	//������������ ��������� ��� RTC	
	uint8_t I2C_RTC_buffer [4] = {0}; //������� ������
	uint8_t * ptr_RTC_buffer = I2C_RTC_buffer;
	signed char ptr = 1;
	for (signed char i = 0; i < 6; i++) 
	{
		if (!(i%2)) //��� i=0, 2, 4
		{
			I2C_RTC_buffer [ptr] += 10 * (*(time+i) - 48); //������� ������ ���, ������, ����
		}
		else
		{
			I2C_RTC_buffer [ptr] += (*(time+i)) - 48; //��������� ������ ���, ������, ����
			I2C_RTC_buffer [ptr] = RTC_ConvertToBinDec(I2C_RTC_buffer [ptr]); //������� � �������-���������� �������������
			ptr++;
		}
	}
	I2C_RTC_buffer [0] = registr_adress; //� ������ ������� ������� - ����� �������� RTC	
	
	if (HAL_I2C_Master_Transmit(&hi2c1, (uint16_t) RTC_adress, (uint8_t*)ptr_RTC_buffer, 4, (uint32_t)0xFFFF)!= HAL_OK) //�������� ������
	{	return  HAL_ERROR;	}
	
	return  HAL_OK;
}

//--------------------�-� �������� ����� �� �������-����������� �������������  � �������--------------------//
uint8_t RTC_ConvertToDec(uint8_t digit)
{
	uint8_t ch = ((digit>>4)*10 + (0x0F & digit));
	return ch;
}

//--------------------�-� �������� ����� �� �������� � �������-����������� �������������--------------------//
uint8_t RTC_ConvertToBinDec(uint8_t digit)
{
	uint8_t ch = ((digit/10) << 4) + (digit%10);
	return ch;
}


//---------------------------------------------------------------------------------------------------------//
uint8_t read_reg_RTC (uint8_t adress, uint8_t sizebuf)
{
	uint8_t reg_adr = 0x2; //����� �������� RTC 
//	uint8_t sizebuf = 0x3; //���������� ���� ��� ������
	uint8_t I2C_RTC_buffer [sizebuf];
	
	if(HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)adress, &reg_adr, 1, (uint32_t)0xFFFF)!= HAL_OK) //�������� ������ RTC DS3231
  {	return  HAL_ERROR;	}
	
	while(HAL_I2C_GetState(&hi2c1)!=HAL_I2C_STATE_READY){}
		
	if (HAL_I2C_Master_Receive (&hi2c1, (uint16_t) adress, (uint8_t*)I2C_RTC_buffer, (uint16_t) sizebuf, (uint32_t)0xFFFF)!=HAL_OK)
	{	return  HAL_ERROR;	}
	
	for (uint8_t count = 0; count < sizebuf; count++) //������� ����� �� �������-����������� �������������  � �������
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
	
	GetTime (RTC_ADDRESS,  FIRST_RTC_REGISTR_DATE, 3, Date); //������ ��������� 0�4-0�6 (����: ��/��/��)
	file_title = ((pred_sum [0][*(Date+1)-1] )+ *Date);
	
	return file_title;
}

//---------------------------------------------------------------------------------------------------------//
void convert_time (unsigned char time_size, unsigned char * mod_time_data, unsigned char * time_data)
{
		
	for (size_t count = 0; count < time_size; count++)
	{
		*mod_time_data++ = (*(time_data + count) / 10) + 0x30;
		*mod_time_data++ = (*(time_data + count) % 10) + 0x30;
	}
}

//---------------------------------------------------------------------------------------------------------//