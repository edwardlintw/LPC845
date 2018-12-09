#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "gpio-driver.h"

static bool gpio_did_init_ = false;

void gpio_init (const gpio_init_struct_t* s)
{
	if (!gpio_did_init_) {
		GPIO_PortInit(GPIO, 0);
		GPIO_PortInit(GPIO, 1);
		gpio_did_init_ = true;
	}

    gpio_pin_config_t pin_config;

	pin_config.pinDirection = s->direction_ == gpio_direction_input ? kGPIO_DigitalInput : kGPIO_DigitalOutput;
	pin_config.outputLogic  = s->output_logic_;
	GPIO_PinInit(GPIO, PIO_PORT(s->pin_), PIO_PIN(s->pin_), &pin_config);
}

uint32_t gpio_read (uint32_t pin)
{
	return GPIO_PinRead(GPIO, PIO_PORT(pin), PIO_PIN(pin));
}

void gpio_write(uint32_t pin, uint32_t value)
{
	GPIO_PinWrite(GPIO, PIO_PORT(pin), PIO_PIN(pin), value);
}
