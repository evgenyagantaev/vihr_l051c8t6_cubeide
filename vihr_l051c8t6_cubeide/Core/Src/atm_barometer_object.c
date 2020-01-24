#include "main.h"
#include "atm_barometer_object.h"
#include "atm_barometer_interface.h"
#include "rtc_ds3231_interface.h"
#include "at24c32_interface.h"
#include "pressure_sensor_object.h"


void atm_barometer_init()
{
	uint8_t hours, minutes, seconds;
	rtc_ds3231_get_time(&hours, &minutes, &seconds);
	hours_old = hours;
	// read pressure log from eeprom
	int i;

	for(i=0; i<LOG_LENGTH; i++)
	{
		//at24c32_write_32((uint16_t)(i*4), i*10);
		atm_pressure_log[i] = 0;
	}
	for(i=0; i<LOG_LENGTH; i++)
		at24c32_read_32((uint16_t)(i*4), &(atm_pressure_log[i]));
}


void atm_barometer_action()
{
	int i;
	// check if hour_old != hour
	uint8_t hours, minutes, seconds;
	rtc_ds3231_get_time(&hours, &minutes, &seconds);
	//<debug
	//hours = minutes;
	//debug>
	if(hours_old != hours)
	{
		hours_old = hours;

		// shift pressure log array
		for(i=(LOG_LENGTH-1); i>0; i--)
		{
			atm_pressure_log[i] = atm_pressure_log[i-1];
		}
		double P = pressure_sensor_get_pressure();
		atm_pressure_log[0] = (uint32_t)P;
	}

	// save log to eeprom
	for(i=0; i<LOG_LENGTH; i++)
		at24c32_write_32((uint16_t)(i*4), atm_pressure_log[i]);
	// debug
	// control read from eeprom
	for(i=0; i<LOG_LENGTH; i++)
		at24c32_read_32((uint16_t)(i*4), &(atm_pressure_log[i]));
	// debug
}


void atm_barometer_get_history(uint32_t *buffer)
{
	buffer[0] = atm_pressure_log[5];
	buffer[1] = atm_pressure_log[11];
	buffer[2] = atm_pressure_log[23];
	buffer[3] = atm_pressure_log[47];
}


double atm_barometer_get_mean_pressure()
{
	double mean_presssure;
	mean_presssure = atm_pressure_log[0]*0.5 + atm_pressure_log[1]*0.3 + atm_pressure_log[2]*0.2;

	return mean_presssure;
}




