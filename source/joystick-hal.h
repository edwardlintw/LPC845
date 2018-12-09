
#ifndef JOYSTICK_HAL_H_
#define JOYSTICK_HAL_H_

typedef struct joystick_hal_struct {
	size_t		channel_num_;
	uint32_t*	channels_;
}	joystick_hal_struct_t;

typedef struct joystick_value_t {
    uint32_t        value_;
}	joystick_value_t;

void              joystick_init(joystick_hal_struct_t*, uint32_t delta);
joystick_value_t* joystick_task(void);


#endif /* JOYSTICK_HAL_H_ */