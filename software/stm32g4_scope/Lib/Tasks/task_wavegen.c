/*
 * task_wavegen.c
 *
 *  Created on: Aug 31, 2023
 *      Author: jgpei
 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "tim.h"
#include "adc.h"
#include "dac.h"
#include "opamp.h"
#include "spi.h"

#include "lcd.h"
#include "wavegen.h"
#include "ui.h"

#define DAC_BUFFER_LEN 	(512)


uint16_t dac1_buffer[DAC_BUFFER_LEN];
uint16_t dac2_buffer[DAC_BUFFER_LEN];

struct sQueueUiWavegen {
    uint16_t type;
    uint16_t data[8];
};

extern osMessageQueueId_t queueUiWavegenHandle;
extern osSemaphoreId_t semaphoreLcdHandle;
extern tUi ui;
extern tLcd lcd;

enum eQueueUiWavegenType
{
	QUEUE_UI_WAVEGEN_TYPE_START,
	QUEUE_UI_WAVEGEN_TYPE_STOP,
	QUEUE_UI_WAVEGEN_TYPE_CONFIG_HORIZONTAL,
	QUEUE_UI_WAVEGEN_TYPE_CONFIG_VERTICAL,
};

void StartTaskWavegen(void *argument)
{
	static tWaveGen wavegen = {0};
	int is_visible = 0;
	int is_visible_bck = 0;
	int ch1_is_enabled = 0;
	int ch2_is_enabled = 0;
	int ch1_is_enabled_bck = 0;
	int ch2_is_enabled_bck = 0;
	struct sQueueUiWavegen msgUiWavegen = {0};

	wavegen_init_ll( &wavegen,
		&hdac1, // hdac
		&htim4, // htim1
		&htim6  // htim2
	);

	wavegen_init( &wavegen,
		dac1_buffer,
		dac2_buffer,
		DAC_BUFFER_LEN
	);

	wavegen_config_horizontal( &wavegen, WAVEGEN_CHANNEL_1, 1000 );
	wavegen_config_vertical( &wavegen, WAVEGEN_CHANNEL_1, WAVEGEN_TYPE_SINE, 2048, 2000, 0 );

	wavegen_config_horizontal( &wavegen, WAVEGEN_CHANNEL_2, 1000 );
	wavegen_config_vertical( &wavegen, WAVEGEN_CHANNEL_2, WAVEGEN_TYPE_SINE, 2048, 2000, 0 );

	wavegen_start( &wavegen, WAVEGEN_CHANNEL_1 );
	wavegen_start( &wavegen, WAVEGEN_CHANNEL_2 );

	//if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
	//{
	//	wavegen_draw( &wavegen, &lcd );
	//	osSemaphoreRelease( semaphoreLcdHandle );
	//}

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1;
	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		is_visible = ui.wavegen.is_visible;
		ch1_is_enabled = ui.wavegen.waveforms[0].enabled;
		ch2_is_enabled = ui.wavegen.waveforms[1].enabled;

		if( is_visible && !is_visible_bck )
		{
			if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
			{
				wavegen_draw( &wavegen, &lcd );
				osSemaphoreRelease( semaphoreLcdHandle );
			}
		}

		if( osMessageQueueGet(queueUiWavegenHandle, &msgUiWavegen, NULL, 0U) == osOK )
		{
			if( is_visible )
			{
				if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
				{
					wavegen_erase( &wavegen, &lcd );
					osSemaphoreRelease( semaphoreLcdHandle );
				}
			}

			switch( msgUiWavegen.type )
			{
				case QUEUE_UI_WAVEGEN_TYPE_START:
					wavegen_start( &wavegen,
						msgUiWavegen.data[0] // channel
					);
					break;
				case QUEUE_UI_WAVEGEN_TYPE_STOP:
					wavegen_stop( &wavegen,
						msgUiWavegen.data[0] // channel
					);
					break;
				case QUEUE_UI_WAVEGEN_TYPE_CONFIG_HORIZONTAL:
					wavegen_config_horizontal( &wavegen,
						msgUiWavegen.data[0], // channel
						msgUiWavegen.data[1]  // frequency
					);
					break;
				case QUEUE_UI_WAVEGEN_TYPE_CONFIG_VERTICAL:
					wavegen_config_vertical( &wavegen,
						msgUiWavegen.data[0], // channel
						msgUiWavegen.data[1], // type
						msgUiWavegen.data[2], // offset
						msgUiWavegen.data[3], // scale
						msgUiWavegen.data[4]  // duty_cycle
					);
					break;
			}
			if( is_visible )
			{
				if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
				{
					wavegen_draw( &wavegen, &lcd );
					osSemaphoreRelease( semaphoreLcdHandle );
				}
			}
		}

		if( !is_visible && is_visible_bck )
		{
			if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
			{
				wavegen_erase( &wavegen, &lcd );
				osSemaphoreRelease( semaphoreLcdHandle );
			}
		}

		is_visible_bck = is_visible;


	}
}

