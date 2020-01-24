#ifndef VOLTMETER_OBJECT_H
#define VOLTMETER_OBJECT_H

#include "main.h"

#define LOW_BOUND 660
#define UP_BOUND 820

static double voltage_coefficient = 3.3/4096.0;
static double accu_voltage;
static double accu_percentage;


void voltmeter_measure_voltage();
double voltmeter_get_voltage();
double voltmeter_get_percentage();



#endif
