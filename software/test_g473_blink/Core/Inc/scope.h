/*
 * scope.h
 *
 *  Created on: 21 jun. 2023
 *      Author: Jose
 */

#ifndef INC_SCOPE_H_
#define INC_SCOPE_H_

#include <stdint.h>
#include "stm32g4xx_hal.h"

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
};
typedef struct sScope tScope;

void scope_init( tScope *scope, uint16_t trigger_level, uint16_t sample_rate, uint16_t *buffer1, uint16_t *buffer2, uint16_t *buffer3, uint16_t *buffer4, uint16_t len );
void scope_reset( tScope *scope );
void scope_start( tScope *scope );
void scope_stop( tScope *scope );
uint8_t scope_is_busy( tScope *scope );
int32_t scope_get_trigger( tScope *scope );

void test_scope( int collapsed );

#endif /* INC_SCOPE_H_ */
