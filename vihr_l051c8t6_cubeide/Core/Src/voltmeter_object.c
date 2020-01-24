#include "voltmeter_object.h"
#include "adc.h"







void voltmeter_measure_voltage()
{
	int i;

	accu_voltage = 0;

	for(i=0; i<11; i++)
	{
		// measure accu voltage
	    HAL_ADC_Start(&hadc);
	    HAL_ADC_PollForConversion(&hadc, 500);
	    uint32_t adc_voltage =  HAL_ADC_GetValue(&hadc);
	    adc_voltage *= 3;
	    accu_voltage += (double)adc_voltage * voltage_coefficient * 100.0 * 1.0351;
	    
	}

	accu_voltage /= 11;
	    
	double current_voltage;
	if (accu_voltage > UP_BOUND)
	    current_voltage = UP_BOUND;
	else if (accu_voltage < LOW_BOUND)
	    current_voltage = LOW_BOUND;
	else
	    current_voltage = accu_voltage;
	    
	accu_percentage = (current_voltage - LOW_BOUND)/(UP_BOUND - LOW_BOUND)*100.0;
}



double voltmeter_get_voltage()
{
	return accu_voltage;
}



double voltmeter_get_percentage()
{
	return accu_percentage;
}







