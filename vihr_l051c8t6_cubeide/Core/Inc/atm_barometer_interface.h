#ifndef ATM_BAROMETER_INTERFACE_H
#define ATM_BAROMETER_INTERFACE_H

void atm_barometer_init();

void atm_barometer_action();
void atm_barometer_get_history();
double atm_barometer_get_mean_pressure();

#endif
