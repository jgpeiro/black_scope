/*
 * wavegen.c
 *
 *  Created on: Jul 31, 2023
 *      Author: jgpei
 */

#include <math.h>
#include "wavegen.h"

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
