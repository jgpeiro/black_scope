/*
 * task_scope.c
 *
 *  Created on: Oct 28, 2023
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


#define ADC_BUFFER_LEN	(512)

struct sQueueUiScope
{
    uint16_t type; /**< Type of the message. */
    uint16_t data[3]; /**< Message data. */
    // sizeof(tQueueUiScope) == 8
};
typedef struct sQueueUiScope tQueueUiScope;

void _StartTaskScope(void *argument)
{
	tScope scope = {0};

	int i;
	static float buf_sine[ADC_BUFFER_LEN] = {0};

	static uint16_t buf0[ADC_BUFFER_LEN] = {0};
	static uint16_t buf1[ADC_BUFFER_LEN] = {0};
	static uint16_t buf2[ADC_BUFFER_LEN] = {0};
	static uint16_t buf3[ADC_BUFFER_LEN] = {0};

	static uint16_t buf4[ADC_BUFFER_LEN] = {0};
	static uint16_t buf5[ADC_BUFFER_LEN] = {0};
	static uint16_t buf6[ADC_BUFFER_LEN] = {0};
	static uint16_t buf7[ADC_BUFFER_LEN] = {0};

	int gain = 0;
	int step = 0;
	volatile int speed = 100;
	int y0, y1, h;
	int h0, h1, h2, h3;
	int h4, h5, h6, h7;

	//for( i = 0 ; i < 480 ; i++ )
	//{
	//	buf_sine[i] = sinf( 4*(2*M_PIf*i)/480 );
	//}

#define DAC_BUFFER_LEN (512)

// DAC buffer for channel 1 and channel 2
static uint16_t dac1_buffer[DAC_BUFFER_LEN];
static uint16_t dac2_buffer[DAC_BUFFER_LEN];
/*
	tWaveGen wavegen = {0};
    // Initialize wavegen with DAC and timer handles
    wavegen_init_ll(&wavegen, &hdac1, &htim4, &htim6);

    // Initialize wavegen settings for both channels
    wavegen_init(&wavegen, dac1_buffer, dac2_buffer, DAC_BUFFER_LEN);
    wavegen_config_horizontal(&wavegen, WAVEGEN_CHANNEL_1, 2000);
    wavegen_config_vertical(&wavegen, WAVEGEN_CHANNEL_1, WAVEGEN_TYPE_SINE, 2048, 1000, 0);
    wavegen_config_horizontal(&wavegen, WAVEGEN_CHANNEL_2, 1000);
    wavegen_config_vertical(&wavegen, WAVEGEN_CHANNEL_2, WAVEGEN_TYPE_SINE, 2048, 2000, 0);
    wavegen_start(&wavegen, WAVEGEN_CHANNEL_1);
    wavegen_start(&wavegen, WAVEGEN_CHANNEL_2);
*/


    // Initialize hardware components and buffers
    scope_init_ll(&scope,
        &htim2,  // horizontal.htim_clock
        &htim3,  // horizontal.htim_stop
        &hdac2,  // vertical.hdac
        &hopamp1, // vertical.hopamp1
        &hopamp3, // vertical.hopamp2
        &hopamp5, // vertical.hopamp3
        &hopamp6, // vertical.hopamp4
        &hadc1,  // trigger.hadc1
        &hadc3,  // trigger.hadc2
        &hadc5,  // trigger.hadc3
        &hadc4   // trigger.hadc4
    );

    scope_init(&scope,
		buf0,
		buf1,
		buf2,
		buf3,
		buf4,
		buf5,
		buf6,
		buf7,
        ADC_BUFFER_LEN
    );

    scope_config_horizontal(&scope,
    	0,		// offset
		100 	// scale [KHz]
	);
    scope_config_vertical(&scope,
		2048+60,	// offset
    	0, 		// scale1
		0, 		// scale2
		0, 		// scale3
		0 		// scale4
	);

    scope.vertical.offset1 = 192;
	scope.vertical.offset2 = 192;
	scope.vertical.offset3 = 192;
	scope.vertical.offset4 = 192;

	scope.vertical.enable1 = 1;
	scope.vertical.enable2 = 1;
	scope.vertical.enable3 = 1;
	scope.vertical.enable4 = 1;

	scope.vertical.scale1 = 80;
	scope.vertical.scale2 = 80;
	scope.vertical.scale3 = 80;
	scope.vertical.scale4 = 80;

    scope_config_trigger(&scope,
		0, 							// channel
		1, //UI_TRIGGER_MODE_NORMAL, 	// mode
		3092, 						// level
		SCOPE_TRIGGER_SLOPE_RISING	// slope
	);
    volatile int ddd = 5;
	while(1)
	{
		int msg_get = 0;
		//if( run )
		{
			tQueueUiScope msg = {0};
			if( osMessageQueueGet(queueUiScopeHandle, &msg, NULL, 0U) == osOK)
			{
				msg_get = 1;
				if( osSemaphoreAcquire( semLcdHandle, portMAX_DELAY ) == osOK )
				{
					if( msg_get )
					{
						scope_erase(&scope, &lcd,
										1,
										1,
										1,
										1,
										(menu==0)?0:1);
					}
					osSemaphoreRelease( semLcdHandle );
				}
				if( msg.type == 1)
				{
					scope_config_horizontal( &scope, msg.data[0], scope.horizontal.scale );
				}
				else if( msg.type == 2 )
				{
					scope_config_horizontal( &scope, scope.horizontal.offset, msg.data[0] );
				}

				if( osSemaphoreAcquire( semLcdHandle, portMAX_DELAY ) == osOK )
				{
					if( msg_get )
					{
						scope_draw(&scope, &lcd,
										1,
										1,
										1,
										1,
										(menu==0)?0:1);
					}
					osSemaphoreRelease( semLcdHandle );
				}
			}
			if( run )
			{
				scope_start( &scope, 0 );
				int timeout_ms = (1000*480*5)/(scope.horizontal.scale*1000);
				scope_wait( &scope, timeout_ms );
				scope_stop( &scope );
			}
		}
		if( osSemaphoreAcquire( semLcdHandle, portMAX_DELAY ) == osOK )
		{
			scope_draw_signals(&scope, &lcd, (menu==0)?0:1);
			scope_draw(&scope, &lcd,
				1,
				1,
				1,
				1,
				(menu==0)?0:1);
			osSemaphoreRelease( semLcdHandle );
		}

		if( run )
		{
	        osDelay(ddd);
		}
		else
		{
			osDelay(50);
		}
	}
}

