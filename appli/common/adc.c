/*
 * adc.c
 *
 *  Created on: 9 juin 2020
 *      Author: Nirgal
 */

#include "adc.h"

#if USE_ADC

//TODO généraliser pour tout les canaux...
#define ADC_MAX_NB_CHANNELS	16

static nrfx_saadc_config_t adc_config;
static nrf_saadc_channel_config_t adc_channels[ADC_USE_CHANNELS_NB];
static uint8_t channel_nb = 0;
static bool_e adc_initialized = FALSE;

static int8_t channel_pin_to_index[ADC_MAX_NB_CHANNELS];

void ADC_init(void)
{
	adc_config = (nrfx_saadc_config_t)NRFX_SAADC_DEFAULT_CONFIG;
	nrfx_saadc_init(&adc_config, NULL);
	channel_nb = 0;

#if USE_ADC_CHANNEL_AIN0
	adc_channels[channel_nb] = (nrf_saadc_channel_config_t)NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN0);	//P0.02 -> AN0
	adc_channels[channel_nb].acq_time = SAADC_CH_CONFIG_TACQ_40us;
	nrfx_saadc_channel_init(channel_nb, &adc_channels[channel_nb]);
	channel_pin_to_index[0] = channel_nb;
	channel_nb++;
#endif

#if USE_ADC_CHANNEL_AIN1
	adc_channels[channel_nb] = (nrf_saadc_channel_config_t)NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN1);	//P0.03 -> AN1
	adc_channels[channel_nb].acq_time = SAADC_CH_CONFIG_TACQ_40us;
	nrfx_saadc_channel_init(channel_nb, &adc_channels[channel_nb]);
	channel_pin_to_index[1] = channel_nb;
	channel_nb++;

#endif

#if USE_ADC_CHANNEL_AIN2

#endif

#if USE_ADC_CHANNEL_AIN7
	adc_channels[channel_nb] = (nrf_saadc_channel_config_t)NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN7);	//P0.31 -> AN7
	adc_channels[channel_nb].acq_time = SAADC_CH_CONFIG_TACQ_40us;
	nrfx_saadc_channel_init(channel_nb, &adc_channels[channel_nb]);
	channel_pin_to_index[7] = channel_nb;
	channel_nb++;

#endif
	adc_initialized = TRUE;
}

void ADC_read(uint8_t channel, nrf_saadc_value_t * value)
{
	if(!adc_initialized)
		ADC_init();
	uint8_t index = channel_pin_to_index[channel];
	nrfx_saadc_sample_convert(index, value);
}


#endif

