#ifndef RTC_DS3231_OBJECT_H
#define RTC_DS3231_OBJECT_H	

#include "main.h"

#define DS3231_BUFFER_LENGTH 7
#define DS3231_ADDRESS 0x68

static uint8_t ds3231_buffer[DS3231_BUFFER_LENGTH];

static I2C_HandleTypeDef *ds3231_i2c_handle;

static uint16_t ds3231_shifted_address = DS3231_ADDRESS << 1;

static uint8_t year, month, date, hours, minutes, seconds;
#endif
