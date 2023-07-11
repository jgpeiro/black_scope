/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "opamp.h"
#include "quadspi.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#include "psram.h"

#define NK_INCLUDE_STANDARD_VARARGS (1)
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
	int DataIdx;
	for(DataIdx = 0; DataIdx < len; DataIdx++)
	{
		ITM_SendChar(*ptr++);
	}
    return len;
}

extern DMA_HandleTypeDef hdma_adc1;
uint8_t scope_filled = 0;
int scope_trigger_location = 0;
uint8_t scope_done = 0;

#define BUFFER_LEN (1024)

uint32_t micros(void) {
  return HAL_GetTick() * 1000 + (1000 - SysTick->VAL) / 170;
}

uint32_t scope_t0 = 0;
uint32_t scope_t1 = 0;
uint32_t scope_t2 = 0;
uint32_t scope_t3 = 0;
uint32_t scope_t4 = 0;

uint32_t scope_x0 = 0;
uint32_t scope_y0 = 0;
uint32_t scope_x1 = 0;
uint32_t scope_y1 = 0;
uint32_t scope_x2 = 0;
uint32_t scope_y2 = 0;
uint32_t scope_x3 = 0;
uint32_t scope_y3 = 0;
uint32_t scope_x4 = 0;
uint32_t scope_y4 = 0;
uint32_t scope_trigger_location2 = 0;
void _HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	if( htim == &htim2 )
	{
		HAL_TIM_Base_Stop( &htim1 );
		//scope_trigger_location = hdma_adc1.Instance->CNDTR;
		scope_done = 1;
		HAL_TIM_OnePulse_Stop_IT(&htim2, TIM_CHANNEL_1);
		HAL_TIM_Base_Stop( &htim2 );
		HAL_ADC_Stop_DMA(&hadc1 );

		scope_x4 = BUFFER_LEN-hdma_adc1.Instance->CNDTR;
		scope_y4 = hadc1.Instance->DR;
		scope_t4 = micros();
		scope_y4 += 0;
	}
}

void _HAL_ADCEx_LevelOutOfWindow2Callback(ADC_HandleTypeDef *hadc)
{
	scope_trigger_location = BUFFER_LEN-hdma_adc1.Instance->CNDTR;
	HAL_TIM_Base_Start( &htim2 );
	__HAL_TIM_CLEAR_FLAG(&htim2, TIM_IT_CC1);
	HAL_TIM_OnePulse_Start_IT( &htim2, TIM_CHANNEL_1 );
	LL_ADC_DisableIT_AWD1( hadc->Instance );
	LL_ADC_DisableIT_AWD2( hadc->Instance );
	__HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
	__HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD2);
	scope_x3 = BUFFER_LEN-hdma_adc1.Instance->CNDTR;
	scope_y3 = hadc1.Instance->DR;
	scope_t3 = micros();
	scope_y3 += 0;
}

void _HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc)
{

	LL_ADC_DisableIT_AWD1( hadc->Instance );
	__HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
	__HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD2);
	LL_ADC_EnableIT_AWD2( hadc->Instance );
	scope_x2 = BUFFER_LEN-hdma_adc1.Instance->CNDTR;
	scope_y2 = hadc1.Instance->DR;
	scope_t2 = micros();
	scope_y2 += 0;
}
uint8_t scope_stop();
int cnt_conv_cplt = 0;
void _HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if( hadc == &hadc1 )
	{
		//scope_filled = 2;
		if( !scope_filled )
		{
			__HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
			__HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD2);
			LL_ADC_EnableIT_AWD1( hadc1.Instance );
			scope_filled = 1;
			scope_x1 = BUFFER_LEN-hdma_adc1.Instance->CNDTR;
			scope_y1 = hadc1.Instance->DR;
			scope_t1 = micros();
			scope_y1 += 0;
		}
		if( cnt_conv_cplt < 2 )
		{
			cnt_conv_cplt += 1;
		}
		else
		{
			//scope_stop();
		}
	}
}
void _HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
	if( hadc == &hadc1 )
	{

	}
}

uint8_t _scope_start( uint16_t *buffer, uint16_t len, uint32_t sample_rate, uint16_t trigger_level )
{
	scope_filled = 0;
	scope_done = 0;
	scope_trigger_location = 0;
	scope_trigger_location2 = 0;
	cnt_conv_cplt = 0;
	float vcc = 3.256;

	__HAL_DBGMCU_FREEZE_TIM1();
	__HAL_DBGMCU_FREEZE_TIM2();

	HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (vcc/2.0)/vcc*4095);
	HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);

	HAL_OPAMP_Start(&hopamp1);

	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);


	LL_ADC_DisableIT_AWD1( hadc1.Instance );
	LL_ADC_DisableIT_AWD2( hadc1.Instance );
	__HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD1);
	__HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_AWD2);

	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)buffer, len );
	HAL_TIM_Base_Start( &htim1 );

	scope_x0 = BUFFER_LEN-hdma_adc1.Instance->CNDTR;
	scope_y0 = hadc1.Instance->DR;
	scope_t0 = micros();
	scope_y0 += 0;
}

uint8_t _scope_is_done()
{
	return scope_done;
}

