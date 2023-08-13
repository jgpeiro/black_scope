/*
 * wavegen.c
 *
 *  Created on: Jul 31, 2023
 *      Author: jgpei
 */

#include <math.h>
#include <stdlib.h>
#include "wavegen.h"
/*
void wavegen_build_dc( uint16_t *buffer, uint16_t len, float offset )
{
    uint16_t i;
    for( i = 0; i < len; i++ )
    {
        buffer[i] = offset;
    }
}

void wavegen_build_sine( uint16_t *buffer, uint16_t len, float sample_rate, float amplitude, float offset, float frequency )
{
    uint16_t i;
    float t;
    float dt = 1.0f / sample_rate;
    float omega = 2.0f * M_PI * frequency;
    for( i = 0; i < len; i++ )
    {
        t = i * dt;
        buffer[i] = amplitude * sinf( omega * t ) + offset;
    }
}
*/

void wavegen_init( tWaveGen *pThis, 
	DAC_HandleTypeDef *hdac,
	TIM_HandleTypeDef *htim,
	uint16_t *buffer0, 
	uint16_t *buffer1, 
	uint16_t len,
	float sample_rate )
{
    pThis->hdac = hdac;
    pThis->htim = htim;
    pThis->buffer0 = buffer0;
    pThis->buffer1 = buffer1;
    pThis->len = len;
    pThis->type = WAVEGEN_TYPE_DC;
    pThis->sample_rate = sample_rate;

    memset( pThis->buffer0, 0, len * sizeof( uint16_t ) );
    memset( pThis->buffer1, 0, len * sizeof( uint16_t ) );
}

void wavegen_start( tWaveGen *pThis, uint8_t channels )
{
	//if( channels & 0x01 )
    {
        HAL_DAC_Start_DMA( pThis->hdac, DAC_CHANNEL_1, (uint32_t*)pThis->buffer0, pThis->len, DAC_ALIGN_12B_R );
    }
    //if( channels & 0x02 )
    {
        HAL_DAC_Start_DMA( pThis->hdac, DAC_CHANNEL_2, (uint32_t*)pThis->buffer1, pThis->len, DAC_ALIGN_12B_R );
    }
    //if( channels )
    {
        HAL_TIM_Base_Start( pThis->htim );
    }
}

void wavegen_stop( tWaveGen *pThis, uint8_t channels )
{
	//if( channels & 0x01 )
    {
        HAL_DAC_Stop_DMA( pThis->hdac, DAC_CHANNEL_1 );
    }
    //if( channels & 0x02 )
    {
        HAL_DAC_Stop_DMA( pThis->hdac, DAC_CHANNEL_2 );
    }
    //if( channels == 0x03 )
    {
        HAL_TIM_Base_Start( pThis->htim );
    }
}

void wavegen_build_dc( tWaveGen *pThis, uint8_t channels, float offset )
{
    uint16_t i;
    for( i = 0; i < pThis->len; i++ )
    {
        if( channels & 0x01 )
        {
            pThis->buffer0[i] = offset;
        }
        if( channels & 0x02 )
        {
            pThis->buffer1[i] = offset;
        }
    }
}

void wavegen_build_sine( tWaveGen *pThis, uint8_t channels, float frequency, float amplitude, float offset )
{
    uint16_t i;
    float t;
    float dt = 1.0f / pThis->sample_rate;
    float omega = 2.0f * M_PI * frequency;
    for( i = 0; i < pThis->len; i++ )
    {
        t = i * dt;
        if( channels & 0x01 )
        {
            pThis->buffer0[i] = amplitude * sinf( omega * t ) + offset;
        }
        if( channels & 0x02 )
        {
            pThis->buffer1[i] = amplitude * sinf( omega * t ) + offset;
        }
    }
}

