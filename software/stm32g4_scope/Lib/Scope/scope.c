/*
 * scope.c
 *
 *  Created on: Jul 26, 2023
 *      Author: jgpei
 */


#include "scope.h"
/*
static tScope *_scope = NULL;

#include <stdio.h>
#include <stdlib.h>

void scope_init_ll( tScope *scope,
		TIM_HandleTypeDef *htim_clock,
		TIM_HandleTypeDef *htim_stop,
		DAC_HandleTypeDef *hdac,
		OPAMP_HandleTypeDef *hopamp1,
		OPAMP_HandleTypeDef *hopamp2,
		OPAMP_HandleTypeDef *hopamp3,
		OPAMP_HandleTypeDef *hopamp4,
		ADC_HandleTypeDef *hadc1,
		ADC_HandleTypeDef *hadc2,
		ADC_HandleTypeDef *hadc3,
		ADC_HandleTypeDef *hadc4
	)
{
	scope->horizontal.htim_clock = htim_clock;
	scope->horizontal.htim_stop = htim_stop;
	scope->vertical.hdac = hdac;
	scope->vertical.hopamp1 = hopamp1;
	scope->vertical.hopamp2 = hopamp2;
	scope->vertical.hopamp3 = hopamp3;
	scope->vertical.hopamp4 = hopamp4;
	scope->trigger.hadc1 = hadc1;
	scope->trigger.hadc2 = hadc2;
	scope->trigger.hadc3 = hadc3;
	scope->trigger.hadc4 = hadc4;
}
uint8_t scope_is_running( tScope *scope )
{
	return scope->state != SCOPE_STATE_DONE;
}

uint8_t scope_wait( tScope *scope, uint32_t timeout_ms )
{
	uint32_t start = HAL_GetTick();
	while( scope_is_busy( scope ) )
	{
		if( HAL_GetTick() - start > timeout_ms )
		{
			return 0;
		}
	}
	return 1;
}

void get_prescaler_and_period(int clock, int freq, int *prescaler, int *period) {
    int best_prescaler = 0;
    int best_period = 0;
    double min_error = 1e9;  // A large initial value

    for (int prescaler_val = 1; prescaler_val <= 65535; prescaler_val++) {
        int period_val = (int)(0.5 + (double)clock / (prescaler_val * freq));

        // Limit the period to fit within timer limitations
        if (period_val > 65535) {
            period_val = 65535;
        }

        // Calculate the actual frequency using the found prescaler and period
        double actual_freq = (double)clock / (prescaler_val * period_val);

        double error = abs(actual_freq - (double)freq);

        if (error < min_error) {
            min_error = error;
            best_prescaler = prescaler_val;
            best_period = period_val;
        }
    }

    *prescaler = best_prescaler;
    *period = best_period;
}

#include "integer_factorization.h"
void scope_config_horizontal( tScope *scope, int sample_rate, int buffer_len )
{
    // This function configures the timers so that the ADC is triggered at the frequency and to stop the conversion when the buffer is completed.
    int tim_freq = 170e6;
    int prescaler = 0;
    int period = 0;
*/
    /*get_prescaler_and_period( tim_freq, sample_rate, &prescaler, &period );
    // Compute required Prescaler and Period for the ADC clock.
    scope->horizontal.htim_clock->Init.Prescaler = prescaler-1;
    scope->horizontal.htim_clock->Init.Period = period-1;
    HAL_TIM_Base_Init( scope->horizontal.htim_clock );

    get_prescaler_and_period( tim_freq, sample_rate/512, &prescaler, &period );
    // Compute required Prescaler and Period for the Stop timer.
    scope->horizontal.htim_stop->Init.Prescaler = prescaler-1;
    scope->horizontal.htim_stop->Init.Period = period-1;
    HAL_TIM_Base_Init( scope->horizontal.htim_stop );
	*/
/*
    if( sample_rate == 0 )
    {
    	sample_rate = 1;
    }
    //uint32_t n = 2*(tim_freq / sample_rate);
    //uint32_t mx = 65536;
    //uint32_t a = 0;
    //uint32_t b = 0;
    //integer_factorization( n, mx, &a, &b );

    // Compute required Prescaler and Period for the ADC clock.
    scope->horizontal.htim_clock->Init.Prescaler = (tim_freq / sample_rate)/2 - 1;
    scope->horizontal.htim_clock->Init.Period = 1;
    //scope->horizontal.htim_clock->Init.Prescaler = a - 1;
    //scope->horizontal.htim_clock->Init.Period = 2*b - 1;
    HAL_TIM_Base_Init( scope->horizontal.htim_clock );

    // Compute required Prescaler and Period for the Stop timer.
    scope->horizontal.htim_stop->Init.Prescaler = (tim_freq / sample_rate)/2 - 1;
    scope->horizontal.htim_stop->Init.Period = buffer_len - 1;
    //scope->horizontal.htim_stop->Init.Prescaler = a - 1;
    //scope->horizontal.htim_stop->Init.Period = 2*b*buffer_len - 1;
    HAL_TIM_Base_Init( scope->horizontal.htim_stop );

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_TIMING;
    sConfigOC.Pulse = buffer_len - 1;
    //sConfigOC.Pulse = 2*b*buffer_len - 1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_OC_ConfigChannel(scope->horizontal.htim_stop, &sConfigOC, TIM_CHANNEL_1);

}

int PgaGain_from_gain[6] =
{ // Table to convert gain to OPAMP PGA gain.{
    OPAMP_PGA_GAIN_2_OR_MINUS_1,
    OPAMP_PGA_GAIN_4_OR_MINUS_3,
    OPAMP_PGA_GAIN_8_OR_MINUS_7,
    OPAMP_PGA_GAIN_16_OR_MINUS_15,
    OPAMP_PGA_GAIN_32_OR_MINUS_31,
    OPAMP_PGA_GAIN_64_OR_MINUS_63,
};

int gain_to_pgagain( int gain )
{
    // convert gain from 1, 2, 5, 10, 20, 50 to a values from 0 t0 5.
    if( 0 <= gain && gain <= 1 )
    {
        return 0;
    }
    else if( 1 < gain && gain <= 5 )
    {
        return 1;
    }
    else if( 5 <= gain && gain <= 10 )
    {
        return 2;
    }
    else if( 10 <= gain && gain <= 20 )
    {
        return 3;
    }
    else if( 20 <= gain && gain <= 50 )
    {
        return 4;
    }
    else if( 50 <= gain )
    {
        return 5;
    }
    else
    {
        return 0;
    }
}

void scope_config_vertical( tScope *scope, int gain1, int gain2, int gain3, int gain4, int offset )
{
	static int last_PgaGain1 = -1;
	static int last_PgaGain2 = -1;
	static int last_PgaGain3 = -1;
	static int last_PgaGain4 = -1;
    // This function configures the OPAMPs and DACs for the vertical channels.
*/    /*gain1 = gain_to_pgagain( gain1 );
    gain2 = gain_to_pgagain( gain2 );
    gain3 = gain_to_pgagain( gain3 );
    gain4 = gain_to_pgagain( gain4 );*/
