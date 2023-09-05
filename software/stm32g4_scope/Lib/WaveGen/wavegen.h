/**
  ******************************************************************************
  * @file    wavegen.h
  * @brief   Waveform Generator HAL module driver.
  * @author  jgpei
  *
  ******************************************************************************
  * @attention
  *
  * Copyright (c) [Year] [Author].
  * All rights reserved.
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                    ##### Waveform Generator Peripheral features #####
  ==============================================================================
  [..]
    (+) This module provides firmware functions to manage the Waveform Generator
        (WaveGen) peripheral for generating various types of waveforms.

    (+) The WaveGen peripheral can generate waveforms such as DC, sine, square,
        triangle, sawtooth, PWM, and noise on specific channels.

    (+) It supports configuring the frequency, type, offset, scale, and duty cycle
        of the generated waveforms.

    (+) The module uses the DAC and TIM peripherals for waveform generation.

                     ##### How to use this driver #####
  ==============================================================================
  [..]
    (#) Include this header file in your project:

        ```c
        #include "wavegen.h"
        ```

    (#) Initialize the WaveGen module using `wavegen_init_ll()` for low-level
        initialization or `wavegen_init()` for waveform buffer setup.

    (#) Start and stop waveform generation on specific channels using `wavegen_start()`
        and `wavegen_stop()`.

    (#) Configure horizontal parameters (frequency) using `wavegen_config_horizontal()`.

    (#) Configure vertical parameters (type, offset, scale, duty cycle) using
        `wavegen_config_vertical()`.

    (#) Build specific waveforms (DC, sine, square, triangle, sawtooth, PWM, noise)
        using the corresponding `wavegen_build_XXX()` functions.

  @endverbatim
  ******************************************************************************
  */

#ifndef WAVEGEN_H_
#define WAVEGEN_H_

#include <stdint.h>
#include "stm32g4xx_hal.h"

/**
 * @brief Enumeration of WaveGen channels.
 */
enum eWaveGenChannel
{
    WAVEGEN_CHANNEL_1, /**< WaveGen Channel 1 */
    WAVEGEN_CHANNEL_2  /**< WaveGen Channel 2 */
};

/**
 * @brief Enumeration of WaveGen waveform types.
 */
enum eWaveGenType
{
    WAVEGEN_TYPE_DC,       /**< DC waveform */
    WAVEGEN_TYPE_SINE,     /**< Sine waveform */
    WAVEGEN_TYPE_SQUARE,   /**< Square waveform */
    WAVEGEN_TYPE_TRIANGLE, /**< Triangle waveform */
    WAVEGEN_TYPE_SAWTOOTH, /**< Sawtooth waveform */
    WAVEGEN_TYPE_PWM,      /**< PWM waveform */
    WAVEGEN_TYPE_NOISE,    /**< Noise waveform */
    WAVEGEN_TYPE_MAX       /**< Maximum waveform type value */
};

/**
 * @brief Structure to hold WaveGen configuration.
 */
struct sWaveGen
{
    DAC_HandleTypeDef *hdac;   /**< DAC handle */
    TIM_HandleTypeDef *htim1;  /**< TIM1 handle for channel 1 */
    TIM_HandleTypeDef *htim2;  /**< TIM2 handle for channel 2 */

    uint16_t *buffer1;         /**< Waveform buffer for channel 1 */
    uint16_t *buffer2;         /**< Waveform buffer for channel 2 */
    uint16_t len;              /**< Buffer length */
};

/**
 * @brief Type definition for WaveGen structure.
 */
typedef struct sWaveGen tWaveGen;

/**
 * @brief Initialize the WaveGen module at a low level.
 *
 * This function initializes the WaveGen module, providing low-level
 * initialization of the DAC and TIM peripherals.
 *
 * @param pThis Pointer to the WaveGen structure.
 * @param hdac Pointer to the DAC handle.
 * @param htim1 Pointer to the TIM1 handle for Channel 1.
 * @param htim2 Pointer to the TIM2 handle for Channel 2.
 */
void wavegen_init_ll(tWaveGen *pThis, DAC_HandleTypeDef *hdac, TIM_HandleTypeDef *htim1, TIM_HandleTypeDef *htim2);

