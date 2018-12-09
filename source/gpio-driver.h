#ifndef GPIO_DRIVER_H_
#define GPIO_DRIVER_H_

#include "gpio-def.h"

typedef enum gpio_direction {
	gpio_direction_input,
	gpio_direction_output
}	gpio_direction_t;

typedef struct gpio_init_struct {
    uint32_t            pin_;   // use pin def in gpio-def.h
    gpio_direction_t   	direction_;
    uint8_t             output_logic_;
}   gpio_init_struct_t;

void 	 gpio_init (const gpio_init_struct_t*);
uint32_t gpio_read (uint32_t pin);
void 	 gpio_write(uint32_t pin, uint32_t value);

#endif
