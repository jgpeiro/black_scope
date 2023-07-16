/*
 * scope.c
 *
 *  Created on: 21 jun. 2023
 *      Author: Jose
 */

#include "scope.h"

static tScope *_scope;

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
	extern TIM_HandleTypeDef htim1;
	extern TIM_HandleTypeDef htim2;

	scope->hadc1 = &hadc1;
	scope->hadc2 = &hadc3;
	scope->hadc3 = &hadc5;
	scope->hadc4 = &hadc4;
	scope->hdma_adc1 = &hdma_adc1;
	scope->hdma_adc2 = &hdma_adc3;
	scope->hdma_adc3 = &hdma_adc5;
	scope->hdma_adc4 = &hdma_adc4;
	scope->htim1 = &htim1;
	scope->htim2 = &htim2;

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
	return scope->state != SCOPE_STATE_DONE;
}

int32_t scope_get_trigger( tScope *scope )
{
	return scope->len - scope->CNDTRs[SCOPE_STATE_WAIT_FOR_STOP];
}

// ********************** IRQs ********************** //
int cndtr1 = 0;
int cndtr2 = 0;
void HAL_ADC_ConvHalfCpltCallback( ADC_HandleTypeDef* hadc )
{
	if( hadc == _scope->hadc1 )
	{
	}
}

