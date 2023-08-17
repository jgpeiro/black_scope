/*
 * scope.c
 *
 *  Created on: Jul 26, 2023
 *      Author: jgpei
 */


#include "scope.h"

static tScope *_scope = NULL;

#include <stdio.h>
#include <stdlib.h>

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


void scope_config_horizontal( tScope *scope, int sample_rate, int buffer_len )
{
    // This function configures the timers so that the ADC is triggered at the frequency and to stop the conversion when the buffer is completed.
    int tim_freq = 170e6;
    int prescaler = 0;
    int period = 0;

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

    // Compute required Prescaler and Period for the ADC clock.
    scope->horizontal.htim_clock->Init.Prescaler = (tim_freq / sample_rate)/2 - 1;
    scope->horizontal.htim_clock->Init.Period = 1;
    HAL_TIM_Base_Init( scope->horizontal.htim_clock );

    // Compute required Prescaler and Period for the Stop timer.
    scope->horizontal.htim_stop->Init.Prescaler = (tim_freq / sample_rate)/2 - 1;
    scope->horizontal.htim_stop->Init.Period = buffer_len - 1;
    HAL_TIM_Base_Init( scope->horizontal.htim_stop );

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_TIMING;
    sConfigOC.Pulse = buffer_len - 1;
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
    // This function configures the OPAMPs and DACs for the vertical channels.
    gain1 = gain_to_pgagain( gain1 );
    gain2 = gain_to_pgagain( gain2 );
    gain3 = gain_to_pgagain( gain3 );
    gain4 = gain_to_pgagain( gain4 );

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

    HAL_OPAMP_SelfCalibrate( scope->vertical.hopamp1 );
    HAL_OPAMP_SelfCalibrate( scope->vertical.hopamp2 );
    HAL_OPAMP_SelfCalibrate( scope->vertical.hopamp3 );
    HAL_OPAMP_SelfCalibrate( scope->vertical.hopamp4 );

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
    AnalogWDGConfig_arm_1.Channel = ADC_CHANNEL_VOPAMP1;
    AnalogWDGConfig_arm_1.ITMode = ENABLE;
    AnalogWDGConfig_arm_1.HighThreshold = 0; // 0 and 0, forces automatically the trigger, so its auto mode.
    AnalogWDGConfig_arm_1.LowThreshold = 0;
    AnalogWDGConfig_arm_1.FilteringConfig = ADC_AWD_FILTERING_NONE;
    AnalogWDGConfig_arm_2 = AnalogWDGConfig_arm_1;
    AnalogWDGConfig_arm_3 = AnalogWDGConfig_arm_1;
    AnalogWDGConfig_arm_4 = AnalogWDGConfig_arm_1;

    AnalogWDGConfig_trig_1 = AnalogWDGConfig_arm_1;
    AnalogWDGConfig_trig_1.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REGINJEC;
    AnalogWDGConfig_trig_2 = AnalogWDGConfig_trig_1;
    AnalogWDGConfig_trig_3 = AnalogWDGConfig_trig_1;
    AnalogWDGConfig_trig_4 = AnalogWDGConfig_trig_1;

    // reset all
    AnalogWDGConfig_arm_1.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    AnalogWDGConfig_arm_2.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    AnalogWDGConfig_arm_3.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    AnalogWDGConfig_arm_4.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    AnalogWDGConfig_trig_1.WatchdogNumber = ADC_ANALOGWATCHDOG_2;
    AnalogWDGConfig_trig_2.WatchdogNumber = ADC_ANALOGWATCHDOG_2;
    AnalogWDGConfig_trig_3.WatchdogNumber = ADC_ANALOGWATCHDOG_2;
    AnalogWDGConfig_trig_4.WatchdogNumber = ADC_ANALOGWATCHDOG_2;

    // Configure the required for arm the oscilloscope.
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

void scope_init( tScope *scope, uint16_t trigger_level, uint16_t sample_rate, uint16_t *buffer1, uint16_t *buffer2, uint16_t *buffer3, uint16_t *buffer4, uint16_t len )
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
	HAL_TIM_OnePulse_Stop_IT( scope->htim2, TIM_CHANNEL_1);
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

int32_t scope_get_trigger( tScope *scope )
{
	//return scope->len - scope->CNDTRs[SCOPE_STATE_WAIT_FOR_STOP];
	return scope->len - scope->CNDTRs[SCOPE_STATE_DONE] - 256;
}

// ********************** IRQs ********************** //
void HAL_ADC_ConvHalfCpltCallback( ADC_HandleTypeDef* hadc )
{
	if( _scope && hadc == _scope->hadc1 )
	{
	}
	else if( _scope && hadc == _scope->hadc2 )
	{
	}
	else if( _scope && hadc == _scope->hadc3 )
	{
	}
	else if( _scope && hadc == _scope->hadc4 )
	{
	}
}

void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef* hadc )
{
	if( _scope && hadc == _scope->hadc1 )
	{
		if( 1 && _scope->state == SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE )
		{
			_scope->state = SCOPE_STATE_WAIT_FOR_ARM;
			_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_ARM] = _scope->hdma_adc1->Instance->CNDTR;
			__HAL_ADC_CLEAR_FLAG( _scope->hadc1, ADC_FLAG_AWD1 );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc1, ADC_FLAG_AWD2 );
			LL_ADC_EnableIT_AWD1( _scope->hadc1->Instance );
		}

		if( 0 && _scope->state == SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE )
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
			//start_cpu = SysTick->VAL;
			HAL_TIM_Base_Start( _scope->htim2 );
			HAL_TIM_OnePulse_Start_IT( _scope->htim2, TIM_CHANNEL_1);
			//LL_ADC_DisableIT_AWD1( _scope->hadc1->Instance );
			LL_ADC_DisableIT_AWD2( _scope->hadc1->Instance );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc1, ADC_FLAG_AWD1 );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc1, ADC_FLAG_AWD2 );
		}
	}

	else if( _scope && hadc == _scope->hadc2 )
		{
			if( 1 && _scope->state == SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE )
			{
				_scope->state = SCOPE_STATE_WAIT_FOR_ARM;
				_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_ARM] = _scope->hdma_adc2->Instance->CNDTR;
				__HAL_ADC_CLEAR_FLAG( _scope->hadc2, ADC_FLAG_AWD1 );
				//__HAL_ADC_CLEAR_FLAG( _scope->hadc2, ADC_FLAG_AWD2 );
				LL_ADC_EnableIT_AWD1( _scope->hadc2->Instance );
			}

			if( 0 && _scope->state == SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE )
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
				//start_cpu = SysTick->VAL;
				HAL_TIM_Base_Start( _scope->htim2 );
				HAL_TIM_OnePulse_Start_IT( _scope->htim2, TIM_CHANNEL_1);
				//LL_ADC_DisableIT_AWD1( _scope->hadc2->Instance );
				LL_ADC_DisableIT_AWD2( _scope->hadc2->Instance );
				//__HAL_ADC_CLEAR_FLAG( _scope->hadc2, ADC_FLAG_AWD1 );
				//__HAL_ADC_CLEAR_FLAG( _scope->hadc2, ADC_FLAG_AWD2 );
			}
		}
}

void HAL_ADC_LevelOutOfWindowCallback( ADC_HandleTypeDef* hadc )
{
	if( _scope && hadc == _scope->hadc1 )
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

	else if( _scope && hadc == _scope->hadc2 )
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
}
uint32_t start_cpu;
uint32_t stop_cpu;

void HAL_ADCEx_LevelOutOfWindow2Callback( ADC_HandleTypeDef* hadc )
{
	if( _scope && hadc == _scope->hadc1 )
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
	if( _scope && hadc == _scope->hadc2 )
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
}

void HAL_TIM_OC_DelayElapsedCallback( TIM_HandleTypeDef *htim )
{
	if( _scope && htim == _scope->htim2 ) // htim2
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_STOP )
		{
			stop_cpu = SysTick->VAL;
			_scope->state = SCOPE_STATE_DONE;
			_scope->CNDTRs[SCOPE_STATE_DONE] = _scope->hdma_adc1->Instance->CNDTR;
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
