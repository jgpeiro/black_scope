/*
 * task_ui.c
 *
 *  Created on: Sep 18, 2023
 *      Author: jgpei
 */


#include "cmsis_os.h"

#include "main.h"
#include "nuklear.h"
#include "framebuf.h"
#include "FontUbuntuBookRNormal16.h"
#include "Lcd.h"
#include "spi.h"
#include "tsc.h"
#include "ui.h"

extern void nk_draw_fb(struct nk_context *ctx, tFramebuf *pfb, int16_t x0, int16_t y0 );

extern osMessageQueueId_t queueTscUiHandle;
extern osSemaphoreId_t semSpiHandle;

#define FB_WIDTH    (320)
#define FB_HEIGHT   (40)

#define NK_BUFFER_CMDS_LEN  (1024*4)
#define NK_BUFFER_POOL_LEN  (1024*4)

#define COLOR_UI_CROSS      (LCD_COLOR_WHITE)

uint8_t nk_buf_cmds[NK_BUFFER_CMDS_LEN] = {0};
uint8_t nk_buf_pool[NK_BUFFER_POOL_LEN] = {0};
uint16_t fb_buf[FB_WIDTH*FB_HEIGHT];

struct nk_context ctx = {0};
tLcd lcd = {0};
tFramebuf fb = {0};

float text_width_f( nk_handle handle, float h, const char* text, int len )
{
    return font_text_width( &fontUbuntuBookRNormal16, text );
}

void StartTaskUi(void *argument)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 50;

    struct sQueueTscUi msgTscUi = {0};

    struct nk_buffer cmds = {0};
    struct nk_buffer pool = {0};
    struct nk_user_font font = {0};

    uint16_t x0 = 0;
    uint16_t y0 = 0;

    // Initialize LCD and UI components
    //if( osSemaphoreAcquire( semSpiHandle, portMAX_DELAY ) == osOK )
    {
        lcd_init( &lcd,
        	&hspi1,
			LCD_CS_GPIO_Port, LCD_CS_Pin,
			LCD_DC_GPIO_Port, LCD_DC_Pin,
        	LCD_BL_GPIO_Port, LCD_BL_Pin,
			LCD_WIDTH, LCD_HEIGHT
        );
        osSemaphoreRelease( semSpiHandle );
    }

	if( osSemaphoreAcquire( semSpiHandle, portMAX_DELAY ) == osOK )
	{
		lcd_rect( &lcd,     50,     50, 5, 5, 0xFFFF);
		lcd_rect( &lcd,     50, 240-50, 5, 5, 0xFFFF);
		lcd_rect( &lcd, 320-50, 240-50, 5, 5, 0xFFFF);
		lcd_rect( &lcd, 320-50,     50, 5, 5, 0xFFFF);

		lcd_rect( &lcd, 320/2, 240/2, 5, 5, 0x001F);

		osSemaphoreRelease( semSpiHandle );
	}

    framebuf_init( &fb, FB_WIDTH, FB_HEIGHT, fb_buf );

    nk_buffer_init_fixed( &cmds, nk_buf_cmds, NK_BUFFER_CMDS_LEN );
    nk_buffer_init_fixed( &pool, nk_buf_pool, NK_BUFFER_POOL_LEN);

    font.height = fontUbuntuBookRNormal16.bbxh;
    font.width = text_width_f;
    nk_init_custom( &ctx, &cmds, &pool, &font );
    nk_set_theme(&ctx, THEME_DARK);

    xLastWakeTime = xTaskGetTickCount();
	/* Infinite loop */
	for(;;)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );

		// Read touch data from the TSC UI queue
		if( osMessageQueueGet(queueTscUiHandle, &msgTscUi, NULL, portMAX_DELAY) == osOK )
		{
			nk_input_begin( &ctx );
			nk_input_motion( &ctx, msgTscUi.x, msgTscUi.y );
			nk_input_button( &ctx, NK_BUTTON_LEFT, msgTscUi.x, msgTscUi.y, msgTscUi.p );
			nk_input_end( &ctx );
		}

		ui_build( &ctx );

        for( x0 = 0; x0 < lcd.width; x0 += fb.width)
        {
			for( y0 = 0; y0 < lcd.height; y0 += fb.height)
			{
				framebuf_fill( &fb, LCD_COLOR_BLACK );
				nk_draw_fb( &ctx, &fb, x0, y0 );
				if( osSemaphoreAcquire( semSpiHandle, portMAX_DELAY ) == osOK )
				{
					lcd_bmp( &lcd, x0, y0, fb.width, fb.height, fb.buf);
					osSemaphoreRelease( semSpiHandle );
				}
			}
        }
        nk_clear(&ctx);
	}
}
