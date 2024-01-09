/*
 * scope.c
 *
 *  Created on: Sep 20, 2023
 *      Author: jgpei
 */
#include "scope.h"

tScope *pScope = NULL;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if( pScope->state == SCOPE_STATE_WAIT_FOR_CONV_CPLT )
    {
    	LL_ADC_DisableIT_AWD1(pScope->adc->Instance);
    	if( pScope->slope == SCOPE_SLOPE_RISING )
    	{
    		pScope->adc->Instance->HTR = 0x0FFF;
    		pScope->adc->Instance->LTR = pScope->trigger_v;
    	}
    	else
    	{
    		pScope->adc->Instance->HTR = pScope->trigger_v;
    		pScope->adc->Instance->LTR = 0x0000;
    	}
    	__HAL_ADC_CLEAR_FLAG(pScope->adc, ADC_FLAG_AWD);
        LL_ADC_EnableIT_AWD1(pScope->adc->Instance);
        pScope->state = SCOPE_STATE_WAIT_FOR_ARM;
    }
}

void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
	if( pScope->state == SCOPE_STATE_WAIT_FOR_ARM )
	{
		LL_ADC_DisableIT_AWD1(pScope->adc->Instance);
    	if( pScope->slope == SCOPE_SLOPE_RISING )
    	{
    		pScope->adc->Instance->HTR = pScope->trigger_v;
    		pScope->adc->Instance->LTR = 0x0000;
    	}
    	else
    	{
    		pScope->adc->Instance->HTR = 0x0FFF;
    		pScope->adc->Instance->LTR = pScope->trigger_v;
    	}
    	__HAL_ADC_CLEAR_FLAG( pScope->adc, ADC_FLAG_AWD );
		LL_ADC_EnableIT_AWD1( pScope->adc->Instance );
		pScope->state = SCOPE_STATE_WAIT_FOR_TRIGGER;
	}
	else if( pScope->state == SCOPE_STATE_WAIT_FOR_TRIGGER )
	{
		LL_ADC_DisableIT_AWD1( pScope->adc->Instance );

        __HAL_TIM_CLEAR_IT( pScope->tim_stop, TIM_IT_CC1 );
        __HAL_TIM_CLEAR_IT( pScope->tim_stop, TIM_IT_CC2 );
        __HAL_TIM_CLEAR_IT( pScope->tim_stop, TIM_IT_UPDATE );
        __HAL_TIM_CLEAR_FLAG( pScope->tim_stop, TIM_FLAG_CC1 );
        __HAL_TIM_CLEAR_FLAG( pScope->tim_stop, TIM_FLAG_CC2 );
        __HAL_TIM_CLEAR_FLAG( pScope->tim_stop, TIM_FLAG_UPDATE );

		HAL_TIM_OnePulse_Start_IT( pScope->tim_stop, TIM_CHANNEL_1 );
		HAL_TIM_Base_Start( pScope->tim_stop );
		pScope->state = SCOPE_STATE_WAIT_FOR_STOP;
	}
}
extern int NTDR;
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef* htim)
{
	if( pScope->state == SCOPE_STATE_WAIT_FOR_STOP )
	{
		pScope->trigger_h = pScope->adc->DMA_Handle->Instance->NDTR;
		//pScope->trigger_h = NTDR;

		HAL_TIM_Base_Stop( pScope->tim_stop );
		HAL_TIM_OnePulse_Stop_IT( pScope->tim_stop, TIM_CHANNEL_1 );

		HAL_TIM_Base_Stop( pScope->tim_clock );
		pScope->state = SCOPE_STATE_DONE;
	}
}

void scope_init( tScope *pThis,
	ADC_HandleTypeDef *adc,
	TIM_HandleTypeDef *tim_clock,
	TIM_HandleTypeDef *tim_stop,
	uint16_t *bufferA,
	uint16_t *bufferB,
	uint16_t len )
{
	pScope = pThis;

	pThis->adc = adc;
	pThis->tim_clock = tim_clock;
	pThis->tim_stop = tim_stop;

	pThis->state = SCOPE_STATE_IDLE;

	pThis->offset_h = 0;
	pThis->scale_h = 0;

	pThis->trigger_v = 0;
	pThis->trigger_h = 0;

	pThis->channel = 0;
	pThis->slope = 0;

	pThis->bufferA = bufferA;
	pThis->bufferB = bufferB;
	pThis->len = len;

	pThis->cnt = 0;
}