void wavegen_build_square( tWaveGen *pThis, uint8_t channels, float frequency, float amplitude, float offset )
{
    uint16_t i;
    float t;
    float dt = 1.0f / pThis->sample_rate;
    float omega = 2.0f * M_PI * frequency;
    for( i = 0; i < pThis->len; i++ )
    {
        t = i * dt;
        if( channels & 0x01 )
        {
            if( sinf( omega * t ) > 0.0f )
            {
                pThis->buffer0[i] = amplitude + offset;
            }
            else
            {
                pThis->buffer0[i] = -amplitude + offset;
            }
        }
        if( channels & 0x02 )
        {
            if( sinf( omega * t ) > 0.0f )
            {
                pThis->buffer0[i] = amplitude + offset;
            }
            else
            {
                pThis->buffer0[i] = -amplitude + offset;
            }
        }
    }
}

void wavegen_build_triangle( tWaveGen *pThis, uint8_t channels, float frequency, float amplitude, float offset )
{
    uint16_t i;
    float t;
    float dt = 1.0f / pThis->sample_rate;
    float omega = 2.0f * M_PI * frequency;
    for( i = 0; i < pThis->len; i++ )
    {
        t = i * dt;
        if( channels & 0x01 )
        {
            pThis->buffer0[i] = amplitude * asinf( sinf( omega * t ) ) + offset;
        }
        if( channels & 0x02 )
        {
            pThis->buffer1[i] = amplitude * asinf( sinf( omega * t ) ) + offset;
        }
    }
}

void wavegen_build_sawtooth( tWaveGen *pThis, uint8_t channels, float frequency, float amplitude, float offset )
{
    uint16_t i;
    float t;
    float dt = 1.0f / pThis->sample_rate;
    float omega = 2.0f * M_PI * frequency;
    for( i = 0; i < pThis->len; i++ )
    {
        t = i * dt;
        if( channels & 0x01 )
        {
            pThis->buffer0[i] = amplitude * ( t - floorf( t ) ) + offset;
        }
        if( channels & 0x02 )
        {
            pThis->buffer1[i] = amplitude * ( t - floorf( t ) ) + offset;
        }
    }
}

void wavegen_build_pwm( tWaveGen *pThis, uint8_t channels, float frequency, float amplitude, float offset, float duty_cycle )
{
    uint16_t i;
    float t;
    float dt = 1.0f / pThis->sample_rate;
    float omega = 2.0f * M_PI * frequency;
    for( i = 0; i < pThis->len; i++ )
    {
        t = i * dt;
        if( channels & 0x01 )
        {
            if( ( t - floorf( t ) ) < duty_cycle )
            {
                pThis->buffer0[i] = amplitude + offset;
            }
            else
            {
                pThis->buffer0[i] = -amplitude + offset;
            }
        }
        if( channels & 0x02 )
        {
            if( ( t - floorf( t ) ) < duty_cycle )
            {
                pThis->buffer0[i] = amplitude + offset;
            }
            else
            {
                pThis->buffer0[i] = -amplitude + offset;
            }
        }
    }
}
void wavegen_build_noise( tWaveGen *pThis, uint8_t channels, float frequency, float amplitude, float offset )
{
    uint16_t i;
    float t;
    float dt = 1.0f / pThis->sample_rate;
    float omega = 2.0f * M_PI * frequency;
    for( i = 0; i < pThis->len; i++ )
    {
        t = i * dt;
        if( channels & 0x01 )
        {
            pThis->buffer0[i] = amplitude * ( 2.0f * ( (float)rand() / (float)RAND_MAX ) - 1.0f ) + offset;
        }
        if( channels & 0x02 )
        {
            pThis->buffer1[i] = amplitude * ( 2.0f * ( (float)rand() / (float)RAND_MAX ) - 1.0f ) + offset;
        }
    }
}

/*
int main()
{
    tWavegen wavegen;
    wavegen_init( &wavegen, 1000, 1000 );
    wavegen_build_sine( &wavegen, 0x01, 1000, 1.0f, 0.0f );
    wavegen_build_sine( &wavegen, 0x02, 1000, 1.0f, 0.0f );
    wavegen_start( &wavegen, 0x03 );
    HAL_Delay( 1000 );
    wavegen_stop( &wavegen, 0x03 );
}
*/
