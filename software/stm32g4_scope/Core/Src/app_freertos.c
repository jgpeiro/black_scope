/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 2048 * 4
};
/* Definitions for taskTsc */
osThreadId_t taskTscHandle;
const osThreadAttr_t taskTsc_attributes = {
  .name = "taskTsc",
  .priority = (osPriority_t) osPriorityNormal1,
  .stack_size = 256 * 4
};

/* Definitions for taskUI */
osThreadId_t taskUiHandle;
const osThreadAttr_t taskUi_attributes = {
  .name = "taskUI",
  .priority = (osPriority_t) osPriorityNormal1,
  .stack_size = 2048 * 4
};

osThreadId_t taskScopeHandle;
const osThreadAttr_t taskScope_attributes = {
  .name = "taskScope",
  .priority = (osPriority_t) osPriorityNormal4,
  .stack_size = 256 * 4
};

osThreadId_t taskWavegenHandle;
const osThreadAttr_t taskWavegen_attributes = {
  .name = "taskWavegen",
  .priority = (osPriority_t) osPriorityNormal3,
  .stack_size = 256 * 4
};
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
struct sQueueTscUi {
    uint16_t p;
    uint16_t x;
    uint16_t y;
};
osMessageQueueId_t queueTscUiHandle;
const osMessageQueueAttr_t queueTscUi_attributes = {
  .name = "queueTscUi"
};

struct sQueueUiLcd {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    uint16_t *buf;
};

osMessageQueueId_t queueUiLcdHandle;
const osMessageQueueAttr_t queueUiLcd_attributes = {
  .name = "queueUiLcd"
};

// declare and initialize the semaphoreLcdHandle
osSemaphoreId_t semaphoreLcdHandle;
const osSemaphoreAttr_t semaphoreLcd_attributes = {
  .name = "semaphoreLcd"
};

struct sQueueUiScope {
    uint16_t type;
    uint16_t data[8];
};

osMessageQueueId_t queueUiScopeHandle;
const osMessageQueueAttr_t queueUiScope_attributes = {
  .name = "queueUiScope"
};


struct sQueueUiWavegen {
    uint16_t type;
    uint16_t data[8];
};

