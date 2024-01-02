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

#define NK_BUFFER_CMDS_LEN  (1024*4)
#define NK_BUFFER_POOL_LEN  (1024*4)

#define COLOR_UI_CROSS      (LCD_COLOR_WHITE)

uint8_t nk_buf_cmds[NK_BUFFER_CMDS_LEN] = {0};
uint8_t nk_buf_pool[NK_BUFFER_POOL_LEN] = {0};
uint16_t fb_buf[FB_WIDTH*FB_HEIGHT];

struct nk_context ctx = {0};
tUi ui = {0};
tLcd lcd = {0};
tFramebuf fb = {0};

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
NK_API void nk_input_gamepad_button(struct nk_context *ctx, int button, int down) {
    NK_ASSERT(ctx);
    if (!ctx) return;
    ctx->input.gamepad.keys[button] = down;
}

NK_API nk_bool nk_input_is_gamepad_button_pressed(const struct nk_input *i, int button) {
    if (!i) return nk_false;
    return (i->gamepad.keys[button] && !i->gamepad_prev.keys[button]);
}

NK_API nk_bool nk_input_is_gamepad_button_released(const struct nk_input *i, int button) {
    if (!i) return nk_false;
    return (!i->gamepad.keys[button] && i->gamepad_prev.keys[button]);
}
#include "nuklear.h"
/*
// Define gamepad button mapping
#define GAMEPAD_UP    0
#define GAMEPAD_DOWN  1
#define GAMEPAD_LEFT  2
#define GAMEPAD_RIGHT 3

NK_API void nk_input_gamepad_navigation(struct nk_context *ctx) {
    if (!ctx) return;

    struct nk_input *in = &ctx->input;

    // Handle gamepad input
    if (in->gamepad.keys[GAMEPAD_UP]) {
        // Implement behavior for the UP button press
        // Navigate to the previous widget
    } else if (in->gamepad.keys[GAMEPAD_DOWN]) {
        // Implement behavior for the DOWN button press
        // Navigate to the next widget
    } else if (in->gamepad.keys[GAMEPAD_LEFT]) {
        // Implement behavior for the LEFT button press
        // Navigate to the previous widget or perform some other action
    } else if (in->gamepad.keys[GAMEPAD_RIGHT]) {
        // Implement behavior for the RIGHT button press
        // Navigate to the next widget or perform some other action
    }

    // Check if there is an active window
    if (ctx->active) {
        struct nk_window *win = ctx->active;

        // You can use win->layout and win->widgets to access the layout and widgets
        // within the currently active window for navigation
        // For example, you can use a counter to keep track of the focused widget index
        int focused_widget_index = 0;

        // Iterate through the widgets in the layout
        struct nk_rect *layout = &win->layout;
        struct nk_widget *widget = win->widgets;
        while (widget) {
            if (focused_widget_index == 0) {
                // This widget is currently in focus, implement your logic here
                // For example, set the widget state to hovered or pressed
                nk_widget_state_reset(&widget->state);
                widget->state |= NK_WIDGET_STATE_HOVERED;

                // Handle gamepad input to interact with this widget
                if (in->gamepad.keys[0]) {
                    // Implement the behavior for the gamepad button press
                    // For example, activate the widget or perform some action
                }
            } else {
                // This widget is not in focus, reset its state
                nk_widget_state_reset(&widget->state);
            }

            // Increment the focused widget index
            focused_widget_index++;

            // Move to the next widget
            widget = widget->next;
        }
    }
}
*/


#include <stdlib.h>
typedef struct {
    float x;
    float y;
    float z;
} Vector3D;

typedef struct {
    Vector3D vertices[8];
} Cube;

