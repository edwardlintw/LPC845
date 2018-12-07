/*
 * time_tick.c
 *
 *  Created on: Nov 9, 2018
 *      Author: Edward
 */
#include "time_tick.h"

static volatile uint32_t	time_tick_ms;

uint32_t get_time_tick_ms(void)
{
	return time_tick_ms;
}

void inc_time_tick_ms(void)
{
	++time_tick_ms;
}

