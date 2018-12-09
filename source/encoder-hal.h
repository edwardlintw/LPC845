#ifndef ENCODER_HAL_H_
#define ENCODER_HAL_H_

typedef enum encoder_result {
    encoder_result_none,
    encoder_result_turn_left,
    encoder_result_turn_right
}   encoder_result_t;

typedef struct encoder_state {
    int32_t             id_;
    encoder_result_t    result_;
}   encoder_state_t;

typedef struct encoder_hal_struct {
    int32_t             id_;
    uint32_t            pinA_;
    uint32_t            pinB_;
}   encoder_hal_struct_t;

uint32_t         encoder_init(void);
encoder_state_t* encoder_task(void);

#endif

