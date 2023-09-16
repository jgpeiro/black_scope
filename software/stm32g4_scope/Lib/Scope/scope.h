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

/**
 * @brief Enumeration representing the possible states of the scope.
 */
enum eScopeState
{
	SCOPE_STATE_IDLE = 0,                    /**< Scope is idle. */
	SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE = 1, /**< Waiting for conversion to complete. */
	SCOPE_STATE_WAIT_FOR_ARM = 2,            /**< Waiting for arm signal. */
	SCOPE_STATE_WAIT_FOR_TRIGGER = 3,        /**< Waiting for trigger signal. */
	SCOPE_STATE_WAIT_FOR_STOP = 4,           /**< Waiting for stop signal. */
	SCOPE_STATE_DONE = 5                     /**< Scope operation is complete. */
};
typedef enum eScopeState tScopeState;

/**
 * @brief Enumeration representing the trigger slope for the scope.
 */
enum eScopeTriggerSlope
{
	SCOPE_TRIGGER_SLOPE_RISING = 0, /**< Trigger on a rising edge. */
	SCOPE_TRIGGER_SLOPE_FALLING     /**< Trigger on a falling edge. */
};
typedef enum eScopeTriggerSlope tScopeTriggerSlope;

/**
 * @brief Structure representing the horizontal configuration of the scope.
 */
struct sScope_Horizontal
{
	TIM_HandleTypeDef *htim_clock; /**< Pointer to the timer handle for clock generation. */
	TIM_HandleTypeDef *htim_stop;  /**< Pointer to the timer handle for stop control. */
	
	int32_t offset; /**< Horizontal offset. */
	int32_t scale;  /**< Horizontal scale. */
};
typedef struct sScope_Horizontal tScope_Horizontal;

/**
 * @brief Structure representing the vertical configuration of the scope.
 */
struct sScope_Vertical
{
	DAC_HandleTypeDef *hdac; /**< Pointer to the DAC handle. */
	OPAMP_HandleTypeDef *hopamp1; /**< Pointer to OPAMP1 handle. */
	OPAMP_HandleTypeDef *hopamp2; /**< Pointer to OPAMP2 handle. */
	OPAMP_HandleTypeDef *hopamp3; /**< Pointer to OPAMP3 handle. */
	OPAMP_HandleTypeDef *hopamp4; /**< Pointer to OPAMP4 handle. */

	int32_t offset; /**< Vertical offset. */
	int32_t gain1;  /**< Vertical gain for channel 1. */
	int32_t gain2;  /**< Vertical gain for channel 2. */
	int32_t gain3;  /**< Vertical gain for channel 3. */
	int32_t gain4;  /**< Vertical gain for channel 4. */

	uint8_t enable1;
	uint8_t enable2;
	uint8_t enable3;
	uint8_t enable4;

	int32_t offset1;
	int32_t offset2;
	int32_t offset3;
	int32_t offset4;

	int32_t scale1;
	int32_t scale2;
	int32_t scale3;
	int32_t scale4;
};
typedef struct sScope_Vertical tScope_Vertical;

/**
 * @brief Structure representing the trigger configuration of the scope.
 */
struct sScope_Trigger
{
	ADC_HandleTypeDef *hadc1; /**< Pointer to ADC handle for channel 1. */
	ADC_HandleTypeDef *hadc2; /**< Pointer to ADC handle for channel 2. */
	ADC_HandleTypeDef *hadc3; /**< Pointer to ADC handle for channel 3. */
	ADC_HandleTypeDef *hadc4; /**< Pointer to ADC handle for channel 4. */

	int channel; /**< Trigger channel. */
	int mode;    /**< Trigger mode. */
	int level;   /**< Trigger level. */
	int slope;   /**< Trigger slope. */
};
typedef struct sScope_Trigger tScope_Trigger;

/**
 * @brief Structure representing the scope.
 */
struct sScope
{
	tScope_Horizontal horizontal; /**< Horizontal configuration. */
	tScope_Vertical vertical;     /**< Vertical configuration. */
	tScope_Trigger trigger;       /**< Trigger configuration. */

	uint16_t *buffer1; /**< Pointer to buffer for channel 1 data. */
	uint16_t *buffer2; /**< Pointer to buffer for channel 2 data. */
	uint16_t *buffer3; /**< Pointer to buffer for channel 3 data. */
	uint16_t *buffer4; /**< Pointer to buffer for channel 4 data. */

	uint16_t *buffer5; /**< Pointer to buffer for channel 5 data. */
	uint16_t *buffer6; /**< Pointer to buffer for channel 6 data. */
	uint16_t *buffer7; /**< Pointer to buffer for channel 7 data. */
	uint16_t *buffer8; /**< Pointer to buffer for channel 8 data. */

	uint16_t len;     /**< Length of the data buffers. */
	uint16_t cnt;     /**< Internal counter. */
	tScopeState state;/**< Current state of the scope. */

