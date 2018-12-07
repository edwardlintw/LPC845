/*
 * encoder.c
 *
 *  Created on: Nov 8, 2018
 *      Author: Edward
 */

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_mrt.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "gpio_def.h"
#include "encoder.h"

typedef enum encoder_event {
	encoder_event_pinA_up,
	encoder_event_pinB_up,
	encoder_event_pinA_down,
	encoder_event_pinB_down,
	encoder_event_max
}	encoder_event_t;

static encoder_event_t expected_event[2][encoder_event_max-1] = {
	// when pin A is down first
	{ encoder_event_pinB_down, encoder_event_pinA_up, encoder_event_pinB_up },
	// when pin B is down first
	{ encoder_event_pinA_down, encoder_event_pinB_up, encoder_event_pinA_up }
};

typedef struct encoder_struct {
	uint32_t	pinA_;
	uint32_t	pinB_;
	uint32_t	a_value_;
	uint32_t	b_value_;
	int32_t		expected_path_;	// -1: initial, 0, 1
	int32_t		current_step_;
}	encoder_struct_t;

static encoder_struct_t	encoders[] = {
		{ P0_6, P0_7, 1, 1, -1, -1 }
};

void determine_state(uint32_t a_value, uint32_t b_value, encoder_struct_t* encoder, encoder_state_t* state);
void reset_encoder(encoder_struct_t* encoder);

uint32_t init_encoders(void)
{
    gpio_pin_config_t pinConfig = {
        kGPIO_DigitalInput, 0,
    };

	for (uint32_t i = 0; i < sizeof encoders / sizeof encoders[0]; ++i) {
	    GPIO_PinInit(GPIO, PIO_PORT(encoders[i].pinA_), PIO_PIN(encoders[i].pinA_), &pinConfig);
	    GPIO_PinInit(GPIO, PIO_PORT(encoders[i].pinA_), PIO_PIN(encoders[i].pinB_), &pinConfig);
	}

	return sizeof encoders / sizeof encoders[0];
}

// mark the following line 'TEST' to run with real encoders
//
#define TEST

encoder_state_t* encoder_task(void)
{
	static encoder_state_t	state[sizeof encoders / sizeof encoders[0]];
	volatile uint32_t		a_value;
	volatile uint32_t		b_value;

#ifdef TEST
	static uint32_t a[] = { 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1 };
	static uint32_t b[] = { 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1 };
	static uint32_t	counter = 0;
	static uint32_t position = 0;

	if (++counter == 100) {
		counter = 0;
		position = (position+1) % (sizeof a / sizeof a[0]);
	}
	for (uint32_t i = 0; i < sizeof encoders / sizeof encoders[0]; ++i) {
		state[i].result_ = encoder_result_none;
		state[i].encoder_id_ = i;
		a_value = a[position];
		b_value = b[position];
		determine_state(a_value, b_value, &encoders[i], &state[i]);
	}
#else
	for (uint32_t i = 0; i < sizeof encoders / sizeof encoders[0]; ++i) {
		state[i].result_ = encoder_result_none;
		state[i].encoder_id_ = i;
		a_value = GPIO_PinRead(GPIO, PIO_PORT(encoders[i].pinA_), PIO_PIN(encoders[i].pinA_));
		b_value = GPIO_PinRead(GPIO, PIO_PORT(encoders[i].pinB_), PIO_PIN(encoders[i].pinB_));
		determine_state(a_value, b_value, &encoders[i], &state[i]);
	}
#endif

	return state;
}

void reset_encoder(encoder_struct_t* encoder)
{
	encoder->a_value_ = 1;
	encoder->b_value_ = 1;
	encoder->expected_path_ = -1;
	encoder->current_step_ = -1;
}

void determine_state(uint32_t a_value, uint32_t b_value, encoder_struct_t* encoder, encoder_state_t* state)
{
	bool did_reset_encoder = false;

	if (a_value != encoder->a_value_ || b_value != encoder->b_value_) {
		bool a_value_changed = a_value != encoder->a_value_;

		if (-1 == encoder->expected_path_ && -1 == encoder->current_step_) {
			encoder->expected_path_ = a_value_changed ? 0 : 1;
			encoder->current_step_ = 0;
		}
		else {
			encoder_event_t	evt;

			if (a_value_changed)
				evt = a_value ? encoder_event_pinA_up : encoder_event_pinA_down;
			else
				evt = b_value ? encoder_event_pinB_up : encoder_event_pinB_down;
			if (evt == expected_event[encoder->expected_path_][encoder->current_step_]) {
				if (++encoder->current_step_ == encoder_event_max - 2) {
					state->result_ = 0 == encoder->expected_path_ ? encoder_result_turn_left
																  : encoder_result_turn_right;
					reset_encoder(encoder);
					did_reset_encoder = true;
				}
			}
			else {
				// invalid (wrong) encoder pin phase
				reset_encoder(encoder);
				did_reset_encoder = true;
			}
		}
	}
	if (!did_reset_encoder) {
		encoder->a_value_ = a_value;
		encoder->b_value_ = b_value;
	}
}
