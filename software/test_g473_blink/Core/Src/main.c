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

#include "nuklear.h"
#include "framebuf.h"
#include "tsc2046.h"
#include "psram.h"

//#define NK_INCLUDE_STANDARD_VARARGS (1)
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

int32_t get_systick()
{
	return SysTick->VAL;
}

int32_t diff_systick( int32_t b, int32_t a )
{
	int d = 0;
	if( b < a )
	{
	  d = a-b;
	}
	else
	{
	  d = a+(SysTick->LOAD-b);
	}
	return d;
}
#include "FontUbuntuBookRNormal16.h"
struct sRectangle
{
	int16_t x;
	int16_t y;
	int16_t width;
	int16_t height;
};
typedef struct sRectangle tRectangle;
extern const tFont fontUbuntuBookRNormal16;
void draw_char( const tFramebuf *fb, const tFont *pFont, int16_t x0, int16_t y0, uint8_t c, uint32_t color );
tRectangle get_char_rect( const tFont *pFont, uint8_t c );
tRectangle get_text_rect( const tFont *pFont, char *pString );

float text_width_f( nk_handle handle, float h, const char* t, int len )
{

	return get_text_rect( &fontUbuntuBookRNormal16, t ).width;
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
	*addr_data = color_l;
	*addr_data = color_h;
}


int32_t lcd_set_pixel_bench( int16_t x, int16_t y, uint32_t color )
{
	int32_t a, b, d;
	int16_t x0 = x;
	int16_t y0 = y;
	int16_t x1 = x + 4;
	int16_t y1 = y + 4;
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

	a = get_systick();
	*addr_cmd = 0x2C;
	*addr_data = color_l;
	*addr_data = color_h;
	*addr_data = color_l;
	*addr_data = color_h;
	*addr_data = color_l;
	*addr_data = color_h;
	*addr_data = color_l;
	*addr_data = color_h;

	*addr_data = color_l;
	*addr_data = color_h;
	*addr_data = color_l;
	*addr_data = color_h;
	*addr_data = color_l;
	*addr_data = color_h;
	*addr_data = color_l;
	*addr_data = color_h;

	*addr_data = color_l;
	*addr_data = color_h;
	*addr_data = color_l;
	*addr_data = color_h;
	*addr_data = color_l;
	*addr_data = color_h;
	*addr_data = color_l;
	*addr_data = color_h;

	*addr_data = color_l;
	*addr_data = color_h;
	*addr_data = color_l;
	*addr_data = color_h;
	*addr_data = color_l;
	*addr_data = color_h;
	*addr_data = color_l;
	*addr_data = color_h;
	b = get_systick();
	d = diff_systick(b,a);
	return d;
}
void lcd_text( const tFramebuf *fb, uint16_t x0, uint16_t y0, char *str, uint32_t color )
{
	for(; *str; ++str) {
	        // get char and make sure its in range of font
	        int chr = *(uint8_t *)str;
	        if (chr < 32 || chr > 127) {
	            chr = 127;
	        }
	        draw_char( fb, &fontUbuntuBookRNormal16, x0, y0, chr, color );
	        x0 += get_char_rect( &fontUbuntuBookRNormal16, chr ).width;
	}
}
void draw_char( const tFramebuf *fb, const tFont *pFont, int16_t x0, int16_t y0, uint8_t c, uint32_t color )
{
    int16_t x, y, w, bitmap, b;
    int16_t px, py;
    const tGlyph *pGlyph;

    pGlyph = pFont->pGlyphs[c-32];

    y0 += pFont->bbxh;
    y0 -= pFont->descent;
    y0 -= pGlyph->bbxh;
    y0 -= pGlyph->bbxy;

    for( y = 0 ; y < pGlyph->bbxh ; y++ )
    {
        py = y0+y;

        w = (pGlyph->bbxw-1)/8+1;

        int16_t yw = y*w;
        for( x = 0 ; x < pGlyph->bbxw ; x+=8 )
        {
            bitmap = pGlyph->pBitmap[yw+x/8];

            for( b = 0 ; b < 8 ; b++ )
            {
                if( x+b >= pGlyph->bbxw )
                {
                    break;
                }

                px = x0+x+b;

                if( bitmap & (0x80>>b) )
                {
                    //set_pixel( px, py, color );
                    //lcd_rect( px, py, 1, 1, color);
                	setpixel_checked(fb, px, py, color, 1);
                }
            }
        }
    }
}


