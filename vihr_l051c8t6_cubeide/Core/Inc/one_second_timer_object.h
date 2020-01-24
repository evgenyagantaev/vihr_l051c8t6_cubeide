#ifndef ONE_SECOND_TIMER_OBJ_H
#define ONE_SECOND_TIMER_OBJ_H

#include "tim.h"

static int one_second_timer_flag;
static uint32_t seconds_counter = 0;

void one_second_timer_init();
void one_second_timer_start();
int one_second_timer_get_flag();
void one_second_timer_reset_flag();


#endif
