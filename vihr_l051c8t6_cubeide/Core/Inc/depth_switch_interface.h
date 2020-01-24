#ifndef DEPTH_SWITCH_INTERFACE_H
#define DEPTH_SWITCH_INTERFACE_H



double depth_switch_get_current_depth();
void depth_switch_action();
void depth_switch_turn_signal_led(int led_number);
int depth_switch_check_gpio();



#endif
