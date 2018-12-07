/*
 * test_gpio_high_low.h
 *
 *  Created on: Nov 12, 2018
 *      Author: Edward
 */

#ifndef TEST_GPIO_HIGH_LOW_H_
#define TEST_GPIO_HIGH_LOW_H_

#include "gpio_def.h"

#define TEST_OUT_PIN	P1_18
#define TEST_IN_PIN		P1_14

void init_gpio_test(void);
void test_gpio_task(void);

#endif /* TEST_GPIO_HIGH_LOW_H_ */
