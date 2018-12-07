/*
 * test_gpio_high_low.c
 *
 *  Created on: Nov 12, 2018
 *      Author: Edward
 */
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_mrt.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "gpio_def.h"
#include "test_gpio_high_low.h"

void init_gpio_test(void)
{
    gpio_pin_config_t pinConfigIn = {
        kGPIO_DigitalInput, 0,
    };

    gpio_pin_config_t pinConfigOut = {
    	kGPIO_DigitalOutput, 1,
    };

    GPIO_PinInit(GPIO, PIO_PORT(TEST_IN_PIN), PIO_PIN(TEST_IN_PIN), &pinConfigIn);
    GPIO_PinInit(GPIO, PIO_PORT(TEST_OUT_PIN), PIO_PIN(TEST_OUT_PIN), &pinConfigOut);
}

void test_gpio_task(void)
{
	static uint32_t c = 0;
	if (++c % 2000 == 0) {
		GPIO_PinWrite(GPIO, PIO_PORT(TEST_OUT_PIN), PIO_PIN(TEST_OUT_PIN), 0);
	}
	uint32_t	value;
	value = GPIO_PinRead(GPIO, PIO_PORT(TEST_IN_PIN), PIO_PIN(TEST_IN_PIN));
	if (0 == value) {
		PRINTF("\r\ninput value 0");
	}
	GPIO_PinWrite(GPIO, PIO_PORT(TEST_OUT_PIN), PIO_PIN(TEST_OUT_PIN), 1);

}