/*
    scope->vertical.hopamp1->Init.PgaGain = PgaGain_from_gain[gain1];
    scope->vertical.hopamp2->Init.PgaGain = PgaGain_from_gain[gain2];
    scope->vertical.hopamp3->Init.PgaGain = PgaGain_from_gain[gain3];
    scope->vertical.hopamp4->Init.PgaGain = PgaGain_from_gain[gain4];

    HAL_OPAMP_Stop( scope->vertical.hopamp1 );
    HAL_OPAMP_Stop( scope->vertical.hopamp2 );
    HAL_OPAMP_Stop( scope->vertical.hopamp3 );
    HAL_OPAMP_Stop( scope->vertical.hopamp4 );

    HAL_OPAMP_Init( scope->vertical.hopamp1 );
    HAL_OPAMP_Init( scope->vertical.hopamp2 );
    HAL_OPAMP_Init( scope->vertical.hopamp3 );
    HAL_OPAMP_Init( scope->vertical.hopamp4 );

    if( last_PgaGain1 != scope->vertical.hopamp1->Init.PgaGain )
    {
    	HAL_OPAMP_SelfCalibrate( scope->vertical.hopamp1 );
    	last_PgaGain1 = scope->vertical.hopamp1->Init.PgaGain;
    }
    if( last_PgaGain2 != scope->vertical.hopamp2->Init.PgaGain )
    {
    	HAL_OPAMP_SelfCalibrate( scope->vertical.hopamp2 );
    	last_PgaGain2 = scope->vertical.hopamp2->Init.PgaGain;
    }
    if( last_PgaGain3 != scope->vertical.hopamp3->Init.PgaGain )
    {
    	HAL_OPAMP_SelfCalibrate( scope->vertical.hopamp3 );
    	last_PgaGain3 = scope->vertical.hopamp3->Init.PgaGain;
    }
    if( last_PgaGain4 != scope->vertical.hopamp4->Init.PgaGain )
    {
    	HAL_OPAMP_SelfCalibrate( scope->vertical.hopamp4 );
    	last_PgaGain4 = scope->vertical.hopamp4->Init.PgaGain;
    }

    HAL_OPAMP_Start( scope->vertical.hopamp1 );
    HAL_OPAMP_Start( scope->vertical.hopamp2 );
    HAL_OPAMP_Start( scope->vertical.hopamp3 );
    HAL_OPAMP_Start( scope->vertical.hopamp4 );

    HAL_DAC_SetValue( scope->vertical.hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, offset );
	HAL_DAC_Start( scope->vertical.hdac, DAC_CHANNEL_1 );
}

#include "ui.h"
int _channel = 0;
void scope_config_trigger( tScope *scope, int channel, int mode, int level, int slope )
{
	_channel = channel;
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_arm_1 = {0};
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_arm_2 = {0};
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_arm_3 = {0};
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_arm_4 = {0};

    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_trig_1 = {0};
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_trig_2 = {0};
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_trig_3 = {0};
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_trig_4 = {0};

    AnalogWDGConfig_arm_1.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    AnalogWDGConfig_arm_1.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
    AnalogWDGConfig_arm_1.Channel = 0;
    AnalogWDGConfig_arm_1.ITMode = ENABLE;
    AnalogWDGConfig_arm_1.HighThreshold = 4095;
    AnalogWDGConfig_arm_1.LowThreshold = 0;
    AnalogWDGConfig_arm_1.FilteringConfig = ADC_AWD_FILTERING_NONE;

    AnalogWDGConfig_arm_2 = AnalogWDGConfig_arm_1;
    AnalogWDGConfig_arm_3 = AnalogWDGConfig_arm_1;
    AnalogWDGConfig_arm_4 = AnalogWDGConfig_arm_1;

    AnalogWDGConfig_trig_1.WatchdogNumber = ADC_ANALOGWATCHDOG_2;
    AnalogWDGConfig_trig_1.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REGINJEC;
    AnalogWDGConfig_trig_1.Channel = 0;
    AnalogWDGConfig_trig_1.ITMode = ENABLE;
    AnalogWDGConfig_trig_1.HighThreshold = 4095;
    AnalogWDGConfig_trig_1.LowThreshold = 0;
    AnalogWDGConfig_trig_1.FilteringConfig = ADC_AWD_FILTERING_NONE;

    AnalogWDGConfig_trig_2 = AnalogWDGConfig_trig_1;
    AnalogWDGConfig_trig_3 = AnalogWDGConfig_trig_1;
    AnalogWDGConfig_trig_4 = AnalogWDGConfig_trig_1;

    AnalogWDGConfig_arm_1.Channel = ADC_CHANNEL_VOPAMP1;
    AnalogWDGConfig_arm_2.Channel = ADC_CHANNEL_VOPAMP3_ADC3;
    AnalogWDGConfig_arm_3.Channel = ADC_CHANNEL_VOPAMP5;
    AnalogWDGConfig_arm_4.Channel = ADC_CHANNEL_VOPAMP6;

    AnalogWDGConfig_trig_1.Channel = ADC_CHANNEL_VOPAMP1;
    AnalogWDGConfig_trig_2.Channel = ADC_CHANNEL_VOPAMP3_ADC3;
    AnalogWDGConfig_trig_3.Channel = ADC_CHANNEL_VOPAMP5;
    AnalogWDGConfig_trig_4.Channel = ADC_CHANNEL_VOPAMP6;

    // Configure the required for arm the oscilloscope.
    if( mode == UI_TRIGGER_MODE_AUTO )
    {
        AnalogWDGConfig_arm_1.HighThreshold = 0;
        AnalogWDGConfig_arm_1.LowThreshold = 0;
        AnalogWDGConfig_trig_1.HighThreshold = 0;
        AnalogWDGConfig_trig_1.LowThreshold = 0;
    }
    if( mode == UI_TRIGGER_MODE_NORMAL )
    {
        if( slope == UI_TRIGGER_SLOPE_RISING ) // Note: PGA gain is negative.
        {
            if( channel == 0 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_1.HighThreshold = 4095;
                AnalogWDGConfig_arm_1.LowThreshold = level;
                AnalogWDGConfig_trig_1.HighThreshold = level;
                AnalogWDGConfig_trig_1.LowThreshold = 0;
            }
            else if( channel == 1 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_2.HighThreshold = 4095;
                AnalogWDGConfig_arm_2.LowThreshold = level;
                AnalogWDGConfig_trig_2.HighThreshold = level;
                AnalogWDGConfig_trig_2.LowThreshold = 0;
            }
            else if( channel == 2 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_3.HighThreshold = 4095;
                AnalogWDGConfig_arm_3.LowThreshold = level;
                AnalogWDGConfig_trig_3.HighThreshold = level;
                AnalogWDGConfig_trig_3.LowThreshold = 0;
            }
            else if( channel == 3 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_4.HighThreshold = 4095;
                AnalogWDGConfig_arm_4.LowThreshold = level;
                AnalogWDGConfig_trig_4.HighThreshold = level;
                AnalogWDGConfig_trig_4.LowThreshold = 0;
            }
        }
        else
        {
            if( channel == 0 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_1.HighThreshold = level;
                AnalogWDGConfig_arm_1.LowThreshold = 0;
                AnalogWDGConfig_trig_1.HighThreshold = 4095;
                AnalogWDGConfig_trig_1.LowThreshold = level;
            }
            else if( channel == 1 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_2.HighThreshold = level;
                AnalogWDGConfig_arm_2.LowThreshold = 0;
                AnalogWDGConfig_trig_2.HighThreshold = 4095;
                AnalogWDGConfig_trig_2.LowThreshold = level;
            }
            else if( channel == 2 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_3.HighThreshold = level;
                AnalogWDGConfig_arm_3.LowThreshold = 0;
                AnalogWDGConfig_trig_3.HighThreshold = 4095;
                AnalogWDGConfig_trig_3.LowThreshold = level;
            }
            else if( channel == 3 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_4.HighThreshold = level;
                AnalogWDGConfig_arm_4.LowThreshold = 0;
                AnalogWDGConfig_trig_4.HighThreshold = 4095;
                AnalogWDGConfig_trig_4.LowThreshold = level;
            }
        }
    }

    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc1, &AnalogWDGConfig_arm_1 );
    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc2, &AnalogWDGConfig_arm_2 );
    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc3, &AnalogWDGConfig_arm_3 );
    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc4, &AnalogWDGConfig_arm_4 );

    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc1, &AnalogWDGConfig_trig_1 );
    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc2, &AnalogWDGConfig_trig_2 );
    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc3, &AnalogWDGConfig_trig_3 );
    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc4, &AnalogWDGConfig_trig_4 );

    // disable ADCs irq. Leave only the selected channel enabled.
    __HAL_ADC_DISABLE_IT( scope->trigger.hadc1, ADC_IT_AWD1 );
    __HAL_ADC_DISABLE_IT( scope->trigger.hadc2, ADC_IT_AWD1 );
    __HAL_ADC_DISABLE_IT( scope->trigger.hadc3, ADC_IT_AWD1 );
    __HAL_ADC_DISABLE_IT( scope->trigger.hadc4, ADC_IT_AWD1 );

    __HAL_ADC_DISABLE_IT( scope->trigger.hadc1, ADC_IT_AWD2 );
    __HAL_ADC_DISABLE_IT( scope->trigger.hadc2, ADC_IT_AWD2 );
    __HAL_ADC_DISABLE_IT( scope->trigger.hadc3, ADC_IT_AWD2 );
    __HAL_ADC_DISABLE_IT( scope->trigger.hadc4, ADC_IT_AWD2 );

    // enable only the selected channel.
    if( channel == 0 )
    {
        __HAL_ADC_ENABLE_IT( scope->trigger.hadc1, ADC_IT_AWD1 );
        //__HAL_ADC_ENABLE_IT( scope->trigger.hadc1, ADC_IT_AWD2 );
    }
    else if( channel == 1 )
    {
        __HAL_ADC_ENABLE_IT( scope->trigger.hadc2, ADC_IT_AWD1 );
        //__HAL_ADC_ENABLE_IT( scope->trigger.hadc2, ADC_IT_AWD2 );
    }
    else if( channel == 2 )
    {
        __HAL_ADC_ENABLE_IT( scope->trigger.hadc3, ADC_IT_AWD1 );
        //__HAL_ADC_ENABLE_IT( scope->trigger.hadc3, ADC_IT_AWD2 );
    }
    else if( channel == 3 )
    {
        __HAL_ADC_ENABLE_IT( scope->trigger.hadc4, ADC_IT_AWD1 );
        //__HAL_ADC_ENABLE_IT( scope->trigger.hadc4, ADC_IT_AWD2 );
    }
}

void scope_init( tScope *scope, uint16_t trigger_level, uint32_t sample_rate, uint16_t *buffer1, uint16_t *buffer2, uint16_t *buffer3, uint16_t *buffer4, uint16_t len )
{
	scope->trigger_level = trigger_level;
	scope->sample_rate = sample_rate;
	scope->buffer1 = buffer1;
	scope->buffer2 = buffer2;
	scope->buffer3 = buffer3;
	scope->buffer4 = buffer4;
	scope->len = len;
	scope->trigger_location = 0;
	scope->state = SCOPE_STATE_RESET;

	_scope = scope;

	extern OPAMP_HandleTypeDef hopamp1;
	extern OPAMP_HandleTypeDef hopamp3;
	extern OPAMP_HandleTypeDef hopamp5;
	extern OPAMP_HandleTypeDef hopamp6;
	extern ADC_HandleTypeDef hadc1;
	extern ADC_HandleTypeDef hadc3;
	extern ADC_HandleTypeDef hadc5;
	extern ADC_HandleTypeDef hadc4;
	extern DMA_HandleTypeDef hdma_adc1;
	extern DMA_HandleTypeDef hdma_adc3;
	extern DMA_HandleTypeDef hdma_adc5;
	extern DMA_HandleTypeDef hdma_adc4;
	extern TIM_HandleTypeDef htim2;
	extern TIM_HandleTypeDef htim3;


	scope->hopamp1 = &hopamp1;
	scope->hopamp2 = &hopamp3;
	scope->hopamp3 = &hopamp5;
	scope->hopamp4 = &hopamp6;
	scope->hadc1 = &hadc1;
	scope->hadc2 = &hadc3;
	scope->hadc3 = &hadc5;
	scope->hadc4 = &hadc4;
	scope->hdma_adc1 = &hdma_adc1;
	scope->hdma_adc2 = &hdma_adc3;
	scope->hdma_adc3 = &hdma_adc5;
	scope->hdma_adc4 = &hdma_adc4;
	scope->htim1 = &htim2;
	scope->htim2 = &htim3;

	//config_hadc1( trigger_level, buffer, len);
	//config_htim1( samplerate )
	//config_htim2( samplerate, len )

	//MX_GPIO_Init();
	//MX_DMA_Init();
	//MX_ADC1_Init();
	//MX_OPAMP1_Init();
	//MX_TIM1_Init();
	//MX_TIM2_Init();

	extern DAC_HandleTypeDef hdac2;
	scope->horizontal.htim_clock = &htim2;
	scope->horizontal.htim_stop =  &htim3;
	scope->vertical.hdac = &hdac2;
	scope->vertical.hopamp1 = &hopamp1;
	scope->vertical.hopamp2 = &hopamp3;
	scope->vertical.hopamp3 = &hopamp5;
	scope->vertical.hopamp4 = &hopamp6;
	scope->trigger.hadc1 = &hadc1;
	scope->trigger.hadc2 = &hadc3;
	scope->trigger.hadc3 = &hadc5;
	scope->trigger.hadc4 = &hadc4;

}

void scope_reset( tScope *scope )
{
	scope->CNDTRs[SCOPE_STATE_WAIT_FOR_STOP] = 0;
	scope->state = SCOPE_STATE_RESET;
}

void scope_start( tScope *scope )
{
	for( int i = 0; i < SCOPE_STATE_MAX ; i++ )
	{
		scope->CNDTRs[i] = 0;
	}

	__HAL_DBGMCU_FREEZE_TIM1();
	__HAL_DBGMCU_FREEZE_TIM2();

	//extern DAC_HandleTypeDef hdac2;
	//int vcc = 3.3;
	//HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (vcc/2.0)/vcc*4095);
	//HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);


	extern OPAMP_HandleTypeDef hopamp1;
	extern OPAMP_HandleTypeDef hopamp3;
	extern OPAMP_HandleTypeDef hopamp5;
	extern OPAMP_HandleTypeDef hopamp6;

	HAL_OPAMP_SelfCalibrate(&hopamp1);
	HAL_OPAMP_SelfCalibrate(&hopamp3);
	HAL_OPAMP_SelfCalibrate(&hopamp5);
	HAL_OPAMP_SelfCalibrate(&hopamp6);

	HAL_OPAMP_Start(&hopamp1);
	HAL_OPAMP_Start(&hopamp3);
	HAL_OPAMP_Start(&hopamp5);
	HAL_OPAMP_Start(&hopamp6);

	HAL_ADCEx_Calibration_Start( scope->hadc1, ADC_SINGLE_ENDED );
	HAL_ADCEx_Calibration_Start( scope->hadc2, ADC_SINGLE_ENDED );
	HAL_ADCEx_Calibration_Start( scope->hadc3, ADC_SINGLE_ENDED );
	HAL_ADCEx_Calibration_Start( scope->hadc4, ADC_SINGLE_ENDED );

	if( _channel == 0 )
	{
		LL_ADC_DisableIT_AWD1( scope->hadc1->Instance );
		LL_ADC_DisableIT_AWD2( scope->hadc1->Instance );
		__HAL_ADC_CLEAR_FLAG( scope->hadc1, ADC_FLAG_AWD1 );
		__HAL_ADC_CLEAR_FLAG( scope->hadc1, ADC_FLAG_AWD2 );
	}
	if( _channel == 1 )
	{
		LL_ADC_DisableIT_AWD1( scope->hadc2->Instance );
		LL_ADC_DisableIT_AWD2( scope->hadc2->Instance );
		__HAL_ADC_CLEAR_FLAG( scope->hadc2, ADC_FLAG_AWD1 );
		__HAL_ADC_CLEAR_FLAG( scope->hadc2, ADC_FLAG_AWD2 );
	}
	if( _channel == 2 )
	{
		LL_ADC_DisableIT_AWD1( scope->hadc3->Instance );
		LL_ADC_DisableIT_AWD2( scope->hadc3->Instance );
		__HAL_ADC_CLEAR_FLAG( scope->hadc3, ADC_FLAG_AWD1 );
		__HAL_ADC_CLEAR_FLAG( scope->hadc3, ADC_FLAG_AWD2 );
	}
	if( _channel == 3 )
	{
		LL_ADC_DisableIT_AWD1( scope->hadc4->Instance );
		LL_ADC_DisableIT_AWD2( scope->hadc4->Instance );
		__HAL_ADC_CLEAR_FLAG( scope->hadc4, ADC_FLAG_AWD1 );
		__HAL_ADC_CLEAR_FLAG( scope->hadc4, ADC_FLAG_AWD2 );
	}

	scope->htim1->Instance->CNT = 0;
	scope->htim2->Instance->CNT = 0;

	HAL_ADC_Start_DMA( scope->hadc1, (uint32_t*)scope->buffer1, scope->len );
	HAL_ADC_Start_DMA( scope->hadc2, (uint32_t*)scope->buffer2, scope->len );
	HAL_ADC_Start_DMA( scope->hadc3, (uint32_t*)scope->buffer3, scope->len );
	HAL_ADC_Start_DMA( scope->hadc4, (uint32_t*)scope->buffer4, scope->len );
	HAL_TIM_Base_Start( scope->htim1 );
	scope->state = SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE;
}

void scope_stop( tScope *scope )
{
	HAL_TIM_Base_Stop( scope->htim1 );

	HAL_TIM_OnePulse_Stop_IT( scope->htim2, TIM_CHANNEL_1 );
	HAL_TIM_Base_Stop( scope->htim2 );

	HAL_ADC_Stop_DMA( scope->hadc1 );
	HAL_ADC_Stop_DMA( scope->hadc2 );
	HAL_ADC_Stop_DMA( scope->hadc3 );
	HAL_ADC_Stop_DMA( scope->hadc4 );

	scope->state = SCOPE_STATE_DONE;
}

uint8_t scope_is_busy( tScope *scope )
{
	if( scope->state != SCOPE_STATE_DONE )
	{
		return 1;
	}
	else
	{
		return 0;
	}
	//return scope->state != SCOPE_STATE_DONE;
}

extern int DMA1_Channel1_CNDTR;
int32_t scope_get_trigger( tScope *scope )
{
	//return scope->len - scope->CNDTRs[SCOPE_STATE_WAIT_FOR_STOP];
	return scope->len - scope->CNDTRs[SCOPE_STATE_DONE] - 256;
	//return scope->len - DMA1_Channel1_CNDTR;
}

// ********************** IRQs ********************** //
void _HAL_ADC_ConvHalfCpltCallback( ADC_HandleTypeDef* hadc )
{
	if( _scope && hadc == _scope->hadc1 && _channel == 0 )
	{
	}
	else if( _scope && hadc == _scope->hadc2 && _channel == 1 )
	{
	}
	else if( _scope && hadc == _scope->hadc3 && _channel == 2 )
	{
	}
	else if( _scope && hadc == _scope->hadc4 && _channel == 3 )
	{
	}
}

void _HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef* hadc )
{
	if( _scope && hadc == _scope->hadc1 && _channel == 0 )
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE )
		{
			_scope->state = SCOPE_STATE_WAIT_FOR_ARM;
			_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_ARM] = _scope->hdma_adc1->Instance->CNDTR;
			__HAL_ADC_CLEAR_FLAG( _scope->hadc1, ADC_FLAG_AWD1 );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc1, ADC_FLAG_AWD2 );
			LL_ADC_EnableIT_AWD1( _scope->hadc1->Instance );
		}
	}
	else if( _scope && hadc == _scope->hadc2 && _channel == 1 )
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE )
		{
			_scope->state = SCOPE_STATE_WAIT_FOR_ARM;
			_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_ARM] = _scope->hdma_adc2->Instance->CNDTR;
			__HAL_ADC_CLEAR_FLAG( _scope->hadc2, ADC_FLAG_AWD1 );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc2, ADC_FLAG_AWD2 );
			LL_ADC_EnableIT_AWD1( _scope->hadc2->Instance );
		}
	}
	else if( _scope && hadc == _scope->hadc3 && _channel == 2 )
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE )
		{
			_scope->state = SCOPE_STATE_WAIT_FOR_ARM;
			_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_ARM] = _scope->hdma_adc3->Instance->CNDTR;
			__HAL_ADC_CLEAR_FLAG( _scope->hadc3, ADC_FLAG_AWD1 );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc3, ADC_FLAG_AWD2 );
			LL_ADC_EnableIT_AWD1( _scope->hadc3->Instance );
		}
	}
	else if( _scope && hadc == _scope->hadc4 && _channel == 3 )
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE )
		{
			_scope->state = SCOPE_STATE_WAIT_FOR_ARM;
			_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_ARM] = _scope->hdma_adc4->Instance->CNDTR;
			__HAL_ADC_CLEAR_FLAG( _scope->hadc4, ADC_FLAG_AWD1 );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc4, ADC_FLAG_AWD2 );
			LL_ADC_EnableIT_AWD1( _scope->hadc4->Instance );
		}
	}
}

void _HAL_ADC_LevelOutOfWindowCallback( ADC_HandleTypeDef* hadc )
{
	if( _scope && hadc == _scope->hadc1 && _channel == 0 )
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_ARM )
		{
			_scope->state = SCOPE_STATE_WAIT_FOR_TRIGGER;
			_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_TRIGGER] = _scope->hdma_adc1->Instance->CNDTR;
			LL_ADC_DisableIT_AWD1( _scope->hadc1->Instance );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc1, ADC_FLAG_AWD1 );
			__HAL_ADC_CLEAR_FLAG( _scope->hadc1, ADC_FLAG_AWD2 );
			LL_ADC_EnableIT_AWD2( _scope->hadc1->Instance );
		}
	}
	else if( _scope && hadc == _scope->hadc2 && _channel == 1 )
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_ARM )
		{
			_scope->state = SCOPE_STATE_WAIT_FOR_TRIGGER;
			_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_TRIGGER] = _scope->hdma_adc2->Instance->CNDTR;
			LL_ADC_DisableIT_AWD1( _scope->hadc2->Instance );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc2, ADC_FLAG_AWD1 );
			__HAL_ADC_CLEAR_FLAG( _scope->hadc2, ADC_FLAG_AWD2 );
			LL_ADC_EnableIT_AWD2( _scope->hadc2->Instance );
		}
	}
	else if( _scope && hadc == _scope->hadc3 && _channel == 2 )
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_ARM )
		{
			_scope->state = SCOPE_STATE_WAIT_FOR_TRIGGER;
			_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_TRIGGER] = _scope->hdma_adc3->Instance->CNDTR;
			LL_ADC_DisableIT_AWD1( _scope->hadc3->Instance );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc3, ADC_FLAG_AWD1 );
			__HAL_ADC_CLEAR_FLAG( _scope->hadc3, ADC_FLAG_AWD2 );
			LL_ADC_EnableIT_AWD2( _scope->hadc3->Instance );
		}
	}
	else if( _scope && hadc == _scope->hadc4 && _channel == 3 )
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_ARM )
		{
			_scope->state = SCOPE_STATE_WAIT_FOR_TRIGGER;
			_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_TRIGGER] = _scope->hdma_adc4->Instance->CNDTR;
			LL_ADC_DisableIT_AWD1( _scope->hadc4->Instance );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc4, ADC_FLAG_AWD1 );
			__HAL_ADC_CLEAR_FLAG( _scope->hadc4, ADC_FLAG_AWD2 );
			LL_ADC_EnableIT_AWD2( _scope->hadc4->Instance );
		}
	}
}
uint32_t start_cpu;
uint32_t stop_cpu;

void _HAL_ADCEx_LevelOutOfWindow2Callback( ADC_HandleTypeDef* hadc )
{
	if( _scope && hadc == _scope->hadc1 && _channel == 0 )
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_TRIGGER )
		{
			_scope->state = SCOPE_STATE_WAIT_FOR_STOP;
			_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_STOP] = _scope->hdma_adc1->Instance->CNDTR;

			//_scope->htim2->Instance->CCR1 = 127;
			__HAL_TIM_CLEAR_IT( _scope->htim2, TIM_IT_CC1 );
			__HAL_TIM_CLEAR_IT( _scope->htim2, TIM_IT_CC2 );
			__HAL_TIM_CLEAR_IT( _scope->htim2, TIM_IT_UPDATE );
			__HAL_TIM_CLEAR_FLAG( _scope->htim2, TIM_FLAG_CC1 );
			__HAL_TIM_CLEAR_FLAG( _scope->htim2, TIM_FLAG_CC2 );
			__HAL_TIM_CLEAR_FLAG( _scope->htim2, TIM_FLAG_UPDATE );
			start_cpu = SysTick->VAL;
			HAL_TIM_Base_Start( _scope->htim2 );
			HAL_TIM_OnePulse_Start_IT( _scope->htim2, TIM_CHANNEL_1);
			//LL_ADC_DisableIT_AWD1( _scope->hadc1->Instance );
			LL_ADC_DisableIT_AWD2( _scope->hadc1->Instance );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc1, ADC_FLAG_AWD1 );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc1, ADC_FLAG_AWD2 );
		}
	}
	else if( _scope && hadc == _scope->hadc2 && _channel == 1 )
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_TRIGGER )
		{
			_scope->state = SCOPE_STATE_WAIT_FOR_STOP;
			_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_STOP] = _scope->hdma_adc2->Instance->CNDTR;

			//_scope->htim2->Instance->CCR1 = 127;
			__HAL_TIM_CLEAR_IT( _scope->htim2, TIM_IT_CC1 );
			__HAL_TIM_CLEAR_IT( _scope->htim2, TIM_IT_CC2 );
			__HAL_TIM_CLEAR_IT( _scope->htim2, TIM_IT_UPDATE );
			__HAL_TIM_CLEAR_FLAG( _scope->htim2, TIM_FLAG_CC1 );
			__HAL_TIM_CLEAR_FLAG( _scope->htim2, TIM_FLAG_CC2 );
			__HAL_TIM_CLEAR_FLAG( _scope->htim2, TIM_FLAG_UPDATE );
			start_cpu = SysTick->VAL;
			HAL_TIM_Base_Start( _scope->htim2 );
			HAL_TIM_OnePulse_Start_IT( _scope->htim2, TIM_CHANNEL_1);
			//LL_ADC_DisableIT_AWD1( _scope->hadc2->Instance );
			LL_ADC_DisableIT_AWD2( _scope->hadc2->Instance );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc2, ADC_FLAG_AWD1 );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc2, ADC_FLAG_AWD2 );
		}
	}
	else if( _scope && hadc == _scope->hadc3 && _channel == 2 )
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_TRIGGER )
		{
			_scope->state = SCOPE_STATE_WAIT_FOR_STOP;
			_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_STOP] = _scope->hdma_adc3->Instance->CNDTR;

			//_scope->htim2->Instance->CCR1 = 127;
			__HAL_TIM_CLEAR_IT( _scope->htim2, TIM_IT_CC1 );
			__HAL_TIM_CLEAR_IT( _scope->htim2, TIM_IT_CC2 );
			__HAL_TIM_CLEAR_IT( _scope->htim2, TIM_IT_UPDATE );
			__HAL_TIM_CLEAR_FLAG( _scope->htim2, TIM_FLAG_CC1 );
			__HAL_TIM_CLEAR_FLAG( _scope->htim2, TIM_FLAG_CC2 );
			__HAL_TIM_CLEAR_FLAG( _scope->htim2, TIM_FLAG_UPDATE );
			start_cpu = SysTick->VAL;
			HAL_TIM_Base_Start( _scope->htim2 );
			HAL_TIM_OnePulse_Start_IT( _scope->htim2, TIM_CHANNEL_1);
			//LL_ADC_DisableIT_AWD1( _scope->hadc3->Instance );
			LL_ADC_DisableIT_AWD2( _scope->hadc3->Instance );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc3, ADC_FLAG_AWD1 );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc3, ADC_FLAG_AWD2 );
		}
	}
	else if( _scope && hadc == _scope->hadc4 && _channel == 3 )
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_TRIGGER )
		{
			_scope->state = SCOPE_STATE_WAIT_FOR_STOP;
			_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_STOP] = _scope->hdma_adc4->Instance->CNDTR;

			//_scope->htim2->Instance->CCR1 = 127;
			__HAL_TIM_CLEAR_IT( _scope->htim2, TIM_IT_CC1 );
			__HAL_TIM_CLEAR_IT( _scope->htim2, TIM_IT_CC2 );
			__HAL_TIM_CLEAR_IT( _scope->htim2, TIM_IT_UPDATE );
			__HAL_TIM_CLEAR_FLAG( _scope->htim2, TIM_FLAG_CC1 );
			__HAL_TIM_CLEAR_FLAG( _scope->htim2, TIM_FLAG_CC2 );
			__HAL_TIM_CLEAR_FLAG( _scope->htim2, TIM_FLAG_UPDATE );
			start_cpu = SysTick->VAL;
			HAL_TIM_Base_Start( _scope->htim2 );
			HAL_TIM_OnePulse_Start_IT( _scope->htim2, TIM_CHANNEL_1);
			//LL_ADC_DisableIT_AWD1( _scope->hadc4->Instance );
			LL_ADC_DisableIT_AWD2( _scope->hadc4->Instance );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc4, ADC_FLAG_AWD1 );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc4, ADC_FLAG_AWD2 );
		}
	}
}

void _HAL_TIM_OC_DelayElapsedCallback( TIM_HandleTypeDef *htim )
{
	if( _scope && htim == _scope->htim2 ) // htim2
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_STOP )
		{
			stop_cpu = SysTick->VAL;
			_scope->state = SCOPE_STATE_DONE;
			if( _channel == 0 )
			{
				_scope->CNDTRs[SCOPE_STATE_DONE] = _scope->hdma_adc1->Instance->CNDTR;
			}
			else if( _channel == 1 )
			{
				_scope->CNDTRs[SCOPE_STATE_DONE] = _scope->hdma_adc2->Instance->CNDTR;
			}
			else if( _channel == 2 )
			{
				_scope->CNDTRs[SCOPE_STATE_DONE] = _scope->hdma_adc3->Instance->CNDTR;
			}
			else if( _channel == 3 )
			{
				_scope->CNDTRs[SCOPE_STATE_DONE] = _scope->hdma_adc4->Instance->CNDTR;
			}
			HAL_TIM_Base_Stop( _scope->htim1 );
			HAL_ADC_Stop_DMA( _scope->hadc1 );
			HAL_ADC_Stop_DMA( _scope->hadc2 );
			HAL_ADC_Stop_DMA( _scope->hadc3 );
			HAL_ADC_Stop_DMA( _scope->hadc4 );
			HAL_TIM_OnePulse_Stop_IT( _scope->htim2, TIM_CHANNEL_1);
			HAL_TIM_Base_Stop( _scope->htim2 );
		}
	}
}
*/

