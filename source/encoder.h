/*
 * encoder.h
 *
 *  Created on: Nov 8, 2018
 *      Author: Edward
 */

#ifndef ENCODER_H_
#define ENCODER_H_

typedef enum encoder_result {
	encoder_result_none,
	encoder_result_turn_left,
	encoder_result_turn_right
}	encoder_result_t;

typedef struct encoder_state {
	int32_t				encoder_id_;
	encoder_result_t	result_;
}	encoder_state_t;

uint32_t 			init_encoders(void);
encoder_state_t* 	encoder_task(void);

#endif /* ENCODER_H_ */
