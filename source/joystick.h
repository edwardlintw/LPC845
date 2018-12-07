#ifndef JOYSTICK_H
#define JOYSTICK_H

typedef struct joystick_value {
    int32_t         id_;
    uint32_t        value_;
    bool			ready_;
}	joystick_value_t;

uint32_t            	init_joysticks(void);
joystick_value_t*     	joystick_task(void);

#endif

