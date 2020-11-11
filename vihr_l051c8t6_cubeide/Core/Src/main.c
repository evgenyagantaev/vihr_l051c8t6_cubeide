// перенести шапку (сразу после выбора активного лога)
// посчитать время максимальной глубины, с учетом пауз на поверхности
// вычислить поясное время максимальной глубины
// В конце лога выводить общее время погружения без учета пауз на поверхности




/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "math.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"
#include "i2c.h"
#include "spi.h"
#include "adc.h"

#include "ssd1306.h"

#include "one_second_timer_interface.h"
#include "pressure_sensor_object.h"
#include "depth_switch_interface.h"
#include "rtc_ds3231_interface.h"
#include "voltmeter_object.h"
#include "at24c32_interface.h"



static char message[64];
static char timestamp[64];
static char temperature_message[64];


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
//uint8_t primitive_delay()
//{
	//uint32_t volatile i;
	//for(i=0; i<300000; i++);

	//return 0;
//}
/* Private function prototypes -----------------------------------------------*/




//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int main(void)
{

	//int i;



  	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  	HAL_Init();

  	/* Configure the system clock */
  	SystemClock_Config();

    MX_GPIO_Init();
    MX_USART1_UART_Init();
  	MX_I2C1_Init();
  	MX_I2C2_Init();
  	//MX_I2C3_Init();


    //--------init display1------------------------------
	//*
    ssd1306_set_i2c_port(&hi2c1, 1);
  	ssd1306_Init();
  	HAL_Delay(100);
  	ssd1306_Fill(White);
  	ssd1306_UpdateScreen();
  	HAL_Delay(100);
  	ssd1306_Fill(Black);
  	ssd1306_UpdateScreen();

  	HAL_Delay(100);

  	ssd1306_SetCursor(0,0);
  	ssd1306_WriteString("DiveCmp", Font_16x26, White);
  	ssd1306_SetCursor(0,30);
  	ssd1306_WriteString("Start..", Font_16x26, White);
  	ssd1306_UpdateScreen();
  	HAL_Delay(2000);
	//*/
    ssd1306_set_i2c_port(&hi2c1, 1);
  	ssd1306_Init();
  	HAL_Delay(100);
 
 
 	MX_SPI1_Init();
    // enable spi1
    SPI1->CR1 |= SPI_CR1_SPE;
    MX_ADC_Init();

	//---------------------------------
  	//HAL_Delay(100);
	rtc_ds3231_set_i2c_handle(&hi2c2);
	//rtc_ds3231_set_time(16, 17, 0);
	//rtc_ds3231_set_date(5, 11, 20);
	at24c32_set_i2c_handle(&hi2c2);

	one_second_timer_init();
	one_second_timer_start();

	pressure_sensor_object_init();
	HAL_Delay(1000);
	uint32_t surface_pressure = 101325;
	uint32_t P_sym = 101325;
	depth_switch_turn_signal_led(1);

	rtc_ds3231_action();
	//atm_barometer_init();
	int odd_even = 0;
	//int led_counter = 0;

	//int mem_test_base = 0;

	uint8_t at24c32_shifted_address = 0x50 << 1;
	//uint16_t eeprom_address = 64;
	uint16_t eeprom_debug_address = 64;
	int eeprom_number_of_records = 0;
	uint32_t log_counter = 0;

	uint8_t b0;
	int write_delay = 5;
	static I2C_HandleTypeDef *at24c32_i2c_handle = &hi2c2;


	int actuator_counter = 0;

	uint32_t surface_delay_counter = 0;
	int surface_delay_count_flag = 0;

	int debug_state = 0;
	int debug_state_counter = 0;

	if(!depth_switch_check_gpio())
	{
		double max_depth = 0;
		int max_depth_temperature = 0;
		uint32_t max_depth_time = 0;

		// log mode
  	    ssd1306_SetCursor(0,0);
	    sprintf(message, "rezhim loga");
  	    ssd1306_WriteString(message, Font_11x18, White);
  	    ssd1306_SetCursor(0,22);
  	  	sprintf(message, "otpustite  ");
  	    ssd1306_WriteString(message, Font_11x18, White);
  	    ssd1306_SetCursor(0,44);
  	    sprintf(message, "knopku     ");
  	    ssd1306_WriteString(message, Font_11x18, White);
  	    ssd1306_UpdateScreen();                                                                               

	    HAL_Delay(2000);

	    // wait while button is pressed
	    while(!depth_switch_check_gpio());

	    // button released
	    ssd1306_SetCursor(0,0);
		sprintf(message, "log gotov  ");
		ssd1306_WriteString(message, Font_11x18, White);
		ssd1306_SetCursor(0,22);
		sprintf(message, "nazhmite   ");
		ssd1306_WriteString(message, Font_11x18, White);
		ssd1306_SetCursor(0,44);
		sprintf(message, "knopku     ");
		ssd1306_UpdateScreen();

		// wait until button is pressed again
		while(depth_switch_check_gpio());
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); //on
		HAL_Delay(30);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET); //off

		ssd1306_Fill(Black);
  	    ssd1306_SetCursor(0,0);
	    sprintf(message, "zagruzka...");
  	    ssd1306_WriteString(message, Font_11x18, White);
  	    ssd1306_UpdateScreen();                                                                               

