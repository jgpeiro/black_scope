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
  .stack_size = 1024 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTaskTsc(void *argument);

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
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of taskTsc */
  taskTscHandle = osThreadNew(StartTaskTsc, NULL, &taskTsc_attributes);

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

#define BUFFER_LEN 	(512)
#define FB_WIDTH 	(240)
#define FB_HEIGHT	(80)
#define FB2_WIDTH 	(160-1)
#define FB2_HEIGHT	(16)

uint16_t dac1_buffer[BUFFER_LEN];
uint16_t dac2_buffer[BUFFER_LEN];

uint16_t buffer1[BUFFER_LEN];
uint16_t buffer2[BUFFER_LEN];
uint16_t buffer3[BUFFER_LEN];
uint16_t buffer4[BUFFER_LEN];
uint16_t buffer5[BUFFER_LEN];
uint16_t buffer6[BUFFER_LEN];
uint16_t buffer7[BUFFER_LEN];
uint16_t buffer8[BUFFER_LEN];

uint16_t buffer_tmp[BUFFER_LEN];

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

struct nk_rect keypad_size = {0, 30, 108, 208};
int nk_keypad( struct nk_context *ctx, int32_t *value )
{
	int retval = 0;
	char buffer[32];

    if (nk_popup_begin(ctx, NK_POPUP_STATIC, "Keypad", NK_WINDOW_CLOSABLE | NK_WINDOW_NO_SCROLLBAR, keypad_size ) )
    {
    	retval = 1;
    	nk_layout_row(ctx, NK_STATIC, 30, 1, (float[]){30+30+30});

		sprintf( buffer, "%d", *value );

    	nk_label(ctx, buffer, NK_TEXT_RIGHT );
        nk_layout_row(ctx, NK_STATIC, 30, 3, (float[]){30, 30, 30});

        nk_button_set_behavior( ctx, 0 );
        if( nk_button_label( ctx, "7" ) )	*value = *value*10 + 7;
        if( nk_button_label( ctx, "8" ) )	*value = *value*10 + 8;
        if( nk_button_label( ctx, "9" ) )	*value = *value*10 + 9;
        if( nk_button_label( ctx, "4" ) )	*value = *value*10 + 4;
        if( nk_button_label( ctx, "5" ) )	*value = *value*10 + 5;
        if( nk_button_label( ctx, "6" ) )	*value = *value*10 + 6;
        if( nk_button_label( ctx, "1" ) )	*value = *value*10 + 1;
        if( nk_button_label( ctx, "2" ) )	*value = *value*10 + 2;
        if( nk_button_label( ctx, "3" ) )	*value = *value*10 + 3;
        if( nk_button_label( ctx, "C" ) )	*value = *value/10;
        if( nk_button_label( ctx, "0" ) )	*value = *value*10 + 0;
        if( nk_button_label( ctx, "-" ) )	*value = -*value;
        //if( nk_button_symbol( ctx, NK_SYMBOL_TRIANGLE_RIGHT ) )	{
        //	nk_popup_close(ctx);
        //	retval = 0;
        //}
        if( *value > 999999 )
        {
        	*value = 999999;
        }
        if( *value < -999999 )
        {
        	*value = -999999;
        }
        nk_popup_end(ctx);
    }
    else
    {
    	retval = 0;
    }
    return retval;
}



void lcd_draw_cross( tLcd *pLcd, uint16_t x, uint16_t y, uint16_t color )
{
	lcd_rect( pLcd, x-4, y, 4, 2, color );
	lcd_rect( pLcd, x+2, y, 4, 2, color );
	lcd_rect( pLcd, x, y-4, 2, 4, color );
	lcd_rect( pLcd, x, y+2, 2, 4, color );
}

float text_width_f( nk_handle handle, float h, const char* text, int len )
{
	return font_text_width( &fontUbuntuBookRNormal16, text );
}

extern void nk_draw_fb(struct nk_context *ctx, tFramebuf *pfb, tLcd *pLcd);
extern void nk_set_theme(struct nk_context *ctx, int theme);