tScope *pScope = NULL;

void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef* hadc )
{
	if( pScope->state == SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE )
	{
		__HAL_ADC_CLEAR_FLAG( pScope->trigger.hadc1, ADC_FLAG_AWD1 );
		LL_ADC_EnableIT_AWD1( pScope->trigger.hadc1->Instance );
		pScope->state = SCOPE_STATE_WAIT_FOR_ARM;
	}
}

void HAL_ADC_LevelOutOfWindowCallback( ADC_HandleTypeDef* hadc )
{
	if( pScope->state == SCOPE_STATE_WAIT_FOR_ARM )
	{
		LL_ADC_DisableIT_AWD1( pScope->trigger.hadc1->Instance );
		__HAL_ADC_CLEAR_FLAG( pScope->trigger.hadc1, ADC_FLAG_AWD2 );
		LL_ADC_EnableIT_AWD2( pScope->trigger.hadc1->Instance );
		pScope->state = SCOPE_STATE_WAIT_FOR_TRIGGER;
	}
}

void HAL_ADCEx_LevelOutOfWindow2Callback( ADC_HandleTypeDef* hadc )
{
	if( pScope->state == SCOPE_STATE_WAIT_FOR_TRIGGER )
	{
		__HAL_TIM_CLEAR_IT( pScope->horizontal.htim_stop, TIM_IT_CC1 );
		__HAL_TIM_CLEAR_IT( pScope->horizontal.htim_stop, TIM_IT_CC2 );
		__HAL_TIM_CLEAR_IT( pScope->horizontal.htim_stop, TIM_IT_UPDATE );
		__HAL_TIM_CLEAR_FLAG( pScope->horizontal.htim_stop, TIM_FLAG_CC1 );
		__HAL_TIM_CLEAR_FLAG( pScope->horizontal.htim_stop, TIM_FLAG_CC2 );
		__HAL_TIM_CLEAR_FLAG( pScope->horizontal.htim_stop, TIM_FLAG_UPDATE );
		HAL_TIM_Base_Start( pScope->horizontal.htim_stop );
		HAL_TIM_OnePulse_Start_IT( pScope->horizontal.htim_stop, TIM_CHANNEL_1 );
		LL_ADC_DisableIT_AWD2( pScope->trigger.hadc1->Instance );
		pScope->state = SCOPE_STATE_WAIT_FOR_STOP;
	}
}

