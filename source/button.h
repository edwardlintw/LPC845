/*
 * button.h
 *
 *  Created on: Nov 9, 2018
 *      Author: Edward
 */

#ifndef BUTTON_H_
#define BUTTON_H_

typedef enum button_result {
	button_result_none,
	button_result_down,
	button_result_up
} 	button_result_t;

typedef struct button_state {
	uint32_t		button_id_;
	button_result_t	result_;
	uint32_t		pushed_time_ms_;
}	button_state_t;

uint32_t 			init_buttons(void);
button_state_t* 	button_task(void);

#endif /* BUTTON_H_ */
