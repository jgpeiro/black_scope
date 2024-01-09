/*
 * scope.h
 *
 *  Created on: Sep 20, 2023
 *      Author: jgpei
 */

#ifndef SCOPE_SCOPE_H_
#define SCOPE_SCOPE_H_

#include "main.h"
#include "adc.h"
#include "tim.h"
#include "lcd.h"
#include "cmsis_os.h"

enum eScopeState
{
	SCOPE_STATE_IDLE = 0,
	SCOPE_STATE_WAIT_FOR_CONV_CPLT,
	SCOPE_STATE_WAIT_FOR_ARM,
	SCOPE_STATE_WAIT_FOR_TRIGGER,
	SCOPE_STATE_WAIT_FOR_STOP,
	SCOPE_STATE_DONE,
};
typedef enum eScopeState tScopeState;

enum eScopeSlope
{
	SCOPE_SLOPE_FALLING = 0,
	SCOPE_SLOPE_RISING,
};
typedef enum eScopeSlope tScopeSlope;

struct sScope
{
	ADC_HandleTypeDef *adc;
	TIM_HandleTypeDef *tim_clock;
	TIM_HandleTypeDef *tim_stop;

	tScopeState state;

	int32_t offset_h;
	int32_t scale_h;

	int32_t trigger_v;
	int32_t trigger_h;

	int32_t channel;
	tScopeSlope slope;

	uint16_t *bufferA;
	uint16_t *bufferB;
	uint16_t len;

	uint8_t cnt;
};
typedef struct sScope tScope;

void scope_init( tScope *pThis,
	ADC_HandleTypeDef *adc,
	TIM_HandleTypeDef *tim_clock,
	TIM_HandleTypeDef *tim_stop,
	uint16_t *bufferA,
	uint16_t *bufferB,
	uint16_t len );
void scope_config_horizontal( tScope *pThis, int32_t offset, int32_t scale );
void scope_config_vertical( tScope *pThis, int32_t offset, int32_t scale );
void scope_config_trigger( tScope *pThis, int32_t channel, int32_t trigger, tScopeSlope slope );
void scope_start( tScope *pThis );
uint8_t scope_wait( tScope *pThis, uint32_t timeout_ms );
void scope_stop( tScope *pThis );

void scope_draw( tScope *pThis, tLcd *pLcd );

#endif /* SCOPE_SCOPE_H_ */
