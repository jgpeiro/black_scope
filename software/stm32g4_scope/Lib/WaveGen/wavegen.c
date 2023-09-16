/*
 * wavegen.c
 *
 *  Created on: Jul 31, 2023
 *      Author: jgpei
 */

#include <math.h>
#include <stdlib.h>
#include "wavegen.h"

#include "buffer.h"
extern tBuffer usb_rx;


#define DAC_BUFFER_LEN 	(512)
#define BUFFER_SIZE (sizeof(uint16_t)*DAC_BUFFER_LEN)

extern uint16_t dac1_buffer[DAC_BUFFER_LEN];
extern uint16_t dac2_buffer[DAC_BUFFER_LEN];

#include "usbd_cdc_if.h"
extern USBD_HandleTypeDef hUsbDeviceFS;
extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

// Callback functions for DAC buffer transfer completion
void HAL_DACEx_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef *hdac);
void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef *hdac);

/**
  * @brief  HAL DAC Half Transfer Complete callback for Channel 2.
  * @param  hdac: DAC handle
  * @retval None
  */
void HAL_DACEx_ConvHalfCpltCallbackCh2(DAC_HandleTypeDef *hdac)
{
    // If a new block is available, copy it to the DAC buffer.
    if (buffer_size(&usb_rx) >= sizeof(dac2_buffer) / 2)
    {
        //buffer_pop(&usb_rx, dac2_buffer, sizeof(dac2_buffer) / 2);

        // Prepare for USB reception
        //USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
        //USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    }
}

/**
  * @brief  HAL DAC Transfer Complete callback for Channel 2.
  * @param  hdac: DAC handle
  * @retval None
  */
void HAL_DACEx_ConvCpltCallbackCh2(DAC_HandleTypeDef *hdac)
{
    // If a new block is available, copy it to the DAC buffer.
    if (buffer_size(&usb_rx) >= sizeof(dac2_buffer) / 2)
    {
        //buffer_pop(&usb_rx, dac2_buffer + DAC_BUFFER_LEN / 2, sizeof(dac2_buffer) / 2);

        // Prepare for USB reception
        //USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
        //USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    }
}

/**
  * @brief  Initializes the WaveGen structure at a low-level.
  * @param  pThis: Pointer to the WaveGen structure.
  * @param  hdac: DAC handle.
  * @param  htim1: TIM1 handle.
  * @param  htim2: TIM2 handle.
  * @retval None
  */
void wavegen_init_ll(tWaveGen *pThis, DAC_HandleTypeDef *hdac, TIM_HandleTypeDef *htim1, TIM_HandleTypeDef *htim2)
{
    pThis->hdac = hdac;
    pThis->htim1 = htim1;
    pThis->htim2 = htim2;
}

/**
  * @brief  Initializes the WaveGen structure.
  * @param  pThis: Pointer to the WaveGen structure.
  * @param  buffer1: Pointer to the first DAC buffer.
  * @param  buffer2: Pointer to the second DAC buffer.
  * @param  len: Length of the buffers.
  * @retval None
  */
void wavegen_init(tWaveGen *pThis, uint16_t *buffer1, uint16_t *buffer2, uint16_t len)
{
    pThis->buffer1 = buffer1;
    pThis->buffer2 = buffer2;
    pThis->len = len;
}

/**
  * @brief  Starts the waveform generation for the specified channel.
  * @param  pThis: Pointer to the WaveGen structure.
  * @param  channel: WaveGen channel (CHANNEL_1 or CHANNEL_2).
  * @retval None
  */
void wavegen_start(tWaveGen *pThis, enum eWaveGenChannel channel)
{
    if (channel == WAVEGEN_CHANNEL_1)
    {
        HAL_DAC_Start_DMA(pThis->hdac, DAC_CHANNEL_1, (uint32_t *)pThis->buffer1, pThis->len, DAC_ALIGN_12B_R);
        HAL_TIM_Base_Start(pThis->htim1);
    }
    else if (channel == WAVEGEN_CHANNEL_2)
    {
        HAL_DAC_Start_DMA(pThis->hdac, DAC_CHANNEL_2, (uint32_t *)pThis->buffer2, pThis->len, DAC_ALIGN_12B_R);
        HAL_TIM_Base_Start(pThis->htim2);
    }
}

/**
  * @brief  Stops the waveform generation for the specified channel.
  * @param  pThis: Pointer to the WaveGen structure.
  * @param  channel: WaveGen channel (CHANNEL_1 or CHANNEL_2).
  * @retval None
  */
void wavegen_stop(tWaveGen *pThis, enum eWaveGenChannel channel)
{
    if (channel == WAVEGEN_CHANNEL_1)
    {
        HAL_DAC_Stop_DMA(pThis->hdac, DAC_CHANNEL_1);
        HAL_TIM_Base_Stop(pThis->htim1);
    }
    else if (channel == WAVEGEN_CHANNEL_2)
    {
        HAL_DAC_Stop_DMA(pThis->hdac, DAC_CHANNEL_2);
        HAL_TIM_Base_Stop(pThis->htim2);
    }
}

/**
  * @brief  Configures the horizontal parameters of the waveform.
  * @param  pThis: Pointer to the WaveGen structure.
  * @param  channel: WaveGen channel (CHANNEL_1 or CHANNEL_2).
  * @param  frequency: Waveform frequency (in Hz).
  * @retval None
  */
void wavegen_config_horizontal(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t frequency)
{
    if (frequency == 0)
    {
        frequency = 1;
    }
    if (channel == WAVEGEN_CHANNEL_1)
    {
        pThis->htim1->Init.Prescaler = (170e6 / (frequency * pThis->len)) / 2 - 1;
        pThis->htim1->Init.Period = 2 - 1;
        HAL_TIM_Base_Init(pThis->htim1);
    }
    else if (channel == WAVEGEN_CHANNEL_2)
    {
        pThis->htim2->Init.Prescaler = (170e6 / (frequency * pThis->len)) / 2 - 1;
        pThis->htim2->Init.Period = 2 - 1;
        HAL_TIM_Base_Init(pThis->htim2);
    }
}

/**
  * @brief  Configures the vertical parameters of the waveform.
  * @param  pThis: Pointer to the WaveGen structure.
  * @param  channel: WaveGen channel (CHANNEL_1 or CHANNEL_2).
  * @param  type: Waveform type.
  * @param  offset: Vertical offset.
  * @param  scale: Vertical scale (amplitude).
  * @param  duty_cycle: Duty cycle (for PWM waveforms).
  * @retval None
  */
void wavegen_config_vertical(tWaveGen *pThis, enum eWaveGenChannel channel, enum eWaveGenType type, uint16_t offset, uint16_t scale, uint16_t duty_cycle)
{
    switch (type)
    {
    case WAVEGEN_TYPE_DC:
        wavegen_build_dc(pThis, channel, offset);
        break;
    case WAVEGEN_TYPE_SINE:
        wavegen_build_sine(pThis, channel, offset, scale);
        break;
    case WAVEGEN_TYPE_SQUARE:
        wavegen_build_square(pThis, channel, offset, scale);
        break;
    case WAVEGEN_TYPE_TRIANGLE:
        wavegen_build_triangle(pThis, channel, offset, scale);
        break;
    case WAVEGEN_TYPE_SAWTOOTH:
        wavegen_build_sawtooth(pThis, channel, offset, scale);
        break;
    case WAVEGEN_TYPE_PWM:
        wavegen_build_pwm(pThis, channel, offset, scale, duty_cycle);
        break;
    case WAVEGEN_TYPE_NOISE:
        wavegen_build_noise(pThis, channel, offset, scale);
        break;
    }
}

/**
  * @brief  Generates a DC waveform with the specified offset.
  * @param  pThis: Pointer to the WaveGen structure.
  * @param  channel: WaveGen channel (CHANNEL_1 or CHANNEL_2).
  * @param  offset: Vertical offset.
  * @retval None
  */
void wavegen_build_dc(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset)
{
    uint16_t i;
    uint16_t *buffer = (channel == WAVEGEN_CHANNEL_1) ? pThis->buffer1 : pThis->buffer2;
    for (i = 0; i < pThis->len; i++)
    {
        buffer[i] = offset;
    }
}

/**
  * @brief  Generates a sine waveform with the specified offset and scale.
  * @param  pThis: Pointer to the WaveGen structure.
  * @param  channel: WaveGen channel (CHANNEL_1 or CHANNEL_2).
  * @param  offset: Vertical offset.
  * @param  scale: Vertical scale (amplitude).
  * @retval None
  */
void wavegen_build_sine(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale)
{
    uint16_t i;
    uint16_t *buffer = (channel == WAVEGEN_CHANNEL_1) ? pThis->buffer1 : pThis->buffer2;
    for (i = 0; i < pThis->len; i++)
    {
        buffer[i] = sinf(2 * M_PI * i / pThis->len) * scale + offset;
    }
}

/**
  * @brief  Generates a square waveform with the specified offset and scale.
  * @param  pThis: Pointer to the WaveGen structure.
  * @param  channel: WaveGen channel (CHANNEL_1 or CHANNEL_2).
  * @param  offset: Vertical offset.
  * @param  scale: Vertical scale (amplitude).
  * @retval None
  */
void wavegen_build_square(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale)
{
    uint16_t i;
    uint16_t *buffer = (channel == WAVEGEN_CHANNEL_1) ? pThis->buffer1 : pThis->buffer2;
    for (i = 0; i < pThis->len; i++)
    {
        buffer[i] = (i < pThis->len / 2) ? offset - scale : offset + scale;
    }
}

/**
  * @brief  Generates a triangle waveform with the specified offset and scale.
  * @param  pThis: Pointer to the WaveGen structure.
  * @param  channel: WaveGen channel (CHANNEL_1 or CHANNEL_2).
  * @param  offset: Vertical offset.
  * @param  scale: Vertical scale (amplitude).
  * @retval None
  */
void wavegen_build_triangle(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale)
{
    uint16_t i;
    uint16_t *buffer = (channel == WAVEGEN_CHANNEL_1) ? pThis->buffer1 : pThis->buffer2;
    for (i = 0; i < pThis->len; i++)
    {
        if (i < pThis->len / 2)
        {
            // Generate the rising portion of the triangle waveform
            buffer[i] = offset - scale + (uint16_t)(((float)i / (pThis->len / 2)) * (2 * scale));
        }
        else
        {
            // Generate the falling portion of the triangle waveform
            buffer[i] = offset + scale - (uint16_t)(((float)(i - pThis->len / 2) / (pThis->len / 2)) * (2 * scale));
        }
    }
}

/**
  * @brief  Generates a sawtooth waveform with the specified offset and scale.
  * @param  pThis: Pointer to the WaveGen structure.
  * @param  channel: WaveGen channel (CHANNEL_1 or CHANNEL_2).
  * @param  offset: Vertical offset.
  * @param  scale: Vertical scale (amplitude).
  * @retval None
  */
void wavegen_build_sawtooth(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale)
{
    uint16_t i;
    uint16_t *buffer = (channel == WAVEGEN_CHANNEL_1) ? pThis->buffer1 : pThis->buffer2;
    for (i = 0; i < pThis->len; i++)
    {
        buffer[i] = offset - scale + i * 2 * scale / pThis->len;
    }
}

/**
  * @brief  Generates a PWM waveform with the specified offset, scale, and duty cycle.
  * @param  pThis: Pointer to the WaveGen structure.
  * @param  channel: WaveGen channel (CHANNEL_1 or CHANNEL_2).
  * @param  offset: Vertical offset.
  * @param  scale: Vertical scale (amplitude).
  * @param  duty_cycle: Duty cycle (0-100%).
  * @retval None
  */
void wavegen_build_pwm(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale, uint16_t duty_cycle)
{
    uint16_t i;
    uint16_t *buffer = (channel == WAVEGEN_CHANNEL_1) ? pThis->buffer1 : pThis->buffer2;
    for (i = 0; i < pThis->len; i++)
    {
        buffer[i] = (i < pThis->len * duty_cycle / 100) ? offset - scale : offset + scale;
    }
}

/**
  * @brief  Generates a noise waveform with the specified offset and scale.
  * @param  pThis: Pointer to the WaveGen structure.
  * @param  channel: WaveGen channel (CHANNEL_1 or CHANNEL_2).
  * @param  offset: Vertical offset.
  * @param  scale: Vertical scale (amplitude).
  * @retval None
  */
void wavegen_build_noise(tWaveGen *pThis, enum eWaveGenChannel channel, uint16_t offset, uint16_t scale)
{
    uint16_t i;
    uint16_t *buffer = (channel == WAVEGEN_CHANNEL_1) ? pThis->buffer1 : pThis->buffer2;
    for (i = 0; i < pThis->len; i++)
    {
        buffer[i] = rand() % scale + offset;
    }
}