osMessageQueueId_t queueUiWavegenHandle;
const osMessageQueueAttr_t queueUiWavegen_attributes = {
  .name = "queueUiWavegen"
};

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTaskTsc(void *argument);
void StartTaskUi(void *argument);
void StartTaskScope(void *argument);
void StartTaskWavegen(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	semaphoreLcdHandle = osSemaphoreNew(1, 0, &semaphoreLcd_attributes);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	queueTscUiHandle = osMessageQueueNew(1, sizeof(struct sQueueTscUi), &queueTscUi_attributes );
	//queueUiLcdHandle = osMessageQueueNew(1, sizeof(struct sQueueUiLcd), &queueUiLcd_attributes );
	queueUiScopeHandle = osMessageQueueNew(1, sizeof(struct sQueueUiScope), &queueUiScope_attributes );
	queueUiWavegenHandle = osMessageQueueNew(1, sizeof(struct sQueueUiWavegen), &queueUiWavegen_attributes );
 /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  //defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of taskTsc */
  taskTscHandle = osThreadNew(StartTaskTsc, NULL, &taskTsc_attributes);
  taskUiHandle = osThreadNew(StartTaskUi, NULL, &taskUi_attributes);
  taskScopeHandle = osThreadNew(StartTaskScope, NULL, &taskScope_attributes);
  taskWavegenHandle = osThreadNew(StartTaskWavegen, NULL, &taskWavegen_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
#include <stdio.h>
#include <math.h>

#include "tim.h"
#include "adc.h"
#include "dac.h"
#include "opamp.h"
#include "spi.h"

#include "Lcd.h"
#include "Tsc.h"
#include "Scope.h"
#include "tools.h"
#include "framebuf.h"
#include "FontUbuntuBookRNormal16.h"
#include "psram.h"
#include "wavegen.h"
#include "ui.h"


#include "nuklear.h"

#define ADC_BUFFER_LEN 	(512)
#define DAC_BUFFER_LEN 	(512)
#define FB_WIDTH 	(240)
#define FB_HEIGHT	(80)
#define FB2_WIDTH 	(160-1)
#define FB2_HEIGHT	(16)

uint16_t dac1_buffer[DAC_BUFFER_LEN];
uint16_t dac2_buffer[DAC_BUFFER_LEN];

uint16_t buffer1[ADC_BUFFER_LEN];
uint16_t buffer2[ADC_BUFFER_LEN];
uint16_t buffer3[ADC_BUFFER_LEN];
uint16_t buffer4[ADC_BUFFER_LEN];
uint16_t buffer5[ADC_BUFFER_LEN];
uint16_t buffer6[ADC_BUFFER_LEN];
uint16_t buffer7[ADC_BUFFER_LEN];
uint16_t buffer8[ADC_BUFFER_LEN];

uint16_t buffer_tmp[ADC_BUFFER_LEN];

uint16_t fb_buf[FB_WIDTH*FB_HEIGHT];
uint16_t fb2_buf[FB2_WIDTH*FB2_HEIGHT];

#define NK_BUFFER_CMDS_LEN 	(1024*6)
#define NK_BUFFER_POOL_LEN 	(1024*6)

uint8_t nk_buf_cmds[NK_BUFFER_CMDS_LEN] = {0};
uint8_t nk_buf_pool[NK_BUFFER_POOL_LEN] = {0};

int _write(int file, char *ptr, int len)
{
	int DataIdx;
	for(DataIdx = 0; DataIdx < len; DataIdx++)
	{
		ITM_SendChar(*ptr++);
	}
    return len;
}

float text_width_f( nk_handle handle, float h, const char* text, int len )
{
	return font_text_width( &fontUbuntuBookRNormal16, text );
}



/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void StartTaskTsc(void *argument)
{
	const TickType_t xFrequency = 1;
	TickType_t xLastWakeTime;

	struct sQueueTscUi msgTscUi = {0};

    float AX = 38.0/151.0;
	float BX = -1950.0/151.0;
	float AY = 11.0/62.0;
	float BY = -1157.0/62.0;

	tTsc tsc = {0};

	tsc_init( &tsc,
		&hspi3,
		TSC_nSS_GPIO_Port, TSC_nSS_Pin,
		AX, BX, AY, BY,
		32
	);

	xLastWakeTime = xTaskGetTickCount();
	uint16_t x;
	uint16_t y;
	uint16_t x_low = 0;
	uint16_t y_low = 0;
	int16_t cnt = 0;
	int released_sent = 0;
	int cnt2 = 0;
	for(;;)
  	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		//tsc_read( &tsc, &msgTscUi.p, &msgTscUi.x, &msgTscUi.y );
		tsc_read_ll( &tsc, &x, &y );
		if( x )
		{
			if( cnt < 50 )
			{
				cnt += 1;
			}
		}
		else
		{
			cnt = 0;
		}
		x_low = x_low*0.9 + x*0.1;
		y_low = y_low*0.9 + y*0.1;

		if( x )
		{

		}
		else
		{
			x_low = 0;
			y_low = 0;
		}

		msgTscUi.x = tsc.ax * x_low + tsc.bx;
		msgTscUi.y = tsc.ay * y_low + tsc.by;
		msgTscUi.p = x && (cnt>40);
		if( x )
		{
			if( cnt2 < 10 )
			{
				cnt2 += 1;
			}
			else
			{
				cnt2 = 0;
				osMessageQueuePut( queueTscUiHandle, &msgTscUi, 0U, 0U );
			}
			released_sent = 0;
		}
		else
		{
			if( released_sent == 0 )
			{
				osMessageQueuePut( queueTscUiHandle, &msgTscUi, 0U, portMAX_DELAY );
				released_sent = 1;
			}
		}
  	}
}

#define LCD_COLOR_WHITE		(0xFFFF)
#define COLOR_UI_CROSS		(LCD_COLOR_WHITE)

void ui_init( tUi *pThis )
{
	pThis->acquire.run = 0;
	pThis->acquire.single = 0;

	pThis->horizontal.offset = 0;
	pThis->horizontal.scale = 1000;

	pThis->vertical.offset = 2048;
	pThis->vertical.channels[0].enabled = 1;
	pThis->vertical.channels[0].coupling = 0;
	pThis->vertical.channels[0].scale = 0;
	pThis->vertical.channels[1].enabled = 1;
	pThis->vertical.channels[1].coupling = 0;
	pThis->vertical.channels[1].scale = 0;
	pThis->vertical.channels[2].enabled = 1;
	pThis->vertical.channels[2].coupling = 0;
	pThis->vertical.channels[2].scale = 0;
	pThis->vertical.channels[3].enabled = 1;
	pThis->vertical.channels[3].coupling = 0;
	pThis->vertical.channels[3].scale = 0;

	pThis->trigger.level = 2048;
	pThis->trigger.mode = UI_TRIGGER_MODE_NORMAL;
	pThis->trigger.slope = UI_TRIGGER_SLOPE_RISING;
	pThis->trigger.source = 0;

	pThis->wavegen.waveforms[0].enabled = 1;
	pThis->wavegen.waveforms[0].type = 1;
	pThis->wavegen.waveforms[0].offset = 2048;
	pThis->wavegen.waveforms[0].scale = 2000;
	pThis->wavegen.waveforms[0].frequency = 1000;
	pThis->wavegen.waveforms[0].duty_cycle = 0;

	pThis->wavegen.waveforms[1].enabled = 1;
	pThis->wavegen.waveforms[1].type = 1;
	pThis->wavegen.waveforms[1].offset = 2048;
	pThis->wavegen.waveforms[1].scale = 2000;
	pThis->wavegen.waveforms[1].frequency = 1000;
	pThis->wavegen.waveforms[1].duty_cycle = 0;
}

void lcd_draw_cross( tLcd *pLcd, uint16_t x, uint16_t y, uint16_t color )
{
	lcd_rect( pLcd, x-4, y, 4, 2, color );
	lcd_rect( pLcd, x+2, y, 4, 2, color );
	lcd_rect( pLcd, x, y-4, 2, 4, color );
	lcd_rect( pLcd, x, y+2, 2, 4, color );
}

struct nk_context ctx = {0};
tLcd lcd = {0};
void StartTaskUi(void *argument)
{
    tFramebuf fb = {0};
    static tUi ui = {0};

    struct sQueueTscUi msgTscUi = {0};
    //struct sQueueUiLcd msgUiLcd = {0};

	struct nk_buffer cmds = {0};
	struct nk_buffer pool = {0};
	struct nk_user_font font = {0};

	uint16_t x0 = 0, y0 = 0;

	//if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
	{
		lcd_init( &lcd,
			LCD_nRST_GPIO_Port, LCD_nRST_Pin,
			LCD_BL_GPIO_Port, LCD_BL_Pin,
			480, 320
		);
		osSemaphoreRelease( semaphoreLcdHandle );
	}

    framebuf_init( &fb, FB_WIDTH, FB_HEIGHT, fb_buf );

	nk_buffer_init_fixed( &cmds, nk_buf_cmds, NK_BUFFER_CMDS_LEN );
	nk_buffer_init_fixed( &pool, nk_buf_pool, NK_BUFFER_POOL_LEN);

	font.height = fontUbuntuBookRNormal16.bbxh;
	font.width = text_width_f;
	nk_init_custom( &ctx, &cmds, &pool, &font );
	
	ui_init( &ui );

    TickType_t xLastWakeTime;
	const TickType_t xFrequency = 10;
	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );

        if( osMessageQueueGet(queueTscUiHandle, &msgTscUi, NULL, portMAX_DELAY) == osOK )
		//if( osMessageQueueGet(queueTscUiHandle, &msgTscUi, NULL, xFrequency*10) == osOK )
		{
        	msgTscUi.x -= lcd.width/2;
            nk_input_begin( &ctx );
            nk_input_motion( &ctx, msgTscUi.x, msgTscUi.y );
            nk_input_button( &ctx, NK_BUTTON_LEFT, msgTscUi.x, msgTscUi.y, msgTscUi.p );
            nk_input_end( &ctx );
        }

        ui_build( &ui, &ctx );

        x0 = 0;
        for( y0 = 0; y0 < lcd.height; y0 += fb.height)
        {
	    	framebuf_fill( &fb, 0x0000 );
            nk_draw_fb2( &ctx, &fb, x0, y0 );
        	if( osSemaphoreAcquire( semaphoreLcdHandle, 0 ) == osOK )
			{
				lcd_bmp( &lcd, lcd.width/2, y0, fb.width, fb.height, fb.buf);
				osSemaphoreRelease( semaphoreLcdHandle );
			}
        }
                nk_clear(&ctx);

		if( msgTscUi.p && (0 <= msgTscUi.x) && (msgTscUi.x < lcd.width/2-5) )
		{
			lcd_draw_cross( &lcd, msgTscUi.x+lcd.width/2, msgTscUi.y, COLOR_UI_CROSS );
        }
    }
}