/**
 * @brief Initialize the WaveGen module.
 *
 * This function initializes the WaveGen module and sets up the waveform
 * buffer.
 *
 * @param pThis Pointer to the WaveGen structure.
 * @param buffer1 Pointer to the waveform buffer for Channel 1.
 * @param buffer2 Pointer to the waveform buffer for Channel 2.
 * @param len Length of the waveform buffer.
 */
void wavegen_init(tWaveGen *pThis, uint16_t *buffer1, uint16_t *buffer2, uint16_t len);

/**
 * @brief Start waveform generation on a specific channel.
 *
 * @param pThis Pointer to the WaveGen structure.
 * @param channel WaveGen channel (CHANNEL_1 or CHANNEL_2).
 */
void wavegen_start(tWaveGen *pThis, enum eWaveGenChannel channel);

/**
 * @brief Stop waveform generation on a specific channel.
 *
 * @param pThis Pointer to the WaveGen structure.
 * @param channel WaveGen channel (CHANNEL_1 or CHANNEL_2).
 */
void wavegen_stop(tWaveGen *pThis, enum eWaveGenChannel channel);

/**
 * @brief Configure horizontal parameters (frequency) for a specific channel.
 *
 * @param pThis Pointer to the WaveGen structure.
 * @param channel WaveGen channel (CHANNEL_1 or CHANNEL_2).
 * @param frequency Frequency value.
 */
void wavegen_config_horizontal(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t frequency);

/**
 * @brief Configure vertical parameters (type, offset, scale, duty cycle) for a specific channel.
 *
 * @param pThis Pointer to the WaveGen structure.
 * @param channel WaveGen channel (CHANNEL_1 or CHANNEL_2).
 * @param type Waveform type.
 * @param offset Waveform offset.
 * @param scale Waveform scale.
 * @param duty_cycle Duty cycle (for PWM waveform).
 */
void wavegen_config_vertical(tWaveGen *pThis, enum eWaveGenChannel channel, enum eWaveGenType type, uint16_t offset, uint16_t scale, uint16_t duty_cycle);

/**
 * @brief Build a DC waveform on a specific channel.
 *
 * @param pThis Pointer to the WaveGen structure.
 * @param channel WaveGen channel (CHANNEL_1 or CHANNEL_2).
 * @param offset DC offset value.
 */
void wavegen_build_dc(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset);

/**
 * @brief Build a sine waveform on a specific channel.
 *
 * @param pThis Pointer to the WaveGen structure.
 * @param channel WaveGen channel (CHANNEL_1 or CHANNEL_2).
 * @param offset Offset value.
 * @param scale Scale value.
 */
void wavegen_build_sine(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale);

/**
 * @brief Build a square waveform on a specific channel.
 *
 * @param pThis Pointer to the WaveGen structure.
 * @param channel WaveGen channel (CHANNEL_1 or CHANNEL_2).
 * @param offset Offset value.
 * @param scale Scale value.
 */
void wavegen_build_square(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale);

/**
 * @brief Build a triangle waveform on a specific channel.
 *
 * @param pThis Pointer to the WaveGen structure.
 * @param channel WaveGen channel (CHANNEL_1 or CHANNEL_2).
 * @param offset Offset value.
 * @param scale Scale value.
 */
void wavegen_build_triangle(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale);

/**
 * @brief Build a sawtooth waveform on a specific channel.
 *
 * @param pThis Pointer to the WaveGen structure.
 * @param channel WaveGen channel (CHANNEL_1 or CHANNEL_2).
 * @param offset Offset value.
 * @param scale Scale value.
 */
void wavegen_build_sawtooth(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale);

/**
 * @brief Build a PWM waveform on a specific channel.
 *
 * @param pThis Pointer to the WaveGen structure.
 * @param channel WaveGen channel (CHANNEL_1 or CHANNEL_2).
 * @param offset Offset value.
 * @param scale Scale value.
 * @param duty_cycle Duty cycle value.
 */
void wavegen_build_pwm(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale, uint16_t duty_cycle);

/**
 * @brief Build a noise waveform on a specific channel.
 *
 * @param pThis Pointer to the WaveGen structure.
 * @param channel WaveGen channel (CHANNEL_1 or CHANNEL_2).
 * @param offset Offset value.
 * @param scale Scale value.
 */
void wavegen_build_noise(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale);

#endif /* WAVEGEN_H_ */
