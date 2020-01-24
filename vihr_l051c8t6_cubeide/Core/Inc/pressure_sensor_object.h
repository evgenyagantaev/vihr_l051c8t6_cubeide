#ifndef PRESSURE_SENSOR_OBJECT_H
#define PRESSURE_SENSOR_OBJECT_H

#include "main.h"


#define DEG_2_8 256.0
#define DEG_2_23 8388608.0
#define DEG_2_18 262144.0
#define DEG_2_5 32.0
#define DEG_2_17 131072.0
#define DEG_2_7 128.0
#define DEG_2_21 2097152.0
#define DEG_2_15 32768.0
#define DEG_2_33 8589934592.0

#define PRESSURE_OVERSAMPLING 100

static uint32_t pressure;
static uint32_t temperature;
static double dT;
static double actual_temperature;
static double OFF;
static double SENS;
static double P;


static uint16_t sensor_prom[7];


void pressure_sensor_object_init();
uint8_t write_byte(uint8_t data);
void pressure_sensor_measure_pressure_temperature();
double pressure_sensor_get_pressure();
double pressure_sensor_get_temperature();



#endif
