#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "fsl_adc.h"
#include "fsl_clock.h"
#include "fsl_power.h"
#include "fsl_mrt.h"
#include "fsl_common.h"
#include "fsl_iocon.h"
#include "fsl_swm.h"
#include "fsl_swm_connections.h"
#include "adc-driver.h"

static bool 				adc_did_init_ = false;

#define ADC_BASE    		ADC0
#define ADC_CLOCK_SOURCE 	kCLOCK_Fro


static void adc_pin_config(uint32_t pin, swm_select_fixed_pin_t sel);

void adc_init(adc_sample_bit_t sample_bit)
{
	if (!adc_did_init_) {
	    // ADC code
	    CLOCK_Select(kADC_Clk_From_Fro);
	    CLOCK_SetClkDivider(kCLOCK_DivAdcClk, 1U);
	    POWER_DisablePD(kPDRUNCFG_PD_ADC0);

	    uint32_t    freq;
	    freq = CLOCK_GetFreq(ADC_CLOCK_SOURCE) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk);
	    if (ADC_DoSelfCalibration(ADC0, freq))
	    {
	        PRINTF("ADC_DoSelfCalibration() Done.\r\n");
	    }
	    else
	    {
	        PRINTF("ADC_DoSelfCalibration() Failed.\r\n");
	    }

	    adc_config_t  adcConfigStruct;

#if defined(FSL_FEATURE_ADC_HAS_CTRL_RESOL) & FSL_FEATURE_ADC_HAS_CTRL_RESOL
	    adc_resolution_t adc_res[] = {
	    		// fsl_adc.h
	    		kADC_Resolution6bit = 0U,  /*!< 6-bit resolution. */
	    	    kADC_Resolution8bit = 1U,  /*!< 8-bit resolution. */
	    	    kADC_Resolution10bit = 2U, /*!< 10-bit resolution. */
	    	    kADC_Resolution12bit = 3U, /*!< 12-bit resolution. */
	    };
#endif

	    /* Configure the converter. */
#if defined(FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE) & FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE
	    adcConfigStruct.clockMode = kADC_ClockSynchronousMode; /* Using sync clock source. */
#endif                                                     /* FSL_FEATURE_ADC_HAS_CTRL_ASYNMODE */
	    adcConfigStruct.clockDividerNumber = 1;                /* The divider for sync clock is 2. */
#if defined(FSL_FEATURE_ADC_HAS_CTRL_RESOL) & FSL_FEATURE_ADC_HAS_CTRL_RESOL
	    adcConfigStruct.resolution = adc_res[sample_bit];
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
	    adc_did_init_ = true;
	}
}

void adc_channels_init(adc_channel_t* channel, size_t n)
{
	/*
	 * ADC channel to LPC845 pin map
	 * channel 0 - 11
	 * https://www.nxp.com/docs/en/data-sheet/LPC84x.pdf
	 */
	static uint32_t chn_pin[] = {
			45, 46, 49, 51, 55, 57,
			58, 60, 61, 63, 2 , 6
	};

	/*
	 * fsl_swm_connections.h
	 */
	static swm_select_fixed_pin_t chn_config[] = {
			kSWM_ADC_CHN0,
			kSWM_ADC_CHN1,
			kSWM_ADC_CHN2,
			kSWM_ADC_CHN3,
			kSWM_ADC_CHN4,
			kSWM_ADC_CHN5,
			kSWM_ADC_CHN6,
			kSWM_ADC_CHN7,
			kSWM_ADC_CHN8,
			kSWM_ADC_CHN9,
			kSWM_ADC_CHN10,
			kSWM_ADC_CHN11,
	};

	uint32_t 	mask = 0U;
	uint32_t	chn;
	for (size_t i = 0; i < n; ++i) {
		chn = channel[i].channel_;
		adc_pin_config(chn_pin[chn], chn_config[chn]);
		mask |= (1U << channel[i].channel_);
	}

    adc_conv_seq_config_t   adcConvSeqConfigStruct;

    adcConvSeqConfigStruct.channelMask = mask;
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
    for (size_t i = 0; i < n; ++i) {
        while (!ADC_GetChannelConversionResult(ADC_BASE, channel[i].channel_, &adcResultInfoStruct))
        	;
    }
    ADC_GetConvSeqAGlobalConversionResult(ADC_BASE, &adcResultInfoStruct);
}

void adc_read(adc_channel_t* channel, adc_value_t* value, size_t n)
{
	adc_result_info_t adcResultInfoStruct;

    ADC_DoSoftwareTriggerConvSeqA(ADC_BASE);
	for (size_t i = 0; i < n; ++i) {
        //while (!ADC_GetChannelConversionResult(ADC_BASE, channel[i].channel_, &adcResultInfoStruct))
        //	;
		ADC_GetChannelConversionResult(ADC_BASE, channel[i].channel_, &adcResultInfoStruct);
		value[i].value_ = adcResultInfoStruct.result;
	}
}

static void adc_pin_config(uint32_t pin, swm_select_fixed_pin_t sel)
{

	/*
	 * fsl_iocon.h
	 */
    CLOCK_EnableClock(kCLOCK_Iocon);
    CLOCK_EnableClock(kCLOCK_Swm);

    const uint32_t pin_config = (/* Selects pull-up function */
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

    IOCON_PinMuxSet(IOCON, pin, pin_config);

    SWM_SetFixedPinSelect(SWM0, sel, true);	// fsl_swm_connections.h

    CLOCK_DisableClock(kCLOCK_Swm);

}
