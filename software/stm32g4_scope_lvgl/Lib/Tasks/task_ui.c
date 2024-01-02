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
//#include "framebuf.h"
//#include "FontUbuntuBookRNormal16.h"
//#include "ui.h"

//#include "nuklear.h"

#define FB_WIDTH    (240)
#define FB_HEIGHT   (80)

//#define NK_BUFFER_CMDS_LEN  (1024*4)
//#define NK_BUFFER_POOL_LEN  (1024*4)

#define COLOR_UI_CROSS      (LCD_COLOR_WHITE)

//uint8_t nk_buf_cmds[NK_BUFFER_CMDS_LEN] = {0};
//uint8_t nk_buf_pool[NK_BUFFER_POOL_LEN] = {0};
//uint16_t fb_buf[FB_WIDTH*FB_HEIGHT];

//struct nk_context ctx = {0};
//tUi ui = {0};
tLcd lcd = {0};
//tFramebuf fb = {0};

//float text_width_f( nk_handle handle, float h, const char* text, int len )
//{
//   return font_text_width( &fontUbuntuBookRNormal16, text );
//}

//void lcd_draw_cross( tLcd *pLcd, uint16_t x, uint16_t y, uint16_t color )
//{
//    lcd_rect( pLcd, x-4, y, 4, 2, color );
//    lcd_rect( pLcd, x+2, y, 4, 2, color );
//    lcd_rect( pLcd, x, y-4, 2, 4, color );
//    lcd_rect( pLcd, x, y+2, 2, 4, color );
//}

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
//NK_API void nk_input_gamepad_button(struct nk_context *ctx, int button, int down) {
//    NK_ASSERT(ctx);
//    if (!ctx) return;
//    ctx->input.gamepad.keys[button] = down;
//}

//NK_API nk_bool nk_input_is_gamepad_button_pressed(const struct nk_input *i, int button) {
//    if (!i) return nk_false;
//    return (i->gamepad.keys[button] && !i->gamepad_prev.keys[button]);
//}

//NK_API nk_bool nk_input_is_gamepad_button_released(const struct nk_input *i, int button) {
//    if (!i) return nk_false;
//    return (!i->gamepad.keys[button] && i->gamepad_prev.keys[button]);
//}
//#include "nuklear.h"
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



void StartTaskUi(void *argument)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 10;

    //struct sQueueTscUi msgTscUi = {0};

    for(;;)
    {
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}
