#ifndef RTC_DS3231_INTERFACE_H
#define RTC_DS3231_INTERFACE_H	

#include "main.h"

void rtc_ds3231_get_date(uint8_t *Date, uint8_t *Month, uint8_t *Year);
void rtc_ds3231_get_time(uint8_t *Hours, uint8_t *Minutes, uint8_t *Seconds);

void rtc_ds3231_set_date(uint8_t Date, uint8_t Month, uint8_t Year);
void rtc_ds3231_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);

void rtc_ds3231_set_i2c_handle(I2C_HandleTypeDef *handle);

void rtc_ds3231_action();




#endif
