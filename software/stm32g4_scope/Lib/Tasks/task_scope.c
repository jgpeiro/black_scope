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
};

extern osMessageQueueId_t queueUiScopeHandle;
extern osSemaphoreId_t semaphoreLcdHandle;
extern tUi ui;
extern tLcd lcd;

void _StartTaskScope(void *argument)
{
    static tScope scope = {0};
    pScope = &scope;

    struct sQueueUiScope msgScope = {0};
    //struct sQueueUiLcd msgLcd = {0};
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 1;
	const int wait_timeout_ms = 100;

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

	int is_collapsed = 0;//ui.is_collapsed;
	scope_config_horizontal( &scope, 0, 1000 );
	scope_config_vertical( &scope, 0, 0, 0, 0, 2048 );
	scope_config_trigger( &scope, 0, 0, 3096, 0 );
	if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
	{
		scope_draw( &scope, &lcd, is_collapsed );
		osSemaphoreRelease( semaphoreLcdHandle );
	}

    xLastWakeTime = xTaskGetTickCount();
    int running = 0;
    int single = 0;
    for(;;)
    {
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
        //is_collapsed = ui.is_collapsed;

        if( osMessageQueueGet(queueUiScopeHandle, &msgScope, NULL, 0U) == osOK )
        {
            switch( msgScope.type )
            {
				case QUEUE_UI_SCOPE_TYPE_START:
                    //_scope_start( &scope,
                    //	msgScope.data[0] // continuous
					//);
                    running = 1;
                    single = !msgScope.data[0];
                    break;
                case QUEUE_UI_SCOPE_TYPE_STOP:
                    //_scope_stop( &scope );
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
				scope_draw( &scope, &lcd, is_collapsed );
				osSemaphoreRelease( semaphoreLcdHandle );
			}
			if( single )
			{
				running = 0;
			}
        }
    }
}


#define SCOPE_COLOR_ACQUIRE     LCD_COLOR_RED
#define SCOPE_COLOR_HORIZONTAL  LCD_COLOR_GREEN
#define SCOPE_COLOR_VERTICAL    LCD_COLOR_BLUE
#define SCOPE_COLOR_TRIGGER     LCD_COLOR_YELLOW
#define SCOPE_COLOR_CH1         LCD_COLOR_RED
#define SCOPE_COLOR_CH2         LCD_COLOR_GREEN
#define SCOPE_COLOR_CH3         LCD_COLOR_BLUE
#define SCOPE_COLOR_CH4         LCD_COLOR_MAGENTA
#define SCOPE_COLOR_BACKGROUND  LCD_COLOR_BLACK

void scope_stroque2_acquire( tScope *pThis, tLcd *pLcd, uint16_t color, int is_collapsed )
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    x = pThis->state*10;
    y = 0;
    w = 10;
    h = 10;

    lcd_rect( pLcd, x, y, w, h, color );
}

void scope_draw2_acquire( tScope *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque2_acquire( pThis, pLcd, SCOPE_COLOR_ACQUIRE, is_collapsed );
}

void scope_erase2_acquire( tScope *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque2_acquire( pThis, pLcd, SCOPE_COLOR_BACKGROUND, is_collapsed );
}

void scope_stroque2_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, uint16_t color, int is_collapsed )
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    x = pThis->offset/4+120;
    y = 0;
    h = pLcd->height;

    lcd_vline( pLcd, x, y, h, color );
}

void scope_draw2_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque2_horizontal( pThis, pLcd, SCOPE_COLOR_HORIZONTAL, is_collapsed );
}

void scope_erase2_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque2_horizontal( pThis, pLcd, SCOPE_COLOR_BACKGROUND, is_collapsed );
}

void scope_stroque2_vertical( tScope_Vertical *pThis, tLcd *pLcd, uint16_t color,
		uint16_t color1, uint16_t color2, uint16_t color3, uint16_t color4,
		int is_collapsed )
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    x = 0;
    y = pThis->offset;
    w = pLcd->width/2;
    lcd_hline( pLcd, x, y, w, color );

    x = 0;
    y = pThis->gain1;
    w = pLcd->width/2;
    lcd_hline( pLcd, x, y, w, color1 );

    x = 0;
    y = pThis->gain1;
    w = pLcd->width/2;
    lcd_hline( pLcd, x, y, w, color2 );

    x = 0;
    y = pThis->gain1;
    w = pLcd->width/2;
    lcd_hline( pLcd, x, y, w, color3 );

    x = 0;
    y = pThis->gain1;
    w = pLcd->width/2;
    lcd_hline( pLcd, x, y, w, color4 );
}

