/*
 * gpio-driver.h
 */
#ifndef GPIO_DRIVER_H_
#define GPIO_DRIVER_H_

typedef struct gpio_driver_struct {
    uint32_t            pio_pin_;   // use pin def in gpio-def.h
    gpio_pin_config_t   direction_;
    uint8_t             output_logic_;
}   gpio_driver_struct_t;

void init_gpio(const gpio_driver_struct_t*);
uint32_t gpio_read(uint32_t pin);
void gpio_write(uint32_t pin, uint32_t value);

#endif
