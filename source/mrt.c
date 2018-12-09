#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_mrt.h"
#include "fsl_adc.h"
#include "fsl_clock.h"
#include "fsl_power.h"
#include "fsl_gpio.h"

#include "pin_mux.h"
#include "encoder-hal.h"
#include "button-hal.h"
#include "joystick-hal.h"
#include "time_tick.h"

#define MRT_CLK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)
static volatile uint32_t mrtIsrCount = 0;
static uint32_t			 encoders_num = 0;
static uint32_t			 buttons_num = 0;
static uint32_t			 joysticks_num = 0;

void MRT0_IRQHandler(void)
{
    /* Clear interrupt flag.*/
    MRT_ClearStatusFlags(MRT0, kMRT_Channel_0, kMRT_TimerInterruptFlag);
    inc_time_tick_ms();
    ++mrtIsrCount;

#if 0
    // encoders
    encoder_state_t*	encoder_state = encoder_task();
    for (uint32_t i = 0; i < encoders_num; ++i) {
        if (encoder_state[i].result_ != encoder_result_none) {
        	if (encoder_state[i].result_ == encoder_result_turn_left)
        		PRINTF("\r\nencoder %u turn left", encoder_state[i].id_);
        	else
        		PRINTF("\r\nencoder %u turn right", encoder_state[i].id_);
        }
    }

    // buttons
    button_state_t* button_state = button_task();
    for (uint32_t i = 0; i < buttons_num; ++i) {
        if (button_state[i].result_ != button_result_none) {
        	if (button_state[i].result_ == button_result_down)
        		PRINTF("\r\nbutton %u down", button_state[i].button_id_);
            else
            	PRINTF("\r\nbutton %u up, ms = %u", button_state[i].button_id_, button_state[i].pushed_time_ms_);
        }
    }
#endif

    // joysticks
    joystick_value_t* value = joystick_task();
    for (uint32_t i = 0; i < joysticks_num; ++i) {
    	if (value[i].ready_)
    		PRINTF("\r\njoystick value: %u", value[i].value_);
    }
}

int main(void)
{
    uint32_t mrt_clock;

    mrt_config_t mrtConfig;
    CLOCK_Select(BOARD_DEBUG_USART_CLK_ATTACH);

    BOARD_InitPins();
    BOARD_BootClockFRO30M();
    BOARD_InitDebugConsole();

    mrt_clock = MRT_CLK_FREQ;
    MRT_GetDefaultConfig(&mrtConfig);
    MRT_Init(MRT0, &mrtConfig);
    MRT_SetupChannelMode(MRT0, kMRT_Channel_0, kMRT_RepeatMode);
    MRT_EnableInterrupts(MRT0, kMRT_Channel_0, kMRT_TimerInterruptEnable);

    joysticks_num = joystick_init();
    encoders_num  = encoder_init();
    buttons_num   = button_init();

    EnableIRQ(MRT0_IRQn);

    MRT_StartTimer(MRT0, kMRT_Channel_0, USEC_TO_COUNT(1000U, mrt_clock));

    while (true)
    {
    	if (mrtIsrCount == 5000)
    	{
            PRINTF("\r\n 5000 interrupts");
            mrtIsrCount = 0;
    	}
    }
}