void scope_draw2_vertical( tScope_Vertical *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque2_vertical( pThis, pLcd, SCOPE_COLOR_VERTICAL, SCOPE_COLOR_CH1, SCOPE_COLOR_CH2, SCOPE_COLOR_CH3, SCOPE_COLOR_CH4, is_collapsed );
}

void scope_erase2_vertical( tScope_Vertical *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque2_vertical( pThis, pLcd, SCOPE_COLOR_BACKGROUND, SCOPE_COLOR_BACKGROUND, SCOPE_COLOR_BACKGROUND, SCOPE_COLOR_BACKGROUND, SCOPE_COLOR_BACKGROUND, is_collapsed );
}

void scope_stroque2_trigger( tScope_Trigger *pThis, tLcd *pLcd, uint16_t color, int is_collapsed )
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    x = 0;
    y = pLcd->height-pThis->level*pLcd->height/4096.0f;;
    w = pLcd->width/2;
    lcd_hline( pLcd, x, y, w, color );
}
void scope_draw2_trigger( tScope_Trigger *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque2_trigger( pThis, pLcd, SCOPE_COLOR_TRIGGER, is_collapsed );
}

void scope_erase2_trigger( tScope_Trigger *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque2_trigger( pThis, pLcd, SCOPE_COLOR_BACKGROUND, is_collapsed );
}
void scope_draw2( tScope *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_draw2_acquire( pThis, pLcd, is_collapsed );
    scope_draw2_horizontal( &pThis->horizontal, pLcd, is_collapsed );
    scope_draw2_vertical( &pThis->vertical, pLcd, is_collapsed );
    scope_draw2_trigger( &pThis->trigger, pLcd, is_collapsed );
    //scope_draw2_signals( pThis, pLcd, is_collapsed );
}

void scope_erase2( tScope *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_erase2_acquire( pThis, pLcd, is_collapsed );
    scope_erase2_horizontal( &pThis->horizontal, pLcd, is_collapsed );
    scope_erase2_vertical( &pThis->vertical, pLcd, is_collapsed );
    scope_erase2_trigger( &pThis->trigger, pLcd, is_collapsed );
    //scope_erase2_signals( pThis, pLcd, is_collapsed );
}