void HAL_TIM_OC_DelayElapsedCallback( TIM_HandleTypeDef *htim )
{
	if( pScope->state == SCOPE_STATE_WAIT_FOR_STOP )
	{
		HAL_TIM_Base_Stop( pScope->horizontal.htim_clock );
		HAL_ADC_Stop_DMA( pScope->trigger.hadc1 );
		HAL_ADC_Stop_DMA( pScope->trigger.hadc2 );
		HAL_ADC_Stop_DMA( pScope->trigger.hadc3 );
		HAL_ADC_Stop_DMA( pScope->trigger.hadc4 );
		HAL_TIM_OnePulse_Stop_IT( pScope->horizontal.htim_stop, TIM_CHANNEL_1);
		HAL_TIM_Base_Stop( pScope->horizontal.htim_stop );
		pScope->state = SCOPE_STATE_IDLE;
	}
}

//void StartTaskScope(void *argument);

void scope_init_ll( tScope *pThis,
	TIM_HandleTypeDef *htim_clock,
	TIM_HandleTypeDef *htim_stop,
	DAC_HandleTypeDef *hdac,
	OPAMP_HandleTypeDef *hopamp1,
	OPAMP_HandleTypeDef *hopamp2,
	OPAMP_HandleTypeDef *hopamp3,
	OPAMP_HandleTypeDef *hopamp4,
	ADC_HandleTypeDef *hadc1,
	ADC_HandleTypeDef *hadc2,
	ADC_HandleTypeDef *hadc3,
	ADC_HandleTypeDef *hadc4
)
{
	pThis->horizontal.htim_clock = htim_clock;
	pThis->horizontal.htim_stop = htim_stop;
	pThis->vertical.hdac = hdac;
	pThis->vertical.hopamp1 = hopamp1;
	pThis->vertical.hopamp2 = hopamp2;
	pThis->vertical.hopamp3 = hopamp3;
	pThis->vertical.hopamp4 = hopamp4;
	pThis->trigger.hadc1 = hadc1;
	pThis->trigger.hadc2 = hadc2;
	pThis->trigger.hadc3 = hadc3;
	pThis->trigger.hadc4 = hadc4;
}

