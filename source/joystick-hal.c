
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_mrt.h"
#include "pin_mux.h"
#include "adc-driver.h"
#include "joystick-hal.h"

static uint32_t				delta_ = 5U;
static adc_channel_t  		channels_[] =
	{
		{ .channel_ = 0 }
	};
static adc_value_t 			adc_values_[] =
	{
		{ .value_ = 0 }
	};
static adc_value_t 			adc_old_values_[] =
	{
		{ .value_ = 0 }
	};
static joystick_value_t 	joystick_values_[] =
	{
		{
			.value_ = 0,
			.ready_ = false
		}
	};
static size_t 				channel_num_ = sizeof channels_ / sizeof channels_[0];

size_t joystick_init(void)
{
	adc_init(adc_sample_12bit);
	adc_channels_init(channels_, channel_num_);
	return channel_num_;
}

joystick_value_t* joystick_task(void)
{
	adc_read(channels_, adc_values_, channel_num_);
	for (size_t i = 0; i < channel_num_; ++i) {
		joystick_values_[i].ready_ = false;

		volatile int32_t	nv = adc_values_[i].value_;
		volatile int32_t	ov = adc_old_values_[i].value_;
		if (((nv > ov) && (nv - ov > delta_)) || ((ov > nv) && (ov - nv > delta_))) {
			adc_old_values_ [i].value_ = nv;
			joystick_values_[i].value_ = nv;
			joystick_values_[i].ready_ = true;
		}
	}
	return joystick_values_;
}