uint8_t _scope_stop()
{
	HAL_TIM_Base_Stop( &htim1 );
	HAL_TIM_OnePulse_Stop_IT(&htim2, TIM_CHANNEL_1);
	HAL_TIM_Base_Stop( &htim2 );
	HAL_ADC_Stop_DMA(&hadc1 );
	scope_done = 1;
}

uint16_t scope_get_trigger_location()
{
	return scope_trigger_location;
}
#include <math.h>
float get_mean( uint16_t *data, uint16_t len )
{
	int i;
	float sum;

	sum = 0;
	for( i = 0; i < len; i++ )
	{
		sum += data[i];
	}

	return sum / len;
}

float get_std( uint16_t *data, uint16_t len )
{
    int i;
    float mean;
    float err;
    float sum;

    mean = get_mean( data, len );

    sum = 0;
    for( i = 0; i < len; i++ )
    {
    	err = data[i] - mean;
    	sum += err*err;
    }

    return sqrtf(sum/len);
}
float std_mean = 0;
uint8_t N = 1;
uint8_t M = 20;
uint8_t scope_test()
{
	uint16_t buffer[ BUFFER_LEN ] = {0};
	uint32_t sample_rate = 1000;
	uint16_t trigger_level = 2048;
	ITM->TCR |= ITM_TCR_ITMENA_Msk;

	for( int i = 0 ; i < M ; i++ )
	{
		_scope_start( buffer, BUFFER_LEN, sample_rate, trigger_level );
		while( !_scope_is_done() );
		scope_get_trigger_location();
		_scope_stop();

		//printf("#%d, %d, %d\n", scope_t0, scope_x0, scope_y0 );
		//printf("#%d, %d, %d\n", scope_t1-scope_t0, scope_x1, scope_y1 );
		//printf("#%d, %d, %d\n", scope_t2-scope_t0, scope_x2, scope_y2 );
		//printf("#%d, %d, %d\n", scope_t3-scope_t0, scope_x3, scope_y3 );
		//printf("#%d, %d, %d\n", scope_t4-scope_t0, scope_x4, scope_y4 );

		/*printf("%d, %d, %d, %d, %d, %d\n",
				scope_y0>2200,
				scope_y1>2200,
				scope_y2>2200,
				scope_y3>2200,
				scope_y4>2200,
				scope_trigger_location>512,
				scope_t4-scope_t3<20
		);*/

		if( scope_t4-scope_t3>20 )
		{
			printf( "data%d = np.array( [", i );

			for( int j = -BUFFER_LEN/2 ; j < BUFFER_LEN/2 ; j++ )
			{
				int n = scope_trigger_location + j;
				if( n < 0 )
				{
					n += BUFFER_LEN;
				}
				else if( n >= BUFFER_LEN )
				{
					n -= BUFFER_LEN;
				}
				printf( "%d, ", buffer[n] );
			}
			printf( "], dtype=np.float32 )\n" );
		}

		/*if( std_mean == 0 )
		{
			std_mean = get_std( buffer, BUFFER_LEN );
		}
		else
		{
			std_mean = std_mean*0.99 + get_std( buffer, BUFFER_LEN )*0.01;
		}

		if( (i&0x1F) == 0 )
			printf( "%f, %f\n", get_mean( buffer, BUFFER_LEN ), std_mean );
		*/
		HAL_Delay( 100 );
	}
	printf( "print(\"done\")\n" );
	return 1;
}


#include "nuklear.h"
float text_width_f( nk_handle handle, float h, const char* t, int len )
{
    return 8*len;
}

#define LCD_CMD_ADDR	(0x60000000)
#define LCD_DATA_ADDR	(0x60000002) // 0x60000001 ??
uint16_t *addr_cmd = (uint16_t*) LCD_CMD_ADDR;
uint16_t *addr_data = (uint16_t*) LCD_DATA_ADDR;

void lcd_config( void );
void lcd_rect( int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color );

void lcd_config( void )
{
	HAL_GPIO_WritePin( GPIOA, GPIO_PIN_8, GPIO_PIN_SET );
	HAL_GPIO_WritePin( GPIOA, GPIO_PIN_9, GPIO_PIN_RESET );
	HAL_Delay( 10 );
	HAL_GPIO_WritePin( GPIOA, GPIO_PIN_9, GPIO_PIN_SET );
	HAL_Delay( 100 );

	*addr_cmd = 0x01;
	HAL_Delay( 100 );
	*addr_cmd = 0x11;
	HAL_Delay( 10 );
	*addr_cmd = 0x3A;
	*addr_data = 0x06;//0x05;
	*addr_cmd = 0x36;
	*addr_data = (0x01<<5)|(0x01<<6)|(0x01<<7);
	*addr_cmd = 0x29;
	HAL_Delay( 10 );

	lcd_rect( 0, 0, 480, 320, 0x0000 );
}

uint32_t nk_colot_to_rgb666( struct nk_color color )
{
	uint32_t rgb666 = 0;

	color.r = (color.r*color.a)/256.0;
	color.g = (color.g*color.a)/256.0;
	color.b = (color.b*color.a)/256.0;

	color.r >>= 2;
	color.g >>= 2;
	color.b >>= 2;
	rgb666 = (color.r<<12) | (color.g<<6) | (color.b<<0);
	return rgb666;
}