void scope_config_horizontal( tScope *pThis, int32_t offset, int32_t scale )
{
	pThis->offset_h = offset;
	pThis->scale_h = scale;

	pThis->tim_clock->Init.Prescaler = (SystemCoreClock / (pThis->scale_h * 1000))/2 - 1;
	pThis->tim_clock->Init.Period = 2 - 1;
	HAL_TIM_Base_Init( pThis->tim_clock );

	pThis->tim_stop->Init.Prescaler = (SystemCoreClock / (pThis->scale_h * 1000))/2 - 1;
	pThis->tim_stop->Init.Period = 2*(pThis->len/2 + pThis->offset_h) - 1;
	HAL_TIM_Base_Init( pThis->tim_stop );
}

void scope_config_vertical( tScope *pThis, int32_t offset, int32_t scale )
{
	;
}

const uint32_t ADC_CHANNEL_X[4] = {
	ADC_CHANNEL_0,
	ADC_CHANNEL_1,
	ADC_CHANNEL_2,
	ADC_CHANNEL_3,
};

void scope_config_trigger( tScope *pThis, int32_t channel, int32_t trigger, tScopeSlope slope )
{
	pThis->channel = channel;
	pThis->trigger_v = trigger;
	pThis->slope = slope;

	pThis->adc->Instance->CR1 &=~ADC_CR1_AWDCH;
	pThis->adc->Instance->CR1 |= ADC_CHANNEL_X[ pThis->channel ];
}

void scope_start( tScope *pThis )
{
	pThis->state = SCOPE_STATE_WAIT_FOR_CONV_CPLT;
	LL_ADC_DisableIT_AWD1( pThis->adc->Instance );
	__HAL_ADC_CLEAR_FLAG( pThis->adc, ADC_FLAG_AWD);
	pThis->cnt += 1;
	if( pThis->cnt&0x01 )
	{
		HAL_ADC_Start_DMA( pThis->adc, (uint32_t*)pThis->bufferA, pThis->len );
	}
	else
	{
		HAL_ADC_Start_DMA( pThis->adc, (uint32_t*)pThis->bufferB, pThis->len );
	}
	HAL_TIM_Base_Start( pThis->tim_clock );
}

uint8_t scope_wait( tScope *pThis, uint32_t timeout_ms )
{
	uint32_t i;
	for( i = 0 ; i < timeout_ms ; i++ )
	{
		if( pThis->state == SCOPE_STATE_DONE )
		{
			return 1;
		}
		//HAL_Delay(1);
		vTaskDelay(1);
	}
	return 0;
}

void scope_stop( tScope *pThis )
{
	HAL_TIM_Base_Stop( pThis->tim_clock );
	HAL_ADC_Stop_DMA( pThis->adc );
	HAL_TIM_Base_Stop( pThis->tim_stop );
	pThis->state = SCOPE_STATE_DONE;
}

void scope_draw( tScope *pThis, tLcd *pLcd )
{
	int i;
	static unsigned int trigger_h_bck = 0;
	unsigned int n_bck = 0;
	unsigned int trigger_h = 0;
	unsigned int n = 0;

	pThis->trigger_h &= 0xFFFFFFFE;
	trigger_h = (pThis->len-pThis->trigger_h)-pThis->len/2-pLcd->width/2;
	trigger_h %= pThis->len;

	for( i = 0 ; i < pLcd->width*2 ; i++ )
	{
		n = trigger_h+i;
		n %= pThis->len;

		n_bck = trigger_h_bck+i;
		n_bck %= pThis->len;

		if( (i&0x01) == 0 )
		{
			if( pThis->cnt&0x01 )
			{
				lcd_set_pixel( pLcd, i/2, (pThis->bufferB[n_bck+0]*pLcd->height)/4096, LCD_COLOR_BLACK );
				lcd_set_pixel( pLcd, i/2, (pThis->bufferB[n_bck+1]*pLcd->height)/4096, LCD_COLOR_BLACK );

				lcd_set_pixel( pLcd, i/2, (pThis->bufferA[n+0]*pLcd->height)/4096, LCD_COLOR_RED );
				lcd_set_pixel( pLcd, i/2, (pThis->bufferA[n+1]*pLcd->height)/4096, LCD_COLOR_GREEN );
			}
			else
			{
				lcd_set_pixel( pLcd, i/2, (pThis->bufferA[n_bck+0]*pLcd->height)/4096, LCD_COLOR_BLACK );
				lcd_set_pixel( pLcd, i/2, (pThis->bufferA[n_bck+1]*pLcd->height)/4096, LCD_COLOR_BLACK );

				lcd_set_pixel( pLcd, i/2, (pThis->bufferB[n+0]*pLcd->height)/4096, LCD_COLOR_RED );
				lcd_set_pixel( pLcd, i/2, (pThis->bufferB[n+1]*pLcd->height)/4096, LCD_COLOR_GREEN );
			}
		}
	}
	trigger_h_bck = trigger_h;
}
