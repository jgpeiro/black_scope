/*
 * wavegen.c
 *
 *  Created on: Jul 31, 2023
 *      Author: jgpei
 */

#include <math.h>
#include <stdlib.h>
#include "wavegen.h"

#include "buffer.h"
extern tBuffer usb_rx;


#define DAC_BUFFER_LEN 	(512)
#define BUFFER_SIZE (sizeof(uint16_t)*DAC_BUFFER_LEN)


extern uint16_t dac1_buffer[DAC_BUFFER_LEN];
extern uint16_t dac2_buffer[DAC_BUFFER_LEN];

void HAL_DACEx_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef *hdac)
{
    // if new block is available, copy to the dac buffer.
    if( buffer_size( &usb_rx ) >= BUFFER_SIZE / 2 )
    {
        buffer_pop( &usb_rx, dac2_buffer, BUFFER_SIZE / 2 );
    }
}
void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef *hdac)
{
    // if new block is available, copy to the dac buffer.
    if( buffer_size( &usb_rx ) >= BUFFER_SIZE / 2 )
    {
        buffer_pop( &usb_rx, dac2_buffer + BUFFER_SIZE / 2, BUFFER_SIZE / 2 );
    }
}

void wavegen_init_ll( tWaveGen *pThis,
	DAC_HandleTypeDef *hdac,
	TIM_HandleTypeDef *htim1,
	TIM_HandleTypeDef *htim2 )
{
	pThis->hdac = hdac;
	pThis->htim1 = htim1;
	pThis->htim2 = htim2;
}

void wavegen_init( tWaveGen *pThis,
	uint16_t *buffer1,
	uint16_t *buffer2,
	uint16_t len )
{
	pThis->buffer1 = buffer1;
	pThis->buffer2 = buffer2;
	pThis->len = len;
}

void wavegen_start( tWaveGen *pThis, enum eWaveGenChannel channel )
{
	if( channel == WAVEGEN_CHANNEL_1 )
	{
		HAL_DAC_Start_DMA( pThis->hdac, DAC_CHANNEL_1, (uint32_t*)pThis->buffer1, pThis->len, DAC_ALIGN_12B_R );
		HAL_TIM_Base_Start( pThis->htim1 );
	}
	else if( channel == WAVEGEN_CHANNEL_2 )
	{
		HAL_DAC_Start_DMA( pThis->hdac, DAC_CHANNEL_2, (uint32_t*)pThis->buffer2, pThis->len, DAC_ALIGN_12B_R );
		HAL_TIM_Base_Start( pThis->htim2 );
	}
}

void wavegen_stop( tWaveGen *pThis, enum eWaveGenChannel channel )
{
	if( channel == WAVEGEN_CHANNEL_1 )
	{
		HAL_DAC_Stop_DMA( pThis->hdac, DAC_CHANNEL_1 );
		HAL_TIM_Base_Stop( pThis->htim1 );
	}
	else if( channel == WAVEGEN_CHANNEL_2 )
	{
		HAL_DAC_Stop_DMA( pThis->hdac, DAC_CHANNEL_2 );
		HAL_TIM_Base_Stop( pThis->htim2 );
	}
}


void wavegen_config_horizontal( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t frequency )
{
	if( frequency == 0 )
	{
		frequency = 1;
	}
	if( channel == WAVEGEN_CHANNEL_1 )
	{
		pThis->htim1->Init.Prescaler = (170e6 / (frequency*pThis->len))/2 - 1;
		pThis->htim1->Init.Period = 2 - 1;
		HAL_TIM_Base_Init( pThis->htim1 );
	}
	else if( channel == WAVEGEN_CHANNEL_2 )
	{
		pThis->htim2->Init.Prescaler = (170e6 / (frequency*pThis->len))/2 - 1;
		pThis->htim2->Init.Period = 2 - 1;
		HAL_TIM_Base_Init( pThis->htim2 );
	}
}

void wavegen_config_vertical( tWaveGen *pThis, enum eWaveGenChannel channel, enum eWaveGenType type, uint16_t offset, uint16_t scale, uint16_t duty_cycle )
{
	switch( type )
	{
		case WAVEGEN_TYPE_DC:
			wavegen_build_dc( pThis, channel, offset );
			break;
		case WAVEGEN_TYPE_SINE:
			wavegen_build_sine( pThis, channel, offset, scale );
			break;
		case WAVEGEN_TYPE_SQUARE:
			wavegen_build_square( pThis, channel, offset, scale );
			break;
		case WAVEGEN_TYPE_TRIANGLE:
			wavegen_build_triangle( pThis, channel, offset, scale );
			break;
		case WAVEGEN_TYPE_SAWTOOTH:
			wavegen_build_sawtooth( pThis, channel, offset, scale );
			break;
		case WAVEGEN_TYPE_PWM:
			wavegen_build_pwm( pThis, channel, offset, scale, duty_cycle );
			break;
		case WAVEGEN_TYPE_NOISE:
			wavegen_build_noise( pThis, channel, offset, scale );
			break;
	}
}

