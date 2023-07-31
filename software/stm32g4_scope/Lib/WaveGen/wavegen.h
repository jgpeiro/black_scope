/*
 * wavegen.h
 *
 *  Created on: Jul 31, 2023
 *      Author: jgpei
 */

#ifndef WAVEGEN_H_
#define WAVEGEN_H_

#include <stdint.h>

enum eWaveGenType
{
	WAVEGEN_TYPE_NONE,
	WAVEGEN_TYPE_DC,
	WAVEGEN_TYPE_PWM,
	WAVEGEN_TYPE_SINE,
	WAVEGEN_TYPE_TRIANGLE,
	WAVEGEN_TYPE_SAWTOOTH,
	WAVEGEN_TYPE_NOISE,
	WAVEGEN_TYPE_MAX
};



void wavegen_build_dc( uint16_t *buffer, uint16_t len, float offset );
void wavegen_build_sine( uint16_t *buffer, uint16_t len, float sample_rate, float amplitude, float offset, float frequency );



#endif /* WAVEGEN_H_ */
