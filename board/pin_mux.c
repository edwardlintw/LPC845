/*
 * The Clear BSD License
 * Copyright (c) 2018, NXP Semiconductors, Inc.
 * All rights reserved.
 *
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

/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Pins v4.1
processor: LPC845
package_id: LPC845M301JBD64
mcu_data: ksdk2_0
processor_version: 0.0.1
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

#include "fsl_common.h"
#include "fsl_iocon.h"
#include "fsl_swm.h"
#include "pin_mux.h"

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitBootPins
 * Description   : Calls initialization functions.
 *
 * END ****************************************************************************************************************/
void BOARD_InitBootPins(void)
{
    BOARD_InitPins();
}

/* clang-format off */
/*
 * TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
BOARD_InitPins:
- options: {callFromInitBoot: 'true', coreID: core0, enableClock: 'true'}
- pin_list:
  - {pin_num: '37', peripheral: USART0, signal: TXD, pin_signal: PIO1_17, mode: pullUp, invert: disabled, hysteresis: enabled, opendrain: disabled, smode: bypass,
    clkdiv: div0}
  - {pin_num: '36', peripheral: USART0, signal: RXD, pin_signal: PIO1_16, mode: pullUp, invert: disabled, hysteresis: enabled, opendrain: disabled, smode: bypass,
    clkdiv: div0}
  - {pin_num: '4', peripheral: GPIO, signal: 'PIO0, 12', pin_signal: PIO0_12, mode: pullUp, invert: disabled, hysteresis: enabled, opendrain: disabled, smode: bypass,
    clkdiv: div0}
  - {pin_num: '45', peripheral: ADC0, signal: 'CH, 0', pin_signal: PIO0_7, mode: inactive, invert: disabled, hysteresis: enabled, opendrain: disabled, smode: bypass,
    clkdiv: div0}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS ***********
 */
/* clang-format on */

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
/* Function assigned for the Cortex-M0P */
void BOARD_InitPins(void)
{
    /* Enables clock for IOCON.: enable */
    CLOCK_EnableClock(kCLOCK_Iocon);
    /* Enables clock for switch matrix.: enable */
    CLOCK_EnableClock(kCLOCK_Swm);

    const uint32_t pio2_config = (/* Selects pull-up function */
                                  IOCON_PIO_MODE_PULLUP |
                                  /* Enable hysteresis */
                                  IOCON_PIO_HYS_EN |
                                  /* Input not invert */
                                  IOCON_PIO_INV_DI |
                                  /* Disables Open-drain function */
                                  IOCON_PIO_OD_DI |
                                  /* Bypass input filter */
                                  IOCON_PIO_SMODE_BYPASS |
                                  /* IOCONCLKDIV0 */
                                  IOCON_PIO_CLKDIV0);
    /* PORT2 PIN (coords: ) is configured as  */
    IOCON_PinMuxSet(IOCON, 2, pio2_config);

    const uint32_t pio44_config = (/* Selects pull-up function */
                                   IOCON_PIO_MODE_PULLUP |
                                   /* Enable hysteresis */
                                   IOCON_PIO_HYS_EN |
                                   /* Input not invert */
                                   IOCON_PIO_INV_DI |
                                   /* Disables Open-drain function */
                                   IOCON_PIO_OD_DI |
                                   /* Bypass input filter */
                                   IOCON_PIO_SMODE_BYPASS |
                                   /* IOCONCLKDIV0 */
                                   IOCON_PIO_CLKDIV0);
    /* PORT4 PIN4 (coords: ) is configured as  */
    IOCON_PinMuxSet(IOCON, 44, pio44_config);

#if 1
    // ADC
    const uint32_t pio45_config = (/* Selects pull-up function */
                                   IOCON_PIO_MODE_PULLUP |
                                   /* Enable hysteresis */
                                   IOCON_PIO_HYS_EN |
                                   /* Input not invert */
                                   IOCON_PIO_INV_DI |
                                   /* Disables Open-drain function */
                                   IOCON_PIO_OD_DI |
                                   /* Bypass input filter */
                                   IOCON_PIO_SMODE_BYPASS |
                                   /* IOCONCLKDIV0 */
                                   IOCON_PIO_CLKDIV0);
    /* PORT4 PIN5 (coords: ) is configured as  */
    IOCON_PinMuxSet(IOCON, 45, pio45_config);
#endif

    /* USART0_TXD connect to P1_17 */
    SWM_SetMovablePinSelect(SWM0, kSWM_USART0_TXD, kSWM_PortPin_P1_17);

    /* USART0_RXD connect to P1_16 */
    SWM_SetMovablePinSelect(SWM0, kSWM_USART0_RXD, kSWM_PortPin_P1_16);

#if 1
    // ADC
    /* ADC_CHN0 connect to P0_7 */
    SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN0, true);
#endif

    /* Disable clock for switch matrix. */
    CLOCK_DisableClock(kCLOCK_Swm);
}
/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
