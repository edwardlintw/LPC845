/*
 * encoder-hal.c
 */
#include <gpio-def.h>
#include <stdlib.h>
#include "fsl_debug_console.h"
#include "board.h"
#include "encoder-hal.h"
#include "gpio-driver.h"

typedef enum encoder_event {
    encoder_event_pinA_up,
    encoder_event_pinB_up,
    encoder_event_pinA_down,
    encoder_event_pinB_down,
    encoder_event_max
}   encoder_event_t;

typedef struct encoder_data {
    uint32_t    valueA_;
    uint32_t    valueB_;
    int32_t     expected_path_; // -1: initial, 0, 1
    int32_t     current_step_;
}   encoder_data_t;

static void determine_state(uint32_t, uint32_t, encoder_data_t*, encoder_state_t*);
static void reset_encoder(encoder_data_t*);

static encoder_hal_struct_t    encoders_[] = {
	{
		.id_ 	= 0,
		.pinA_  = P0_6,
		.pinB_  = P1_7
	}
};
static encoder_data_t         enc_data_[] = {
	{
			.valueA_		= 1,
			.valueB_		= 1,
			.expected_path_ = -1,
			.current_step_  = -1
	}
};
static encoder_state_t         enc_state_[] = {
	{
			.id_		= 0,
			.result_	= encoder_result_none
	}
};
static size_t                  enc_num_ = sizeof encoders_ / sizeof encoders_[0];

size_t encoder_init(void)
{
    gpio_init_struct_t gpio;
    for (size_t i = 0; i < enc_num_; ++i) {
       gpio.pin_ = encoders_[i].pinA_;
       gpio.direction_ = gpio_direction_input;
       gpio.output_logic_ = 0;
       gpio_init(&gpio);

       gpio.pin_ = encoders_[i].pinB_;
       gpio.direction_ = gpio_direction_input;
       gpio.output_logic_ = 0;
       gpio_init(&gpio);

       reset_encoder(&enc_data_[i]);
    }
    return enc_num_;
}

encoder_state_t* encoder_task(void)
{
    volatile uint32_t   valueA;
    volatile uint32_t   valueB;

    for (size_t i = 0; i < enc_num_; ++i) {
        enc_state_[i].result_ = encoder_result_none;
        enc_state_[i].id_     = i;
        valueA = gpio_read(encoders_[i].pinA_);
        valueB = gpio_read(encoders_[i].pinB_);
        determine_state(valueA, valueB, &enc_data_[i], &enc_state_[i]);
    }

    return enc_state_;
}

static void reset_encoder(encoder_data_t* s)
{
    s->valueA_ = 1;
    s->valueB_ = 1;
    s->expected_path_ = -1;
    s->current_step_ = -1;
}

static void determine_state
    (
        uint32_t            valueA,
        uint32_t            valueB,
        encoder_data_t*     enc_data, 
        encoder_state_t*    enc_state
    )
{
	static encoder_event_t expected_event[2][encoder_event_max-1] = {
	    // when pin A is down first
	    { encoder_event_pinB_down, encoder_event_pinA_up, encoder_event_pinB_up },
	    // when pin B is down first
	    { encoder_event_pinA_down, encoder_event_pinB_up, encoder_event_pinA_up }
	};

    bool did_reset_encoder = false;

    if (valueA != enc_data->valueA_ || valueB != enc_data->valueB_) {
        bool a_value_changed = valueA != enc_data->valueA_;
        if (-1 == enc_data->expected_path_ && -1 == enc_data->current_step_) {
            enc_data->expected_path_ = a_value_changed ? 0 : 1;
            enc_data->current_step_ = 0;
        }
        else {
            encoder_event_t evt;
            
            if (a_value_changed)
                evt = valueA ? encoder_event_pinA_up : encoder_event_pinA_down;
            else
                evt = valueB ? encoder_event_pinB_up : encoder_event_pinB_down;

            if (evt == expected_event[enc_data->expected_path_][enc_data->current_step_]) {
                if (++enc_data->current_step_ == encoder_event_max - 2) {
                    enc_state->result_ = 0 == enc_data->expected_path_ ? encoder_result_turn_left
                                                                       : encoder_result_turn_right;
                    reset_encoder(enc_data);
                    did_reset_encoder = true;
                }
            }
            else {
                // invalid (wrong) encoder pin phase
                reset_encoder(enc_data);
                did_reset_encoder = true;
            }
        }
    }
    if (!did_reset_encoder) {
        enc_data->valueA_ = valueA;
        enc_data->valueB_ = valueB;
    }
}