void lcd_rect( int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color )
{
	int16_t x0 = x;
	int16_t y0 = y;
	int16_t x1 = x + w - 1;
	int16_t y1 = y + h - 1;

	if( x0 < 0 )
	{
		x0 = 0;
	}
	else if( x0 > 480 - 1 )
	{
		x0 = 480 - 1;
	}

	if( x1 < 0 )
	{
		x1 = 0;
	}
	else if( x1 > 480 - 1 )
	{
		x1 = 480 - 1;
	}

	if( y0 < 0 )
	{
		y0 = 0;
	}
	else if( y0 > 320 - 1 )
	{
		y0 = 320 - 1;
	}

	if( y1 < 0 )
	{
		y1 = 0;
	}
	else if( y1 > 320 - 1 )
	{
		y1 = 320 - 1;
	}

	if( x0 > x1 )
	{
		int tmp = x0;
		x0 = x1;
		x1 = tmp;
	}

	if( y0 > y1 )
	{
		int tmp = y0;
		y0 = y1;
		y1 = tmp;
	}

	w = x1 - x0 + 1;
	h = y1 - y0 + 1;

	uint32_t i = 0;
	uint32_t n = w*h;

	uint16_t color_l = (color>>0)&0x1FF;
	uint16_t color_h = (color>>9)&0x1FF;

	*addr_cmd = 0x2A;
	*addr_data = (x0>>8)&0xFF;
	*addr_data = (x0>>0)&0xFF;
	*addr_data = (x1>>8)&0xFF;
	*addr_data = (x1>>0)&0xFF;

	*addr_cmd = 0x2B;
	*addr_data = (y0>>8)&0xFF;
	*addr_data = (y0>>0)&0xFF;
	*addr_data = (y1>>8)&0xFF;
	*addr_data = (y1>>0)&0xFF;

	*addr_cmd = 0x2C;
	for( i = 0 ; i < n ; i++ )
	{
		*addr_data = color_l;
		*addr_data = color_h;
	}
}

void lcd_set_pixel( int16_t x, int16_t y, uint32_t color )
{
	int16_t x0 = x;
	int16_t y0 = y;
	int16_t x1 = x + 1;
	int16_t y1 = y + 1;
	uint16_t color_l = (color>>0)&0x1FF;
	uint16_t color_h = (color>>9)&0x1FF;

	*addr_cmd = 0x2A;
	*addr_data = (x0>>8)&0xFF;
	*addr_data = (x0>>0)&0xFF;
	*addr_data = (x1>>8)&0xFF;
	*addr_data = (x1>>0)&0xFF;

	*addr_cmd = 0x2B;
	*addr_data = (y0>>8)&0xFF;
	*addr_data = (y0>>0)&0xFF;
	*addr_data = (y1>>8)&0xFF;
	*addr_data = (y1>>0)&0xFF;

	*addr_cmd = 0x2C;
	//for( int i = 0 ; i < n ; i++ )
	{
		*addr_data = color_l;
		*addr_data = color_h;
	}
}

void lcd_text( uint16_t x0, uint16_t y0, char *str, uint32_t color )
{
	for(; *str; ++str) {
	        // get char and make sure its in range of font
	        int chr = *(uint8_t *)str;
	        if (chr < 32 || chr > 127) {
	            chr = 127;
	        }
	        // get char data
	        extern const uint8_t font_petme128_8x8[];
	        const uint8_t *chr_data = &font_petme128_8x8[(chr - 32) * 8];
	        // loop over char data
	        for (int j = 0; j < 8; j++, x0++) {
	            if (0 <= x0 && x0 < 480) { // clip x
	                uint32_t vline_data = chr_data[j]; // each byte is a column of 8 pixels, LSB at top
	                for (int y = y0; vline_data; vline_data >>= 1, y++) { // scan over vertical column
	                    if (vline_data & 1) { // only draw if pixel set
	                        if (0 <= y && y < 320) { // clip y
	                        	lcd_rect( x0, y, 1, 1, color);
	                        }
	                    }
	                }
	            }
	        }
	    }
}