void draw_buffers(
		tLcd *pLcd,
		int32_t trigger,
		int32_t trigger_bck,
		uint16_t *buffer1,
		uint16_t *buffer2,
		uint16_t *buffer3,
		uint16_t *buffer4,
		uint16_t *buffer5,
		uint16_t *buffer6,
		uint16_t *buffer7,
		uint16_t *buffer8,
		uint16_t len,
		uint32_t collapsed,
		uint8_t a_b )
{
	int j, j2, n, n2;
	int x0, ya, yb, yc, yd;

	uint16_t width = pLcd->width;
	uint16_t height = pLcd->height;

	lcd_set_window( pLcd, 0, 0, pLcd->width, pLcd->height );

	for( j = 0; j < width; j++ )
	{
		j2 = (j*len)/width;
		n = trigger + j2;
		if( n < 0 )
		{
			n += len;
		}
		else if( n >= len )
		{
			n -= len;
		}

		n2 = trigger_bck + j2;
		if( n2 < 0 )
		{
			n2 += len;
		}
		else if( n2 >= len )
		{
			n2 -= len;
		}

		x0 = collapsed? j : width/2 + j/2;

		ya = height-((a_b?buffer5[n2]:buffer1[n2])*height)/4096;
		yb = height-((a_b?buffer6[n2]:buffer2[n2])*height)/4096;
		yc = height-((a_b?buffer7[n2]:buffer3[n2])*height)/4096;
		yd = height-((a_b?buffer8[n2]:buffer4[n2])*height)/4096;
		//lcd_set_pixel( pLcd, x0, ya, 0x0000 );
		//lcd_set_pixel( pLcd, x0, yb, 0x0000 );
		//lcd_set_pixel( pLcd, x0, yc, 0x0000 );
		//lcd_set_pixel( pLcd, x0, yd, 0x0000 );
		lcd_rect( pLcd, x0-1, ya-1, 2, 2, 0x0000 );
		lcd_rect( pLcd, x0-1, yb-1, 2, 2, 0x0000 );
		lcd_rect( pLcd, x0-1, yc-1, 2, 2, 0x0000 );
		lcd_rect( pLcd, x0-1, yd-1, 2, 2, 0x0000 );

		int jj = j - 4;
		if( j > 4 )
		{
			j2 = (jj*len)/width;
			n = trigger + j2;
			if( n < 0 )
			{
				n += len;
			}
			else if( n >= len )
			{
				n -= len;
			}

			n2 = trigger_bck + j2;
			if( n2 < 0 )
			{
				n2 += len;
			}
			else if( n2 >= len )
			{
				n2 -= len;
			}

			x0 = collapsed? jj : width/2 + jj/2;
			ya = height-((a_b?buffer1[n]:buffer5[n])*height)/4096;
			yb = height-((a_b?buffer2[n]:buffer6[n])*height)/4096;
			yc = height-((a_b?buffer3[n]:buffer7[n])*height)/4096;
			yd = height-((a_b?buffer4[n]:buffer8[n])*height)/4096;
			//lcd_set_pixel( pLcd, x0, ya, 0x001F );
			//lcd_set_pixel( pLcd, x0, yb, 0x07E0 );
			//lcd_set_pixel( pLcd, x0, yc, 0xF800 );
			//lcd_set_pixel( pLcd, x0, yd, 0xF81F );
			lcd_rect( pLcd, x0-1, ya-1, 2, 2, 0x001F );
			lcd_rect( pLcd, x0-1, yb-1, 2, 2, 0x07E0 );
			lcd_rect( pLcd, x0-1, yc-1, 2, 2, 0xF800 );
			lcd_rect( pLcd, x0-1, yd-1, 2, 2, 0xF81F );
		}

		buffer_tmp[j] = a_b?buffer5[n]:buffer1[n];
	}
}

void draw_horizontal_offset( tLcd *pLcd, int32_t offset, uint32_t collapsed )
{
	static int last_offset = 0;
	if( collapsed )
	{
		lcd_rect( pLcd, last_offset/2+480/2, 0, 1, pLcd->height, 0x0000 );
		lcd_rect( pLcd, offset/2+480/2, 0, 1, pLcd->height, 0x07FF );
	}
	else
	{
		lcd_rect( pLcd, 480/2+last_offset/4+480/4, 0, 1, pLcd->height, 0x0000 );
		lcd_rect( pLcd, 480/2+offset/4+480/4, 0, 1, pLcd->height, 0x07FF );
	}
	last_offset = offset;
}