void scope_init( tScope *pThis,
	uint16_t *buffer1,
	uint16_t *buffer2,
	uint16_t *buffer3,
	uint16_t *buffer4,
	uint16_t *buffer5,
	uint16_t *buffer6,
	uint16_t *buffer7,
	uint16_t *buffer8,
	uint16_t len
)
{
	pThis->buffer1 = buffer1;
	pThis->buffer2 = buffer2;
	pThis->buffer3 = buffer3;
	pThis->buffer4 = buffer4;
	pThis->buffer5 = buffer5;
	pThis->buffer6 = buffer6;
	pThis->buffer7 = buffer7;
	pThis->buffer8 = buffer8;
	pThis->len = len;
	pThis->cnt = 0;
	pThis->state = SCOPE_STATE_IDLE;
	pThis->continuous = 0;
	pThis->dma_cndtr = 0;
}

void scope_config_horizontal( tScope *pThis, int offset, int scale )
{
	pThis->horizontal.offset = offset;
	pThis->horizontal.scale = scale;

	pThis->horizontal.htim_clock->Init.Prescaler = (170e6 / (scale*1000))/2 - 1;
	pThis->horizontal.htim_clock->Init.Period = 2-1;
    HAL_TIM_Base_Init( pThis->horizontal.htim_clock );
	pThis->horizontal.htim_stop->Init.Prescaler = (170e6 / (scale*1000))/2 - 1;
	pThis->horizontal.htim_stop->Init.Period = pThis->len + offset - 1;
    HAL_TIM_Base_Init( pThis->horizontal.htim_stop );

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_TIMING;
    sConfigOC.Pulse = pThis->len + offset - 1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_OC_ConfigChannel(pThis->horizontal.htim_stop, &sConfigOC, TIM_CHANNEL_1);
}

