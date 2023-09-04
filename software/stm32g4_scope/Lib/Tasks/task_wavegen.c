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

#include "scope_tasks.h"

#define DAC_BUFFER_LEN 	(512)


uint16_t dac1_buffer[DAC_BUFFER_LEN];
uint16_t dac2_buffer[DAC_BUFFER_LEN];

extern tUi ui;
extern tLcd lcd;


void StartTaskWavegen(void *argument)
{
	static tWaveGen wavegen = {0};
	int is_visible = 0;
	int is_collapsed = 0;
	int slow_cnt = 0;

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

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1;
	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );

		if( osMessageQueueGet(queueUiWavegenHandle, &msgUiWavegen, NULL, 0U) == osOK )
		{
			if( is_visible )
			{
				if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
				{
					wavegen_erase( &wavegen, &lcd, is_collapsed );
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
				case QUEUE_UI_WAVEGEN_TYPE_CHANGE_VISIBILITY:
					is_visible = msgUiWavegen.data[0];
					break;
				case QUEUE_UI_WAVEGEN_TYPE_CHANGE_COLLAPSED:
					is_collapsed = msgUiWavegen.data[0];
					break;
				default:
					break;
			}
			if( is_visible )
			{
				if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
				{
					wavegen_draw( &wavegen, &lcd, is_collapsed );
					osSemaphoreRelease( semaphoreLcdHandle );
				}
			}
		}


        if( slow_cnt == 0 )
        {
        	if( is_visible )
        	{
				if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
				{
					wavegen_draw( &wavegen, &lcd, is_collapsed );
					osSemaphoreRelease( semaphoreLcdHandle );
				}
        	}
        }
        slow_cnt = (slow_cnt+1)%10;
	}
}

