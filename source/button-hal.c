/*
 * button.c
 *
 *  Created on: Nov 9, 2018
 *      Author: Edward
 */

#include <gpio-def.h>
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_mrt.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "time_tick.h"
#include "button-hal.h"
#include "gpio-driver.h"

typedef struct button_struct {
	uint32_t	pin_;
	uint32_t	pin_value_;
	uint32_t	down_tick_ms_;
}	button_struct_t;

static button_struct_t	buttons[] = {
	{
		.pin_ 		= P1_19,
		.pin_value_ = 1
	}
};

size_t button_init(void)
{
    gpio_init_struct_t gpio;

	for (size_t i = 0; i < sizeof buttons / sizeof buttons[0]; ++i) {
		gpio.pin_ = buttons[i].pin_;
		gpio.direction_ = gpio_direction_input;
		gpio.output_logic_ = 0;
		gpio_init(&gpio);
	}

	return sizeof buttons / sizeof buttons[0];
}

button_state_t* button_task(void)
{
	volatile uint32_t 		value;
	static button_state_t	state[sizeof buttons / sizeof buttons[0]];

	for (uint32_t i = 0; i < sizeof buttons / sizeof buttons[0]; ++i) {
		state[i].result_ = button_result_none;
		state[i].button_id_ = i;
		gpio_read(buttons[i].pin_);
		if (value == buttons[i].pin_value_)
			continue;
		if (0 == value) {
			buttons[i].down_tick_ms_ = get_time_tick_ms();
			state[i].result_ = button_result_down;
		}
		else {
			state[i].result_ = button_result_up;
			state[i].pushed_time_ms_ = get_time_tick_ms() - buttons[i].down_tick_ms_;
		}
		buttons[i].pin_value_ = value;
	}
	return state;
}
