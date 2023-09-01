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

#define SCOPE_COLOR_ACQUIRE    LCD_COLOR_RED
#define SCOPE_COLOR_HORIZONTAL LCD_COLOR_GREEN
#define SCOPE_COLOR_VERTICAL   LCD_COLOR_BLUE
#define SCOPE_COLOR_TRIGGER    LCD_COLOR_MAGENTA

#define SCOPE_COLOR_CH1			LCD_COLOR_RED
#define SCOPE_COLOR_CH2			LCD_COLOR_GREEN
#define SCOPE_COLOR_CH3			LCD_COLOR_BLUE
#define SCOPE_COLOR_CH4			LCD_COLOR_MAGENTA

/*
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

void scope_init_ll( tScope *scope,
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
void scope_init( tScope *scope, uint16_t trigger_level, uint32_t sample_rate, uint16_t *buffer1, uint16_t *buffer2, uint16_t *buffer3, uint16_t *buffer4, uint16_t len );
void scope_reset( tScope *scope );
void scope_start( tScope *scope );
void scope_stop( tScope *scope );
uint8_t scope_is_busy( tScope *scope );
int32_t scope_get_trigger( tScope *scope );

uint8_t scope_is_running( tScope *scope );
uint8_t scope_wait( tScope *scope, uint32_t timeout_ms );

void test_scope( tLcd *pLcd, int collapsed );

void scope_config_horizontal( tScope *scope, int sample_rate, int buffer_len );
void scope_config_vertical( tScope *scope, int gain1, int gain2, int gain3, int gain4, int offset );
void scope_config_trigger( tScope *scope, int channel, int mode, int level, int slope );
*/

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

void scope_draw( tScope *pThis, tLcd *pLcd, int is_collapsed );
void scope_draw_acquire( tScope *pThis, tLcd *pLcd, int is_collapsed );
void scope_draw_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, int len, int is_collapsed );
void scope_draw_vertical( tScope_Vertical *pThis, tLcd *pLcd, int is_collapsed );
void scope_draw_trigger( tScope_Trigger *pThis, tLcd *pLcd, int is_collapsed );
void scope_draw_signals( tScope *pThis, tLcd *pLcd, int is_collapsed );
void scope_clear( tScope *pThis, tLcd *pLcd, int is_collapsed );

void scope_draw( tScope *pThis, tLcd *pLcd, int is_collapsed );
void scope_erase( tScope *pThis, tLcd *pLcd, int is_collapsed );
void scope_stroque( tScope *pThis, tLcd *pLcd, uint16_t color_h, uint16_t color_v, uint16_t color_t, int is_collapsed );

void scope_draw_acquire( tScope *pThis, tLcd *pLcd, int is_collapsed );
void scope_erase_acquire( tScope *pThis, tLcd *pLcd, int is_collapsed );
void scope_stroque_acquire( tScope *pThis, tLcd *pLcd, uint16_t color_h, uint16_t color_v, uint16_t color_t, int is_collapsed );

//void scope_draw_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, int is_collapsed );
//void scope_erase_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, int is_collapsed );
//void scope_stroque_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, uint16_t color_h, uint16_t color_v, uint16_t color_t, int is_collapsed );



uint8_t scope_is_continuous( tScope *pThis );
uint8_t scope_is_running( tScope *pThis );
uint8_t scope_is_busy( tScope *pThis );

#endif /* INC_SCOPE_H_ */
