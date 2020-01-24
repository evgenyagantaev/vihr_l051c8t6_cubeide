#include "one_second_timer_object.h"
#include "one_second_timer_interface.h"
#include "main.h"


void one_second_timer_init()
{

	one_second_timer_reset_flag();

    MX_TIM2_Init();
	
}
void one_second_timer_start()
{
	HAL_TIM_Base_Start_IT(&htim2);
}
void one_second_timer_stop()
{
	HAL_TIM_Base_Stop_IT(&htim2);
}



void one_second_timer_set_flag()
{
	one_second_timer_flag = 1;
}
void one_second_timer_reset_flag()
{
	one_second_timer_flag = 0;
}
int one_second_timer_get_flag()
{
	return one_second_timer_flag;
}


void one_second_timer_increment_counter()
{
	seconds_counter++;
}

uint32_t one_second_timer_get_counter()
{
	return seconds_counter;
}