const uint32_t PgaGain_from_gain[6] = {
	OPAMP_PGA_GAIN_2_OR_MINUS_1,
	OPAMP_PGA_GAIN_4_OR_MINUS_3,
	OPAMP_PGA_GAIN_8_OR_MINUS_7,
	OPAMP_PGA_GAIN_16_OR_MINUS_15,
	OPAMP_PGA_GAIN_32_OR_MINUS_31,
	OPAMP_PGA_GAIN_64_OR_MINUS_63,
};

void scope_config_vertical( tScope *pThis, int offset, int scale1, int scale2, int scale3, int scale4 )
{
	pThis->vertical.offset = offset;

	pThis->vertical.gain1 = scale1;
	pThis->vertical.gain2 = scale2;
	pThis->vertical.gain3 = scale3;
	pThis->vertical.gain4 = scale4;
	//pThis->vertical_gain1 = gain1;
	//pThis->vertical_gain2 = gain2;
	//pThis->vertical_gain3 = gain3;
	//pThis->vertical_gain4 = gain4;
	//pThis->vertical_offset = offset;
	HAL_DAC_Stop(pThis->vertical.hdac, DAC_CHANNEL_1);
	HAL_DAC_SetValue( pThis->vertical.hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, offset );
	HAL_DAC_Start( pThis->vertical.hdac, DAC_CHANNEL_1 );
	//HAL_DAC_SetValue( pThis->vertical.hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, offset );
	//HAL_DAC_SetValue( pThis->vertical.hdac, DAC_CHANNEL_3, DAC_ALIGN_12B_R, offset );
	//HAL_DAC_SetValue( pThis->vertical.hdac, DAC_CHANNEL_4, DAC_ALIGN_12B_R, offset );
	//HAL_OPAMP_Stop( pThis->vertical.hopamp1 );
	//HAL_OPAMP_Stop( pThis->vertical.hopamp2 );
	//HAL_OPAMP_Stop( pThis->vertical.hopamp3 );
	//HAL_OPAMP_Stop( pThis->vertical.hopamp4 );
	//HAL_OPAMP_SelfCalibrate( pThis->vertical.hopamp1 );
	//HAL_OPAMP_SelfCalibrate( pThis->vertical.hopamp2 );
	//HAL_OPAMP_SelfCalibrate( pThis->vertical.hopamp3 );
	//HAL_OPAMP_SelfCalibrate( pThis->vertical.hopamp4 );

	//HAL_OPAMP_Start( pThis->vertical.hopamp1 );
	//HAL_OPAMP_Start( pThis->vertical.hopamp2 );
	//HAL_OPAMP_Start( pThis->vertical.hopamp3 );
	//HAL_OPAMP_Start( pThis->vertical.hopamp4 );

	//HAL_OPAMP_SetGain( pThis->vertical.hopamp1, gain_to_opamp_follower_gain[gain1] );
	//HAL_OPAMP_SetGain( pThis->vertical.hopamp2, gain_to_opamp_follower_gain[gain2] );
	//HAL_OPAMP_SetGain( pThis->vertical.hopamp3, gain_to_opamp_follower_gain[gain3] );
	//HAL_OPAMP_SetGain( pThis->vertical.hopamp4, gain_to_opamp_follower_gain[gain4] );

	pThis->vertical.hopamp1->Init.PgaGain = PgaGain_from_gain[scale1];
    pThis->vertical.hopamp2->Init.PgaGain = PgaGain_from_gain[scale2];
    pThis->vertical.hopamp3->Init.PgaGain = PgaGain_from_gain[scale3];
    pThis->vertical.hopamp4->Init.PgaGain = PgaGain_from_gain[scale4];

    HAL_OPAMP_Stop( pThis->vertical.hopamp1 );
    HAL_OPAMP_Stop( pThis->vertical.hopamp2 );
    HAL_OPAMP_Stop( pThis->vertical.hopamp3 );
    HAL_OPAMP_Stop( pThis->vertical.hopamp4 );

    HAL_OPAMP_Init( pThis->vertical.hopamp1 );
    HAL_OPAMP_Init( pThis->vertical.hopamp2 );
    HAL_OPAMP_Init( pThis->vertical.hopamp3 );
    HAL_OPAMP_Init( pThis->vertical.hopamp4 );

	HAL_OPAMP_Start( pThis->vertical.hopamp1 );
	HAL_OPAMP_Start( pThis->vertical.hopamp2 );
	HAL_OPAMP_Start( pThis->vertical.hopamp3 );
	HAL_OPAMP_Start( pThis->vertical.hopamp4 );
}

void scope_config_trigger( tScope *pThis, int channel, int mode, int level, int slope )
{
	pThis->trigger.channel = channel;
	pThis->trigger.mode = mode;
	pThis->trigger.level = level;
	pThis->trigger.slope = slope;

	ADC_AnalogWDGConfTypeDef AnalogWDGConfig_arm = {0};
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_trig = {0};

	if( channel == 0 )
	{
		// set the values for AnalogWDGConfig_arm and AnalogWDGConfig_trig of channel 0 in case of mode == UI_TRIGGER_MODE_NORMAL
		// Use the appropiate level and slope for AnalogWDGConfig_arm and AnalogWDGConfig_trig lower and upper thresholds

		AnalogWDGConfig_arm.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
		AnalogWDGConfig_arm.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
		AnalogWDGConfig_arm.Channel = 0;
		AnalogWDGConfig_arm.ITMode = ENABLE;
		AnalogWDGConfig_arm.HighThreshold = 4095;
		AnalogWDGConfig_arm.LowThreshold = 0;
		AnalogWDGConfig_arm.FilteringConfig = ADC_AWD_FILTERING_NONE;

		AnalogWDGConfig_trig.WatchdogNumber = ADC_ANALOGWATCHDOG_2;
		AnalogWDGConfig_trig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REGINJEC;
		AnalogWDGConfig_trig.Channel = 0;
		AnalogWDGConfig_trig.ITMode = ENABLE;
		AnalogWDGConfig_trig.HighThreshold = 4095;
		AnalogWDGConfig_trig.LowThreshold = 0;
		AnalogWDGConfig_trig.FilteringConfig = ADC_AWD_FILTERING_NONE;

		if( slope == SCOPE_TRIGGER_SLOPE_RISING )
		{
			AnalogWDGConfig_arm.HighThreshold = 4095;
			AnalogWDGConfig_arm.LowThreshold = level;
			AnalogWDGConfig_trig.HighThreshold = level;
			AnalogWDGConfig_trig.LowThreshold = 0;
		}
		else
		{
			AnalogWDGConfig_arm.HighThreshold = level;
			AnalogWDGConfig_arm.LowThreshold = 0;
			AnalogWDGConfig_trig.HighThreshold = 4095;
			AnalogWDGConfig_trig.LowThreshold = level;
		}
	}

	AnalogWDGConfig_arm.Channel = ADC_CHANNEL_VOPAMP1;
    //AnalogWDGConfig_arm.Channel = ADC_CHANNEL_VOPAMP3_ADC3;
    //AnalogWDGConfig_arm.Channel = ADC_CHANNEL_VOPAMP5;
    //AnalogWDGConfig_arm.Channel = ADC_CHANNEL_VOPAMP6;

    AnalogWDGConfig_trig.Channel = ADC_CHANNEL_VOPAMP1;
    //AnalogWDGConfig_trig.Channel = ADC_CHANNEL_VOPAMP3_ADC3;
    //AnalogWDGConfig_trig.Channel = ADC_CHANNEL_VOPAMP5;
    //AnalogWDGConfig_trig.Channel = ADC_CHANNEL_VOPAMP6;
	HAL_ADC_AnalogWDGConfig( pThis->trigger.hadc1, &AnalogWDGConfig_arm );
    HAL_ADC_AnalogWDGConfig( pThis->trigger.hadc1, &AnalogWDGConfig_trig );

    AnalogWDGConfig_arm.HighThreshold = 4095;
	AnalogWDGConfig_arm.LowThreshold = 0;
	AnalogWDGConfig_trig.HighThreshold = 4095;
	AnalogWDGConfig_trig.LowThreshold = 0;

	AnalogWDGConfig_arm.ITMode = DISABLE;
    AnalogWDGConfig_trig.ITMode = DISABLE;

    AnalogWDGConfig_arm.Channel = ADC_CHANNEL_VOPAMP3_ADC3;
    AnalogWDGConfig_trig.Channel = ADC_CHANNEL_VOPAMP3_ADC3;
	HAL_ADC_AnalogWDGConfig( pThis->trigger.hadc2, &AnalogWDGConfig_arm );
    HAL_ADC_AnalogWDGConfig( pThis->trigger.hadc2, &AnalogWDGConfig_trig );

    AnalogWDGConfig_arm.Channel = ADC_CHANNEL_VOPAMP5;
    AnalogWDGConfig_trig.Channel = ADC_CHANNEL_VOPAMP5;
    HAL_ADC_AnalogWDGConfig( pThis->trigger.hadc3, &AnalogWDGConfig_arm );
	HAL_ADC_AnalogWDGConfig( pThis->trigger.hadc3, &AnalogWDGConfig_trig );

    AnalogWDGConfig_arm.Channel = ADC_CHANNEL_VOPAMP6;
    AnalogWDGConfig_trig.Channel = ADC_CHANNEL_VOPAMP6;
    HAL_ADC_AnalogWDGConfig( pThis->trigger.hadc4, &AnalogWDGConfig_arm );
	HAL_ADC_AnalogWDGConfig( pThis->trigger.hadc4, &AnalogWDGConfig_trig );
}