//*
		int end_of_log_reached = 0;
		HAL_Delay(1000);
		uint16_t eeprom_debug_address = 64;
		uint8_t at24c32_shifted_address = 0x50 << 1;
		static I2C_HandleTypeDef *at24c32_i2c_handle = &hi2c2;

		sprintf(message, "\r\n");
		HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
		HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
		HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);

		sprintf(message, "log bank 1 start\r\n***********\r\n");
		HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
	
		// read timestamp time
		char log1_time_txt[5];

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);		
		message[0] = b0;
		eeprom_debug_address++;

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[1] = b0;
		eeprom_debug_address++;

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[2] = b0;
		eeprom_debug_address++;

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[3] = b0;
		eeprom_debug_address++;

		if((message[0] == 0) && (message[1] == 0))
		{
			end_of_log_reached = 1;
		}
		else
		{
			strncpy(log1_time_txt, message, 4);

			message[4] = '\r';
			message[5] = '\n';
			message[6] = 0;
			HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
		}
		// read timestamp date
		char log1_date_txt[5];

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);		
		message[0] = b0;
		eeprom_debug_address++;

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[1] = b0;
		eeprom_debug_address++;

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[2] = b0;
		eeprom_debug_address++;

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[3] = b0;
		eeprom_debug_address++;

		if((message[0] == 0) && (message[1] == 0))
		{
			end_of_log_reached = 1;
		}
		else
		{
			strncpy(log1_date_txt, message, 4);

			message[4] = '\r';
			message[5] = '\n';
			message[6] = 0;
			HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
		}
		
		// read emergency depth
		char log1_emerg_depth_txt[3];

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[0] = b0;
		eeprom_debug_address++;

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[1] = b0;
		eeprom_debug_address++;

		if((message[0] == 0) && (message[1] == 0))
		{
			end_of_log_reached = 1;
		}
		else
		{
			strncpy(log1_emerg_depth_txt, message, 2);
			log1_emerg_depth_txt[2] = 0;

			message[2] = '\r';
			message[3] = '\n';
			message[4] = 0;
			HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
		}



		int seconds_counter = 0;
		int dive_hours = 0;
		int dive_minutes = 0;
		int dive_seconds = 0;


		while(!end_of_log_reached)
		{

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[0] = b0;
			eeprom_debug_address++;
			HAL_Delay(3);

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[1] = b0;
			eeprom_debug_address++;

			message[2] = '.';

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[3] = b0;
			eeprom_debug_address++;

			message[4] = ' ';

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[5] = b0;
			eeprom_debug_address++;

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[6] = b0;
			eeprom_debug_address++;

			if((message[0] == 0) && (message[1] == 0))
			{
				end_of_log_reached = 1;
			}
			else if(message[0] == 'd')
			{
				// read surface delay value
				uint32_t surface_delay = 0;
				surface_delay += (uint32_t)(message[1]);
				surface_delay += (uint32_t)(((uint32_t)(message[3]))<<8);
				surface_delay += (uint32_t)(((uint32_t)(message[5]))<<16);
				surface_delay += (uint32_t)(((uint32_t)(message[6]))<<24);

				sprintf(message, "pauza %d sekund\r\n", surface_delay);
				HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
			}
			else
			{

				message[7] = '\r';
				message[8] = '\n';
				message[9] = 0;
				HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
			}
		}  // while(!end_of_log_reached)

		sprintf(message, "\r\n");
		HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
		sprintf(message, "**********\r\nlog bank 1 finish\r\n");
		HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);


		eeprom_debug_address = 64;
		at24c32_shifted_address = 0x51 << 1;

		sprintf(message, "\r\n");
		HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
		HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
		HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);

		sprintf(message, "log bank 2 start\r\n***********\r\n");
		HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);

		end_of_log_reached = 0;

	
		// read timestamp time
		char log2_time_txt[5];

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);		
		message[0] = b0;
		eeprom_debug_address++;

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[1] = b0;
		eeprom_debug_address++;

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[2] = b0;
		eeprom_debug_address++;

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[3] = b0;
		eeprom_debug_address++;

		if((message[0] == 0) && (message[1] == 0))
		{
			end_of_log_reached = 1;
		}
		else
		{
			strncpy(log2_time_txt, message, 4);

			message[4] = '\r';
			message[5] = '\n';
			message[6] = 0;
			HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
		}
		// read timestamp date
		char log2_date_txt[5];

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);		
		message[0] = b0;
		eeprom_debug_address++;

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[1] = b0;
		eeprom_debug_address++;

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[2] = b0;
		eeprom_debug_address++;

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[3] = b0;
		eeprom_debug_address++;

		if((message[0] == 0) && (message[1] == 0))
		{
			end_of_log_reached = 1;
		}
		else
		{
			strncpy(log2_date_txt, message, 4);

			message[4] = '\r';
			message[5] = '\n';
			message[6] = 0;
			HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
		}

		// read emergency depth
		char log2_emerg_depth_txt[3];

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[0] = b0;
		eeprom_debug_address++;

		HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
		message[1] = b0;
		eeprom_debug_address++;

		if((message[0] == 0) && (message[1] == 0))
		{
			end_of_log_reached = 1;
		}
		else
		{
			strncpy(log2_emerg_depth_txt, message, 2);
			log2_emerg_depth_txt[2] = 0;

			message[2] = '\r';
			message[3] = '\n';
			message[4] = 0;
			HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
		}



		while(!end_of_log_reached)
		{

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[0] = b0;
			eeprom_debug_address++;
			HAL_Delay(3);

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[1] = b0;
			eeprom_debug_address++;

			message[2] = '.';

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[3] = b0;
			eeprom_debug_address++;

			message[4] = ' ';

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[5] = b0;
			eeprom_debug_address++;

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[6] = b0;
			eeprom_debug_address++;

			if((message[0] == 0) && (message[1] == 0))
			{
				end_of_log_reached = 1;
			}
			else if(message[0] == 'd')
			{
				// read surface delay value
				uint32_t surface_delay = 0;
				surface_delay += (uint32_t)(message[1]);
				surface_delay += (uint32_t)(((uint32_t)(message[3]))<<8);
				surface_delay += (uint32_t)(((uint32_t)(message[5]))<<16);
				surface_delay += (uint32_t)(((uint32_t)(message[6]))<<24);

				sprintf(message, "pauza %d sekund\r\n", surface_delay);

				HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
			}
			else
			{
				// debug output on lcd
				//message[7] = 0;
				//ssd1306_SetCursor(0,22);
				//ssd1306_WriteString(message, Font_11x18, White);
				//ssd1306_UpdateScreen();
				//HAL_Delay(700);

				message[7] = '\r';
				message[8] = '\n';
				message[9] = 0;
				HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
			}
		}

		sprintf(message, "\r\n");
		HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
		sprintf(message, "**********\r\nlog bank 2 finish\r\n");
		HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);


		// loop menyu vybora loga
		int log_chosen = 0;
		int active_log = 0;

		while(!log_chosen)
		{
			if(active_log == 0)
			{
				// vyvod menyu vybora odnogo iz dvuh logov
				ssd1306_SetCursor(0,0);
				sprintf(message, "%c%c.%c%c %c%c:%c%c", log1_date_txt[0], log1_date_txt[1], log1_date_txt[2], log1_date_txt[3],
														log1_time_txt[0], log1_time_txt[1], log1_time_txt[2], log1_time_txt[3]);
				ssd1306_WriteString(message, Font_11x18, White);
				ssd1306_SetCursor(0,44);
				sprintf(message, "%c%c.%c%c %c%c:%c%c", log2_date_txt[0], log2_date_txt[1], log2_date_txt[2], log2_date_txt[3],
														log2_time_txt[0], log2_time_txt[1], log2_time_txt[2], log2_time_txt[3]);
				ssd1306_WriteString(message, Font_11x18, White);
				ssd1306_SetCursor(0,19);
				sprintf(message, "^^^^^^^^^^^");
				ssd1306_WriteString(message, Font_11x18, White);
				ssd1306_UpdateScreen();
			}
			else
			{
				// vyvod menyu vybora odnogo iz dvuh logov
				ssd1306_SetCursor(0,0);
				sprintf(message, "%c%c.%c%c %c%c:%c%c", log2_date_txt[0], log2_date_txt[1], log2_date_txt[2], log2_date_txt[3],
														log2_time_txt[0], log2_time_txt[1], log2_time_txt[2], log2_time_txt[3]);

				ssd1306_WriteString(message, Font_11x18, White);
				ssd1306_SetCursor(0,44);
				sprintf(message, "%c%c.%c%c %c%c:%c%c", log1_date_txt[0], log1_date_txt[1], log1_date_txt[2], log1_date_txt[3],
														log1_time_txt[0], log1_time_txt[1], log1_time_txt[2], log1_time_txt[3]);
				ssd1306_WriteString(message, Font_11x18, White);
				ssd1306_SetCursor(0,19);
				sprintf(message, "^^^^^^^^^^^");
				ssd1306_WriteString(message, Font_11x18, White);
				ssd1306_UpdateScreen();
			}

			if(!depth_switch_check_gpio())
			{
				int start_counter = one_second_timer_get_counter();
				HAL_Delay(1100);
				// short blink
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); //on
				HAL_Delay(30);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET); //off

				int long_blinked = 0;

				while(!depth_switch_check_gpio())
				{
					if(((one_second_timer_get_counter() - start_counter) > 2) && (!long_blinked))
					{
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); //on
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET); //on
						HAL_Delay(30);
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET); //off
						HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET); //off

						long_blinked = 1;
					}
				}
				if((one_second_timer_get_counter() - start_counter) <= 2)
					active_log = (active_log + 1)%2;
				else
					log_chosen = 1;
			}
		}


		ssd1306_SetCursor(0,19);
		sprintf(message, "           ");
		ssd1306_WriteString(message, Font_11x18, White);
		ssd1306_UpdateScreen();


		ssd1306_SetCursor(0,26);
		sprintf(message, "ok nazhmite");
		ssd1306_WriteString(message, Font_11x18, White);
		ssd1306_SetCursor(0,44);
		sprintf(message, "           ");
		ssd1306_WriteString(message, Font_11x18, White);
		ssd1306_SetCursor(0,44);
		sprintf(message, "knopku     ");
		ssd1306_WriteString(message, Font_11x18, White);
		ssd1306_UpdateScreen();

		while(depth_switch_check_gpio());

		int start_counter = one_second_timer_get_counter();
		HAL_Delay(330);

		int short_blinked = 0;
		while(!depth_switch_check_gpio())
		{
			if(((one_second_timer_get_counter() - start_counter) > 2) && (!short_blinked))
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); //on
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET); //on
				HAL_Delay(30);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET); //off
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET); //off

				short_blinked = 1;
			}
		}

		eeprom_debug_address = 64 + 10;
		if(active_log == 0)
			at24c32_shifted_address = 0x50 << 1;
		else
			at24c32_shifted_address = 0x51 << 1;
		end_of_log_reached = 0;

		while(!end_of_log_reached)
		{

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[0] = b0;
			eeprom_debug_address++;

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[1] = b0;
			eeprom_debug_address++;

			message[2] = '.';

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[3] = b0;
			eeprom_debug_address++;

			message[4] = ' ';

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[5] = b0;
			eeprom_debug_address++;

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[6] = b0;
			eeprom_debug_address++;

			if((message[0] == 0) && (message[1] == 0))
			{
				end_of_log_reached = 1;
			}
			else if(message[0] == 'd')
			{
				// read surface delay value
				uint32_t surface_delay = 0;
				surface_delay += (uint32_t)(message[1]);
				surface_delay += (uint32_t)(((uint32_t)(message[3]))<<8);
				surface_delay += (uint32_t)(((uint32_t)(message[5]))<<16);
				surface_delay += (uint32_t)(((uint32_t)(message[6]))<<24);

				// add surface delay value to seconds counter
				seconds_counter += surface_delay;
			}
			else // regular record of depth and temperature
			{

				char aux_message[16];
				int depth_int, depth_fract, temp;
				// prochitat' glubinu i temperaturu
				sscanf(message, "%02d.%01d%02d", &depth_int, &depth_fract, &temp);
				double dive_depth = depth_int + (double)(((double)depth_fract)/10.0);
				if(max_depth < dive_depth)
				{
					max_depth = dive_depth;
					max_depth_temperature = temp;
					max_depth_time = seconds_counter;
				}

				//if((seconds_counter % 1) == 0)
				if(0)
				{
					// debug output on lcd
					//**************************************************************************
					ssd1306_SetCursor(0,0);
					sprintf(aux_message, "gl %02d.%01d m  ", depth_int, depth_fract);
					ssd1306_WriteString(aux_message, Font_11x18, White);
					ssd1306_SetCursor(0,22);
					dive_minutes = seconds_counter/60;
					dive_seconds = seconds_counter % 60;
					dive_hours = dive_minutes/60;
					dive_minutes = dive_minutes % 60;
					sprintf(aux_message, "%02dh %02d'%02d''", dive_hours, dive_minutes, dive_seconds);
					ssd1306_WriteString(aux_message, Font_11x18, White);
					ssd1306_SetCursor(0,44);
					sprintf(aux_message, "T %+02d C    ", temp);
					ssd1306_WriteString(aux_message, Font_11x18, White);
					ssd1306_UpdateScreen();
					HAL_Delay(700);
				}

				seconds_counter++;

				//*/
				//********************************************************************************
				// debug output on lcd

				// debug!!!!!
				//message[7] = '\r';
				//message[8] = '\n';
				//message[9] = 0;
				//HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
			}
		}  // while(!end_of_log_reached)

		ssd1306_SetCursor(0,0);
		sprintf(message, "Max %02d.%01d m ", (int)max_depth, (int)((max_depth - (int)max_depth)*10));
		ssd1306_WriteString(message, Font_11x18, White);
		ssd1306_SetCursor(0,22);
		dive_minutes = max_depth_time/60;
		dive_seconds = max_depth_time % 60;
		dive_hours = dive_minutes/60;
		dive_minutes = dive_minutes % 60;
		sprintf(message, "%02dh %02d'%02d''", dive_hours, dive_minutes, dive_seconds);
		ssd1306_WriteString(message, Font_11x18, White);
		ssd1306_SetCursor(0,44);
		//sprintf(message, "Min T %+02d C", max_depth_temperature);
		if(active_log == 0)
			sprintf(message, "Avar gl %2sm", log1_emerg_depth_txt);
		else
			sprintf(message, "Avar gl %2sm", log2_emerg_depth_txt);
		ssd1306_WriteString(message, Font_11x18, White);
		ssd1306_UpdateScreen();

		eeprom_debug_address = 64 + 10;
		seconds_counter = 0;
		end_of_log_reached = 0;

		// ozhidaniye nazhatiya knopki
		while(depth_switch_check_gpio());
		short_blinked = 0;
		while(!depth_switch_check_gpio())
		{
			if(((one_second_timer_get_counter() - start_counter) > 1) && (!short_blinked))
			{
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); //on
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET); //on
				HAL_Delay(30);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET); //off
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET); //off

				short_blinked = 1;
			}
		}

		//*****************************************************
		//vyvod loga pominutno s ozhidaniem nazhatiya knopki
		//*****************************************************
		//uint8_t log_period = 60;
		uint8_t log_period = 1;
		while(!end_of_log_reached)
		{

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[0] = b0;
			eeprom_debug_address++;

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[1] = b0;
			eeprom_debug_address++;

			message[2] = '.';

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[3] = b0;
			eeprom_debug_address++;

			message[4] = ' ';

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[5] = b0;
			eeprom_debug_address++;

			HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
			message[6] = b0;
			eeprom_debug_address++;

			if((message[0] == 0) && (message[1] == 0))
			{
				end_of_log_reached = 1;
			}
			else if(message[0] == 'd')
			{
				// read surface delay value
				uint32_t surface_delay = 0;
				surface_delay += (uint32_t)(message[1]);
				surface_delay += (uint32_t)(((uint32_t)(message[3]))<<8);
				surface_delay += (uint32_t)(((uint32_t)(message[5]))<<16);
				surface_delay += (uint32_t)(((uint32_t)(message[6]))<<24);

				// add surface delay value to seconds counter
				seconds_counter += surface_delay;
			}
			else if(message[0] == '$')   // mark of actuators activation
			{
				char aux_message[16];
				// wait for button press
				while(depth_switch_check_gpio())
				{
					// debug output on lcd
					//**************************************************************************
					ssd1306_SetCursor(0,44);
					sprintf(aux_message, ">>>>>>>>>>>");
					ssd1306_WriteString(aux_message, Font_11x18, White);
					ssd1306_UpdateScreen();
					HAL_Delay(300);
					ssd1306_SetCursor(0,44);
					sprintf(aux_message, "-----------");
					ssd1306_WriteString(aux_message, Font_11x18, White);
					ssd1306_UpdateScreen();
					HAL_Delay(300);
				}
				// short blink
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET); //on
				HAL_Delay(30);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET); //off

			}
			else // regular record of depth and temperature
			{

				char aux_message[16];
				int depth_int, depth_fract, temp;
				// prochitat' glubinu i temperaturu
				sscanf(message, "%02d.%01d%02d", &depth_int, &depth_fract, &temp);
				double dive_depth = depth_int + (double)(((double)depth_fract)/10.0);
				if(max_depth < dive_depth)
				{
					max_depth = dive_depth;
					max_depth_temperature = temp;
					max_depth_time = seconds_counter;
				}

				if((seconds_counter % log_period) == 0)
				{
					// debug output on lcd
					//**************************************************************************
					ssd1306_SetCursor(0,0);
					sprintf(aux_message, "gl %02d.%01d m  ", depth_int, depth_fract);
					ssd1306_WriteString(aux_message, Font_11x18, White);
					ssd1306_SetCursor(0,22);
					dive_minutes = seconds_counter/60;
					dive_seconds = seconds_counter % 60;
					dive_hours = dive_minutes/60;
					dive_minutes = dive_minutes % 60;
					sprintf(aux_message, "%02dh %02d'%02d''", dive_hours, dive_minutes, dive_seconds);
					ssd1306_WriteString(aux_message, Font_11x18, White);
					ssd1306_SetCursor(0,44);
					sprintf(aux_message, "T %+02d C    ", temp);
					ssd1306_WriteString(aux_message, Font_11x18, White);
					ssd1306_UpdateScreen();
					HAL_Delay(300);
				}

				seconds_counter++;

				// proverka nazhatiya knopki
				if(!depth_switch_check_gpio())
				{
					// ostanovka
					HAL_Delay(330);
					// ozhidaniye otpuskaniya knopki
					while(!depth_switch_check_gpio());

					// ozhidaniye nazhatiya knopki
					while(depth_switch_check_gpio());
					HAL_Delay(330);
					int button_press_counter = 0;
					while(!depth_switch_check_gpio())
					{
						button_press_counter++;
						HAL_Delay(1000);
					}

					if(button_press_counter > 4) // dlitelnoe nazhatie
					{
						// menyaem period
						if(log_period == 60)
							log_period = 1;
						else
							log_period = 60;
					}

				}// end if(!depth_switch_check_gpio())

			}// end if((message[0] == 0) && (message[1] == 0))

		}// end while(!end_of_log_reached)

		ssd1306_SetCursor(0,0);
		sprintf(message, "Max %02d.%01d m ", (int)max_depth, (int)((max_depth - (int)max_depth)*10));
		ssd1306_WriteString(message, Font_11x18, White);
		ssd1306_SetCursor(0,22);
		dive_minutes = max_depth_time/60;
		dive_seconds = max_depth_time % 60;
		dive_hours = dive_minutes/60;
		dive_minutes = dive_minutes % 60;
		sprintf(message, "%02dh %02d'%02d''", dive_hours, dive_minutes, dive_seconds);
		ssd1306_WriteString(message, Font_11x18, White);
		ssd1306_SetCursor(0,44);
		//sprintf(message, "Min T %+02d C", max_depth_temperature);
		if(active_log == 0)
			sprintf(message, "Avar gl %2sm", log1_emerg_depth_txt);
		else
			sprintf(message, "Avar gl %2sm", log2_emerg_depth_txt);

		ssd1306_WriteString(message, Font_11x18, White);
		ssd1306_UpdateScreen();


		while(1)
		{
		}

	//*/
	}
	else	// if(!depth_switch_check_gpio())
	{
		while(1)                                                                                                         
	    {
                                                                                                                                                                      
                                                                                                                                                                      
	    	if(one_second_timer_get_flag())
	    	{
	    		one_second_timer_reset_flag();
	    		odd_even = (odd_even+1)%2;
  	    
	    		pressure_sensor_measure_pressure_temperature();                                                                                                   	
	    	    double P = pressure_sensor_get_pressure();
	    	    double actual_temperature = pressure_sensor_get_temperature();
                                                                                                                                                                  
	    	    voltmeter_measure_voltage();
	    	    double accu_voltage = voltmeter_get_voltage();
	    	    double accu_percentage = voltmeter_get_percentage();
	                                                                                                                                                              
	    		rtc_ds3231_action();
	    		// time-date calculation ----------------------------------------
	    		uint8_t seconds, minutes, hours;
	    		rtc_ds3231_get_time(&hours, &minutes, &seconds);
	    		uint8_t date, month, year;
	    		rtc_ds3231_get_date(&date, &month, &year);
	    		//--------------------------------------------------------------
              
	    		if(P <= surface_pressure)
	    			surface_pressure = P;

                                                                                                                                                                      
	    		// debug!!!
	    	  	//P_sym += 980;
	    		//P = P_sym;
	    		// debug!!!debug!!!
	    		/*
	    		if(debug_state == 0)
	    		{
	    			// pauza pered pervym pogruzheniem
	    			P_sym = surface_pressure;
	    			debug_state_counter++;
	    			if(debug_state_counter >= 60)
	    			{
	    				debug_state = 1;
	    				debug_state_counter = 0;
	    			}
	    		}
	    		else if(debug_state == 1)
	    		{
	    			// pervoe pogruzhenie 15 metrov
	    			P_sym += 980;
	    			P = P_sym;
	    			debug_state_counter++;
	    			if(debug_state_counter >= 150)
					{
						debug_state = 2;
						debug_state_counter = 0;
					}
	    		}
	    		else if(debug_state == 2)
				{
					// vsplytie posle pervogo pogruzheniya
					P_sym -= 980;
					P = P_sym;
					if(P <= surface_pressure)
					{
						P_sym = surface_pressure;
						P = surface_pressure;
						debug_state = 3;
						debug_state_counter = 0;
					}
				}
	    		if(debug_state == 3)
				{
					// pauza mezhdu pervym i vtorym pogruzheniem
					debug_state_counter++;
					if(debug_state_counter >= 60)
					{
						debug_state = 4;
						debug_state_counter = 0;
					}
				}
	    		else if(debug_state == 4)
				{
					// vtoroye pogruzhenie 17 metrov
					P_sym += 980;
					P = P_sym;
					debug_state_counter++;
					if(debug_state_counter >= 170)
					{
						debug_state = 5;
						debug_state_counter = 0;
					}
				}
	    		else if(debug_state == 5)
				{
					// vsplytie posle vtorogo pogruzheniya
					P_sym -= 980;
					P = P_sym;
					if(P <= surface_pressure)
					{
						P_sym = surface_pressure;
						P = surface_pressure;
						debug_state = 6;
						debug_state_counter = 0;
					}
				}
                //*/
                                                                                                                                                                      

                                                                                                                                                                      
	    		int we_are_under_water = 0;
                                                                                                                                                                      
	    		if(P > (surface_pressure + 9800)) // underwater
	    			we_are_under_water = 1;
                                                                                                                                                                      
	    		if(!we_are_under_water)  // we are not under water
	    		{
	    			if(surface_delay_count_flag)
	    				surface_delay_counter++;

	    			depth_switch_action();		    
                                                                                                                                                                      
	    			
	    			if(actuator_counter == 0)
	    			{
	    				//ssd1306_Fill(Black);                                                                                         
  	    	        	ssd1306_SetCursor(0,0);
	    	        	//sprintf(timestamp, "%02d:%02d %02d.%02d", hours, minutes, date, month);
	    				if(odd_even)
	    	        		sprintf(timestamp, "%02d:%02d %02d.%02d", hours, minutes, date, month);
	    				else
	    	        		sprintf(timestamp, "%02d %02d %02d %02d", hours, minutes, date, month);
  	    	        	ssd1306_WriteString(timestamp, Font_11x18, White);
  	    	        	ssd1306_SetCursor(0,22);
	    	        	sprintf(message, "AVAR GL %02dm", (int)depth_switch_get_current_depth());
	    	        	//sprintf(message, "%d", (int)actual_temperature);
  	    	        	ssd1306_WriteString(message, Font_11x18, White);
  	    	        	ssd1306_SetCursor(0,44);
	    	        	sprintf(message, "akkum %02d%%", (int)accu_percentage);
	    	        	//sprintf(message, "%d     ", debug_state_counter);
  	    	        	ssd1306_WriteString(message, Font_11x18, White);
  	    	        	ssd1306_UpdateScreen();

	    				/*
	    				if(odd_even)
	    	        		sprintf(timestamp, "%02d:%02d:%02d %02d.%02d\r\n", hours, minutes, seconds, date, month);
	    				else
	    	        		sprintf(timestamp, "%02d %02d %02d %02d.%02d\r\n", hours, minutes, seconds, date, month);
	    				HAL_UART_Transmit(&huart1, (uint8_t *)timestamp, strlen((const char *)timestamp), 500);
	    	        	sprintf(message, "AVAR GL %02dm\r\n", (int)depth_switch_get_current_depth());
	    				HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
	    	        	sprintf(message, "akkum %02d%%\r\n", (int)accu_percentage);
	    				HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
	    				*/
	    			}
                                                                                                                                                                      
	    			
                                                                                                                                                                      
	    		}
	    		else // we are under water
	    		{
        //*
	    			uint16_t data;
                                                                                                                                                                      
	    			log_counter++;

	    			surface_delay_count_flag = 1;
	    			if(surface_delay_counter != 0)
	    			{

	    				// write surface delay value in memory
	    				message[0] = 'd';
	    				message[1] = (uint8_t)surface_delay_counter;
	    				message[2] = (uint8_t)(surface_delay_counter >> 8);
	    				message[3] = (uint8_t)(surface_delay_counter >> 16);
	    				message[4] = (uint8_t)(surface_delay_counter >> 24);
	    				// write surface delay record
						b0 = 0;
						HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address + 5, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
						HAL_Delay(write_delay);
						HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address + 6, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
						HAL_Delay(write_delay);
						eeprom_number_of_records++;
						b0 = message[0];
						HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
						HAL_Delay(write_delay);
						eeprom_debug_address++;
						b0 = message[1];
						HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
						HAL_Delay(write_delay);
						eeprom_debug_address++;
						b0 = message[2];
						HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
						HAL_Delay(write_delay);
						eeprom_debug_address++;
						b0 = message[3];
						HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
						HAL_Delay(write_delay);
						eeprom_debug_address++;
						b0 = message[4];
						HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
						HAL_Delay(write_delay);
						eeprom_debug_address++;

	    				surface_delay_counter = 0;
	    			}

                                                                                                                                                                      
	    			// calculate depth
	    			double depth = ((double)(P - surface_pressure))/9800.0;
	    			/*
	    			if(depth > 1.0)
	    				depth -= 1.0;
	    			else
	    				depth = 0.0;
	    			//*/

	    			if(depth < 0.7)
	    				depth = 0.0;
                                                                                                                                                                      
	    			if(actuator_counter == 0)
	    			{
  	    	        	ssd1306_SetCursor(0,0);
	    	        	//sprintf(timestamp, "%02d:%02d %02d.%02d", hours, minutes, date, month);
	    				if(odd_even)
	    	        		sprintf(timestamp, "%02d:%02d %02d.%02d", hours, minutes, date, month);
	    				else
	    	        		sprintf(timestamp, "%02d %02d %02d %02d", hours, minutes, date, month);
	    	        	//sprintf(timestamp, "timestamp");
  	    	        	ssd1306_WriteString(timestamp, Font_11x18, White);
  	    	        	ssd1306_SetCursor(0,22);
	    	        	//sprintf(message, "glubina %02dm", (int)depth);
	    	        	sprintf(message, "gl--> %02d.%01dm", (int)depth, (int)((depth - (int)depth)*10.0));
  	    	        	ssd1306_WriteString(message, Font_11x18, White);
  	    	        	ssd1306_SetCursor(0,44);
	    	        	sprintf(message, "akkum %02d%%", (int)accu_percentage);
	    	        	//sprintf(message, "%d     ", debug_state_counter);
  	    	        	ssd1306_WriteString(message, Font_11x18, White);
  	    	        	ssd1306_UpdateScreen();  
                                                                                                                                                                      
	    				/*
	    	        	sprintf(timestamp, "%02d:%02d:%02d %02d.%02d\r\n", hours, minutes, seconds, date, month);
	    				HAL_UART_Transmit(&huart1, (uint8_t *)timestamp, strlen((const char *)timestamp), 500);
	    	        	sprintf(message, "glubina %02dm\r\n", (int)depth);
	    				HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
	    				*/
	    			}
                                                                                                                                                                      
	    			// log depth
	    			//--------------------------------------------------------------------------
                                                                                                                                                                      
	    			if(eeprom_number_of_records == 0)
	    			{
	    				// no records yet
	    				
	    				// read memory bank id
	    				HAL_I2C_Mem_Read(at24c32_i2c_handle, at24c32_shifted_address, 0, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);	
                                                                                                                                                                      
	    				if(b0 == 1)   // pishem v bank 1
	    				{
	    					// pri sleduyuschem zapuske budem pisat' v bank 0
	    					b0 = 0;
	    					HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, 0, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    					HAL_Delay(write_delay);
                                                                                                                                                                      
	    					// nastraivaem address i2c banka pamyati (nomer 1)
	    					at24c32_shifted_address = 0x51 << 1;
	    				}
	    				else
	    				{
	    					// pri sleduyuschem zapuske budem pisat' v bank 1
	    					b0 = 1;
	    					HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, 0, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    					HAL_Delay(write_delay);
	    				}
                                                                                                                                                                      
                                                                                                                                                                      
	    				// write timestamp
	    	        	sprintf(timestamp, "%02d:%02d %02d.%02d", hours, minutes, date, month);
	    				b0 = timestamp[0];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = timestamp[1];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = timestamp[3];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = timestamp[4];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				//b0 = ' ';
	    				//HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				//HAL_Delay(write_delay);
	    				//eeprom_debug_address++;
	    				b0 = timestamp[6];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = timestamp[7];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = timestamp[9];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = timestamp[10];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				//b0 = ' ';
	    				//HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				//HAL_Delay(write_delay);
	    				//eeprom_debug_address++;

	    				// write emergency depth
	    				sprintf(message, "%02d", (int)depth_switch_get_current_depth());
	    				b0 = message[0];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = message[1];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;

                                                                                                                                                                      
                                                                                                                                                                      
	    				// write first depth record
	    	        	sprintf(message, "%02d.%01d", (int)depth, (int)((depth - (int)depth)*10.0));
	    	       		sprintf(temperature_message, "%02d", (int)(actual_temperature/100.0));
	    				b0 = message[0];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = message[1];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				//b0 = message[2];
	    				//HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				//HAL_Delay(write_delay);
	    				//eeprom_debug_address++;
	    				b0 = message[3];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = temperature_message[0];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = temperature_message[1];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = 0;
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = 0;
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address--;
	    				eeprom_number_of_records++;
                                                                                                                                                                      
	    			}
	    			else
	    			{
	    				// there are depth records
                                                                                                                                                                      
	    				// write new record
	    				b0 = 0;
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address + 5, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address + 6, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_number_of_records++;
	    	        	sprintf(message, "%02d.%01d", (int)depth, (int)((depth - (int)depth)*10.0));
	    	       		sprintf(temperature_message, "%02d", (int)(actual_temperature/100.0));
	    				b0 = message[0];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = message[1];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				//b0 = message[2];
	    				//HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				//HAL_Delay(write_delay);
	    				//eeprom_debug_address++;
	    				b0 = message[3];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = temperature_message[0];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = temperature_message[1];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
                                                                                                                                                                      
	    			}
                                                                                                                                                                      
                                                                                                                                                                      
                                                                                                                                                                      
                                                                                                                                                                      
                                                                                                                                                                      
	    			//--------------------------------------------------------------------------
                                                                                                                                                                      
	    			if((actuator_counter > 0) && (actuator_counter < 100))
	    			{
	    				if(actuator_counter >= 21)
	    				{
	    					// switch off actuators
  	    					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_RESET);// turn actuators off
	    					actuator_counter = 100;
	    				}
	    				else
	    				{
	    					actuator_counter++;
	    				}
	    			}
                                                                                                                                                                      
                                                                                                                                                                      
	    			if((depth >= (depth_switch_get_current_depth())) && actuator_counter == 0)
	    			{
                                                                                                                                                                      
	    				actuator_counter++;
	    				// switch on actuators
  	    				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_SET);// turn actuators on
                                                                                                                                                                      
	    				// switch on signal leds
  	    				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_SET);// turn leds off
                                                                                                                                                                      
                                                                                                                                                                      
	    				// save info about activation conditions (time, depth, etc)
	    				ssd1306_Fill(Black);
  	    	        	ssd1306_SetCursor(0,0);
	    	        	sprintf(timestamp, "%02d:%02d %02d.%02d", hours, minutes, date, month);
	    	        	//sprintf(timestamp, "timestamp");
  	    	        	ssd1306_WriteString(timestamp, Font_11x18, White);
  	    	        	ssd1306_SetCursor(0,22);
	    	        	sprintf(message, ">>>>> %02dm", (int)depth);
  	    	        	ssd1306_WriteString(message, Font_11x18, White);
  	    	        	//ssd1306_SetCursor(0,44);
	    	        	//sprintf(message, "activated!!!");
  	    	        	//ssd1306_WriteString(message, Font_11x18, White);
  	    	        	ssd1306_UpdateScreen();   
                                                                                                                                                                      
	    				/*
	    	        	sprintf(timestamp, "%02d:%02d:%02d %02d.%02d\r\n", hours, minutes, seconds, date, month);
	    				HAL_UART_Transmit(&huart1, (uint8_t *)timestamp, strlen((const char *)timestamp), 500);
	    	        	sprintf(message, ">>>>> %02dm\r\n", (int)depth);
	    				HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
	    	        	sprintf(message, "activated!!!\r\n");
	    				HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen((const char *)message), 500);
	    				*/
                                                                                                                                                                      
	    				// write depth of activation 
	    	        	//sprintf(message, "%02d", (int)depth);
	    	        	sprintf(message, "%02d.%01d", (int)depth, (int)((depth - (int)depth)*10.0));
	    	       		sprintf(temperature_message, "%02d", (int)(actual_temperature/100.0));
	    				b0 = message[0];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = message[1];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				//b0 = message[2];
	    				//HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				//HAL_Delay(write_delay);
	    				//eeprom_debug_address++;
	    				b0 = message[3];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = temperature_message[0];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = temperature_message[1];
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = '$';
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				b0 = '$';
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				eeprom_number_of_records++;
	    				b0 = '$';
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				eeprom_number_of_records++;
	    				b0 = '$';
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				eeprom_number_of_records++;
	    				b0 = '$';
	    				HAL_I2C_Mem_Write(at24c32_i2c_handle, at24c32_shifted_address, eeprom_debug_address, I2C_MEMADD_SIZE_16BIT, &b0, 1, 100);
	    				HAL_Delay(write_delay);
	    				eeprom_debug_address++;
	    				eeprom_number_of_records++;
	    				/*
	    	        	sprintf(message, "%02d", (int)depth);
	    				data = (uint16_t)(((uint16_t)(message[0])<<8) + (uint16_t)message[1]);
	    				at24c32_write_16(eeprom_address, data);
	    				eeprom_address+=2;
	    				at24c32_write_16(eeprom_address, (uint16_t)0x0101);
	    				eeprom_number_of_records++;
	    				*/
                                                                                                                                                                      
	    				// pause 21 sec
	    				//HAL_Delay(21000);
                                                                                                                                                                      
                                                                                                                                                                      
	    				// switch off actuators
  	    				//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_RESET);// turn actuators off
                                                                                                                                                                      
	    				// stop
	    				//while(1);
	    			}
                                                                                                                                                                      
	    //*/
	    		}// end if(!we_are_under_water)  // we are not under water
                
                                                                                                                                                                      
                                                                                                                                                                      
                                                                                                                                                                      
                                                                                                                                                                      
	    	}// end if(one_second_timer_get_flag())
                                                                                                                                                                      
                                                                                                                                                                      
                                                                                                                                                                      
                                                                                                                                                                      
	    }// end while   

	}// end if


}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
















/** System Clock Configuration
*/
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}


#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