void draw_trigger( tLcd *pLcd, int32_t level, uint32_t collapsed )
{
	static int last_level = 0;
	if( collapsed )
	{ 
		lcd_rect( pLcd, 0, pLcd->height-((last_level)*pLcd->height)/4096, pLcd->width, 1, 0x0000 );
		lcd_rect( pLcd, 0, pLcd->height-((level)*pLcd->height)/4096, pLcd->width, 1, 0x07FF );
	}
	else
	{
		lcd_rect( pLcd, 0/2+pLcd->width/2, pLcd->height-((last_level)*pLcd->height)/4096, pLcd->width, 1, 0x0000 );
		lcd_rect( pLcd, 0/2+pLcd->width/2, pLcd->height-((level)*pLcd->height)/4096, pLcd->width, 1, 0x07FF );
	}
	last_level = level;
}

void draw_grid( tLcd *pLcd, uint32_t collapsed )
{
	static int i = 0;
	if( i < 10 )
	{
		i++;
		return;
	}
	else
	{
		i = 0;
	}
	if( collapsed )
	{
		for( int d = 0 ; d < pLcd->width ; d += 40 )
		{
			if( d < 40*6 )
			{
				lcd_rect( pLcd, d, 40, 1, pLcd->height-40, 0x8410 );
			}
			else
			{
				lcd_rect( pLcd, d, 0, 1, pLcd->height, 0x8410 );
			}
		}
		lcd_rect( pLcd, pLcd->width, 0, 1, pLcd->height, 0x8410 );

		for( int d = 0 ; d < pLcd->height ; d += 40 )
		{
			if( d < 1 )
			{
				lcd_rect( pLcd, 40*6, d, pLcd->width-40*6, 1, 0x8410 );
			}
			else
			{
				lcd_rect( pLcd, 0, d, pLcd->width, 1, 0x8410 );
			}
		}
		lcd_rect( pLcd, 0, pLcd->height, pLcd->width, 1, 0x8410 );
	}
	else
	{
		for( int d = 0 ; d < pLcd->width ; d += 40 )
		{
			lcd_rect( pLcd, d/2+pLcd->width/2, 0, 1, pLcd->height, 0x8410 );
		}
		lcd_rect( pLcd, pLcd->width/2+pLcd->width/2, 0, 1, pLcd->height, 0x8410 );

		for( int d = 0 ; d < pLcd->height ; d += 40 )
		{
			lcd_rect( pLcd, 0/2+pLcd->width/2, d, pLcd->width, 1, 0x8410 );
		}
		lcd_rect( pLcd, 0/2+pLcd->width/2, pLcd->height, pLcd->width, 1, 0x8410 );
	}

}


void draw_measurements( tLcd *pLcd, int collapsed )
{
	tFramebuf fb;
	uint16_t min, max, avg, period, duty;
	char buffer[32];

	min = get_vmin( buffer_tmp, 480 );
	max = get_vmax( buffer_tmp, 480 );
	avg = get_vavg( buffer_tmp, 480 );
	period = get_period( buffer_tmp, 480, max, min, avg );
	duty = get_duty( buffer_tmp, 480, max, min, avg );

	framebuf_init( &fb, FB2_WIDTH, FB2_HEIGHT, fb2_buf );
	framebuf_fill( &fb, 0x0000 );

	sprintf( buffer, "%04d %04d %04d %04d", min, max, period, duty );
	framebuf_text( &fb, &fontUbuntuBookRNormal16, 0, 0, buffer, 0xFFFF );
	if( collapsed )
	{
		lcd_bmp( pLcd, 240+40+1, 1, fb.width, fb.height, fb.buf );
	}
	else
	{
		lcd_bmp( pLcd, 240+40+1, 1, fb.width, fb.height, fb.buf );
	}
}
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
uint16_t x_tsc = 0;
uint16_t y_tsc = 0;
uint16_t p_tsc = 0;

