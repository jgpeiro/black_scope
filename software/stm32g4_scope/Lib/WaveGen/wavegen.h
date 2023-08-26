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

#include "lcd.h"

enum eWaveGenChannel
{
	WAVEGEN_CHANNEL_1,
	WAVEGEN_CHANNEL_2
};

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
	TIM_HandleTypeDef *htim1;
	TIM_HandleTypeDef *htim2;

	uint16_t *buffer1;
	uint16_t *buffer2;
	uint16_t len;
};
typedef struct sWaveGen tWaveGen;

void wavegen_init_ll( tWaveGen *pThis,
	DAC_HandleTypeDef *hdac,
	TIM_HandleTypeDef *htim1,
	TIM_HandleTypeDef *htim2
);

void wavegen_init( tWaveGen *pThis,
	uint16_t *buffer1,
	uint16_t *buffer2,
	uint16_t len
);

void wavegen_start( tWaveGen *pThis, enum eWaveGenChannel channel );
void wavegen_stop( tWaveGen *pThis, enum eWaveGenChannel channel );
void wavegen_config_horizontal( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t frequency );
void wavegen_config_vertical( tWaveGen *pThis, enum eWaveGenChannel channel, enum eWaveGenType type, uint16_t offset, uint16_t scale, uint16_t duty_cycle );

void wavegen_build_dc( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset );
void wavegen_build_sine( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale );
void wavegen_build_square( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale );
void wavegen_build_triangle( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale );
void wavegen_build_sawtooth( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale );
void wavegen_build_pwm( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale, uint16_t duty_cycle );
void wavegen_build_noise( tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale );

void wavegen_draw( tWaveGen *pThis, tLcd *pLcd );
void wavegen_erase( tWaveGen *pThis, tLcd *pLcd );

#endif /* WAVEGEN_H_ */