enum eQueueUiScopeType
{
	QUEUE_UI_SCOPE_TYPE_START,
	QUEUE_UI_SCOPE_TYPE_STOP,
	QUEUE_UI_SCOPE_TYPE_HORIZONTAL,
	QUEUE_UI_SCOPE_TYPE_VERTICAL,
	QUEUE_UI_SCOPE_TYPE_TRIGGER
};




void StartTaskScope(void *argument)
{
    static tScope scope = {0};
    pScope = &scope;

    struct sQueueUiScope msgScope = {0};
    //struct sQueueUiLcd msgLcd = {0};
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 1;
	const int wait_timeout_ms = 10;
	
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
	scope_config_trigger( &scope, 0, 0, 3096, 0 );
	if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
	{
		scope_draw( &scope, &lcd );
		osSemaphoreRelease( semaphoreLcdHandle );
	}

    xLastWakeTime = xTaskGetTickCount();
    int running = 0;
    int single = 0;
    for(;;)
    {
        vTaskDelayUntil( &xLastWakeTime, xFrequency );

        /*if( _scope_is_running( &scope ) )
		{
			if( osSemaphoreAcquire( semaphoreLcdHandle, 0 ) == osOK )
			{
				_scope_clear( &scope, &lcd );
				osSemaphoreRelease( semaphoreLcdHandle );
			}
		}*/

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
				scope_draw( &scope, &lcd );
				osSemaphoreRelease( semaphoreLcdHandle );
			}
			if( single )
			{
				running = 0;
			}
        }
    }
}

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
	wavegen_config_vertical( &wavegen, WAVEGEN_CHANNEL_1, WAVEGEN_TYPE_SINE, 1000, 1000, 0 );

	wavegen_config_horizontal( &wavegen, WAVEGEN_CHANNEL_2, 1000 );
	wavegen_config_vertical( &wavegen, WAVEGEN_CHANNEL_2, WAVEGEN_TYPE_SINE, 2000, 2000, 0 );

	wavegen_start( &wavegen, WAVEGEN_CHANNEL_1 );
	wavegen_start( &wavegen, WAVEGEN_CHANNEL_2 );

	if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
	{
		wavegen_draw( &wavegen, &lcd );
		osSemaphoreRelease( semaphoreLcdHandle );
	}

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1;
	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );

		if( osMessageQueueGet(queueUiWavegenHandle, &msgUiWavegen, NULL, 0U) == osOK )
		{
			if( osSemaphoreAcquire( semaphoreLcdHandle, 0 ) == osOK )
			{
				wavegen_erase( &wavegen, &lcd );
				osSemaphoreRelease( semaphoreLcdHandle );
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

			if( osSemaphoreAcquire( semaphoreLcdHandle, 0 ) == osOK )
			{
				wavegen_draw( &wavegen, &lcd );
				osSemaphoreRelease( semaphoreLcdHandle );
			}
		}
	}
}
