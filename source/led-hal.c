#include "gpio-def.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "gpio-driver.h"
#include "led-hal.h"

typedef struct led_pin {
	uint32_t	pin_;
}	led_pin_t;

// test built-in GREEN LED, it's P0_0
//#define TEST

static led_pin_t led_pin_[] = {
#ifdef TEST
	{ .pin_ = P0_0 }
#else
	{ .pin_ = P0_30 },	// encoder's LED 1
	{ .pin_ = P1_2  },
	{ .pin_ = P1_6  },
	{ .pin_ = P1_10 },
	{ .pin_ = P1_16 },
	{ .pin_ = P1_20 },	// encoder's LED 6
#endif
};

uint32_t led_init(void)
{
    gpio_init_struct_t gpio;
    for (size_t i = 0; i < sizeof led_pin_ / sizeof led_pin_[0]; ++i) {
       gpio.pin_ = led_pin_[i].pin_;
       gpio.direction_ = gpio_direction_output;
       gpio.output_logic_ = 1;
       gpio_init(&gpio);
    }
    return sizeof led_pin_ / sizeof led_pin_[0];
}

void led_on_off(uint32_t pin, bool on_off)
{
	if ((size_t)pin < sizeof led_pin_ / sizeof led_pin_[0]) {
		gpio_write(pin, on_off ? 0 : 1);
	}
}
