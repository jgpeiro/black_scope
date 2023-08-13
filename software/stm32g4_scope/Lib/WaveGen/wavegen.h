/*
 * wavegen.h
 *
 *  Created on: Jul 31, 2023
 *      Author: jgpei
 */

#ifndef WAVEGEN_H_
#define WAVEGEN_H_

#include <stdint.h>
#include "stm32g4xx_hal.h"

enum eWaveGenType
{
	WAVEGEN_TYPE_DC,
	WAVEGEN_TYPE_SINE,
	WAVEGEN_TYPE_SQUARE,
	WAVEGEN_TYPE_TRIANGLE,
	WAVEGEN_TYPE_SAWTOOTH,
	WAVEGEN_TYPE_PWM,
	WAVEGEN_TYPE_NOISE,
	WAVEGEN_TYPE_MAX
};

struct sWaveGen
{
	DAC_HandleTypeDef *hdac;
	TIM_HandleTypeDef *htim;

	uint16_t *buffer0;
	uint16_t *buffer1;
	uint16_t len;
	enum eWaveGenType type;
	float sample_rate;
};
typedef struct sWaveGen tWaveGen;

void wavegen_init( tWaveGen *pThis, 
	DAC_HandleTypeDef *hdac,
	TIM_HandleTypeDef *htim,
	uint16_t *buffer0, 
	uint16_t *buffer1, 
	uint16_t len,
	float sample_rate
);

void wavegen_start( tWaveGen *pThis, uint8_t channels );
void wavegen_stop( tWaveGen *pThis, uint8_t channels );

void wavegen_build_dc( tWaveGen *pThis, uint8_t channels, float offset );
void wavegen_build_sine( tWaveGen *pThis, uint8_t channels, float frequency, float amplitude, float offset );
void wavegen_build_square( tWaveGen *pThis, uint8_t channels, float frequency, float amplitude, float offset );
void wavegen_build_triangle( tWaveGen *pThis, uint8_t channels, float frequency, float amplitude, float offset );
void wavegen_build_sawtooth( tWaveGen *pThis, uint8_t channels, float frequency, float amplitude, float offset );
void wavegen_build_pwm( tWaveGen *pThis, uint8_t channels, float frequency, float amplitude, float offset, float duty_cycle );
void wavegen_build_noise( tWaveGen *pThis, uint8_t channels, float frequency, float amplitude, float offset );

#endif /* WAVEGEN_H_ */
