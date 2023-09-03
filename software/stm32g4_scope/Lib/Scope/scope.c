/*
 * scope.c
 *
 *  Created on: Jul 26, 2023
 *      Author: jgpei
 */


#include "scope.h"

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


//#define MIN_OFFSET_VALUE (-100)
//#define MAX_OFFSET_VALUE (100)
/*
int16_t map_value(int32_t value, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
    return (int16_t)((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}*/



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

