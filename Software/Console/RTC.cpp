#include "stdafx.h"
#include "stdlib.h"
#include "stdint.h"
using namespace std;

// Internal vars [ISO format used: 15-03-22T12:26:53Z]
char input[] = { '1', '5', '-', '0', '3', '-', '2', '2', 'T', '1', '2', ':', '2', '6', ':', '5', '3', 'Z' };


class DateTime{
	public:
		DateTime(uint8_t Address);
		bool	processState(char const ch);
		void	setDateTime();
		uint8_t *getDateTime()							{ return &dateTimeBuffer[0]; }
	private:
		uint8_t bcdToDec(uint8_t const &val)			{ return ((val / 16 * 10) + (val % 16)); }
		uint8_t decToBcd(uint8_t const &val)			{ return ((val / 10 * 16) + (val % 10)); }
		uint8_t year, month, day, hour, minute, second;
		uint8_t dateTimeBuffer[7];
		uint8_t index;
		uint8_t DS1307_ADDRESS;
		char chInput[2];
		enum
		{
			CHECK_YEAR,
			CHECK_MONTH,
			CHECK_DAY,
			CHECK_HOUR,
			CHECK_MINUTE,
			CHECK_SECOND,
		} CheckSate;
};


DateTime::DateTime(uint8_t Address){
	// basic configuration, as an emergency use basic[] to set the date & time   
	DS1307_ADDRESS = Address;
	year = month = day = hour = minute = second = 0;
	uint8_t tmp[7] = { year, month, day, 7, hour, minute, second };
	for(uint8_t i=0; i<7; i++)
		dateTimeBuffer[i] = tmp[i];
	index     = 0;
	CheckSate = CHECK_YEAR;
} 


bool DateTime::processState(char const ch){
	bool    success = false;
	uint8_t number  = 0;
	// ensure the char is a number
	if((ch >= '0') && (ch <= '9'))
		chInput[index++] = ch;
	else
		index = 0;
	// now process the chInput buffer
	if (index == 2){
		index = 0;
		number = atoi(chInput);
		switch (CheckSate){
			case CHECK_YEAR:
				if ((number >= 0) && (number <= 99)){
					year = number;
					CheckSate  = CHECK_MONTH;
				}
				else
					CheckSate = CHECK_YEAR;
				break;
			case CHECK_MONTH:
				if ((number >= 1) && (number <= 12)){
					month = number;
					CheckSate   = CHECK_DAY;
				}
				else
					CheckSate = CHECK_YEAR;
				break;
			case CHECK_DAY:
				if ((number >= 1) && (number <= 31)){
					day = number;
					CheckSate = CHECK_HOUR;
				}
				else
					CheckSate = CHECK_YEAR;
				break;
			case CHECK_HOUR:
				if ((number >= 0) && (number <= 23)){
					hour = number;
					CheckSate  = CHECK_MINUTE;
				}
				else
					CheckSate = CHECK_YEAR;
				break;
			case CHECK_MINUTE:
				if ((number >= 0) && (number <= 59)){
					minute = number;
					CheckSate    = CHECK_SECOND;
				}
				else
					CheckSate = CHECK_YEAR;
				break;
			case CHECK_SECOND:
				if ((number >= 0) && (number <= 59)){
					success      = true;
					this->second = number;
				}
				CheckSate = CHECK_YEAR;
				break;
			default:
				CheckSate = CHECK_YEAR;
		}
	}
	return success;
}


void DateTime::setDateTime(){
	// interfacing with RTC over I2C is in this format (reverse ISO order):
	//				 ss(0-59), mm(0-59), hh(0-23), DayOfWeek(not used), DD(1-31), MM(1-12), YY(00-99) 
	uint8_t tmp[] = { second,  minute,   hour,     7,                   day,      month,    year };
	
	//Wire.beginTransmission(DS1307_ADDRESS);
	//Wire.write(0x00);						// stop Oscillator
	for (int i = 0; i < 7; i++){
		dateTimeBuffer[i] = tmp[i];
		//Wire.write(decToBcd(tmp[i]));
	}
	//Wire.write(0x00);                       // start Oscillator
	//Wire.endTransmission();
}




int _tmain(int argc, _TCHAR* argv[])
{
	DateTime RTC(0x68);
	bool test = false;
	for (int i = 0; i < 18; i++){
		if (RTC.processState(input[i]))
			RTC.setDateTime();
	}

	return 0;
}