void StartTaskScope(void *argument)
{
    static tScope scope = {0};
    pScope = &scope;

    struct sQueueUiScope msgScope = {0};
    //struct sQueueUiLcd msgLcd = {0};
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 1;
	const int wait_timeout_ms = 50;

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

	int is_collapsed = 0;
	scope_config_horizontal( &scope, 0, 1000 );
	scope_config_vertical( &scope, 0, 0, 0, 0, 2048 );
	scope_config_trigger( &scope, 0, 0, 3096, 0 );
	if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
	{
		scope_draw( &scope, &lcd, is_collapsed );
		osSemaphoreRelease( semaphoreLcdHandle );
	}

    xLastWakeTime = xTaskGetTickCount();
    int running = 0;
    int single = 0;
    int horizontal_is_visible = 0;
    int vertical_is_visible = 0;
    int trigger_is_visible = 0;
    //int horizontal_is_visible_bck = 0;
    //int vertical_is_visible_bck = 0;
    //int trigger_is_visible_bck = 0;
    for(;;)
    {
    	vTaskDelayUntil( &xLastWakeTime, xFrequency );
    	//horizontal_is_visible = ui.horizontal.is_visible;
		//vertical_is_visible = ui.vertical.is_visible;
		//trigger_is_visible = ui.trigger.is_visible;

		/*if( horizontal_is_visible && !horizontal_is_visible_bck ||
			vertical_is_visible && !vertical_is_visible_bck ||
			trigger_is_visible && !trigger_is_visible_bck )
		{
			if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
			{
				//scope_draw2( &scope, &lcd, is_collapsed );
				scope_draw2_acquire( &scope, &lcd, is_collapsed );
				if( horizontal_is_visible )
					scope_draw2_horizontal( &scope.horizontal, &lcd, is_collapsed );
				if( vertical_is_visible )
					scope_draw2_vertical( &scope.vertical, &lcd, is_collapsed );
				if( trigger_is_visible )
					scope_draw2_trigger( &scope.trigger, &lcd, is_collapsed );
				osSemaphoreRelease( semaphoreLcdHandle );
			}
		}*/

		if( osMessageQueueGet(queueUiScopeHandle, &msgScope, NULL, 0U) == osOK )
		{
			if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
			{
				scope_erase2_acquire( &scope, &lcd, is_collapsed );
				if( horizontal_is_visible )
					scope_erase2_horizontal( &scope.horizontal, &lcd, is_collapsed );
				if( vertical_is_visible )
					scope_erase2_vertical( &scope.vertical, &lcd, is_collapsed );
				if( trigger_is_visible )
					scope_erase2_trigger( &scope.trigger, &lcd, is_collapsed );

				osSemaphoreRelease( semaphoreLcdHandle );
			}

			switch( msgScope.type )
			{
				case QUEUE_UI_SCOPE_TYPE_START:
					//_scope_start( &scope,
					//	msgScope.data[0] // continuous
					//);
					running = 1;
					single = !msgScope.data[0];
					break;
				case QUEUE_UI_SCOPE_TYPE_STOP:
					//_scope_stop( &scope );
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
					/*if( msgScope.data[0] == 1 )
					{
						if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
						{
							if( 1 || msgScope.data[1] == 0 )
								scope_draw2_acquire( &scope, &lcd, is_collapsed );
							if( msgScope.data[1] == 1 )
								scope_draw2_horizontal( &scope.horizontal, &lcd, is_collapsed );
							if( msgScope.data[1] == 2 )
								scope_draw2_vertical( &scope.vertical, &lcd, is_collapsed );
							if( msgScope.data[1] == 3 )
								scope_draw2_trigger( &scope.trigger, &lcd, is_collapsed );
							osSemaphoreRelease( semaphoreLcdHandle );
						}
					}
					else
					{
						if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
						{
							if( 1 || msgScope.data[1] == 0 )
								scope_erase2_acquire( &scope, &lcd, is_collapsed );
							if( msgScope.data[1] == 1 )
								scope_erase2_horizontal( &scope.horizontal, &lcd, is_collapsed );
							if( msgScope.data[1] == 2 )
								scope_erase2_vertical( &scope.vertical, &lcd, is_collapsed );
							if( msgScope.data[1] == 3 )
								scope_erase2_trigger( &scope.trigger, &lcd, is_collapsed );
							osSemaphoreRelease( semaphoreLcdHandle );
						}
					}*/
					if( msgScope.data[1] == 0 )
						;
					if( msgScope.data[1] == 1 )
						horizontal_is_visible = msgScope.data[0];
					if( msgScope.data[1] == 2 )
						vertical_is_visible = msgScope.data[0];
					if( msgScope.data[1] == 3 )
						trigger_is_visible = msgScope.data[0];
					break;
				//case QUEUE_UI_SCOPE_TYPE_CHANGE_COLLAPSED:
				//	is_collapsed = msgScope.data[0];
				//	break;
				default:
					break;
			}

			if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
			{
				//scope_draw2( &scope, &lcd, is_collapsed );
				scope_draw2_acquire( &scope, &lcd, is_collapsed );
				if( horizontal_is_visible )
					scope_draw2_horizontal( &scope.horizontal, &lcd, is_collapsed );
				if( vertical_is_visible )
					scope_draw2_vertical( &scope.vertical, &lcd, is_collapsed );
				if( trigger_is_visible )
					scope_draw2_trigger( &scope.trigger, &lcd, is_collapsed );
				osSemaphoreRelease( semaphoreLcdHandle );
			}
		}

		/*if( !horizontal_is_visible && horizontal_is_visible_bck ||
			!vertical_is_visible && vertical_is_visible_bck ||
			!trigger_is_visible && trigger_is_visible_bck )
		{
			if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
			{
				//scope_erase2( &scope, &lcd, is_collapsed );
				scope_erase2_acquire( &scope, &lcd, is_collapsed );
				if( !horizontal_is_visible )
					scope_erase2_horizontal( &scope.horizontal, &lcd, is_collapsed );
				if( !vertical_is_visible )
					scope_erase2_vertical( &scope.vertical, &lcd, is_collapsed );
				if( !trigger_is_visible )
					scope_erase2_trigger( &scope.trigger, &lcd, is_collapsed );
				osSemaphoreRelease( semaphoreLcdHandle );
			}
		}*/


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

            if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
			{
            	scope_draw_grid( &scope, &lcd, is_collapsed );
            	osSemaphoreRelease( semaphoreLcdHandle );
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
			if( single )
			{
				running = 0;
			}
        }

    	//horizontal_is_visible_bck = horizontal_is_visible;
    	//vertical_is_visible_bck = vertical_is_visible;
    	//trigger_is_visible_bck = trigger_is_visible;
    }
}