	uint32_t dma_cndtr; /**< DMA counter for data transfer. */

	uint8_t continuous; /**< Flag indicating continuous operation. */
	uint8_t trigged;    /**< Flag indicating a trigger event. */
};
typedef struct sScope tScope;

extern tScope *pScope; /**< Pointer to the scope instance. */

/**
 * @brief Initialize the scope.
 *
 * @param pThis Pointer to the scope structure.
 * @param htim_clock Pointer to the timer handle for clock generation.
 * @param htim_stop Pointer to the timer handle for stop control.
 * @param hdac Pointer to the DAC handle.
 * @param hopamp1 Pointer to OPAMP1 handle.
 * @param hopamp2 Pointer to OPAMP2 handle.
 * @param hopamp3 Pointer to OPAMP3 handle.
 * @param hopamp4 Pointer to OPAMP4 handle.
 * @param hadc1 Pointer to ADC handle for channel 1.
 * @param hadc2 Pointer to ADC handle for channel 2.
 * @param hadc3 Pointer to ADC handle for channel 3.
 * @param hadc4 Pointer to ADC handle for channel 4.
 */
void scope_init_ll(tScope *pThis,
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
                   ADC_HandleTypeDef *hadc4);

/**
 * @brief Initialize the scope buffers and configuration.
 *
 * @param pThis Pointer to the scope structure.
 * @param buffer1 Pointer to buffer for channel 1 data.
 * @param buffer2 Pointer to buffer for channel 2 data.
 * @param buffer3 Pointer to buffer for channel 3 data.
 * @param buffer4 Pointer to buffer for channel 4 data.
 * @param buffer5 Pointer to buffer for channel 5 data.
 * @param buffer6 Pointer to buffer for channel 6 data.
 * @param buffer7 Pointer to buffer for channel 7 data.
 * @param buffer8 Pointer to buffer for channel 8 data.
 * @param len Length of the data buffers.
 */
void scope_init(tScope *pThis,
               uint16_t *buffer1,
               uint16_t *buffer2,
               uint16_t *buffer3,
               uint16_t *buffer4,
               uint16_t *buffer5,
               uint16_t *buffer6,
               uint16_t *buffer7,
               uint16_t *buffer8,
               uint16_t len);

/**
 * @brief Configure the horizontal settings of the scope.
 *
 * @param pThis Pointer to the scope structure.
 * @param offset Horizontal offset.
 * @param scale Horizontal scale.
 */
void scope_config_horizontal(tScope *pThis, int offset, int scale);

/**
 * @brief Configure the vertical settings of the scope.
 *
 * @param pThis Pointer to the scope structure.
 * @param offset Vertical offset.
 * @param scale1 Vertical gain for channel 1.
 * @param scale2 Vertical gain for channel 2.
 * @param scale3 Vertical gain for channel 3.
 * @param scale4 Vertical gain for channel 4.
 */
void scope_config_vertical(tScope *pThis, int offset, int scale1, int scale2, int scale3, int scale4);

/**
 * @brief Configure the trigger settings of the scope.
 *
 * @param pThis Pointer to the scope structure.
 * @param channel Trigger channel.
 * @param mode Trigger mode.
 * @param level Trigger level.
 * @param slope Trigger slope.
 */
void scope_config_trigger(tScope *pThis, int channel, int mode, int level, int slope);

/**
 * @brief Start the scope operation.
 *
 * @param pThis Pointer to the scope structure.
 * @param continuous Flag indicating continuous operation.
 */
void scope_start(tScope *pThis, uint8_t continuous);

/**
 * @brief Stop the scope operation.
 *
 * @param pThis Pointer to the scope structure.
 */
void scope_stop(tScope *pThis);

/**
 * @brief Wait for the scope operation to complete.
 *
 * @param pThis Pointer to the scope structure.
 * @param timeout_ms Timeout duration in milliseconds.
 * @return 1 if the scope operation is complete, 0 if the timeout occurs.
 */
uint8_t scope_wait(tScope *pThis, uint32_t timeout_ms);

/**
 * @brief Check if the scope is in continuous mode.
 *
 * @param pThis Pointer to the scope structure.
 * @return 1 if the scope is in continuous mode, 0 otherwise.
 */
uint8_t scope_is_continuous(tScope *pThis);

/**
 * @brief Check if the scope is currently running.
 *
 * @param pThis Pointer to the scope structure.
 * @return 1 if the scope is running, 0 otherwise.
 */
uint8_t scope_is_running(tScope *pThis);

/**
 * @brief Check if the scope is busy with an operation.
 *
 * @param pThis Pointer to the scope structure.
 * @return 1 if the scope is busy, 0 otherwise.
 */
uint8_t scope_is_busy(tScope *pThis);

#endif /* INC_SCOPE_H_ */
