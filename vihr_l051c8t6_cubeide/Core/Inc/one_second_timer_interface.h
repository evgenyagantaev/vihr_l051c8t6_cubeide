#ifndef ONE_SECOND_TIMER_INTERFACE_H
#define ONE_SECOND_TIMER_INTERFACE_H

#include "tim.h"


void one_second_timer_set_flag();
void one_second_timer_reset_flag();
int one_second_timer_get_flag();

void one_second_timer_init();
void one_second_timer_start();
void one_second_timer_stop();

void one_second_timer_increment_counter();
uint32_t one_second_timer_get_counter();

#endif
