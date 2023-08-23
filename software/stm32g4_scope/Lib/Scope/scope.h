/*
 * scope.h
 *
 *  Created on: Jul 26, 2023
 *      Author: jgpei
 */

#ifndef INC_SCOPE_H_
#define INC_SCOPE_H_

#include <stdint.h>
#include "stm32g4xx_hal.h"
#include "lcd.h"

enum eScopeState
{
	SCOPE_STATE_RESET = 0,
	SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE = 1,
	SCOPE_STATE_WAIT_FOR_ARM = 2,
	SCOPE_STATE_WAIT_FOR_TRIGGER = 3,
	SCOPE_STATE_WAIT_FOR_STOP = 4,
	SCOPE_STATE_DONE = 5,
	SCOPE_STATE_MAX,
};
typedef enum eScopeState tScopeState;


struct sScope_Horizontal
{
	TIM_HandleTypeDef *htim_clock; // ADC clock. TIMER2
	TIM_HandleTypeDef *htim_stop;  // Stop timer. TIMER3
};
typedef struct sScope_Horizontal tScope_Horizontal;

struct sScope_Vertical
{
    OPAMP_HandleTypeDef *hopamp1; // CH1 OPAMP1
    OPAMP_HandleTypeDef *hopamp2; // CH2 OPAMP3
    OPAMP_HandleTypeDef *hopamp3; // CH3 OPAMP5
    OPAMP_HandleTypeDef *hopamp4; // CH4 OPAMP6
    DAC_HandleTypeDef *hdac;      // Offset DAC
};
typedef struct sScope_Vertical tScope_Vertical;

struct sScope_Trigger
{
    ADC_HandleTypeDef *hadc1; // CH1 ADC1
    ADC_HandleTypeDef *hadc2; // CH2 ADC3
    ADC_HandleTypeDef *hadc3; // CH3 ADC5
    ADC_HandleTypeDef *hadc4; // CH4 ADC4
};
typedef struct sScope_Trigger tScope_Trigger;

struct sScope_Wavegen
{
    DAC_HandleTypeDef *hdac;  // DAC1 for CH1 and CH2
    TIM_HandleTypeDef *htim1; // CH1 clock. TIMER4
    TIM_HandleTypeDef *htim2; // CH2 clock. TIMER6
};
typedef struct sScope_Wavegen tScope_Wavegen;

struct sScope
{
	volatile tScopeState state;
	uint16_t sample_rate;
	uint16_t *buffer1;
	uint16_t *buffer2;
	uint16_t *buffer3;
	uint16_t *buffer4;
	uint16_t len;
	uint16_t trigger_level;
	uint16_t trigger_location;

	OPAMP_HandleTypeDef *hopamp1;
	OPAMP_HandleTypeDef *hopamp2;
	OPAMP_HandleTypeDef *hopamp3;
	OPAMP_HandleTypeDef *hopamp4;

	ADC_HandleTypeDef *hadc1;
	ADC_HandleTypeDef *hadc2;
	ADC_HandleTypeDef *hadc3;
	ADC_HandleTypeDef *hadc4;

	DMA_HandleTypeDef *hdma_adc1;
	DMA_HandleTypeDef *hdma_adc2;
	DMA_HandleTypeDef *hdma_adc3;
	DMA_HandleTypeDef *hdma_adc4;

	TIM_HandleTypeDef *htim1;
	TIM_HandleTypeDef *htim2;

	volatile int32_t CNDTRs[SCOPE_STATE_MAX];

    tScope_Horizontal horizontal;
    tScope_Vertical vertical;
    tScope_Trigger trigger;
    tScope_Wavegen wavegen;

};
typedef struct sScope tScope;

void scope_init( tScope *scope, uint16_t trigger_level, uint32_t sample_rate, uint16_t *buffer1, uint16_t *buffer2, uint16_t *buffer3, uint16_t *buffer4, uint16_t len );
void scope_reset( tScope *scope );
void scope_start( tScope *scope );
void scope_stop( tScope *scope );
uint8_t scope_is_busy( tScope *scope );
int32_t scope_get_trigger( tScope *scope );

void test_scope( tLcd *pLcd, int collapsed );

void scope_config_horizontal( tScope *scope, int sample_rate, int buffer_len );
void scope_config_vertical( tScope *scope, int gain1, int gain2, int gain3, int gain4, int offset );
void scope_config_trigger( tScope *scope, int channel, int mode, int level, int slope );

#endif /* INC_SCOPE_H_ */
