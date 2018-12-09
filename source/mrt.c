/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
#include "test_gpio_high_low.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define APP_LED_INIT LED_RED_INIT(1)
#define APP_LED_ON (LED_RED_ON())
#define APP_LED_TOGGLE (LED_RED_TOGGLE())
#define MRT_CLK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
static volatile uint32_t mrtIsrCount = 0;
static uint32_t			 encoders_num = 0;
static uint32_t			 buttons_num = 0;
static uint32_t			 joysticks_num = 0;

/*******************************************************************************
 * Code
 ******************************************************************************/
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

    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

/*!
 * @brief Main function
 */
int main(void)
{
    uint32_t mrt_clock;

    /* Structure of initialize MRT */
    mrt_config_t mrtConfig;

    /* Board pin, clock, debug console init */
    /* Attach main clock to USART0 (debug console) */
    CLOCK_Select(BOARD_DEBUG_USART_CLK_ATTACH);

    BOARD_InitPins();
    BOARD_BootClockFRO30M();
    BOARD_InitDebugConsole();

    mrt_clock = MRT_CLK_FREQ;

    /* mrtConfig.enableMultiTask = false; */
    MRT_GetDefaultConfig(&mrtConfig);

    /* Init mrt module */
    MRT_Init(MRT0, &mrtConfig);

    /* Setup Channel 0 to be repeated */
    MRT_SetupChannelMode(MRT0, kMRT_Channel_0, kMRT_RepeatMode);

    /* Enable timer interrupts for channel 0 */
    MRT_EnableInterrupts(MRT0, kMRT_Channel_0, kMRT_TimerInterruptEnable);

    joysticks_num = joystick_init();
    encoders_num  = encoder_init();
    buttons_num   = button_init();

    /* Enable at the NVIC */
    EnableIRQ(MRT0_IRQn);

    /* Start channel 0 */
    PRINTF("\r\nStarting channel No.0 ...");
    MRT_StartTimer(MRT0, kMRT_Channel_0, USEC_TO_COUNT(1000U, mrt_clock));

    while (true)
    {
        /* Check whether occur interupt and toggle LED */
    	if (mrtIsrCount == 5000)
    	{
            PRINTF("\r\n 5000 interrupts");
            mrtIsrCount = 0;
    	}
    }
}
