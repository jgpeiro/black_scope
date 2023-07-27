/*
 * scope.c
 *
 *  Created on: Jul 26, 2023
 *      Author: jgpei
 */


#include "scope.h"

static tScope *_scope = NULL;

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

	extern DAC_HandleTypeDef hdac2;
	int vcc = 3.3;
	HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (vcc/2.0)/vcc*4095);
	HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);


	extern OPAMP_HandleTypeDef hopamp1;
	extern OPAMP_HandleTypeDef hopamp3;
	extern OPAMP_HandleTypeDef hopamp5;
	extern OPAMP_HandleTypeDef hopamp6;
	HAL_OPAMP_Start(&hopamp1);
	HAL_OPAMP_Start(&hopamp3);
	HAL_OPAMP_Start(&hopamp5);
	HAL_OPAMP_Start(&hopamp6);

	HAL_ADCEx_Calibration_Start( scope->hadc1, ADC_SINGLE_ENDED );
	HAL_ADCEx_Calibration_Start( scope->hadc2, ADC_SINGLE_ENDED );
	HAL_ADCEx_Calibration_Start( scope->hadc3, ADC_SINGLE_ENDED );
	HAL_ADCEx_Calibration_Start( scope->hadc4, ADC_SINGLE_ENDED );

	LL_ADC_DisableIT_AWD1( scope->hadc1->Instance );
	LL_ADC_DisableIT_AWD2( scope->hadc1->Instance );
	__HAL_ADC_CLEAR_FLAG( scope->hadc1, ADC_FLAG_AWD1 );
	__HAL_ADC_CLEAR_FLAG( scope->hadc1, ADC_FLAG_AWD2 );

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
	return scope->len - scope->CNDTRs[SCOPE_STATE_WAIT_FOR_STOP];
}

// ********************** IRQs ********************** //
void HAL_ADC_ConvHalfCpltCallback( ADC_HandleTypeDef* hadc )
{
	if( _scope && hadc == _scope->hadc1 )
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