void lcd_bmp( int16_t x, int16_t y, int16_t w, int16_t h, uint8_t *buf )
{
	int16_t x0 = x;
	int16_t y0 = y;
	int16_t x1 = x + w - 1;
	int16_t y1 = y + h - 1;

	if( x0 < 0 )
	{
		x0 = 0;
	}
	else if( x0 > 480 - 1 )
	{
		x0 = 480 - 1;
	}

	if( x1 < 0 )
	{
		x1 = 0;
	}
	else if( x1 > 480 - 1 )
	{
		x1 = 480 - 1;
	}

	if( y0 < 0 )
	{
		y0 = 0;
	}
	else if( y0 > 320 - 1 )
	{
		y0 = 320 - 1;
	}

	if( y1 < 0 )
	{
		y1 = 0;
	}
	else if( y1 > 320 - 1 )
	{
		y1 = 320 - 1;
	}

	if( x0 > x1 )
	{
		int tmp = x0;
		x0 = x1;
		x1 = tmp;
	}

	if( y0 > y1 )
	{
		int tmp = y0;
		y0 = y1;
		y1 = tmp;
	}

	w = x1 - x0 + 1;
	h = y1 - y0 + 1;

	uint32_t i = 0;
	uint32_t n = w*h;

	//uint16_t color_l = (color>>0)&0x1FF;
	//uint16_t color_h = (color>>9)&0x1FF;

	*addr_cmd = 0x2A;
	*addr_data = (x0>>8)&0xFF;
	*addr_data = (x0>>0)&0xFF;
	*addr_data = (x1>>8)&0xFF;
	*addr_data = (x1>>0)&0xFF;

	*addr_cmd = 0x2B;
	*addr_data = (y0>>8)&0xFF;
	*addr_data = (y0>>0)&0xFF;
	*addr_data = (y1>>8)&0xFF;
	*addr_data = (y1>>0)&0xFF;

	*addr_cmd = 0x2C;
	for( i = 0 ; i < n ; i++ )
	{
		*addr_data = buf[2*i+1];
		*addr_data = buf[2*i+0];
	}
}

void nk_scope( struct nk_context *ctx )
{
    if( nk_begin(ctx, "STM32G4 Scope", nk_rect(10, 10, 480-10, 320-10), 0 ) )
    {
        if( nk_tree_push( ctx, NK_TREE_TAB, "Acquire", NK_MAXIMIZED) )
        {
            nk_layout_row_static(ctx, 30, 100, 4);
            if( nk_button_label(ctx, "Run") )
            {
            }
            if( nk_button_label(ctx, "Stop") )
            {
            }
            if( nk_button_label(ctx, "Single") )
            {
            }
            if( nk_button_label(ctx, "Draw") )
            {
            }
            nk_tree_pop(ctx);
        }

        if( nk_tree_push( ctx, NK_TREE_TAB, "Horizontal", NK_MINIMIZED) )
        {
            static int int_slider1 = 0;
            static int int_slider2 = 0;
            nk_layout_row_static(ctx, 20, 100, 4 );
            nk_label( ctx, "Offset", NK_TEXT_LEFT );
            if( nk_button_label(ctx, "+") )
            {
            }
            nk_slider_int(ctx, 0, &int_slider1, 10, 1);
            if( nk_button_label(ctx, "-") )
            {
            }

            nk_layout_row_static(ctx, 20, 100, 4 );
            nk_label( ctx, "Scale", NK_TEXT_LEFT );
            if( nk_button_label(ctx, "+") )
            {
            }
            nk_slider_int(ctx, 0, &int_slider2, 10, 1);
            if( nk_button_label(ctx, "-") )
            {
            }
            nk_tree_pop(ctx);
        }
    }
    nk_end(ctx);
}

#define CHANNEL_COUNT 4
#define WAVEFORM_COUNT 2
#define CURSOR_COUNT 2
/*
enum TRIGGER_MODE {
    TRIGGER_MODE_AUTO,
    TRIGGER_MODE_NORMAL
};*/

enum TRIGGER_SLOPE {
    TRIGGER_SLOPE_RISING,
    TRIGGER_SLOPE_FALLING,
    TRIGGER_SLOPE_BOTH
};

enum CHANNEL_COUPLING {
    CHANNEL_COUPLING_AC,
    CHANNEL_COUPLING_DC,
    CHANNEL_COUPLING_GND
};

struct Oscilloscope {
    nk_bool acquire_run;
    nk_bool acquire_single;

    float horizontal_offset;
    float horizontal_scale;

    float trigger_offset;
    int trigger_mode;
    int trigger_slope;
    int trigger_source;
    nk_bool trigger_armed;

    int channel_selected;
    struct {
        nk_bool enabled;
        int coupling;
        float offset;
        float scale;
        struct nk_color color;
    } channels[CHANNEL_COUNT];

    int waveform_selected;
    struct {
        nk_bool enabled;
        int type;
        float offset;
        float scale;
        float duty_cycle;
        struct nk_color color;
    } waveforms[WAVEFORM_COUNT];

    int cursor_selected;
    struct {
        nk_bool enabled;
        nk_bool horizontal;
        float offset;
        float track;
        struct nk_color color;
    } cursors[CURSOR_COUNT];

