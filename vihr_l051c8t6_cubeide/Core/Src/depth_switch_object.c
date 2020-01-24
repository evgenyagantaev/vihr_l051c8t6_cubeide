#include "depth_switch_object.h"
#include "depth_switch_interface.h"




int depth_switch_check_gpio()
{
	int bitstatus;

	if ((GPIOA->IDR & GPIO_PIN_9) != (uint32_t)GPIO_PIN_RESET)
  	{
    	bitstatus = 1;
  	}
  	else
  	{
    	bitstatus = 0;
  	}

	return bitstatus;
}


void depth_switch_turn_signal_led(int led_number)
{
	// turn all leds off
  	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3, GPIO_PIN_SET);// turn leds off

	if(led_number == 1)
  		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	else if(led_number == 2)
  		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
	else if(led_number == 3)
  		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
	else if(led_number == 4)
  		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
	else if(led_number == 5) // test mode all leds on
	{
  		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3, GPIO_PIN_RESET);// turn leds on

	}

}



double depth_switch_step_current_depth(double increment)
{
	/*
	if(current_depth != DEPTH0)
	{
		if(current_depth == DEPTH1)
		{
			current_depth = DEPTH2;
			depth_switch_turn_signal_led(2);
		}
		else if(current_depth == DEPTH2)
		{
			current_depth = DEPTH3;
			depth_switch_turn_signal_led(3);
		}
		else if(current_depth == DEPTH3)
		{
			current_depth = DEPTH4;
			depth_switch_turn_signal_led(4);
		}
		else if(current_depth == DEPTH4)
		{
			current_depth = DEPTH1;
			depth_switch_turn_signal_led(1);
		}
	}
	*/


	if(increment == INC_ONE)
	{
		order0++;
		if(order0 >= 10)
			order0 = 0;
	}
	else if(increment == INC_TEN)
	{
		order1++;
		if(order1 >= 5)
			order1 = 0;
	}

	current_depth = order1*10.0 + order0;
}


void depth_switch_step_to_test()
{
	current_depth = DEPTH0;
	depth_switch_turn_signal_led(5);
}

double depth_switch_get_current_depth()
{
	return current_depth;
}




void depth_switch_action()
{
	if(!depth_switch_check_gpio()) // key pressed
		depth_switch_key_press_period_counter++;
	else  // key is not pressed
	{
		if(depth_switch_key_press_period_counter > 0)
		{
			if(depth_switch_key_press_period_counter <= 3)     // less then 3 seconds
				depth_switch_step_current_depth(INC_ONE);
			else if((depth_switch_key_press_period_counter > 3) && (depth_switch_key_press_period_counter <= 17))     // 
				depth_switch_step_current_depth(INC_TEN);
			else   // more then 3 seconds
				depth_switch_step_to_test();

			depth_switch_key_press_period_counter = 0;
		}

	}
}