void scope_start( tScope *pThis, uint8_t continuous )
{
	pThis->continuous = continuous;
	pThis->dma_cndtr = 0;

	HAL_OPAMP_SelfCalibrate( pThis->vertical.hopamp1 );
	HAL_OPAMP_SelfCalibrate( pThis->vertical.hopamp2 );
	HAL_OPAMP_SelfCalibrate( pThis->vertical.hopamp3 );
	HAL_OPAMP_SelfCalibrate( pThis->vertical.hopamp4 );

	HAL_OPAMP_Start( pThis->vertical.hopamp1 );
	HAL_OPAMP_Start( pThis->vertical.hopamp2 );
	HAL_OPAMP_Start( pThis->vertical.hopamp3 );
	HAL_OPAMP_Start( pThis->vertical.hopamp4 );

	HAL_ADCEx_Calibration_Start( pThis->trigger.hadc1, ADC_SINGLE_ENDED );
	HAL_ADCEx_Calibration_Start( pThis->trigger.hadc2, ADC_SINGLE_ENDED );
	HAL_ADCEx_Calibration_Start( pThis->trigger.hadc3, ADC_SINGLE_ENDED );
	HAL_ADCEx_Calibration_Start( pThis->trigger.hadc4, ADC_SINGLE_ENDED );

	LL_ADC_DisableIT_AWD1( pThis->trigger.hadc1->Instance );
	LL_ADC_DisableIT_AWD2( pScope->trigger.hadc1->Instance );

	__HAL_ADC_CLEAR_FLAG( pThis->trigger.hadc1, ADC_FLAG_AWD1 );
	__HAL_ADC_CLEAR_FLAG( pThis->trigger.hadc1, ADC_FLAG_AWD2 );

	pThis->cnt += 1;
	if( pThis->cnt & 0x01 )
	{
		HAL_ADC_Start_DMA( pThis->trigger.hadc1, (uint32_t*)pThis->buffer1, pThis->len );
		HAL_ADC_Start_DMA( pThis->trigger.hadc2, (uint32_t*)pThis->buffer2, pThis->len );
		HAL_ADC_Start_DMA( pThis->trigger.hadc3, (uint32_t*)pThis->buffer3, pThis->len );
		HAL_ADC_Start_DMA( pThis->trigger.hadc4, (uint32_t*)pThis->buffer4, pThis->len );
	}
	else
	{
		HAL_ADC_Start_DMA( pThis->trigger.hadc1, (uint32_t*)pThis->buffer5, pThis->len );
		HAL_ADC_Start_DMA( pThis->trigger.hadc2, (uint32_t*)pThis->buffer6, pThis->len );
		HAL_ADC_Start_DMA( pThis->trigger.hadc3, (uint32_t*)pThis->buffer7, pThis->len );
		HAL_ADC_Start_DMA( pThis->trigger.hadc4, (uint32_t*)pThis->buffer8, pThis->len );
	}

	HAL_TIM_Base_Start( pThis->horizontal.htim_clock );

	pThis->state = SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE;
}

void scope_stop( tScope *pThis )
{
	HAL_TIM_Base_Stop( pThis->horizontal.htim_clock );
	HAL_TIM_Base_Stop( pThis->horizontal.htim_stop );
	HAL_TIM_OnePulse_Stop_IT( pThis->horizontal.htim_stop, TIM_CHANNEL_1 );

	HAL_ADC_Stop_DMA( pThis->trigger.hadc1 );
	HAL_ADC_Stop_DMA( pThis->trigger.hadc2 );
	HAL_ADC_Stop_DMA( pThis->trigger.hadc3 );
	HAL_ADC_Stop_DMA( pThis->trigger.hadc4 );

	//HAL_DAC_Stop( pThis->vertical.hdac, DAC_CHANNEL_1 );
	//HAL_DAC_Stop( pThis->vertical.hdac, DAC_CHANNEL_2 );

	//HAL_OPAMP_Stop( pThis->vertical.hopamp1 );
	//HAL_OPAMP_Stop( pThis->vertical.hopamp2 );
	//HAL_OPAMP_Stop( pThis->vertical.hopamp3 );
	//HAL_OPAMP_Stop( pThis->vertical.hopamp4 );

	pThis->state = SCOPE_STATE_IDLE;

}

uint8_t scope_wait( tScope *pThis, uint32_t timeout_ms )
{
	{
		uint32_t start = HAL_GetTick();
		while( scope_is_busy( pThis ) )
		{
			vTaskDelay(1);
			if( HAL_GetTick() - start > timeout_ms )
			{
				return 0;
			}
		}
		return 1;
	}
}

/*
void lcd_init( tLcd *pThis, GPIO_TypeDef *reset_port, uint16_t reset_pin, GPIO_TypeDef *bl_port, uint16_t bl_pin, uint16_t width, uint16_t height );
void lcd_reset( tLcd *pThis );
void lcd_config( tLcd *pThis );
void lcd_set_bl( tLcd *pThis, uint8_t on );
void lcd_set_window( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t h );
void lcd_set_pixel( tLcd *pThis, int16_t x, int16_t y, uint16_t color );
void lcd_rect( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color );
void lcd_clear( tLcd *pThis, uint16_t color );
void lcd_bmp( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *buf );
*/

void scope_draw_acquire( tScope *pThis, tLcd *pLcd, int is_collapsed )
{

}

void lcd_hline( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t color );
void lcd_vline( tLcd *pThis, int16_t x, int16_t y, uint16_t h, uint16_t color );

void lcd_hline( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t color )
{
	lcd_rect( pThis, x, y, w, 1, color );
}
void lcd_vline( tLcd *pThis, int16_t x, int16_t y, uint16_t h, uint16_t color )
{
	lcd_rect( pThis, x, y, 1, h, color );
}
#define MIN_OFFSET_VALUE (-100)
#define MAX_OFFSET_VALUE (100)

