/*
 * task_ui.c
 *
 *  Created on: Aug 31, 2023
 *      Author: jgpei
 */

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "Lcd.h"
#include "framebuf.h"
#include "FontUbuntuBookRNormal16.h"
#include "ui.h"

#include "nuklear.h"

#define FB_WIDTH    (240)
#define FB_HEIGHT   (80)

#define NK_BUFFER_CMDS_LEN  (1024*6)
#define NK_BUFFER_POOL_LEN  (1024*6)

#define COLOR_UI_CROSS      (LCD_COLOR_WHITE)

uint8_t nk_buf_cmds[NK_BUFFER_CMDS_LEN] = {0};
uint8_t nk_buf_pool[NK_BUFFER_POOL_LEN] = {0};
uint16_t fb_buf[FB_WIDTH*FB_HEIGHT];

struct nk_context ctx = {0};
tUi ui = {0};
tLcd lcd = {0};

float text_width_f( nk_handle handle, float h, const char* text, int len )
{
    return font_text_width( &fontUbuntuBookRNormal16, text );
}

void lcd_draw_cross( tLcd *pLcd, uint16_t x, uint16_t y, uint16_t color )
{
    lcd_rect( pLcd, x-4, y, 4, 2, color );
    lcd_rect( pLcd, x+2, y, 4, 2, color );
    lcd_rect( pLcd, x, y-4, 2, 4, color );
    lcd_rect( pLcd, x, y+2, 2, 4, color );
}

/**
 * @brief UI Task for managing the user interface.
 *
 * This task initializes and manages the user interface (UI). It reads touch data
 * from the TSC2046 touch controller, updates the UI elements, and displays them
 * on the LCD screen.
 *
 * Pseudocode:
 *
 * Initialize LCD and UI components.
 * Initialize NK (Nuklear) GUI context.
 *
 * while(1) {
 *     Wait for a fixed time interval (xFrequency).
 *
 *     If touch data is available in the TSC UI queue:
 *         Update NK input with touch data.
 *
 *     Build the UI based on the NK context.
 *
 *     Draw the UI on the frame buffer.
 *     Display the frame buffer on the LCD screen.
 *
 *     If UI visibility changes:
 *         Send a message to the Scope and Wavegen tasks to update visibility.
 * }
 *
@startuml
start
:Initialize LCD and UI components;
:Initialize NK (Nuklear) GUI context;
while (true) is (true)
    :Wait for a fixed time interval (xFrequency);
    if (touch data is available in the TSC UI queue) then (yes)
        :Update NK input with touch data;
    endif
    :Build the UI based on the NK context
         Optionally send data to the 
         Scope or Wavegen queues;
    :Draw the UI on the frame buffer;
    :Display the frame buffer on the LCD screen;
    if (UI visibility changes) then (yes)
        :Send messages to update visibility in Scope and Wavegen tasks;
    endif
endwhile
@enduml
 * @param argument Task argument.
 */
void StartTaskUi(void *argument)
{
    tFramebuf fb = {0};

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 10;

    struct sQueueTscUi msgTscUi = {0};

    struct nk_buffer cmds = {0};
    struct nk_buffer pool = {0};
    struct nk_user_font font = {0};

    uint16_t x0 = 0, y0 = 0;

    // Initialize LCD and UI components
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

    xLastWakeTime = xTaskGetTickCount();
    for(;;)
    {
        vTaskDelayUntil( &xLastWakeTime, xFrequency );

        // Read touch data from the TSC UI queue
        if( osMessageQueueGet(queueTscUiHandle, &msgTscUi, NULL, portMAX_DELAY) == osOK )
        {
            msgTscUi.x -= lcd.width/2;
            nk_input_begin( &ctx );
            nk_input_motion( &ctx, msgTscUi.x, msgTscUi.y );
            nk_input_button( &ctx, NK_BUTTON_LEFT, msgTscUi.x, msgTscUi.y, msgTscUi.p );
            nk_input_end( &ctx );
        }

        // Build the UI based on the NK context
        ui_build( &ui, &ctx );

        // Draw the UI on the frame buffer and display it on the LCD screen
        x0 = 0;
        for( y0 = 0; y0 < lcd.height; y0 += fb.height)
        {
            framebuf_fill( &fb, 0x0000 );
            nk_draw_fb2( &ctx, &fb, x0, y0 );
            if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
            {
                lcd_bmp( &lcd, lcd.width/2, y0, fb.width, fb.height, fb.buf);
                osSemaphoreRelease( semaphoreLcdHandle );
            }
        }
        nk_clear(&ctx);

        // Handle UI visibility changes
        {
            tUi *pThis = &ui;
            if( pThis->is_visible != pThis->is_visible_bck )
            {
                if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
                {
                    lcd_clear( &lcd, LCD_COLOR_BLACK );
                    osSemaphoreRelease( semaphoreLcdHandle );
                }

                // Send messages to Scope and Wavegen tasks to update visibility
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
                struct sQueueUiScope msgUiScope = {0};
                msgUiScope.type = QUEUE_UI_SCOPE_TYPE_CHANGE_COLLAPSED;
                msgUiScope.data[0] = pThis->is_visible;
                osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);

                enum eQueueUiWavegenType
                {
                    QUEUE_UI_WAVEGEN_TYPE_START,
                    QUEUE_UI_WAVEGEN_TYPE_STOP,
                    QUEUE_UI_WAVEGEN_TYPE_CONFIG_HORIZONTAL,
                    QUEUE_UI_WAVEGEN_TYPE_CONFIG_VERTICAL,
                    QUEUE_UI_WAVEGEN_TYPE_CHANGE_VISIBILITY,
                    QUEUE_UI_WAVEGEN_TYPE_CHANGE_COLLAPSED
                };
                struct sQueueUiScope msgUiWavegen = {0};
                msgUiWavegen.type = QUEUE_UI_WAVEGEN_TYPE_CHANGE_COLLAPSED;
                msgUiWavegen.data[0] = pThis->is_visible;
                osMessageQueuePut(queueUiWavegenHandle, &msgUiWavegen, 0U, portMAX_DELAY);

                // Wait until msgUiScope and msgUiWavegen are processed
                while( osMessageQueueGetCount(queueUiScopeHandle) || osMessageQueueGetCount(queueUiWavegenHandle) )
                {
                    vTaskDelay( 1 );
                }
            }
            pThis->is_visible_bck = pThis->is_visible;
        }

        // Draw a crosshair if pressure is detected and within a certain range
        if( msgTscUi.p && (0 <= msgTscUi.x) && (msgTscUi.x < lcd.width/2-5) )
        {
            lcd_draw_cross( &lcd, msgTscUi.x+lcd.width/2, msgTscUi.y, COLOR_UI_CROSS );
        }
    }
}