struct nk_context ctx = {0};
tWaveGen wavegen;
tScope scope = {0};

/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	tLcd lcd = {0};
	//tTsc tsc = {0};
	tFramebuf fb = {0};

	//struct nk_context ctx = {0};
	struct nk_buffer cmds = {0};
	struct nk_buffer pool = {0};
	struct nk_user_font font = {0};

	//struct Oscilloscope osc = {0};
	

	tUi ui = {0};


	int i;
	float dac_freq = 3;
	int32_t trigger = 0;
	int trigger_bck = 0;
	uint16_t x = 0, y = 0;
	uint16_t x_bck = 0, y_bck = 0;
	int pressed = 1;
	int pressed_bck = 0;
	int nk_theme =1;
	int collapsed_bck = 0;
	int collapsed = 0;

	//psram_test();

	lcd_init( &lcd, LCD_nRST_GPIO_Port, LCD_nRST_Pin, LCD_BL_GPIO_Port, LCD_BL_Pin, 480, 320 );

	framebuf_init( &fb, FB_WIDTH, FB_HEIGHT, fb_buf );

	nk_buffer_init_fixed( &cmds, nk_buf_cmds, NK_BUFFER_CMDS_LEN );
	nk_buffer_init_fixed( &pool, nk_buf_pool, NK_BUFFER_POOL_LEN);

	font.height = fontUbuntuBookRNormal16.bbxh;
	font.width = text_width_f;
	nk_init_custom( &ctx, &cmds, &pool, &font );
	nk_set_theme( &ctx, THEME_DARK );

	//for( i = 0 ; i < BUFFER_LEN ; i++ )
	//{
	//	dac1_buffer[i] = sinf(2*dac_freq*2*M_PI*i/BUFFER_LEN)*1023 + 2048;
	//	dac2_buffer[i] = sinf(dac_freq*2*M_PI*i/BUFFER_LEN)*1023 + 2048;
	//}

	__HAL_DBGMCU_FREEZE_TIM4();
	__HAL_DBGMCU_FREEZE_TIM6();
	wavegen_init( &wavegen,
		&hdac1,
		&htim4,
		&htim6,
		dac1_buffer,
		dac2_buffer,
		100,
		1e6 );
    wavegen_build_sine( &wavegen, 0x01, 10e3, 2047, 1500 );
    wavegen_build_sine( &wavegen, 0x02, 20e3, 2047, 1500 );
    wavegen_start( &wavegen, 0x03 );
    //HAL_Delay( 1000 );
    //wavegen_stop( &wavegen, 0x03 );

	//osc.acquire_run = 1;
    //osc.channels[0].offset = 2048;
    //osc.channels[1].offset = 2048;
    //osc.channels[2].offset = 2048;
    //osc.channels[3].offset = 2048;
    //HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, osc.channels[0].offset);
    //HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);

	//scope.horizontal.htim_clock
    //osc.horizontal_offset = 0;
    //osc.horizontal_scale = 1000;
    //osc.waveforms[0].type = 1;
    //osc.waveforms[0].offset = 2000;
    //osc.waveforms[0].scale = 1000;

	scope.horizontal.htim_clock = &htim2;
	scope.horizontal.htim_stop =  &htim3;
	scope.vertical.hdac = &hdac2;
	scope.vertical.hopamp1 = &hopamp1;
	scope.vertical.hopamp2 = &hopamp3;
	scope.vertical.hopamp3 = &hopamp5;
	scope.vertical.hopamp4 = &hopamp6;
	scope.trigger.hadc1 = &hadc1;
	scope.trigger.hadc2 = &hadc3;
	scope.trigger.hadc3 = &hadc5;
	scope.trigger.hadc4 = &hadc4;
	scope.wavegen.htim1 = &htim4;
	scope.wavegen.htim2 = &htim6;


    ui.acquire.run = 1;
    ui.acquire.single = 0;

    ui.horizontal.offset = 0;
	ui.horizontal.scale = 1000;
	scope_config_horizontal( &scope, ui.horizontal.scale*1000, 512 );

	ui.vertical.offset = 2048;
	ui.vertical.channels[0].enabled = 1;
	ui.vertical.channels[0].coupling = 0;
	ui.vertical.channels[0].scale = 0;
	ui.vertical.channels[1].enabled = 1;
	ui.vertical.channels[1].coupling = 0;
	ui.vertical.channels[1].scale = 0;
	ui.vertical.channels[2].enabled = 1;
	ui.vertical.channels[3].coupling = 0;
	ui.vertical.channels[2].scale = 0;
	ui.vertical.channels[3].enabled = 1;
	ui.vertical.channels[3].coupling = 0;
	ui.vertical.channels[3].scale = 0;
	scope_config_vertical( &scope,
		ui.vertical.channels[0].scale,
		ui.vertical.channels[1].scale,
		ui.vertical.channels[2].scale,
		ui.vertical.channels[3].scale,
		ui.vertical.offset );

	ui.wavegen.waveforms[0].enabled = 1;
	ui.wavegen.waveforms[0].type = 1;
	ui.wavegen.waveforms[0].offset = 2048;
	ui.wavegen.waveforms[0].scale = 2000;
	ui.wavegen.waveforms[0].frequency = 10000;
	ui.wavegen.waveforms[0].duty_cycle = 0;

	ui.wavegen.waveforms[1].enabled = 1;
	ui.wavegen.waveforms[1].type = 1;
	ui.wavegen.waveforms[1].offset = 2048;
	ui.wavegen.waveforms[1].scale = 2000;
	ui.wavegen.waveforms[1].frequency = 10000;
	ui.wavegen.waveforms[1].duty_cycle = 0;

    wavegen_build_sine( &wavegen,
    	1 << 0,
    	ui.wavegen.waveforms[0].frequency,
		ui.wavegen.waveforms[0].scale,
		ui.wavegen.waveforms[0].offset );
    wavegen_build_sine( &wavegen,
    	1 << 1,
    	ui.wavegen.waveforms[1].frequency,
		ui.wavegen.waveforms[1].scale,
		ui.wavegen.waveforms[1].offset );

	ui.trigger.level = 2048+512;
	ui.trigger.mode = UI_TRIGGER_MODE_NORMAL;
	ui.trigger.slope = UI_TRIGGER_SLOPE_RISING;
	ui.trigger.source = 0;
	scope_config_trigger( &scope,
		ui.trigger.source,
		ui.trigger.mode,
		ui.trigger.level,
		ui.trigger.slope );

	scope_config_horizontal( &scope, ui.horizontal.scale*1000, 512 );
	//void scope_config_vertical( tScope *scope, int gain1, int gain2, int gain3, int gain4, int offset );
	//void scope_config_trigger( tScope *scope, int channel, int mode, int level, int slope );


	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 50;
	xLastWakeTime = xTaskGetTickCount();
	int key_cnt = 0;
	int key = NK_KEY_RIGHT;
	/* Infinite loop */
	for(;;)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		x_bck = x;
		y_bck = y;
		//tsc_read( &tsc, &x, &y );
		x = x_tsc;
		y = y_tsc;

		//continue;
		pressed_bck = pressed;
		pressed = p_tsc && x < fb.width;
		nk_input_begin( &ctx );

		if( key_cnt%10 == 0 )
		{
			nk_input_key( &ctx, key, 1 );
		}
		if( key_cnt%10 == 1 )
		{
			nk_input_key( &ctx, key, 0 );
		}
		key_cnt += 1;

		if( pressed )
		{
			nk_input_motion( &ctx, x, y );
			nk_input_button( &ctx, 0, x, y, 1 );
		}
		else if( pressed_bck )
		{
			nk_input_motion( &ctx, x_bck, y_bck );
			nk_input_button( &ctx, 0, x_bck, y_bck, 0 );
		}
		nk_input_end( &ctx );

	    if( pressed || pressed_bck )
	    {
		    //oscilloscope_process( &osc, &ctx, &scope );
	    	ui_build( &ui, &ctx );
			collapsed = nk_window_is_collapsed( &ctx, "STM32G4 Scope" );
			if( collapsed != collapsed_bck )
			{
				collapsed_bck = collapsed;
				lcd_clear( &lcd, 0x0000 );
			}

			nk_draw_fb( &ctx, &fb, &lcd );
			nk_clear(&ctx);
	    }

	    if( ui.acquire.run || ui.acquire.single )
	    {
	    	if( ui.acquire.single )
	    	{
		    	ui.acquire.run = 0;
		    	ui.acquire.single = 0;
	    	}
	    	scope_init( &scope, 2048, 1000000,
					(i&0x01)?buffer1:buffer5,
					(i&0x01)?buffer2:buffer6,
					(i&0x01)?buffer3:buffer7,
					(i&0x01)?buffer4:buffer8,
					BUFFER_LEN );
			scope_start( &scope );
			int t0 = HAL_GetTick();
			while( scope_is_busy( &scope ) && HAL_GetTick()-t0 < (1000/xFrequency) );
			scope_stop( &scope );

			trigger = scope_get_trigger( &scope ) - BUFFER_LEN/2;

			draw_grid( &lcd, collapsed );

			draw_buffers(
				&lcd,
				trigger,
				trigger_bck,
				buffer1,
				buffer2,
				buffer3,
				buffer4,
				buffer5,
				buffer6,
				buffer7,
				buffer8,
				BUFFER_LEN,
				collapsed,
				i&0x01
			);

			draw_measurements( &lcd, collapsed );

			draw_horizontal_offset( &lcd, ui.horizontal.offset, collapsed );
			draw_trigger( &lcd, ui.trigger.level, collapsed );

			trigger_bck = trigger;
			i += 1;
	    }

		if( pressed && x < 240-5 )
		{
			lcd_draw_cross( &lcd, x, y, 0xFFFF );
		}

		//osDelay(1);


	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTaskTsc */
