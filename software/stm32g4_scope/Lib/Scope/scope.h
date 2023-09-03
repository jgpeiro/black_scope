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
	SCOPE_STATE_IDLE = 0,
	SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE = 1,
	SCOPE_STATE_WAIT_FOR_ARM = 2,
	SCOPE_STATE_WAIT_FOR_TRIGGER = 3,
	SCOPE_STATE_WAIT_FOR_STOP = 4,
	SCOPE_STATE_DONE = 5
};
typedef enum eScopeState tScopeState;

enum eScopeTriggerSlope
{
	SCOPE_TRIGGER_SLOPE_RISING = 0,
	SCOPE_TRIGGER_SLOPE_FALLING
};
typedef enum eScopeTriggerSlope tScopeTriggerSlope;


struct sScope_Horizontal
{
	TIM_HandleTypeDef *htim_clock;
	TIM_HandleTypeDef *htim_stop;
	
	int32_t offset;
	int32_t scale;
};
typedef struct sScope_Horizontal tScope_Horizontal;

struct sScope_Vertical
{
	DAC_HandleTypeDef *hdac;
	OPAMP_HandleTypeDef *hopamp1;
	OPAMP_HandleTypeDef *hopamp2;
	OPAMP_HandleTypeDef *hopamp3;
	OPAMP_HandleTypeDef *hopamp4;

	int32_t offset;
	int32_t gain1;
	int32_t gain2;
	int32_t gain3;
	int32_t gain4;
};
typedef struct sScope_Vertical tScope_Vertical;

struct sScope_Trigger
{
	ADC_HandleTypeDef *hadc1;
	ADC_HandleTypeDef *hadc2;
	ADC_HandleTypeDef *hadc3;
	ADC_HandleTypeDef *hadc4;

	int channel;
	int mode;
	int level;
	int slope;
};
typedef struct sScope_Trigger tScope_Trigger;

struct sScope
{
	tScope_Horizontal horizontal;
	tScope_Vertical vertical;
	tScope_Trigger trigger;

	uint16_t *buffer1;
	uint16_t *buffer2;
	uint16_t *buffer3;
	uint16_t *buffer4;

	uint16_t *buffer5;
	uint16_t *buffer6;
	uint16_t *buffer7;
	uint16_t *buffer8;

	uint16_t len;
	uint16_t cnt;
	tScopeState state;

	uint32_t dma_cndtr;

	uint8_t continuous;
};
typedef struct sScope tScope;

extern tScope *pScope;

//void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef* hadc );
//void HAL_ADC_LevelOutOfWindowCallback( ADC_HandleTypeDef* hadc );
//void HAL_ADCEx_LevelOutOfWindow2Callback( ADC_HandleTypeDef* hadc );
//void HAL_TIM_OC_DelayElapsedCallback( TIM_HandleTypeDef *htim );

//void StartTaskScope(void *argument);

void scope_init_ll( tScope *pThis,
	TIM_HandleTypeDef *htim_clock,
	TIM_HandleTypeDef *htim_stop,
	DAC_HandleTypeDef *hdac,
	OPAMP_HandleTypeDef *hopamp1,
	OPAMP_HandleTypeDef *hopamp2,
	OPAMP_HandleTypeDef *hopamp3,
	OPAMP_HandleTypeDef *hopamp4,
	ADC_HandleTypeDef *hadc1,
	ADC_HandleTypeDef *hadc2,
	ADC_HandleTypeDef *hadc3,
	ADC_HandleTypeDef *hadc4
);

void scope_init( tScope *pThis,
	uint16_t *buffer1,
	uint16_t *buffer2,
	uint16_t *buffer3,
	uint16_t *buffer4,
	uint16_t *buffer5,
	uint16_t *buffer6,
	uint16_t *buffer7,
	uint16_t *buffer8,
	uint16_t len
);

void scope_config_horizontal( tScope *pThis, int offset, int scale );
void scope_config_vertical( tScope *pThis, int offset, int scale1, int scale2, int scale3, int scale4 );
void scope_config_trigger( tScope *pThis, int channel, int mode, int level, int slope );

void scope_start( tScope *pThis, uint8_t continuous );
void scope_stop( tScope *pThis );
uint8_t scope_wait( tScope *pThis, uint32_t timeout_ms );

uint8_t scope_is_continuous( tScope *pThis );
uint8_t scope_is_running( tScope *pThis );
uint8_t scope_is_busy( tScope *pThis );

#endif /* INC_SCOPE_H_ */