    nk_bool draw_bg;
    int draw_signals;
};
int visible = 0;
void oscilloscope_process(struct Oscilloscope *osc, struct nk_context *ctx)
{
	visible = 0;
	if( nk_begin(ctx, "STM32G4 Scope", nk_rect(0, 0, 480, 320), NK_WINDOW_MINIMIZABLE ) )
	{
		visible = 1;
        osc->draw_bg = nk_true;
        if( nk_tree_push( ctx, NK_TREE_TAB, "Acquire", NK_MAXIMIZED) ){
            osc->draw_bg = nk_false;
            nk_layout_row(ctx, NK_STATIC, 30, 4, (float[]){60, 60, 60, 60});
            if (osc->acquire_run) {
                if (nk_button_label(ctx, "Stop")) {
                    osc->acquire_run = nk_false;
                }
            } else {
                if (nk_button_label(ctx, "Run")) {
                    osc->acquire_run = nk_true;
                }
            }

            if (nk_button_label(ctx, "Single")) {
                osc->acquire_single = nk_true;
            }

            if (nk_button_label(ctx, "Draw")) {
                if (osc->draw_signals + 1 < 3) {
                    osc->draw_signals += 1;
                } else {
                    osc->draw_signals = 0;
                }
            }

            nk_button_label(ctx, "...");
            nk_tree_pop(ctx);
        }

        {
        	if( nk_tree_push( ctx, NK_TREE_TAB, "Horizontal", NK_MINIMIZED) ){
        		nk_layout_row(ctx, NK_STATIC, 30, 2, (float[]){60, 60});
                osc->horizontal_offset = nk_slider_float(ctx, -10.0f, &osc->horizontal_offset, 10.0f, 1.0f);
                osc->horizontal_scale = nk_slider_float(ctx, 1.0f, &osc->horizontal_scale, 1000.0f, 1.0f);
                nk_tree_pop(ctx);
            }

        	if( nk_tree_push( ctx, NK_TREE_TAB, "Vertical", NK_MINIMIZED) ){
                osc->channel_selected = nk_combo(ctx, (const char*[]){"Ch1", "Ch2", "Ch3", "Ch4"}, CHANNEL_COUNT, osc->channel_selected, 20, nk_vec2(60, 200));
                nk_layout_row(ctx, NK_STATIC, 30, 2, (float[]){60, 60});
                osc->channels[osc->channel_selected].enabled = nk_combo(ctx, (const char*[]){"Off", "On"}, 2, osc->channels[osc->channel_selected].enabled, 20, nk_vec2(60, 200));
                osc->channels[osc->channel_selected].coupling = nk_combo(ctx, (const char*[]){"DC", "AC", "Gnd"}, 3, osc->channels[osc->channel_selected].coupling, 20, nk_vec2(60, 200));
                osc->channels[osc->channel_selected].offset = nk_slider_float(ctx, -10.0f, &osc->channels[osc->channel_selected].offset, 10.0f, 1.0f);
                osc->channels[osc->channel_selected].scale = nk_slider_float(ctx, -10.0f, &osc->channels[osc->channel_selected].scale, 10.0f, 1.0f);
                nk_tree_pop(ctx);
            }

            if( nk_tree_push( ctx, NK_TREE_TAB, "Trigger", NK_MINIMIZED) ){
                osc->trigger_source = nk_combo(ctx, (const char*[]){"Ch1", "Ch2", "Ch3", "Ch4"}, CHANNEL_COUNT, osc->trigger_source, 20, nk_vec2(60, 200));
                nk_layout_row(ctx, NK_STATIC, 30, 2, (float[]){60, 60});
                osc->trigger_mode = nk_combo(ctx, (const char*[]){"Normal", "Auto"}, 2, osc->trigger_mode, 20, nk_vec2(60, 200));
                osc->trigger_slope = nk_combo(ctx, (const char*[]){"Rising", "Falling", "Both"}, 3, osc->trigger_slope, 20, nk_vec2(60, 200));
                osc->trigger_offset = nk_slider_float(ctx, -10.0f, &osc->trigger_offset, 10.0f, 1.0f);
                nk_tree_pop(ctx);
            }

            if( nk_tree_push( ctx, NK_TREE_TAB, "Waveform", NK_MINIMIZED) ){
                osc->waveform_selected = nk_combo(ctx, (const char*[]){"Wf1", "Wf2"}, WAVEFORM_COUNT, osc->waveform_selected, 20, nk_vec2(60, 200));
                nk_layout_row(ctx, NK_STATIC, 30, 2, (float[]){60, 60});
                osc->waveforms[osc->waveform_selected].enabled = nk_combo(ctx, (const char*[]){"Off", "On"}, 2, osc->waveforms[osc->waveform_selected].enabled, 20, nk_vec2(60, 200));
                osc->waveforms[osc->waveform_selected].type = nk_combo(ctx, (const char*[]){"DC", "PWM", "Sine", "Tria", "Saw", "Noise"}, 6, osc->waveforms[osc->waveform_selected].type, 20, nk_vec2(60, 200));
                osc->waveforms[osc->waveform_selected].offset = nk_slider_float(ctx, -10.0f, &osc->waveforms[osc->waveform_selected].offset, 10.0f, 1.0f);
                osc->waveforms[osc->waveform_selected].scale = nk_slider_float(ctx, -10.0f, &osc->waveforms[osc->waveform_selected].scale, 10.0f, 1.0f);
                osc->waveforms[osc->waveform_selected].duty_cycle = nk_slider_float(ctx, -10.0f, &osc->waveforms[osc->waveform_selected].duty_cycle, 10.0f, 1.0f);
                nk_tree_pop(ctx);
            }

			if( nk_tree_push( ctx, NK_TREE_TAB, "Cursor", NK_MINIMIZED) ){
                osc->cursor_selected = nk_combo(ctx, (const char*[]){"C1", "C2"}, CURSOR_COUNT, osc->cursor_selected, 20, nk_vec2(60, 200));
                nk_layout_row(ctx, NK_STATIC, 30, 2, (float[]){60, 60});
                osc->cursors[osc->cursor_selected].enabled = nk_combo(ctx, (const char*[]){"Off", "On"}, 2, osc->cursors[osc->cursor_selected].enabled, 20, nk_vec2(60, 200));
                osc->cursors[osc->cursor_selected].horizontal = nk_combo(ctx, (const char*[]){"Track", "Free"}, 2, osc->cursors[osc->cursor_selected].horizontal, 20, nk_vec2(60, 200));
                osc->cursors[osc->cursor_selected].offset = nk_slider_float(ctx, -10.0f, &osc->cursors[osc->cursor_selected].offset, 10.0f, 1.0f);
                osc->cursors[osc->cursor_selected].track = nk_slider_float(ctx, -10.0f, &osc->cursors[osc->cursor_selected].track, 10.0f, 1.0f);
                nk_tree_pop(ctx);
            }

			if( nk_tree_push( ctx, NK_TREE_TAB, "Measurements", NK_MINIMIZED) ){
                nk_label(ctx, "Todo", NK_TEXT_LEFT);
                nk_tree_pop(ctx);
            }

			if( nk_tree_push( ctx, NK_TREE_TAB, "Info", NK_MINIMIZED) ){
                nk_layout_row(ctx, NK_STATIC, 30, 2, (float[]){60, 60});

                float fps = 0.0f; // Calculate fps here
                nk_label(ctx, "FPS", NK_TEXT_LEFT);
                nk_label(ctx, "0.0000", NK_TEXT_LEFT); // Replace with actual fps

                float ratio = 0.0f; // Calculate memory ratio here
                nk_label(ctx, "Memory", NK_TEXT_LEFT);
                nk_label(ctx, "0.0000%", NK_TEXT_LEFT); // Replace with actual memory ratio

                nk_tree_pop(ctx);
            }
        }
        //nk_end(ctx);
    }
	nk_end(ctx);
}

