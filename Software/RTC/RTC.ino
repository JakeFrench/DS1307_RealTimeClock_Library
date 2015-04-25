/*
 * RTC.ino
 * // Internal vars [ISO format used: 15-03-22T12:26:53Z]
 * char input[] = { '1', '5', '-', '0', '3', '-', '2', '2', 'T', '1', '2', ':', '2', '6', ':', '5', '3', 'Z' };
 *
 * Created: 4/25/2015 9:31:14 PM
 * Author: Dr Jake French
 */ 

// Internal vars [ISO format used: 15-03-22T12:26:53Z]
char input[] = { '1', '5', '-', '0', '3', '-', '2', '2', 'T', '1', '2', ':', '2', '6', ':', '5', '3', 'Z' };


class DateTime{
	public:
		DateTime(uint8_t Address);
		void	setDateTime();
		uint8_t *getDateTime()							{ return &dateTimeBuffer[0]; }
			
	private:
		bool	processState(char const ch);
		void    readDateTime();
		uint8_t bcdToDec(uint8_t const &val)			{ return ((val / 16 * 10) + (val % 16)); }
		uint8_t decToBcd(uint8_t const &val)			{ return ((val / 10 * 16) + (val % 10)); }
		uint8_t DS1307_ADDRESS, index, dateTimeBuffer[7];
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
	CheckSate      = CHECK_YEAR;
	index          = 0;
	// read RTC data from the clock and populate the dateTimeBuffer[] array
	readDateTime();
}

void DateTime::readDateTime(){
	for(uint8_t i=0; i<7; i++)
		dateTimeBuffer[i] = 1;
}

bool DateTime::processState(char const ch){
	// interfacing with RTC over I2C is in this format (reverse ISO order):
	//						 ss(0-59), mm(0-59), hh(0-23), DayOfWeek(not used), DD(1-31), MM(1-12), YY(00-99)
	// dateTimeBuffer[7] = { second,   minute,   hour,           7,              day,      month,    year }
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
					dateTimeBuffer[6] = number; // set year 
					CheckSate  = CHECK_MONTH;
				}
				else
					CheckSate = CHECK_YEAR;
				break;
			case CHECK_MONTH:
				if ((number >= 1) && (number <= 12)){
					dateTimeBuffer[5] = number; // set month
					CheckSate   = CHECK_DAY;
				}
				else
					CheckSate = CHECK_YEAR;
				break;
			case CHECK_DAY:
				if ((number >= 1) && (number <= 31)){
					dateTimeBuffer[4] = number; // set day
					CheckSate = CHECK_HOUR;
				}
				else
					CheckSate = CHECK_YEAR;
				break;
			case CHECK_HOUR:
				if ((number >= 0) && (number <= 23)){
					dateTimeBuffer[2] = number;  // set hour
					CheckSate = CHECK_MINUTE;
				}
				else
					CheckSate = CHECK_YEAR;
				break;
			case CHECK_MINUTE:
				if ((number >= 0) && (number <= 59)){
					dateTimeBuffer[1] = number; // set minute
					CheckSate = CHECK_SECOND;
				}
				else
					CheckSate = CHECK_YEAR;
				break;
			case CHECK_SECOND:
				if ((number >= 0) && (number <= 59)){
					success = true;
					dateTimeBuffer[0]  = number; // set seconds
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
	//Wire.beginTransmission(DS1307_ADDRESS);
	//Wire.write(0x00);								// stop Oscillator
	//for (int i = 0; i < 7; i++){					// send buffer one byte at a time 
	//	Wire.write(decToBcd(dateTimeBuffer[i]));
	//}
	//Wire.write(0x00);								// start Oscillator
	//Wire.endTransmission();
}



DateTime RTC(0x68);

void setup()
{

	  /* add setup code here, setup code runs once when the processor starts */

}

void loop()
{

	  /* add main program code here, this code starts again each time it ends */

}
