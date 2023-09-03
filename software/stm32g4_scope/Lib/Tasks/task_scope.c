/*
 * task_scope.c
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
#include "scope.h"
#include "ui.h"

#define ADC_BUFFER_LEN 	(512)

uint16_t buffer1[ADC_BUFFER_LEN];
uint16_t buffer2[ADC_BUFFER_LEN];
uint16_t buffer3[ADC_BUFFER_LEN];
uint16_t buffer4[ADC_BUFFER_LEN];
uint16_t buffer5[ADC_BUFFER_LEN];
uint16_t buffer6[ADC_BUFFER_LEN];
uint16_t buffer7[ADC_BUFFER_LEN];
uint16_t buffer8[ADC_BUFFER_LEN];
/*
enum eQueueUiScopeType
{
	QUEUE_UI_SCOPE_TYPE_START,
	QUEUE_UI_SCOPE_TYPE_STOP,
	QUEUE_UI_SCOPE_TYPE_HORIZONTAL,
	QUEUE_UI_SCOPE_TYPE_VERTICAL,
	QUEUE_UI_SCOPE_TYPE_TRIGGER,
	QUEUE_UI_SCOPE_TYPE_CHANGE_VISIBILITY,
	QUEUE_UI_SCOPE_TYPE_CHANGE_COLLAPSED
};

struct sQueueUiScope {
    uint16_t type;
    uint16_t data[8];
};*/

extern osMessageQueueId_t queueUiScopeHandle;
extern osSemaphoreId_t semaphoreLcdHandle;
extern tUi ui;
extern tLcd lcd;


void StartTaskScope(void *argument)
{
    static tScope scope = {0};
    pScope = &scope;

    struct sQueueUiScope msgScope = {0};

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 1;
	const int wait_timeout_ms = 50;

    int running = 0;
    int single = 0;

    int is_collapsed = 0;
    int acquire_is_visible = 0;
    int horizontal_is_visible = 0;
    int vertical_is_visible = 0;
    int trigger_is_visible = 0;

    int slow_cnt = 0;

    scope_init_ll( &scope,
        &htim2, // horizontal.htim_clock
        &htim3, // horizontal.htim_stop
        &hdac2, // vertical.hdac
        &hopamp1, // vertical.hopamp1
        &hopamp3, // vertical.hopamp2
        &hopamp5, // vertical.hopamp3
        &hopamp6, // vertical.hopamp4
        &hadc1, // trigger.hadc1
        &hadc3, // trigger.hadc2
        &hadc5, // trigger.hadc3
        &hadc4 // trigger.hadc4
    );

	scope_init( &scope,
		buffer1,
		buffer2,
		buffer3,
		buffer4,

		buffer5,
		buffer6,
		buffer7,
		buffer8,

		ADC_BUFFER_LEN
	);

	scope_config_horizontal( &scope, 0, 1000 );
	scope_config_vertical( &scope, 0, 0, 0, 0, 2048 );
	scope_config_trigger( &scope, 0, 0, 3092, 0 );
	if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
	{
		scope_draw( &scope, &lcd,
				acquire_is_visible,
				horizontal_is_visible,
				vertical_is_visible,
				trigger_is_visible,
				is_collapsed );
		osSemaphoreRelease( semaphoreLcdHandle );
	}

    xLastWakeTime = xTaskGetTickCount();
    for(;;)
    {
    	vTaskDelayUntil( &xLastWakeTime, xFrequency );

		if( osMessageQueueGet(queueUiScopeHandle, &msgScope, NULL, 0U) == osOK )
		{
			if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
			{
				scope_erase( &scope, &lcd,
					acquire_is_visible,
					horizontal_is_visible,
					vertical_is_visible,
					trigger_is_visible,
					is_collapsed );

				osSemaphoreRelease( semaphoreLcdHandle );
			}

			switch( msgScope.type )
			{
				case QUEUE_UI_SCOPE_TYPE_START:
					running = 1;
					single = !msgScope.data[0];
					break;
				case QUEUE_UI_SCOPE_TYPE_STOP:
					running = 0;
					single = 0;
					break;
				case QUEUE_UI_SCOPE_TYPE_HORIZONTAL:
					scope_config_horizontal( &scope,
						msgScope.data[0], // offset
						msgScope.data[1]  // scale
					);
					break;
				case QUEUE_UI_SCOPE_TYPE_VERTICAL:
					scope_config_vertical( &scope,
						msgScope.data[0], // offset
						msgScope.data[1], // scale1
						msgScope.data[2], // scale2
						msgScope.data[3], // scale3
						msgScope.data[4]  // scale4
					);
					break;
				case QUEUE_UI_SCOPE_TYPE_TRIGGER:
					scope_config_trigger( &scope,
						msgScope.data[0], // source
						msgScope.data[1], // mode
						msgScope.data[2], // level
						msgScope.data[3]  // slope
					);
					break;
				case QUEUE_UI_SCOPE_TYPE_CHANGE_VISIBILITY:
					if( msgScope.data[1] == 0 )
						acquire_is_visible = msgScope.data[0];
					if( msgScope.data[1] == 1 )
						horizontal_is_visible = msgScope.data[0];
					if( msgScope.data[1] == 2 )
						vertical_is_visible = msgScope.data[0];
					if( msgScope.data[1] == 3 )
						trigger_is_visible = msgScope.data[0];
					break;
				case QUEUE_UI_SCOPE_TYPE_CHANGE_COLLAPSED:
					if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
					{
					//	lcd_clear( &lcd, LCD_COLOR_BLACK );
						osSemaphoreRelease( semaphoreLcdHandle );
					}
					is_collapsed = msgScope.data[0];
					break;
				default:
					break;
			}

			if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
			{
				scope_draw( &scope, &lcd,
					acquire_is_visible,
					horizontal_is_visible,
					vertical_is_visible,
					trigger_is_visible,
					is_collapsed );
				osSemaphoreRelease( semaphoreLcdHandle );
			}
		}

        if( running )
        {
        	int a;
        	static int b = 0;
        	a = scope.dma_cndtr;
        	scope_start( &scope, 0 );
            int result = scope_wait( &scope, single?portMAX_DELAY:wait_timeout_ms );
        	if( a != b )
        	{
        		result = 1;
        	}
        	else
        	{
        		result = 0;
        	}
        	b = a;
            scope_stop( &scope );


            if( slow_cnt == 0 )
            {
				if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
				{
					scope_draw_grid( &scope, &lcd, is_collapsed );
					osSemaphoreRelease( semaphoreLcdHandle );
				}
            }

			if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
			{
				if( result )
				{
					lcd_rect( &lcd, 10, 10, 4, 4, 0xFFFF );
				}
				else
				{
					lcd_rect( &lcd, 10, 10, 4, 4, 0x0000 );
				}
				scope_draw_signals( &scope, &lcd, is_collapsed );
				osSemaphoreRelease( semaphoreLcdHandle );
			}

            if( slow_cnt == 0 )
            {
				if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
				{
					scope_draw( &scope, &lcd,
						acquire_is_visible,
						horizontal_is_visible,
						vertical_is_visible,
						trigger_is_visible,
						is_collapsed );
					osSemaphoreRelease( semaphoreLcdHandle );
				}
            }
            slow_cnt = (slow_cnt+1)%10;


			if( single )
			{
				running = 0;
			}
        }

    }
}