#include "framebuf.h"
void test_framebuf()
{
	uint8_t fb_buf[480*64*2];
	tFramebuf fb;

	framebuf_init( &fb, 480, 64, fb_buf );
	framebuf_fill( &fb, 0x00000000 );
	framebuf_text( &fb, 10, 10, "Hello World", 0x003FFFF );

	lcd_config();
	lcd_bmp( 0, 0, 480, 64, fb_buf );

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_QUADSPI1_Init();
  MX_ADC1_Init();
  MX_DAC2_Init();
  MX_OPAMP1_Init();
  MX_DAC1_Init();
  MX_ADC3_Init();
  MX_ADC4_Init();
  MX_ADC5_Init();
  MX_OPAMP3_Init();
  MX_OPAMP5_Init();
  MX_OPAMP6_Init();
  MX_FMC_Init();
  MX_SPI3_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  //test_framebuf();
  //test_scope();

  float ax = 250/989.0;
  float bx = -15800/989.0;
  float ay = 250/1407.0;
  float by = -1150/67.0;
  uint16_t x, y;

#include "tsc2046.h"

  tTsc2046 tsc;
  tsc2046_init( &tsc, &hspi3, GPIOA, GPIO_PIN_15, ax, bx, ay, by);

  for( int qqq = 0 ; qqq < 100 ; qqq++ )
  {
	  tsc2046_read( &tsc, &x, &y );
	  //printf( "%d %d\n", x, y );
  }

  lcd_config();
  for( int qqq = 0 ; qqq < 16 ; qqq++ )
  {
	  lcd_rect( 10, qqq*20, 460, 5, 0x3FFFF );
  }

  lcd_rect( 50, 50, 1, 1, 0x3FFFF );
  lcd_rect( 300, 300, 1, 1, 0x0003F );
  for( int qqq = 0 ; qqq < 100 ; qqq++ )
  {
	  tsc2046_read( &tsc, &x, &y );
	  //printf( "%d %d\n", x, y );
  }

  lcd_rect( 0, 0, 480, 320, 0x0000 );

  static struct nk_context ctx;
  static struct nk_buffer cmds;
  static struct nk_buffer pool;
  static struct nk_user_font font;
  static uint8_t buf_cmds[1024*8] = {0};
  static uint8_t buf_pool[1024*8] = {0};

  nk_buffer_init_fixed( &cmds, buf_cmds, 1024*8 );
  nk_buffer_init_fixed( &pool, buf_pool, 1024*8 );

  font.height = 8;
  font.width = text_width_f;
  nk_init_custom( &ctx, &cmds, &pool, &font );
  int pressed = 1;
  int pressed_bck = 0;
  struct nk_colorf bg;
  bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;


	static uint8_t fb_buf[480*40*2];
	tFramebuf fb;

	framebuf_init( &fb, 480, 40, fb_buf );
	lcd_config();

	static struct Oscilloscope osc = {0};
  while( 1 )
  {

  tsc2046_read( &tsc, &x, &y );

  pressed_bck = pressed;
  pressed = x != 0;

  nk_input_begin( &ctx );
  nk_input_motion( &ctx, x, y );
  nk_input_button( &ctx, 0, x, y, x!=0 );
  nk_input_end( &ctx );

  oscilloscope_process(&osc, &ctx);

  if( (pressed || pressed_bck) && (pressed != pressed_bck) )
  {
	  ;
  }
  else
  {
	    //HAL_Delay( 1 );

	  //if( visible == 0 )
	  {
		  test_scope();
		  nk_clear(&ctx);
		  continue;
	  }
  }

  for( int y0 = 0 ; y0 < 320 ; y0 += 40 )
  {
framebuf_fill( &fb, 0x00000000 );

  //lcd_rect( 0, 0, 480, 320, 0x0000 );
  {
	  const struct nk_command *cmd = NULL;
	  nk_foreach(cmd, &ctx)
	  {
		  switch (cmd->type) {
		  case NK_COMMAND_NOP: break;
		  case NK_COMMAND_SCISSOR: {
			  const struct nk_command_scissor *s = (const struct nk_command_scissor*)cmd;
		  } break;
		  case NK_COMMAND_LINE: {
			  const struct nk_command_line *l = (const struct nk_command_line*)cmd;
			  if( l->begin.y == l->end.y )
			  {
				  framebuf_hline( &fb, l->begin.x, l->begin.y-y0, l->end.x-l->begin.x, nk_colot_to_rgb666( l->color ) );
			  }
			  else if( l->begin.x == l->end.x )
			  {
				  framebuf_vline( &fb, l->begin.x, l->begin.y-y0, l->end.y-l->begin.y, nk_colot_to_rgb666( l->color ) );
			  }
			  else
			  {
				  framebuf_line( &fb, l->begin.x, l->begin.y-y0, l->end.x, l->end.y-y0, nk_colot_to_rgb666( l->color ) );
			  }
		  } break;
		  case NK_COMMAND_RECT: {
			  const struct nk_command_rect *r = (const struct nk_command_rect*)cmd;
			  framebuf_rect( &fb, r->x, r->y-y0, r->w, r->h, nk_colot_to_rgb666( r->color ) );
		  } break;
		  case NK_COMMAND_RECT_FILLED: {
			  const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled*)cmd;
			  framebuf_fill_rect( &fb, r->x, r->y-y0, r->w, r->h, nk_colot_to_rgb666( r->color ) );
		  } break;
		  case NK_COMMAND_CIRCLE: {
			  const struct nk_command_circle *c = (const struct nk_command_circle*)cmd;
			  framebuf_circle( &fb, c->x+(c->w+c->h)/4, c->y-y0+(c->w+c->h)/4, (c->w+c->h)/4, nk_colot_to_rgb666( c->color ) );
		  } break;
		  case NK_COMMAND_CIRCLE_FILLED: {
			  const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
			  framebuf_fill_circle( &fb, c->x+(c->w+c->h)/4, c->y-y0+(c->w+c->h)/4, (c->w+c->h)/4, nk_colot_to_rgb666( c->color ) );
		  } break;
		  case NK_COMMAND_TEXT: {
			  const struct nk_command_text *t = (const struct nk_command_text*)cmd;
			  framebuf_text( &fb, t->x, t->y-y0, t->string, nk_colot_to_rgb666( t->foreground ) );
		  } break;
		  case NK_COMMAND_IMAGE: {
			  const struct nk_command_image *i = (const struct nk_command_image*)cmd;
			  framebuf_fill_rect( &fb, i->x, i->y-y0, i->w, i->h, 0x0003FFFF );
		  } break;
		  default: break;
		  }
	  }
  }

	lcd_bmp( 0, y0, 480, 40, fb_buf );
  }

  nk_clear(&ctx);

  test_scope();

  //HAL_Delay( 100 );
  }
  /*

TIM
AWD
DMA

modpsram
	read( addr, buf, len )
	write( addr, buf, len )

modfsmc
	read( addr, buf, len )
	write( addr, buf, len )

modscope
	config( channel, trigger_mode, trigger_edge, trigger_value )
	read( channel, buf, len )

modawg
	start()
	stop()
	write( channel, buf, len )

  */
  //scope_test();
  test_scope();
  //while( 1 );
  psram_test();

