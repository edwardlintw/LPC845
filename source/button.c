/*
 * button.c
 *
 *  Created on: Nov 9, 2018
 *      Author: Edward
 */

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_mrt.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "gpio_def.h"
#include "button.h"
#include "time_tick.h"

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

uint32_t init_buttons(void)
{
    gpio_pin_config_t pinConfig = {
        kGPIO_DigitalInput, 0,
    };

	for (uint32_t i = 0; i < sizeof buttons / sizeof buttons[0]; ++i) {
	    GPIO_PinInit(GPIO, PIO_PORT(buttons[i].pin_), PIO_PIN(buttons[i].pin_), &pinConfig);
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
		value = GPIO_PinRead(GPIO, PIO_PORT(buttons[i].pin_), PIO_PIN(buttons[i].pin_));
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
