#ifndef ADC_DRIVER_H_
#define ADC_DRIVER_H_

typedef enum adc_sample_bit {
	adc_sample_6bit,
	adc_sample_8bit,
	adc_sample_10bit,
	adc_sample_12bit
}	adc_sample_bit_t;

typedef struct adc_channel {
	uint32_t	channel_;
}	adc_channel_t;

typedef struct adc_value {
	uint32_t	value_;
}	adc_value_t;

void adc_init(adc_sample_bit_t);
void adc_channels_init(adc_channel_t*, size_t);
void adc_read(adc_channel_t*, adc_value_t*, size_t);

#endif