#define LCD_CMD_ADDR	(0x60000000)
#define LCD_DATA_ADDR	(0x60000002) // 0x60000001 ??
  uint16_t *addr_cmd = (uint16_t*) LCD_CMD_ADDR;
  uint16_t *addr_data = (uint16_t*) LCD_DATA_ADDR;
  uint8_t buf_cmd[1] = {0x11};
  uint16_t buf_data[4] = {0};
  uint8_t cmd = 1;
  uint16_t data = 1;
  *addr_cmd = cmd;
  *addr_data = 0x00FF;
  *addr_data = 0xFF00;
  for( int jj = 0 ; jj < 4 ; jj++ )
  {
	  *addr_cmd = cmd;
	  for( int ii = 0 ; ii < 4 ; ii++ )
	  {
		  *addr_data = ii;
	  }
  }
  HAL_GPIO_WritePin( GPIOA, GPIO_PIN_8, GPIO_PIN_SET );
  HAL_GPIO_WritePin( GPIOA, GPIO_PIN_9, GPIO_PIN_RESET );
  HAL_Delay( 10 );
  HAL_GPIO_WritePin( GPIOA, GPIO_PIN_9, GPIO_PIN_SET );
  HAL_Delay( 100 );

  *addr_cmd = 0x01;
  HAL_Delay( 100 );
  *addr_cmd = 0x11;
  HAL_Delay( 10 );
  *addr_cmd = 0x3A;
  *addr_data = 0x05;
  *addr_cmd = 0x36;
  *addr_data = (0x00<<5)|(0x01<<4)|(0x01<<7);
  *addr_cmd = 0x29;
  HAL_Delay( 10 );

  *addr_cmd = 0x2A;
  *addr_data = 0x00;
  *addr_data = 0x00;
  *addr_data = (480>>8)&0xFF;
  *addr_data = (480>>0)&0xFF;

  *addr_cmd = 0x2B;
  *addr_data = 0x00;
  *addr_data = 0x00;
  *addr_data = (320>>8)&0xFF;
  *addr_data = (320>>0)&0xFF;

  *addr_cmd = 0x2C;
  for( int ii = 0 ; ii < 320*480 ; ii++ )
  {
	  *addr_data = 0x0000;
  }
  uint16_t color = 0x001F;
  *addr_cmd = 0x2C;
  for( int ii = 0 ; ii < 320*480 ; ii++ )
  {
	  *addr_data = color;
  }



  //HAL_SRAM_Write_8b( &hsram1, addr_cmd, buf_cmd, 1 );
  //HAL_SRAM_Write_16b( &hsram1, addr_data, buf_data, 4 );

