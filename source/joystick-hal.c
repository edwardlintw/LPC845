
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_mrt.h"
#include "pin_mux.h"
#include "adc-driver.h"
#include "joystick-hal.h"

static uint32_t				delta_ = 0U;
static size_t 				channel_num_ = 0;
static adc_channel_t* 		channels_ = NULL;
static adc_value_t*			adc_values_ = NULL;
static adc_value_t*			adc_old_values_ = NULL;
static joystick_value_t*	joystick_values_ = NULL;

void joystick_init(joystick_hal_struct_t* joystick, uint32_t delta)
{
	channel_num_ 	 = joystick->channel_num_;
	delta_		     = delta;
	channels_ 		 = (adc_channel_t*)   malloc(sizeof(adc_channel_t) * channel_num_);
	adc_values_ 	 = (adc_value_t*)     malloc(sizeof(adc_value_t) * channel_num_);
	adc_old_values_  = (adc_value_t*)     malloc(sizeof(adc_value_t) * channel_num_);
	joystick_values_ = (joystick_value_t*)malloc(sizeof(joystick_value_t) * channel_num_);
	adc_init(adc_sample_12bit);
	for (size_t i = 0; i < channel_num_; ++i) {
		channels_[i].channel_     = joystick->channels_[i];
		adc_values_[i].value_     = 0U;
		adc_old_values_[i].value_  = 0U;
		joystick_values_[i].value_ = 0U;
	}
	adc_channels_init(channels_, channel_num_);
}

joystick_value_t* joystick_task(void)
{
	adc_read(channels_, adc_values_, channel_num_);
	for (size_t i = 0; i < channel_num_; ++i) {
		if (adc_values_[i].value_ - adc_old_values_[i].value_ > delta_ || adc_values_[i].value_ - adc_old_values_[i].value_ < delta_) {
			joystick_values_[i].value_ = adc_old_values_[i].value_
					                   = adc_values_[i].value_;
		}
	}
	return joystick_values_;
}
