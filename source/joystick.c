/*
 * joystick.c
 *
 */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_mrt.h"
#include "pin_mux.h"
#include "fsl_adc.h"
#include "joystick.h"

#define ADC_BASE    ADC0

typedef struct joystick_struct {
    uint32_t    channel_;
    uint32_t	old_value_;
}   joystick_struct_t;

static joystick_struct_t    joysticks[] = {
#if 1
	{ .channel_ = 9, .old_value_ = 0 }
#else
    { .channel_ = 0, .old_value_ = 0 }
#endif
};

uint32_t init_joysticks(void)
{
    adc_config_t            adcConfigStruct;
    adc_conv_seq_config_t   adcConvSeqConfigStruct;

    /* Configure the converter. */
#if defined(FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE) & FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE
    adcConfigStruct.clockMode = kADC_ClockSynchronousMode; /* Using sync clock source. */
#endif                                                     /* FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE */
    adcConfigStruct.clockDividerNumber = 1;                /* The divider for sync clock is 2. */
#if defined(FSL_FEATURE_ADC_HAS_CTRL_RESOL) & FSL_FEATURE_ADC_HAS_CTRL_RESOL
    adcConfigStruct.resolution = kADC_Resolution12bit;	   // fsl_adc.h, 6/8/10/12 bit
#endif /* FSL_FEATURE_ADC_HAS_CTRL_RESOL */
#if defined(FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL) & FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL
    adcConfigStruct.enableBypassCalibration = false;
#endif /* FSL_FEATURE_ADC_HAS_CTRL_BYPASSCAL */
#if defined(FSL_FEATURE_ADC_HAS_CTRL_TSAMP) & FSL_FEATURE_ADC_HAS_CTRL_TSAMP
    adcConfigStruct.sampleTimeNumber = 0U;
#endif /* FSL_FEATURE_ADC_HAS_CTRL_TSAMP */
#if defined(FSL_FEATURE_ADC_HAS_CTRL_LPWRMODE) & FSL_FEATURE_ADC_HAS_CTRL_LPWRMODE
    adcConfigStruct.enableLowPowerMode = false;
#endif /* FSL_FEATURE_ADC_HAS_CTRL_LPWRMODE */
#if defined(FSL_FEATURE_ADC_HAS_TRIM_REG) & FSL_FEATURE_ADC_HAS_TRIM_REG
    adcConfigStruct.voltageRange = kADC_HighVoltageRange;
#endif /* FSL_FEATURE_ADC_HAS_TRIM_REG */
    ADC_Init(ADC_BASE, &adcConfigStruct);    // ADC0, LPC845.h

#if !(defined(FSL_FEATURE_ADC_HAS_NO_INSEL) && FSL_FEATURE_ADC_HAS_NO_INSEL)
    /* Use the temperature sensor input to channel 0. */
    ADC_EnableTemperatureSensor(ADC_BASE, true);
#endif /* FSL_FEATURE_ADC_HAS_NO_INSEL. */

    /* Enable channel ADC_SAMPLE_CHANNEL_NUMBER's conversion in Sequence A. */
    uint32_t    channelMask = 0x00;
    for (size_t i = 0; i < sizeof joysticks / sizeof joysticks[0]; ++i) {
        channelMask |= (1U << joysticks[i].channel_);
    }
    adcConvSeqConfigStruct.channelMask = channelMask;
    adcConvSeqConfigStruct.triggerMask = 0U;
    adcConvSeqConfigStruct.triggerPolarity = kADC_TriggerPolarityPositiveEdge;
    adcConvSeqConfigStruct.enableSingleStep = false;
    adcConvSeqConfigStruct.enableSyncBypass = false;
    adcConvSeqConfigStruct.interruptMode = kADC_InterruptForEachSequence;
    ADC_SetConvSeqAConfig(ADC_BASE, &adcConvSeqConfigStruct);
    ADC_EnableConvSeqA(ADC_BASE, true); /* Enable the conversion sequence A. */
    /* Clear the result register. */
    ADC_DoSoftwareTriggerConvSeqA(ADC_BASE);
    adc_result_info_t adcResultInfoStruct;
    for (size_t i = 0; i < sizeof joysticks / sizeof joysticks[0]; ++i) {
        while (!ADC_GetChannelConversionResult(ADC_BASE, joysticks[i].channel_, &adcResultInfoStruct))
        	;
    }
    ADC_GetConvSeqAGlobalConversionResult(ADC_BASE, &adcResultInfoStruct);
    return sizeof joysticks / sizeof joysticks[0];
}

static joystick_value_t value[] = {
		{ .id_ = 0 }
};

joystick_value_t* joystick_task(void)
{
	adc_result_info_t adcResultInfoStruct;

    ADC_DoSoftwareTriggerConvSeqA(ADC_BASE);
	for (size_t i = 0; i < sizeof joysticks / sizeof joysticks[0]; ++i) {
        //while (!ADC_GetChannelConversionResult(ADC_BASE, joysticks[i].channel_, &adcResultInfoStruct))
        //	;
		ADC_GetChannelConversionResult(ADC_BASE, joysticks[i].channel_, &adcResultInfoStruct);
		uint32_t v = adcResultInfoStruct.result;
		if (v > joysticks[i].old_value_ + 5 || v < joysticks[i].old_value_ - 5) {
			value[i].value_ = v;
			value[i].ready_ = true;
			joysticks[i].old_value_ = v;
		}
		else
			value[i].ready_ = false;
	}
	return value;
}