float vcc = 3.256;
HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED);
HAL_ADCEx_Calibration_Start(&hadc5, ADC_SINGLE_ENDED);
HAL_ADCEx_Calibration_Start(&hadc4, ADC_SINGLE_ENDED);

HAL_OPAMP_SelfCalibrate(&hopamp1);
HAL_OPAMP_SelfCalibrate(&hopamp3);
HAL_OPAMP_SelfCalibrate(&hopamp5);
HAL_OPAMP_SelfCalibrate(&hopamp6);

// Set the DAC1 OUT1 to 1.0V and OUT2 to 2.0V

// Measure VCC with the ADC internal channel.

// Configure the internal channel
//ADC_ChannelConfTypeDef sConfig = {0};
//sConfig.Channel = ADC_CHANNEL_VREFINT;
//sConfig.Rank = ADC_REGULAR_RANK_1;
//sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
//HAL_ADC_ConfigChannel(&hadc1, &sConfig);

// Start the ADC
//HAL_ADC_Start(&hadc1);

// Read the ADC value
//HAL_ADC_PollForConversion(&hadc1, 100);
//uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
//printf("ADC value: %lu\n", adc_value);

// Stop the ADC
//HAL_ADC_Stop(&hadc1);

// Configure ADC for OPAMP1 channel
//sConfig.Channel = ADC_CHANNEL_VOPAMP1;
//sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
//HAL_ADC_ConfigChannel(&hadc1, &sConfig);

  // Set DAC1 OUT 1 to 1.0 V
  // Set DAC1 OUT 2 to 2.0 V
  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 1.0/vcc*4095);
  HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);

  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 1.0/vcc*4095);
  HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);

    // Set the DAC2 OUT1 to 1.66V

  HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (vcc/2.0)/vcc*4095);
  HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);

  HAL_OPAMP_Start(&hopamp1);
  HAL_OPAMP_Start(&hopamp3);
  HAL_OPAMP_Start(&hopamp5);
  HAL_OPAMP_Start(&hopamp6);

  int i, j;
  for( i = 0 ; i < 30 ; i++ )
  {

	  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, i*0.1/vcc*4095);
     // Read the ADC1 ADC_CHANNEL_VOPAMP1 and print the value
      HAL_Delay( 10 );
      for( j = 0 ; j < 1 ; j++ )
      {
		  HAL_ADC_Start(&hadc1);
		  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		  uint32_t adc_value1 = HAL_ADC_GetValue(&hadc1);
		  HAL_ADC_Stop(&hadc1);

		  HAL_ADC_Start(&hadc3);
		  HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
		  uint32_t adc_value3 = HAL_ADC_GetValue(&hadc3);
		  HAL_ADC_Stop(&hadc3);

		  HAL_ADC_Start(&hadc5);
		  HAL_ADC_PollForConversion(&hadc5, HAL_MAX_DELAY);
		  uint32_t adc_value5 = HAL_ADC_GetValue(&hadc5);
		  HAL_ADC_Stop(&hadc5);

		  HAL_ADC_Start(&hadc4);
		  HAL_ADC_PollForConversion(&hadc4, HAL_MAX_DELAY);
		  uint32_t adc_value4 = HAL_ADC_GetValue(&hadc4);
		  HAL_ADC_Stop(&hadc4);

		 printf("%d, %f, %f, %f, %f, %f\n", i, i*0.1/vcc*4095*vcc/4095, adc_value1*vcc/4095, adc_value3*vcc/4095, adc_value5*vcc/4095, adc_value4*vcc/4095 );
  	  }
  }
  printf("\n");


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
	  HAL_Delay( 200 );
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
	  HAL_Delay( 100 );


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV6;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV8;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
