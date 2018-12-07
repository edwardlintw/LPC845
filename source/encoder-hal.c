/*
 * encoder-hal.c
 */
#include <stdlib.h>
#include "fsl_debug_console.h"
#include "board.h"
#include "gpio_def.h"
#include "gpio-driver.h"
#include "encoder-hal.h"

typedef enum encoder_event {
    encoder_event_pinA_up,
    encoder_event_pinB_up,
    encoder_event_pinA_down,
    encoder_event_pinB_down,
    encoder_event_max
}   encoder_event_t;

static encoder_event_t expected_event[2][encoder_event_max-1] = {
    // when pin A is down first
    { encoder_event_pinB_down, encoder_event_pinA_up, encoder_event_pinB_up },
    // when pin B is down first
    { encoder_event_pinA_down, encoder_event_pinB_up, encoder_event_pinA_up }
};

typedef struct encoder_data {
    uint32_t    valueA_;
    uint32_t    valueB_;
    int32_t     expected_path_; // -1: initial, 0, 1
    int32_t     current_step_;
}   encoder_data_t;

void determine_state(uint32_t, uint32_t, encoder_data_t*, encoder_state_t*);
void reset_encoder(encoder_struct_t*);

static encoder_hal_struct_t*    encoders_ = NULL;
static encoder_data_t*          enc_data = NULL;
static encoder_state_t*         enc_state = NULL;
static size_t                   enc_num_ = 0;

void init_encoder(const encoder_hal_struct_t* encoders, size_t num)
{
    encoders_ = encoders;
    enc_num_ = num;
    enc_data = (encoder_data_t*) malloc(sizeof(encoder_data_t) * num);
    enc_state = (encoder_state_t*) malloc(sizeof(encoder_state_t) * num);
    gpio_driver_struct_t gpio;
    for (size_t i = 0; i < num; ++i) {
       gpio.pio_pin_ = encoders[i].pinA_;
       gpio.pio_direction_ = kGPIO_DigitalInput;
       gpio.outout_logic_ = 0;
       init_gpio(&gpio);

       gpio.pio_pin_ = encoders[i].pinB_;
       gpio.pio_direction_ = kGPIO_DigitalInput;
       gpio.outout_logic_ = 0;
       init_gpio(&gpio);

       enc_data[i].valueA_        = 1;
       enc_data[i].valueB_        = 1;
       enc_data[i].expected_path_ = -1;
       enc_data[i].current_step_  = -1;
    }

}

encoder_state_t* encoder_task(void)
{
    volatile uint32_t   valueA;
    volatile uint32_t   valueB;

    for (size_t i = 0; i < encoders_num_; ++i) {
        enc_state[i].result_ = encoder_result_none;
        enc_state[i].id_     = i;
        valueA = gpio_read(encoders[i].pinA_);
        valueB = gpio_read(encoders[i].pinB_);
        determine_state(valueA, valueB, &encoders[i], &enc_state[i]);
    }

    return enc_state;
}

void reset_encoder(encoder_struct_t* s)
{
    s->valueA_ = 1;
    s->valueB_ = 1;
    s->expected_path_ = -1;
    s->expected_path_ = -1;
}

void determine_state
    (
        uint32_t            valueA_, 
        uint32_t            valueB_, 
        encoder_data_t*     enc_data, 
        encoder_state_t*    enc_state
    )
{
    bool did_reset_encoder = false;

    if (valueA != enc_data->valueA_ || valueB != enc_data->valueB_) {
        bool a_value_changed = valueA != enc_data->valueA_;
        if (-1 == enc_data->expected_path_ && -1 == enc_data->current_step_) {
            enc_data->expected_path = a_value_changed ? 0 : 1;
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
                    reset_encoder(encoder);
                    did_reset_encoder = true;
                }
            }
            else {
                // invalid (wrong) encoder pin phase
                reset_endoder(encoder);
                did_reset_encoder = true;
            }
        }
    }
    if (!did_reset_encoder) {
        enc_data->valueA_ = valueA;
        enc_data->valueB_ = valueB;
    }
}