void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef* hadc )
{
	if( hadc == _scope->hadc1 )
	{
		if( 1 && _scope->state == SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE )
		{
			_scope->state = SCOPE_STATE_WAIT_FOR_ARM;
			_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_ARM] = cndtr1;//_scope->hdma_adc1->Instance->CNDTR;
			__HAL_ADC_CLEAR_FLAG( _scope->hadc1, ADC_FLAG_AWD1 );
			//__HAL_ADC_CLEAR_FLAG( _scope->hadc1, ADC_FLAG_AWD2 );
			LL_ADC_EnableIT_AWD1( _scope->hadc1->Instance );
			cndtr1 = 0;
			cndtr2 = 0;
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
	if( hadc == _scope->hadc1 )
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
	if( hadc == _scope->hadc1 )
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_TRIGGER )
		{
			_scope->state = SCOPE_STATE_WAIT_FOR_STOP;
			_scope->CNDTRs[SCOPE_STATE_WAIT_FOR_STOP] = cndtr1;//_scope->hdma_adc1->Instance->CNDTR;

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
	if( htim == _scope->htim2 ) // htim2
	{
		if( _scope->state == SCOPE_STATE_WAIT_FOR_STOP )
		{
			stop_cpu = SysTick->VAL;
			_scope->state = SCOPE_STATE_DONE;
			_scope->CNDTRs[SCOPE_STATE_DONE] = cndtr2;//_scope->hdma_adc1->Instance->CNDTR;
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

#include <math.h>

float get_mean( uint16_t *data, uint16_t len );
float get_std( uint16_t *data, uint16_t len );

uint32_t start;
uint32_t end;
uint32_t diff;

int _scope_init = 0;
#define BUFFER_LEN 1024
uint16_t dac1_buffer[BUFFER_LEN];
uint16_t dac2_buffer[BUFFER_LEN];

uint16_t buffer1[BUFFER_LEN];
uint16_t buffer2[BUFFER_LEN];
uint16_t buffer3[BUFFER_LEN];
uint16_t buffer4[BUFFER_LEN];
uint16_t buffer5[BUFFER_LEN];
uint16_t buffer6[BUFFER_LEN];
uint16_t buffer7[BUFFER_LEN];
uint16_t buffer8[BUFFER_LEN];
tScope scope = {0};
void test_scope( void )
{
	// test scope

	uint16_t start = 0;
	  static int trigger_bck = 0;
	  static int i = 0;

	if( _scope_init == 0 )
	{

		float freq = 3;
		for( int i = 0 ; i < BUFFER_LEN ; i++ )
		{
			dac1_buffer[i] = sinf(freq*2*M_PI*i/BUFFER_LEN)*1023 + 2048;
			//dac2_buffer[i] = sinf(freq*2*M_PI*i/BUFFER_LEN)*2000 + 2048;
			dac2_buffer[i] = 1024+((i*8)%2048);
		}
		extern DAC_HandleTypeDef hdac1;
		extern TIM_HandleTypeDef htim2;
		extern TIM_HandleTypeDef htim3;
		__HAL_DBGMCU_FREEZE_TIM3();
		HAL_DAC_Start_DMA( &hdac1, DAC_CHANNEL_1, (uint32_t*)dac1_buffer, BUFFER_LEN, DAC_ALIGN_12B_R );
		HAL_DAC_Start_DMA( &hdac1, DAC_CHANNEL_2, (uint32_t*)dac2_buffer, BUFFER_LEN, DAC_ALIGN_12B_R );
		//HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
		HAL_TIM_Base_Start( &htim3 );
		//htim3.Instance->CNT = 0x2A;
		extern TIM_HandleTypeDef htim1;
		//htim1.Instance->PSC = 0x1;
		//htim1.Instance->ARR = 0x54;
		//htim1.Instance->CNT = 0x2A;
		//HAL_TIM_Base_Start( &htim1 );


		_scope_init = 1;
	}



	{

		static int dd = 0;
		if( dd<10 )
		{
			dd++;
		}
		else
		{
			dd = 0;
			for( int d = 0 ; d < 480 ; d += 40 )
			{
				lcd_rect( d, 0, 1, 320, 0x55555555 );
			}
			lcd_rect( 480, 0, 1, 320, 0x55555555 );
			for( int d = 0 ; d < 320 ; d += 40 )
			{
				lcd_rect( 0, d, 480, 1, 0x55555555 );
			}
			lcd_rect( 0, 320, 480, 1, 0x55555555 );

			lcd_rect( 0, 320-((2048+768)*320)/4096, 480, 1, 0xFFFFFF00 );

		}


		scope_init( &scope, 2048, 1000000,
				(i&0x01)?buffer1:buffer5,
				(i&0x01)?buffer2:buffer6,
				(i&0x01)?buffer3:buffer7,
				(i&0x01)?buffer4:buffer8,
				BUFFER_LEN );
		scope_start( &scope );
		while( scope_is_busy( &scope ) );
		scope_stop( &scope );

		int32_t trigger = scope_get_trigger( &scope ) - BUFFER_LEN/2;

		//printf( "data%d = np.array( [", i );
		for( int jj = 0; jj < 480; jj++ )
		//for( int jj = 0; jj < BUFFER_LEN; jj++ )
		{
			int j = (jj*BUFFER_LEN)/480.0;
			//int j = jj;
			int n = trigger + j;
			if( n < 0 )
			{
				n += BUFFER_LEN;
			}
			else if( n >= BUFFER_LEN )
			{
				n -= BUFFER_LEN;
			}

			int n2 = trigger_bck + j;
			if( n2 < 0 )
			{
				n2 += BUFFER_LEN;
			}
			else if( n2 >= BUFFER_LEN )
			{
				n2 -= BUFFER_LEN;
			}
			//printf( "%d, ", buffer[n] );

			int x0, y0, y1;
			x0 = jj;//(j*480)/BUFFER_LEN;
			y0 = 320-(((i&0x01)?buffer5[n2]:buffer1[n2])*320)/4096;
			y1 = 320-(((i&0x01)?buffer1[n]:buffer5[n])*320)/4096;
			extern void lcd_set_pixel( int16_t x, int16_t y, uint32_t color );
			lcd_set_pixel( x0, y0, 0x00000000 );
			lcd_set_pixel( x0, y1, 0x0001C007 );

			y0 = 320-(((i&0x01)?buffer6[n2]:buffer2[n2])*320)/4096;
			y1 = 320-(((i&0x01)?buffer2[n]:buffer6[n])*320)/4096;
			lcd_set_pixel( x0, y0, 0x00000000 );
			lcd_set_pixel( x0, y1, 0x00003F00 );

			y0 = 320-(((i&0x01)?buffer7[n2]:buffer3[n2])*320)/4096;
			y1 = 320-(((i&0x01)?buffer3[n]:buffer7[n])*320)/4096;
			lcd_set_pixel( x0, y0, 0x00000000 );
			lcd_set_pixel( x0, y1, 0x000001F8 );

			y0 = 320-(((i&0x01)?buffer8[n2]:buffer4[n2])*320)/4096;
			y1 = 320-(((i&0x01)?buffer4[n]:buffer8[n])*320)/4096;
			lcd_set_pixel( x0, y0, 0x00000000 );
			lcd_set_pixel( x0, y1, 0xFFFFFFFF );



		}

		//lcd_rect( 240, 0, 1, 320, 0xFFFFFF00 );
		//printf( "], dtype=np.float32 )\n" );
		i += 1;
		trigger_bck = trigger;
		//HAL_Delay( 10 );
	}
}
