/*
 * time_tick.h
 *
 *  Created on: Nov 9, 2018
 *      Author: Edward
 */

#ifndef TIME_TICK_H_
#define TIME_TICK_H_

#include <stdint.h>

uint32_t get_time_tick_ms(void);
void     inc_time_tick_ms(void);

#endif /* TIME_TICK_H_ */