tRectangle get_char_rect( const tFont *pFont, uint8_t c )
{
    tRectangle rect = {0};

    rect.width = pFont->pGlyphs[c-32]->dwidthx;
    rect.height = pFont->bbxh;
    return rect;
}

tRectangle get_text_rect( const tFont *pFont, char *pString )
{
	uint8_t i;
	tRectangle rect = {0};

	rect.height = pFont->bbxh;
	for( i = 0 ; pString[i] != '\0' ; i++ )
	{
		rect.width += get_char_rect( pFont, pString[i] ).width;
    }

    return rect;
}

void __lcd_text( uint16_t x0, uint16_t y0, char *str, uint32_t color )
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
		*addr_data = *buf;
		buf++;
		*addr_data = *buf;
		buf++;
	}
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
            nk_tree_pop(ctx);
        }

        {
        	if( nk_tree_push( ctx, NK_TREE_TAB, "Horizontal", NK_MINIMIZED) ){
        		nk_layout_row(ctx, NK_STATIC, 30, 4, (float[]){60, 30, 60, 30});
        		nk_label( ctx, "Offset", NK_TEXT_ALIGN_LEFT );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT) )
        		{
        			osc->horizontal_offset -= 1;
        		}
        		char combo_buffer[32];
        		sprintf(combo_buffer, "%.2f", osc->horizontal_offset);
        		nk_label( ctx, combo_buffer, NK_TEXT_ALIGN_CENTERED );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT) )
        		{
        			osc->horizontal_offset += 1;
        		}

        		nk_label( ctx, "Scale", NK_TEXT_ALIGN_LEFT );
				nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
				if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT) )
				{
					osc->horizontal_scale -= 1;
				}
				//char combo_buffer[32];
				sprintf(combo_buffer, "%.2f", osc->horizontal_scale);
				nk_label( ctx, combo_buffer, NK_TEXT_ALIGN_CENTERED );
				nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
				if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT) )
				{
					osc->horizontal_scale += 1;
				}
                nk_tree_pop(ctx);
            }

        	if( nk_tree_push( ctx, NK_TREE_TAB, "Vertical", NK_MINIMIZED) ){
        		nk_layout_row(ctx, NK_STATIC, 30, 1, (float[]){100});
        		//nk_style_push_style_item(&ctx, &ctx->style.combo.button.text_background, nk_style_item_color(nk_rgb(255,0,0)));
        		osc->channel_selected = nk_combo(ctx, (const char*[]){"Ch1", "Ch2", "Ch3", "Ch4"}, CHANNEL_COUNT, osc->channel_selected, 20, nk_vec2(100, 100));
        		//nk_style_pop_style_item(&ctx);
        		nk_layout_row(ctx, NK_STATIC, 30, 2, (float[]){100, 100});
                osc->channels[osc->channel_selected].enabled = nk_combo(ctx, (const char*[]){"Off", "On"}, 2, osc->channels[osc->channel_selected].enabled, 20, nk_vec2(60, 200));
                osc->channels[osc->channel_selected].coupling = nk_combo(ctx, (const char*[]){"DC", "AC", "Gnd"}, 3, osc->channels[osc->channel_selected].coupling, 20, nk_vec2(60, 200));
                //osc->channels[osc->channel_selected].offset = nk_slider_float(ctx, -10.0f, &osc->channels[osc->channel_selected].offset, 10.0f, 1.0f);
                //osc->channels[osc->channel_selected].scale = nk_slider_float(ctx, -10.0f, &osc->channels[osc->channel_selected].scale, 10.0f, 1.0f);

                nk_layout_row(ctx, NK_STATIC, 30, 4, (float[]){60, 30, 60, 30});
        		nk_label( ctx, "Offset", NK_TEXT_ALIGN_LEFT );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT) )
        		{
        			osc->channels[osc->channel_selected].offset -= 1;
        		}
        		char combo_buffer[32];
        		sprintf(combo_buffer, "%.2f", osc->channels[osc->channel_selected].offset);
        		nk_label( ctx, combo_buffer, NK_TEXT_ALIGN_CENTERED );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT) )
        		{
        			osc->channels[osc->channel_selected].offset += 1;
        		}

        		nk_label( ctx, "Scale", NK_TEXT_ALIGN_LEFT );
				nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
				if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT) )
				{
					osc->channels[osc->channel_selected].scale -= 1;
				}
				//char combo_buffer[32];
				sprintf(combo_buffer, "%.2f", osc->channels[osc->channel_selected].scale);
				nk_label( ctx, combo_buffer, NK_TEXT_ALIGN_CENTERED );
				nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
				if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT) )
				{
					osc->channels[osc->channel_selected].scale += 1;
				}




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