/**
* @brief Function implementing the taskTsc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskTsc */
void StartTaskTsc(void *argument)
{
  /* USER CODE BEGIN StartTaskTsc */
	uint16_t x = 0;
	uint16_t y = 0;

	float x_low = 0;
	float y_low = 0;

	int cnt = 0;

	float AX = 38.0/151.0;
	float BX = -1950.0/151.0;
	float AY = 11.0/62.0;
	float BY = -1157.0/62.0;

	tTsc tsc = {0};
	tsc_init( &tsc, &hspi3, TSC_nSS_GPIO_Port, TSC_nSS_Pin, AX, BX, AY, BY, 32 );

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1;
	xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	  vTaskDelayUntil( &xLastWakeTime, xFrequency );
	  uint16_t xx;
	  uint16_t yy;
	  int acc_x = 0;
	  int acc_y = 0;
	  x = 0;
	  y = 0;
	  int i;
	  int N = 8;
	  for( i = 0 ; i < N ; i++ )
	  {
		  xx = 0;
		  yy = 0;
		  tsc_read_ll( &tsc, &xx, &yy );
		 // printf( "%d, %d, %d\n", HAL_GetTick(), (int)xx, (int)yy );
		  if( xx == 0 )
		  {
			  acc_x = 0;
			  acc_y = 0;
			  break;
		  }
		  acc_x += xx;
		  acc_y += yy;
	  }

	  if( acc_x )
	  {
		  cnt += 1;
	  }
	  else
	  {
		  cnt = 0;
	  }

	  // filter
	  if( acc_x )
	  {
		  x_low = acc_x/N*0.1 + x_low*0.9;
		  y_low = acc_y/N*0.1 + y_low*0.9;
	  }

	  // tsc to pixel coordinates
	  x = tsc.ax*x_low + tsc.bx;
	  y = tsc.ay*y_low + tsc.by;

	  if( cnt > 50 )
	  {
		  x_tsc = x;
		  y_tsc = y;
		  p_tsc = 1;
	  }
	  else
	  {
		  x_tsc = 0;
		  y_tsc = 0;
		  p_tsc = 0;
	  }
	  //osDelay(1);
  }
  /* USER CODE END StartTaskTsc */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