void draw_line(tLcd *pThis, int x0, int y0, int x1, int y1, uint16_t color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx, sy, err, e2;

    if (x0 < x1) {
        sx = 1;
    } else {
        sx = -1;
    }

    if (y0 < y1) {
        sy = 1;
    } else {
        sy = -1;
    }

    err = dx - dy;

    while (1) {
        lcd_set_pixel(pThis, x0, y0, color);

        if (x0 == x1 && y0 == y1) {
            break;
        }

        e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void draw_cube(tLcd *pThis, Cube cube, uint16_t color) {
    // Draw the edges of the cube
    for (int i = 0; i < 4; ++i) {
        int j = (i + 1) % 2;
        draw_line(pThis, cube.vertices[i].x, cube.vertices[i].y, cube.vertices[i + 4].x, cube.vertices[i + 4].y, color);
        draw_line(pThis, cube.vertices[j].x, cube.vertices[j].y, cube.vertices[j + 4].x, cube.vertices[j + 4].y, color);
        draw_line(pThis, cube.vertices[i].x, cube.vertices[i].y, cube.vertices[j].x, cube.vertices[j].y, color);
    }
}

typedef struct {
    Cube cube;
    float angleX;
    float angleY;
} CubeProperties;
void update_cube_rotation(CubeProperties *cubeProps, float delta_time) {
    // Rotate the cube around the X and Y axes at a constant speed
    float rotation_speed = 0.5;  // Adjust the speed as needed
    cubeProps->angleX += rotation_speed * delta_time;
    cubeProps->angleY += rotation_speed * delta_time;
}


int main_cube( tLcd *pLcd ) {
    // Initialize your LCD display and set up necessary parameters

    CubeProperties cubeProps;
    cubeProps.cube = (Cube){
        .vertices = {
            {50, 50, 50},
            {150, 50, 50},
            {150, 150, 50},
            {50, 150, 50},
            {50, 50, 150},
            {150, 50, 150},
            {150, 150, 150},
            {50, 150, 150},
        }
    };
    cubeProps.angleX = 0.0;
    cubeProps.angleY = 0.0;

    // Timing variables for animation
    //clock_t prev_time = clock();
    float frame_time = 0.0;

    while (1) {
        // Calculate delta time since the last frame
        //clock_t current_time = clock();
        //frame_time = (float)(current_time - prev_time) / CLOCKS_PER_SEC;
        //prev_time = current_time;

        // Clear the screen or previous frame

        // Update cube rotation
        update_cube_rotation(&cubeProps, frame_time);
        frame_time += 1/100;

        // Draw the rotated cube on the screen
        lcd_clear( pLcd, 0x0000 );
        draw_cube( pLcd, cubeProps.cube, 0xFFFF);  // Pass your LCD object instead of NULL
        HAL_Delay( 10 );

        // Update the screen or LCD buffer

        // Add a delay or synchronization mechanism to control the frame rate
        // For example, you can use a delay function to limit the frame rate
        // Delay(16);  // 60 FPS (adjust as needed)
    }

    // Close your LCD display

    return 0;
}
#define RECTS_LEN 64
struct nk_rect rects[RECTS_LEN];
int rects_max = 0;
int rects_ptr = 0;
int rects_pressed = 0;
void StartTaskUi(void *argument)
{
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
    nk_set_theme(&ctx, THEME_DARK);

    ui_init( &ui );

    xLastWakeTime = xTaskGetTickCount();

    extern int cnt_hovering;
    extern int cnt_click;
    extern int cnt_target;
    extern int cnt_on;
    volatile int cnt_target2 = 0xFFFF;
    cnt_target = 0xFFFF;

    int cnt_hovering_max = 0;
    int cnt_hovering_bck = 0;

    int first_pass = 1;

    for(;;)
    {
        vTaskDelayUntil( &xLastWakeTime, xFrequency );

        //nk_input_gamepad_navigation(&ctx);
        // Read touch data from the TSC UI queue
        if( !first_pass && osMessageQueueGet(queueTscUiHandle, &msgTscUi, NULL, portMAX_DELAY) == osOK )
        {

            //if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
            {
            	//	lcd_clear( &lcd, LCD_COLOR_BLACK );
            	//    osSemaphoreRelease( semaphoreLcdHandle );
            }

            msgTscUi.x -= lcd.width/2;
            if( (int16_t)msgTscUi.x < 0 )
            {
            	msgTscUi.x = 0;
            }
            nk_input_begin( &ctx );

            /*if( gamepad == 1 )
            {
            	nk_input_gamepad_button( &ctx, gamepad_btn, gamepad_down );
            }*/
            if( msgTscUi.p )
            {
            	cnt_target = cnt_target2;
            }



            nk_input_motion( &ctx, msgTscUi.x, msgTscUi.y );
            nk_input_button( &ctx, NK_BUTTON_LEFT, msgTscUi.x, msgTscUi.y, msgTscUi.p );
            nk_input_end( &ctx );
        }

        if( ui.cursors.is_visible ){
        	ui_erase_cursors( &ui.cursors, &lcd, ui.is_visible );
        }

        // Build the UI based on the NK context
        cnt_hovering = 0;
        cnt_click = 0;
        rects_max = 0;
        rects_pressed = 0;
        ui_build( &ui, &ctx );
        //rects_max_bck = rects_max;
        cnt_hovering_max = cnt_hovering;
        //printf( "%d, %d, %d, %d,\n", cnt_on, cnt_hovering, cnt_hovering, cnt_target );
        printf( "\n" );
        if( cnt_hovering != cnt_hovering_bck )
        {
        	int dt = cnt_hovering-cnt_hovering_bck;
        	if( dt > 0 )
        	{
        		cnt_target = cnt_target;
        	}
        	else
        	{
        		cnt_target = cnt_target;
        	}
        }
        cnt_hovering_bck = cnt_hovering;
        if( ui.cursors.is_visible ){
        	ui_draw_cursors( &ui.cursors, &lcd, ui.is_visible );
        }

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


		struct nk_rect *bounds;
		bounds = &rects[rects_ptr];
		//extern tLcd lcd;
	    if( osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
	    {
	    	if( 0 <= (int)bounds->y && (int)bounds->y < 320 && 0 <= (int)bounds->y+(int)bounds->h && (int)bounds->y+(int)bounds->h < 320 )
	    	{
				lcd_hline( &lcd, 240+(int)bounds->x, (int)bounds->y, (int)bounds->w, LCD_COLOR_GREEN );
				lcd_hline( &lcd, 240+(int)bounds->x, (int)bounds->y+(int)bounds->h-1, (int)bounds->w, LCD_COLOR_GREEN );
				lcd_vline( &lcd, 240+(int)bounds->x, (int)bounds->y, (int)bounds->h, LCD_COLOR_GREEN );
				lcd_vline( &lcd, 240+(int)bounds->x+(int)bounds->w-1, (int)bounds->y, (int)bounds->h, LCD_COLOR_GREEN );
				//uint8_t buffer[32];
				//snprintf( buffer, sizeof(buffer), "%d", rects_ptr );
				//font_draw_text( &fontUbuntuBookRNormal16, 240+(int)bounds->x, (int)bounds->y, buffer, LCD_COLOR_GREEN, lcd_set_pixel, &lcd );
				//if( rects_ptr == rects_max )
				//{
				//	lcd_rect( &lcd, 240+(int)bounds->x, (int)bounds->y, 5, 5, LCD_COLOR_GREEN );
				//}

				//rects[rects_max] = *bounds;
				//rects_max += 1;
	    	}
	        osSemaphoreRelease( semaphoreLcdHandle );
	    }



        // Handle UI visibility changes
        {
            tUi *pThis = &ui;
            if( pThis->is_visible != pThis->is_visible_bck )
            {
                if( !first_pass && osSemaphoreAcquire( semaphoreLcdHandle, portMAX_DELAY ) == osOK )
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
        if( msgTscUi.p && (4 <= msgTscUi.x) && (msgTscUi.x < lcd.width/2-5) )
        {
            lcd_draw_cross( &lcd, msgTscUi.x+lcd.width/2, msgTscUi.y, COLOR_UI_CROSS );
        }

        first_pass = 0;
    }
}