void wavegen_build_dc( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset )
{
	uint16_t i;
	uint16_t *buffer = (channel == WAVEGEN_CHANNEL_1)?pThis->buffer1:pThis->buffer2;
    for( i = 0; i < pThis->len; i++ )
    {
		buffer[i] = offset;
    }
}

void wavegen_build_sine( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale )
{
	uint16_t i;
	uint16_t *buffer = (channel == WAVEGEN_CHANNEL_1)?pThis->buffer1:pThis->buffer2;
	for( i = 0; i < pThis->len; i++ )
	{
		buffer[i] = sinf(2*M_PI*i/pThis->len)*scale + offset;
	}
}

void wavegen_build_square( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale )
{
	uint16_t i;
	uint16_t *buffer = (channel == WAVEGEN_CHANNEL_1)?pThis->buffer1:pThis->buffer2;
	for( i = 0; i < pThis->len; i++ )
	{
		buffer[i] = (i < pThis->len/2)?offset-scale:offset+scale;
	}
}

void wavegen_build_triangle( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale )
{
	uint16_t i;
	uint16_t *buffer = (channel == WAVEGEN_CHANNEL_1)?pThis->buffer1:pThis->buffer2;
	for( i = 0; i < pThis->len; i++ )
	{
        if (i < pThis->len / 2) {
            // Generate the rising portion of the triangle waveform
            buffer[i] = offset - scale + (uint16_t)(((float)i / (pThis->len / 2)) * (2 * scale));
        } else {
            // Generate the falling portion of the triangle waveform
            buffer[i] = offset + scale - (uint16_t)(((float)(i - pThis->len / 2) / (pThis->len / 2)) * (2 * scale));
        }
	}
}

void wavegen_build_sawtooth( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale )
{
	uint16_t i;
	uint16_t *buffer = (channel == WAVEGEN_CHANNEL_1)?pThis->buffer1:pThis->buffer2;
	for( i = 0; i < pThis->len; i++ )
	{
		buffer[i] = offset-scale+i*2*scale/pThis->len;
	}
}

void wavegen_build_pwm( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale, uint16_t duty_cycle )
{
	uint16_t i;
	uint16_t *buffer = (channel == WAVEGEN_CHANNEL_1)?pThis->buffer1:pThis->buffer2;
	for( i = 0; i < pThis->len; i++ )
	{
		buffer[i] = (i < pThis->len*duty_cycle/100)?offset-scale:offset+scale;
	}
}

void wavegen_build_noise( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale )
{
	uint16_t i;
	uint16_t *buffer = (channel == WAVEGEN_CHANNEL_1)?pThis->buffer1:pThis->buffer2;
	for( i = 0; i < pThis->len; i++ )
	{
		buffer[i] = rand()%scale + offset;
	}
}

void _wavegen_erase( tWaveGen *pThis, tLcd *pLcd )
{
	uint16_t i;
	const float scale = 320/4096.0f;
	for( i = 0; i < pThis->len; i++ )
	{
		lcd_set_pixel( pLcd, i/2, pThis->buffer1[i]*scale, 0x0000 );
		lcd_set_pixel( pLcd, i/2, pThis->buffer2[i]*scale, 0x0000 );
	}
}

#define WAVEGEN_CH1_COLOR 0xF00F
#define WAVEGEN_CH2_COLOR 0x0FF0

void wavegen_draw( tWaveGen *pThis, tLcd *pLcd )
{
	wavegen_stroque( pThis, pLcd, WAVEGEN_COLOR_CH1, WAVEGEN_COLOR_CH2 );
}

void wavegen_erase( tWaveGen *pThis, tLcd *pLcd )
{
	wavegen_stroque( pThis, pLcd, LCD_COLOR_BLACK, LCD_COLOR_BLACK );
}

void wavegen_stroque( tWaveGen *pThis, tLcd *pLcd, uint16_t color1, uint16_t color2 )
{
	uint16_t i;
	int offset = pLcd->height;
	float scale = -320/4096.0f;
	for( i = 0; i < pThis->len; i++ )
	{
		lcd_set_pixel( pLcd, i/2, scale*pThis->buffer1[i]+offset, color1 );
		lcd_set_pixel( pLcd, i/2, scale*pThis->buffer2[i]+offset, color2 );
	}
}

