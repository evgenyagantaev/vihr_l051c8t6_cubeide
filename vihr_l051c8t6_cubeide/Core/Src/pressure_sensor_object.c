#include "pressure_sensor_object.h"

//****************************************************************************
uint8_t write_byte(uint8_t data)
{

	uint8_t data_out;
    uint8_t read_data;

	// wait for spi transmitter readiness
	while ((SPI1->SR & SPI_SR_TXE) == RESET );
	data_out = data;
    SPI1->DR = data_out;
    // wait while a transmission complete
	while ((SPI1->SR & SPI_SR_RXNE) == RESET );
    read_data = SPI1->DR;
	
	return read_data;

	
}

//****************************************************************************
void pressure_sensor_object_init()
{
	int i;

	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//                 RESET
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	// reset spi1 cs pin
    spi1_cs_pressure_GPIO_Port->BSRR = (uint32_t)(spi1_cs_pressure_Pin << 16); 	// reset
	// transmit 0x1e                             	
	write_byte( 0x1e);                         	
	// set spi1 cs pin                           	
    spi1_cs_pressure_GPIO_Port->BSRR = (uint32_t)spi1_cs_pressure_Pin ;	// set
	HAL_Delay(3);
	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	for(i=1; i<7; i++)
	{
		//send read prom command
		// reset spi1 cs pin
    	spi1_cs_pressure_GPIO_Port->BSRR = (uint32_t)(spi1_cs_pressure_Pin << 16); 	// reset
		// transmit command with address 
		write_byte( 0xa0 + (((uint8_t)i)<<1));

		// read ms byte
		sensor_prom[i] = write_byte(0x55);
		sensor_prom[i] <<= 8;
		// read ls byte
		sensor_prom[i] += write_byte(0x55);

		// set spi1 cs pin
    	spi1_cs_pressure_GPIO_Port->BSRR = (uint32_t)spi1_cs_pressure_Pin ;	// set
	}
}



void pressure_sensor_measure_pressure_temperature()
{	
	uint32_t aux_p = 0;
	int i;
	                                                                                        	
	for(i=0; i<PRESSURE_OVERSAMPLING; i++)                                                  	
    {
	                                                                                        	
		//send start conversion D1 OSR 1024 command                                         	
        // reset spi1 cs pin                                                                	
		spi1_cs_pressure_GPIO_Port->BSRR = (uint32_t)(spi1_cs_pressure_Pin << 16); 	// reset	
	    // transmit command                                                                 	
	    write_byte(0x44);                                                                   	
        // set spi1 cs pin                                                                  	
		spi1_cs_pressure_GPIO_Port->BSRR = (uint32_t)spi1_cs_pressure_Pin ;	// set       	
	    // pause 3 mS                                                                       	
        HAL_Delay(3);                                                                           
	                                                                                        	
	    //send read adc command                                                             	
        // reset spi1 cs pin                                                                	
		spi1_cs_pressure_GPIO_Port->BSRR = (uint32_t)(spi1_cs_pressure_Pin << 16); 	// reset	
	    // transmit command                                                                 	
        write_byte(0x00);                                                                       
	                                                                                        	
	    // read ms byte                                                                     	
	    pressure = write_byte(0x55);                                                        	
	    pressure <<= 8;                                                                     	
	    // read ls byte                                                                     	
	    pressure += write_byte(0x55);                                                       	
	    pressure <<= 8;                                                                     	
	    // read ls byte                                                                     	
	    pressure += write_byte(0x55);                                                       	
        // set spi1 cs pin                                                                  	
    	spi1_cs_pressure_GPIO_Port->BSRR = (uint32_t)spi1_cs_pressure_Pin ;	// set
	                                                                                        	
    	aux_p += pressure;
	                                                                                        	
    }
	                                                                                        	
    pressure = aux_p/PRESSURE_OVERSAMPLING;
	                                                                                        	
	//----------------------------------------------------                                  	
	                                                                                        	
	//send start conversion D2 OSR 1024 command                                             	
    // reset spi1 cs pin                                                                    	
	spi1_cs_pressure_GPIO_Port->BSRR = (uint32_t)(spi1_cs_pressure_Pin << 16); 	// reset 	
	// transmit command                                                                     	
	write_byte(0x54);                                                                       	
    // set spi1 cs pin                                                                      	
	spi1_cs_pressure_GPIO_Port->BSRR = (uint32_t)spi1_cs_pressure_Pin ;	// set           	
	// pause 3 mS                                                                           	
    HAL_Delay(3);
	                                                                                        	
	//send read adc command                                                                 	
    // reset spi1 cs pin                                                                    	
	spi1_cs_pressure_GPIO_Port->BSRR = (uint32_t)(spi1_cs_pressure_Pin << 16); 	// reset 	
	// transmit command                                                                     	
    write_byte(0x00);
	                                                                                        	
	// read ms byte                                                                         	
	temperature = write_byte(0x55);                                                         	
	temperature <<= 8;                                                                      	
	// read ls byte                                                                         	
	temperature += write_byte(0x55);                                                        	
	temperature <<= 8;                                                                      	
	// read ls byte                                                                         	
	temperature += write_byte(0x55);                                                        	
    // set spi1 cs pin                                                                      	
    spi1_cs_pressure_GPIO_Port->BSRR = (uint32_t)spi1_cs_pressure_Pin ;	// set
	                                                                                        	
    //---------------------------------------------------
	                                                                                        	
	dT = (double)temperature - (double)sensor_prom[5]*DEG_2_8;                              	
    actual_temperature = 2000 + (dT*((double)sensor_prom[6]))/DEG_2_23;
	                                                                                        	
	OFF = ((double)sensor_prom[2])*DEG_2_18 + (((double)sensor_prom[4])*dT)/DEG_2_5;        	
    SENS = ((double)sensor_prom[1])*DEG_2_17 + (((double)sensor_prom[3])*dT)/DEG_2_7;
                                                                                            
	                                                                                        	
	double T2;                                                                              	
	double SENS2;                                                                           	
    double OFF2;
                                                                                            
	                                                                                        	
	if(actual_temperature >= 2000)                                                          	
	{                                                                                       	
		T2 = 0;                                                                             	
		SENS2 = 0;                                                                          	
		OFF2 = 0;                                                                           	
	}                                                                                       	
	else                                                                                    	
	{                                                                                       	
		T2 = 3.0 * dT * dT / DEG_2_33;                                                      	
		double aux_dt = (actual_temperature - 2000);                                        	
		OFF2 = 3.0 * aux_dt * aux_dt / 8.0;                                                 	
    	SENS2 = 7.0 * aux_dt * aux_dt / 8.0;
	                                                                                        	
		if(actual_temperature < -1500)                                                      	
		{                                                                                   	
			double aux_dt = actual_temperature + 1500;                                      	
			SENS2 = SENS2 + 3.0 * aux_dt * aux_dt;                                          	
		}                                                                                   	
    }
	                                                                                        	
	actual_temperature = actual_temperature - T2;                                           	
	                                                                                        	
	OFF = OFF - OFF2;                                                                       	
    SENS = SENS - SENS2;
	                                                                                        	
	P = (((double)pressure*SENS)/DEG_2_21 - OFF)/DEG_2_15;                                  


}




double pressure_sensor_get_pressure()
{
	return P;
}

double pressure_sensor_get_temperature()
{
	return actual_temperature;
}