int quadrant = 0x01;
void nk_draw_fb( struct nk_context *ctx, const tFramebuf *pfb )
{
	  for( int y0 = 0 ; y0 < 320 ; y0 += 40 )
	  {
	   framebuf_fill( pfb, 0x00000000 );

	  {
		  const struct nk_command *cmd = NULL;
		  nk_foreach(cmd, ctx)
		  {
			  //printf( "cmd->type = %d\n", cmd->type );
			  switch (cmd->type) {
			  case NK_COMMAND_NOP: break;
			  case NK_COMMAND_SCISSOR: {
				  const struct nk_command_scissor *s = (const struct nk_command_scissor*)cmd;
			  } break;
			  case NK_COMMAND_LINE: {
				  const struct nk_command_line *l = (const struct nk_command_line*)cmd;
				  if( l->begin.y == l->end.y )
				  {
					  framebuf_hline( pfb, l->begin.x, l->begin.y-y0, l->end.x-l->begin.x, nk_colot_to_rgb666( l->color ) );
				  }
				  else if( l->begin.x == l->end.x )
				  {
					  framebuf_vline( pfb, l->begin.x, l->begin.y-y0, l->end.y-l->begin.y, nk_colot_to_rgb666( l->color ) );
				  }
				  else
				  {
					  framebuf_line( pfb, l->begin.x, l->begin.y-y0, l->end.x, l->end.y-y0, nk_colot_to_rgb666( l->color ) );
				  }
			  } break;
			  case NK_COMMAND_RECT: {
				  const struct nk_command_rect *r = (const struct nk_command_rect*)cmd;
				  //printf( "NK_COMMAND_RECT x: %d, y: %d, width: %d, height: %d, rounding: %d, thickness: %d\n", r->x, r->y, r->w, r->h, r->rounding, r->line_thickness );
				  //framebuf_rect( pfb, r->x, r->y-y0, r->w, r->h, nk_colot_to_rgb666( r->color ) );
				  int rad = 4;//r->rounding;
                    framebuf_circle_quadrant( pfb, r->x+r->w-rad, r->y-y0+r->h-rad, rad, nk_colot_to_rgb666( r->color ), QUADRANT_0 );
                    framebuf_circle_quadrant( pfb, r->x+rad, r->y-y0+r->h-rad, rad, nk_colot_to_rgb666( r->color ), QUADRANT_90 );
                    framebuf_circle_quadrant( pfb, r->x+rad, r->y-y0+rad, rad, nk_colot_to_rgb666( r->color ), QUADRANT_180 );
                    framebuf_circle_quadrant( pfb, r->x+r->w-rad, r->y-y0+rad, rad, nk_colot_to_rgb666( r->color ), QUADRANT_270 );
                    framebuf_hline( pfb, r->x+rad, r->y-y0, r->w-rad-rad, nk_colot_to_rgb666( r->color ) );
                    framebuf_hline( pfb, r->x+rad, r->y-y0+r->h, r->w-rad-rad, nk_colot_to_rgb666( r->color ) );
                    framebuf_vline( pfb, r->x, r->y-y0+rad, r->h-rad-rad, nk_colot_to_rgb666( r->color ) );
                    framebuf_vline( pfb, r->x+r->w, r->y-y0+rad, r->h-rad-rad, nk_colot_to_rgb666( r->color ) );
			  } break;
			  case NK_COMMAND_RECT_FILLED: {
				  const struct nk_command_rect_filled *r = (const struct nk_command_rect_filled*)cmd;
				  //framebuf_fill_rect( pfb, r->x, r->y-y0, r->w, r->h, nk_colot_to_rgb666( r->color ) );
                  int rad = 4;//r->rounding;
                  struct nk_color r_color = r->color;
                  if( r_color.r == 50 )
                  {
                	  r_color.r = 40;
                	  r_color.g = 40;
                	  r_color.b = 40;
                	  r_color.a = 255;
                  }
                    framebuf_fill_circle_quadrant( pfb, r->x+r->w-rad, r->y-y0+r->h-rad, rad, nk_colot_to_rgb666( r_color ), QUADRANT_90 );
                    framebuf_fill_circle_quadrant( pfb, r->x+rad, r->y-y0+r->h-rad, rad, nk_colot_to_rgb666( r_color ), QUADRANT_90 );
                    framebuf_fill_circle_quadrant( pfb, r->x+rad, r->y-y0+rad, rad, nk_colot_to_rgb666( r_color ), QUADRANT_90 );
                    framebuf_fill_circle_quadrant( pfb, r->x+r->w-rad, r->y-y0+rad, rad, nk_colot_to_rgb666( r_color ), QUADRANT_90 );
                    // up and down
                    framebuf_fill_rect( pfb, r->x+rad, r->y-y0, r->w-rad-rad, r->h, nk_colot_to_rgb666( r_color ) );
                    // middle
                    framebuf_fill_rect(  pfb, r->x, r->y-y0+rad, r->w, r->h-rad-rad, nk_colot_to_rgb666( r_color ) );
			 } break;
			  case NK_COMMAND_CIRCLE: {
				  const struct nk_command_circle *c = (const struct nk_command_circle*)cmd;
				  framebuf_circle( pfb, c->x+(c->w+c->h)/4, c->y-y0+(c->w+c->h)/4, (c->w+c->h)/4, nk_colot_to_rgb666( c->color ) );
			  } break;
			  case NK_COMMAND_CIRCLE_FILLED: {
				  const struct nk_command_circle_filled *c = (const struct nk_command_circle_filled *)cmd;
				  framebuf_fill_circle( pfb, c->x+(c->w+c->h)/4, c->y-y0+(c->w+c->h)/4, (c->w+c->h)/4, nk_colot_to_rgb666( c->color ) );
			  } break;
			  case NK_COMMAND_TEXT: {
				  const struct nk_command_text *t = (const struct nk_command_text*)cmd;
				  //framebuf_text( pfb, t->x, t->y-y0, (char *)t->string, nk_colot_to_rgb666( t->foreground ) );
				  lcd_text( pfb,  t->x, t->y-y0, (char *)t->string, 0xFFFF );
			  } break;
			  case NK_COMMAND_IMAGE: {
				  const struct nk_command_image *i = (const struct nk_command_image*)cmd;
				  framebuf_fill_rect( pfb, i->x, i->y-y0, i->w, i->h, 0x0003FFFF );
			  } break;
			  case NK_COMMAND_TRIANGLE_FILLED: {
				  const struct nk_command_triangle_filled *t = (const struct nk_command_triangle_filled*)cmd;
				  struct nk_color t_color = t->color;
				if( 1 )
				{
				  t_color.r = 175;
				  t_color.g = 175;
				  t_color.b = 175;
				  t_color.a = 255;
				}

				  framebuf_line( pfb, t->a.x, t->a.y-y0, t->b.x, t->b.y-y0, 0xFFFF );
				  framebuf_line( pfb, t->b.x, t->b.y-y0, t->c.x, t->c.y-y0, 0xFFFF );
				  framebuf_line( pfb, t->c.x, t->c.y-y0, t->a.x, t->a.y-y0, 0xFFFF );
			  } break;
			  default: break;
			  }
		  }
	  }

		lcd_bmp( 0, y0, 480, 40, pfb->buf );
	  }
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

  //float ax = 250/989.0;
  //float bx = -15800/989.0;
  //float ay = 250/1407.0;
  //float by = -1150/67.0;

  //solve([50=a*250+b,480-50=a*1760+b],[a,b])
  float ax = 38.0/151.0;
  float bx = -1950.0/151.0;
  //solve([50=a*387+b,320-50=a*1627+b],[a,b])
  float ay = 11.0/62.0;
  float by = -1157.0/62.0;

  uint16_t x = 0, y = 0;
  uint16_t x_bck = 0, y_bck = 0;



  tTsc2046 tsc;
  tsc2046_init( &tsc, &hspi3, GPIOA, GPIO_PIN_15, ax, bx, ay, by, 32 );

  lcd_config();

  lcd_rect( 50, 50, 2, 2, 0xFFFF );
  lcd_rect( 480-50, 50, 2, 2, 0xFFFF );
  lcd_rect( 50, 320-50, 2, 2, 0xFFFF );
  lcd_rect( 480-50, 320-50, 2, 2, 0xFFFF );

  while( 0 )
  {
	  static int cnt = 0;
	  tsc.avg = 1;
	  uint16_t x = 0, y = 0;
	  tsc2046_read( &tsc, &x, &y );
	  lcd_rect( x, y, 2, 2, 0xFFFF );
	  printf("%d, %d, %d\n", cnt++, x, y );
	  HAL_Delay( 1 );
  }

  int d = 0;
  d = lcd_set_pixel_bench( 100, 100, 0xFFFF );
  d = d+1;
  static struct nk_context ctx;
  static struct nk_buffer cmds;
  static struct nk_buffer pool;
  static struct nk_user_font font;
  static uint8_t buf_cmds[1024*8] = {0};
  static uint8_t buf_pool[1024*8] = {0};

  nk_buffer_init_fixed( &cmds, buf_cmds, 1024*8 );
  nk_buffer_init_fixed( &pool, buf_pool, 1024*8 );

  font.height = fontUbuntuBookRNormal16.bbxh;
  font.width = text_width_f;
  nk_init_custom( &ctx, &cmds, &pool, &font );

	static uint8_t fb_buf[480*40*2];
	tFramebuf fb;

	framebuf_init( &fb, 480, 40, fb_buf );

	  int pressed = 1;
	  int pressed_bck = 0;
	  int pressed_bck2 = 0;
	int pressed_cnt = 0;
	static struct Oscilloscope osc = {0};

  while( 1 )
  {
	  x_bck = x;
	  y_bck = y;
	  tsc2046_read( &tsc, &x, &y );

	  pressed_bck2 = pressed_bck;
	  pressed_bck = pressed;
	  pressed = (x!=0);

	  if( (pressed || pressed_bck) )// && (pressed != pressed_bck) )
	  {
		  //printf("%d, %d, %d, %d, %d, %d, %d,\n", pressed_cnt, pressed, pressed_bck, x, y, x_bck, y_bck );
		  pressed_cnt++;
		  nk_input_begin( &ctx );
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

		  oscilloscope_process(&osc, &ctx);
		  nk_draw_fb( &ctx, &fb );
		  nk_clear(&ctx);
		  //HAL_Delay(10);

		  lcd_rect( x, y, 2, 2, 0xFFFF );
	  }
	  else if( nk_window_is_collapsed( &ctx, "STM32G4 Scope" ) )
	  {
		  test_scope();
	  }


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

  psram_test();

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