int16_t map_value(int32_t value, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
    return (int16_t)((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

void scope_draw_horizontal(tScope_Horizontal *pThis, tLcd *pLcd, int len, int is_collapsed )
{
	static int last_offset = 0;
	int offset = (pThis->offset*pLcd->width)/len;
	if( is_collapsed )
	{
		lcd_rect( pLcd, last_offset/2+pLcd->width/2, 0, 1, pLcd->height, LCD_COLOR_BLACK );
		lcd_rect( pLcd, offset/2+pLcd->width/2, 0, 1, pLcd->height, SCOPE_COLOR_HORIZONTAL );
	}
	else
	{
		lcd_rect( pLcd, last_offset/4+pLcd->width/4, 0, 1, pLcd->height, LCD_COLOR_BLACK );
		lcd_rect( pLcd, offset/4+pLcd->width/4, 0, 1, pLcd->height, SCOPE_COLOR_HORIZONTAL );
	}
	last_offset = offset;
}

void scope_draw_vertical( tScope_Vertical *pThis, tLcd *pLcd, int is_collapsed )
{
	static int last_offset = 0;
	int offset = pThis->offset;
	float scale = pLcd->height/4096.0f;
	lcd_rect( pLcd, 0, last_offset*scale, pLcd->width/2, 1, LCD_COLOR_BLACK );
	lcd_rect( pLcd, 0, offset*scale, pLcd->width/2, 1, SCOPE_COLOR_VERTICAL );
	last_offset = offset;
}

void scope_draw_trigger( tScope_Trigger *pThis, tLcd *pLcd, int is_collapsed )
{
	static int last_level = 0;
	int level = pThis->level;
	lcd_rect( pLcd, 0, pLcd->height-((last_level)*pLcd->height)/4096, pLcd->width/2, 1, LCD_COLOR_BLACK );
	lcd_rect( pLcd, 0, pLcd->height-((level)*pLcd->height)/4096, pLcd->width/2, 1, SCOPE_COLOR_TRIGGER );
	last_level = level;
}

void scope_draw_grid( tScope *pThis, tLcd *pLcd, int is_collapsed )
{
	//static int i = 10; // force draw the first time.
	//if( i < 10 )
	//{
	//	i++;
	//	return;
	//}
	//else
	//{
	//	i = 0;
	//}

	for( int d = 0 ; d < pLcd->width ; d += 40 )
	{
		lcd_rect( pLcd, d/2, 0, 1, pLcd->height, 0x8410 );
	}
	lcd_rect( pLcd, pLcd->width/2-1, 0, 1, pLcd->height, 0x8410 );

	for( int d = 0 ; d < pLcd->height ; d += 40 )
	{
		lcd_rect( pLcd, 0/2, d, pLcd->width/2, 1, 0x8410 );
	}
	lcd_rect( pLcd, 0/2, pLcd->height, pLcd->width/2, 1, 0x8410 );
}

void scope_draw( tScope *pThis, tLcd *pLcd, int is_collapsed )
{
	scope_draw_grid( pThis, pLcd, is_collapsed );
	//scope_draw_acquire( pThis, pLcd, is_collapsed );
	scope_draw_horizontal( &pThis->horizontal, pLcd, pThis->len, is_collapsed );
	scope_draw_vertical( &pThis->vertical, pLcd, is_collapsed );
	scope_draw_trigger( &pThis->trigger, pLcd, is_collapsed );
	scope_draw_signals( pThis, pLcd, is_collapsed );
}

#define ABS(a)   ((a>0)?(a):-(a))
#define MIN(a,b) ((a<b)?(a):(b))

//extern uint32_t DMA1_Channel1_CNDTR;
void scope_draw_signals( tScope *pThis, tLcd *pLcd, int is_collapsed )
{
	{
		uint16_t i;
		float scale = 320/4096.0f;
		int16_t trigger;
		static int16_t trigger_bck = 0;
		int16_t n, n_bck;
		int pLcd_height = pLcd->height;
        
        uint16_t x;
        int16_t y1, y2, y3, y4, y5, y6, y7, y8;
        int16_t y1_bck = 0;
        int16_t y2_bck = 0;
        int16_t y3_bck = 0;
        int16_t y4_bck = 0;
        int16_t y5_bck = 0;
        int16_t y6_bck = 0;
        int16_t y7_bck = 0;
        int16_t y8_bck = 0;
                
		//lcd_rect( pLcd, 0, 0, 240, 320, 0 );
		trigger = (pThis->len - pThis->dma_cndtr);// - pThis->len/2;
		trigger = (pThis->len - pThis->dma_cndtr) - pThis->len/2 + pThis->len/2;
		trigger = trigger%pThis->len;
		if( pThis->cnt & 0x01 )
		{
			for( i = 0; i < pThis->len; i++ )
			{
				if( 1 )
				{
					n_bck = trigger_bck + i;// - pThis->horizontal.offset/2;
					if( n_bck < 0 )
					{
						n_bck += pThis->len;
					}
					else if( n_bck >= pThis->len )
					{
						n_bck -= pThis->len;
					}

					x = (i)/2;
					y5 = pLcd_height-pThis->buffer5[n_bck]*scale;
					y6 = pLcd_height-pThis->buffer6[n_bck]*scale;
					y7 = pLcd_height-pThis->buffer7[n_bck]*scale;
					y8 = pLcd_height-pThis->buffer8[n_bck]*scale;

					lcd_rect( pLcd, x, MIN(y5,y5_bck), 1, ABS(y5-y5_bck)+1, LCD_COLOR_BLACK );
					lcd_rect( pLcd, x, MIN(y6,y6_bck), 1, ABS(y6-y6_bck)+1, LCD_COLOR_BLACK );
					lcd_rect( pLcd, x, MIN(y7,y7_bck), 1, ABS(y7-y7_bck)+1, LCD_COLOR_BLACK );
					lcd_rect( pLcd, x, MIN(y8,y8_bck), 1, ABS(y8-y8_bck)+1, LCD_COLOR_BLACK );

	                y5_bck = y5;
	                y6_bck = y6;
	                y7_bck = y7;
	                y8_bck = y8;
				}

				if( i > 2 )
				{
					n = trigger + (i-2);// - pThis->horizontal.offset/2;
					if( n < 0 )
					{
						n += pThis->len;
					}
					else if( n >= pThis->len )
					{
						n -= pThis->len;
					}

					x = (i-2)/2;
					y1 = pLcd_height-pThis->buffer1[n]*scale;
					y2 = pLcd_height-pThis->buffer2[n]*scale;
					y3 = pLcd_height-pThis->buffer3[n]*scale;
					y4 = pLcd_height-pThis->buffer4[n]*scale;

					lcd_rect( pLcd, x, MIN(y1,y1_bck), 1, ABS(y1-y1_bck)+1, SCOPE_COLOR_CH1 );
					lcd_rect( pLcd, x, MIN(y2,y2_bck), 1, ABS(y2-y2_bck)+1, SCOPE_COLOR_CH2 );
					lcd_rect( pLcd, x, MIN(y3,y3_bck), 1, ABS(y3-y3_bck)+1, SCOPE_COLOR_CH3 );
					lcd_rect( pLcd, x, MIN(y4,y4_bck), 1, ABS(y4-y4_bck)+1, SCOPE_COLOR_CH4 );

					y1_bck = y1;
					y2_bck = y2;
					y3_bck = y3;
					y4_bck = y4;
				}
			}
		}
		else
		{
			for( i = 0; i < pThis->len; i++ )
			{
				if( 1 )
				{
					n_bck = trigger_bck + i;// - pThis->horizontal.offset/2;
					if( n_bck < 0 )
					{
						n_bck += pThis->len;
					}
					else if( n_bck >= pThis->len )
					{
						n_bck -= pThis->len;
					}

					x = (i)/2;
					y1 = pLcd_height-pThis->buffer1[n_bck]*scale;
					y2 = pLcd_height-pThis->buffer2[n_bck]*scale;
					y3 = pLcd_height-pThis->buffer3[n_bck]*scale;
					y4 = pLcd_height-pThis->buffer4[n_bck]*scale;

					lcd_rect( pLcd, x, MIN(y1,y1_bck), 1, ABS(y1-y1_bck)+1, LCD_COLOR_BLACK );
					lcd_rect( pLcd, x, MIN(y2,y2_bck), 1, ABS(y2-y2_bck)+1, LCD_COLOR_BLACK );
					lcd_rect( pLcd, x, MIN(y3,y3_bck), 1, ABS(y3-y3_bck)+1, LCD_COLOR_BLACK );
					lcd_rect( pLcd, x, MIN(y4,y4_bck), 1, ABS(y4-y4_bck)+1, LCD_COLOR_BLACK );

	                y1_bck = y1;
	                y2_bck = y2;
	                y3_bck = y3;
	                y4_bck = y4;
				}

				if( i > 2 )
				{
					n = trigger + (i-2);// - pThis->horizontal.offset/2;
					if( n < 0 )
					{
						n += pThis->len;
					}
					else if( n >= pThis->len )
					{
						n -= pThis->len;
					}

					x = (i-2)/2;
					y5 = pLcd_height-pThis->buffer5[n]*scale;
					y6 = pLcd_height-pThis->buffer6[n]*scale;
					y7 = pLcd_height-pThis->buffer7[n]*scale;
					y8 = pLcd_height-pThis->buffer8[n]*scale;

					lcd_rect( pLcd, x, MIN(y5,y5_bck), 1, ABS(y5-y5_bck)+1, SCOPE_COLOR_CH1 );
					lcd_rect( pLcd, x, MIN(y6,y6_bck), 1, ABS(y6-y6_bck)+1, SCOPE_COLOR_CH2 );
					lcd_rect( pLcd, x, MIN(y7,y7_bck), 1, ABS(y7-y7_bck)+1, SCOPE_COLOR_CH3 );
					lcd_rect( pLcd, x, MIN(y8,y8_bck), 1, ABS(y8-y8_bck)+1, SCOPE_COLOR_CH4 );

					y5_bck = y5;
					y6_bck = y6;
					y7_bck = y7;
					y8_bck = y8;
				}
			}
		}
		trigger_bck = trigger;
	}
}

void scope_clear( tScope *pThis, tLcd *pLcd, int is_collapsed )
{
	uint16_t i;
	if( pThis->cnt & 0x01 )
	{
		for( i = 0; i < pThis->len; i++ )
		{
			lcd_set_pixel( pLcd, i/2, pThis->buffer1[i], 0x0000 );
			lcd_set_pixel( pLcd, i/2, pThis->buffer2[i], 0x0000 );
			lcd_set_pixel( pLcd, i/2, pThis->buffer3[i], 0x0000 );
			lcd_set_pixel( pLcd, i/2, pThis->buffer4[i], 0x0000 );
		}
	}
	else
	{
		for( i = 0; i < pThis->len; i++ )
		{
			lcd_set_pixel( pLcd, i/2, pThis->buffer5[i], 0x0000 );
			lcd_set_pixel( pLcd, i/2, pThis->buffer6[i], 0x0000 );
			lcd_set_pixel( pLcd, i/2, pThis->buffer7[i], 0x0000 );
			lcd_set_pixel( pLcd, i/2, pThis->buffer8[i], 0x0000 );
		}
	}
}

uint8_t scope_is_continuous( tScope *pThis )
{
	return pThis->continuous;
}

uint8_t scope_is_running( tScope *pThis )
{
	return pThis->state != SCOPE_STATE_DONE && pThis->state != SCOPE_STATE_IDLE;
}
uint8_t scope_is_busy( tScope *pThis )
{
	return pThis->state != SCOPE_STATE_IDLE;
}

