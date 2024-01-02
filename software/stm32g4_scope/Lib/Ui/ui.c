/*
 * ui.c
 *
 *  Created on: Aug 1, 2023
 *      Author: jgpei
 */

#include <stdio.h>
#include "ui.h"


void nk_style_push_color_button( struct nk_context *pCtx, uint8_t enabled )
{
    if( enabled )
    {
        nk_style_push_color( pCtx, &pCtx->style.button.normal.data.color, COLOR_BUTTON_ENABLED );
        nk_style_push_color( pCtx, &pCtx->style.button.hover.data.color, COLOR_BUTTON_ENABLED );
        nk_style_push_color( pCtx, &pCtx->style.button.active.data.color, COLOR_BUTTON_ENABLED );
    }
    else
    {
        nk_style_push_color( pCtx, &pCtx->style.button.normal.data.color, COLOR_BUTTON_DISABLED );
        nk_style_push_color( pCtx, &pCtx->style.button.hover.data.color, COLOR_BUTTON_DISABLED );
        nk_style_push_color( pCtx, &pCtx->style.button.active.data.color, COLOR_BUTTON_DISABLED );
    }
}

void nk_style_pop_color_button( struct nk_context *pCtx )
{
    nk_style_pop_color( pCtx );
    nk_style_pop_color( pCtx );
    nk_style_pop_color( pCtx );
}

void nk_style_push_color_channel( struct nk_context *pCtx, uint8_t channel_selected )
{
    if( channel_selected == 0 )
    {
        nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, COLOR_CHANNEL1 );
        nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, COLOR_CHANNEL1 );
        nk_style_push_color( pCtx, &pCtx->style.combo.label_active, COLOR_CHANNEL1 );
    }
    else if( channel_selected == 1 )
    {
        nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, COLOR_CHANNEL2 );
        nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, COLOR_CHANNEL2 );
        nk_style_push_color( pCtx, &pCtx->style.combo.label_active, COLOR_CHANNEL2 );
    }
    else if( channel_selected == 2 )
    {
        nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, COLOR_CHANNEL3 );
        nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, COLOR_CHANNEL3 );
        nk_style_push_color( pCtx, &pCtx->style.combo.label_active, COLOR_CHANNEL3 );
    }
    else if( channel_selected == 3 )
    {
        nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, COLOR_CHANNEL4 );
        nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, COLOR_CHANNEL4 );
        nk_style_push_color( pCtx, &pCtx->style.combo.label_active, COLOR_CHANNEL4 );
    }
}

void nk_style_push_color_wavegen_channel( struct nk_context *pCtx, uint8_t channel_selected )
{
    if( channel_selected == 0 )
    {
        nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, COLOR_WAVEGEN_CHANNEL1 );
        nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, COLOR_WAVEGEN_CHANNEL1 );
        nk_style_push_color( pCtx, &pCtx->style.combo.label_active, COLOR_WAVEGEN_CHANNEL1 );
    }
    else if( channel_selected == 1 )
    {
        nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, COLOR_WAVEGEN_CHANNEL2 );
        nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, COLOR_WAVEGEN_CHANNEL2 );
        nk_style_push_color( pCtx, &pCtx->style.combo.label_active, COLOR_WAVEGEN_CHANNEL2 );
    }
}


void nk_style_pop_color_channel( struct nk_context *pCtx )
{
    nk_style_pop_color( pCtx );
    nk_style_pop_color( pCtx );
    nk_style_pop_color( pCtx );
}

void ui_init( tUi *pThis )
{
	//pThis = (tUi){0};
	pThis->acquire.run = 0;
	pThis->acquire.single = 0;

	pThis->horizontal.offset = 0;
	pThis->horizontal.scale = 100;

	pThis->vertical.offset = 2048;
	pThis->vertical.channels[0].enabled = 1;
	pThis->vertical.channels[0].coupling = 0;
	pThis->vertical.channels[0].gain = 0;
	pThis->vertical.channels[0].scale = 80;
	pThis->vertical.channels[0].offset = 192;
	pThis->vertical.channels[1].enabled = 0;
	pThis->vertical.channels[1].coupling = 0;
	pThis->vertical.channels[1].gain = 0;
	pThis->vertical.channels[1].scale = 80;
	pThis->vertical.channels[1].offset = 192;
	pThis->vertical.channels[2].enabled = 0;
	pThis->vertical.channels[2].coupling = 0;
	pThis->vertical.channels[2].gain = 0;
	pThis->vertical.channels[2].scale = 80;
	pThis->vertical.channels[2].offset = 192;
	pThis->vertical.channels[3].enabled = 0;
	pThis->vertical.channels[3].coupling = 0;
	pThis->vertical.channels[3].gain = 0;
	pThis->vertical.channels[3].scale = 80;
	pThis->vertical.channels[3].offset = 192;

	pThis->trigger.level = 3096;
	pThis->trigger.mode = UI_TRIGGER_MODE_AUTO;
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

	pThis->is_visible = 1;
	pThis->is_visible_bck = 0;
}

int nk_progress_gross_fine(struct nk_context *pCtx, int32_t min, int32_t *pValue, int32_t max) {
    struct nk_style style = pCtx->style;

    int32_t gross_scale = max / 256;   // Scale for gross values
    int32_t fine_scale = 256;           // Scale for fine values

    int32_t gross_value = *pValue / gross_scale;  // Gross value
    int32_t fine_value = (*pValue % gross_scale) * fine_scale; // Fine value

    nk_layout_row_dynamic(pCtx, 30, 1);
    nk_progress(pCtx, &gross_value, max / gross_scale, 1);

    // Create a horizontal layout for fine adjustment
    //nk_layout_row_dynamic(pCtx, 30, 2);
    //nk_label(pCtx, "Fine:", NK_TEXT_LEFT);

    // Create fine progress bar (using 0-255 range)
    nk_progress(pCtx, &fine_value, fine_scale - 1, 1);
    *pValue = gross_value * gross_scale + (fine_value / fine_scale); // Combine gross and fine values


    return *pValue; // Return the updated value
}
uint8_t nk_keypad( struct nk_context *pCtx, int32_t min, int32_t *pValue, int32_t max )
{
    int value = *pValue;
	int retval = 0;
	char buffer[32];

//	if (nk_popup_begin( pCtx, NK_POPUP_STATIC, "Keypad", NK_WINDOW_CLOSABLE | NK_WINDOW_NO_SCROLLBAR, (struct nk_rect){0, -30, 108, 208} ) )
	if (nk_popup_begin( pCtx, NK_POPUP_STATIC, "Keypad", NK_WINDOW_CLOSABLE | NK_WINDOW_NO_SCROLLBAR, (struct nk_rect){0, -30, 108, 208+30+30+8} ) )
    {
        retval = 1;
    	nk_layout_row( pCtx, NK_STATIC, 30, 1, (float[]){30+30+30});

		snprintf( buffer, sizeof( buffer), "%d", value );
    	nk_label( pCtx, buffer, NK_TEXT_RIGHT );

    	nk_layout_row( pCtx, NK_STATIC, 30, 3, (float[]){30, 30, 30});
        if( nk_button_label( pCtx, "7" ) )	value = value*10 + 7;
        if( nk_button_label( pCtx, "8" ) )	value = value*10 + 8;
        if( nk_button_label( pCtx, "9" ) )	value = value*10 + 9;
        if( nk_button_label( pCtx, "4" ) )	value = value*10 + 4;
        if( nk_button_label( pCtx, "5" ) )	value = value*10 + 5;
        if( nk_button_label( pCtx, "6" ) )	value = value*10 + 6;
        if( nk_button_label( pCtx, "1" ) )	value = value*10 + 1;
        if( nk_button_label( pCtx, "2" ) )	value = value*10 + 2;
        if( nk_button_label( pCtx, "3" ) )	value = value*10 + 3;
        if( nk_button_label( pCtx, "C" ) )	value = value/10;
        if( nk_button_label( pCtx, "0" ) )	value = value*10 + 0;
        if( nk_button_label( pCtx, "-" ) )	value =-value;

        if( value > max )
        {
        	value = max;
        }
        if( value < min )
        {
        	value = min;
        }

        nk_layout_row( pCtx, NK_STATIC, 30, 1, (float[]){30+30+30+8});
        int v2 = value;
        int vH = value>>8;
        int vL = value&0xFF;
        nk_progress(pCtx, &vH, max>>8, 1);
        nk_progress(pCtx, &vL, 0xFF, 1);
        value = vH<<8 | (vL&0xFF);

        if( value > max )
        {
        	value = max;
        }
        if( value < min )
        {
        	value = min;
        }
        *pValue = value;

        nk_popup_end( pCtx );
    }
    else
    {
    	retval = 0;
    }

    return retval;
}


uint8_t nk_property_keypad( struct nk_context *pCtx, uint8_t *pLabel, int32_t min, int32_t *pValue, int32_t max, uint8_t *pShow_keypad )
{
	uint8_t retval = 0;
	uint8_t buffer[32];
    nk_layout_row( pCtx, NK_STATIC, 30, 4, (float[]){60, 30, 60, 30});
    nk_label( pCtx, pLabel, NK_TEXT_LEFT );

    nk_button_push_behavior( pCtx, NK_BUTTON_REPEATER );
    if( nk_button_symbol( pCtx, NK_SYMBOL_TRIANGLE_LEFT ) )
    {
        if( *pValue > min )
        {
            *pValue -= 1;
        }
        retval = 1;
    }
    nk_button_pop_behavior( pCtx );

    snprintf( buffer, sizeof(buffer), "%d", *pValue );
    *pShow_keypad |= nk_button_label( pCtx, buffer );
    if( *pShow_keypad )
    {
        *pShow_keypad = nk_keypad( pCtx, min, pValue, max );
        retval = 1;
    }

    nk_button_push_behavior( pCtx, NK_BUTTON_REPEATER );
    if( nk_button_symbol( pCtx, NK_SYMBOL_TRIANGLE_RIGHT ) )
    {
        if( *pValue < max )
        {
            *pValue += 1;
        }
        retval = 1;
    }
    nk_button_pop_behavior( pCtx );
    return retval;
}

static int show_keypad_red = 0;
static int show_keypad_green = 0;
static int show_keypad_blue = 0;
static int show_keypad_alpha = 0;
static struct nk_color color = {0,0,0,0};

static int red = 0;
static int green = 0;
static int blue = 0;
static int alpha = 255;
void ui_build_color_picker( tUi *pThis, struct nk_context *pCtx )
{
    // This menu shows thre nk_property_keypad widgets for red, green and blue.
    // The color choosed is used to draw a button.

    if( nk_tree_push( pCtx, NK_TREE_TAB, "Color", NK_MINIMIZED) )
    {
        nk_property_keypad( pCtx, "Red", 0, &red, 255, &show_keypad_red );
        nk_property_keypad( pCtx, "Green", 0, &green, 255, &show_keypad_green );
        nk_property_keypad( pCtx, "Blue", 0, &blue, 255, &show_keypad_blue );
        nk_property_keypad( pCtx, "Alpha", 0, &alpha, 255, &show_keypad_alpha );
        
        nk_layout_row( pCtx, NK_STATIC, 30, 4, (float[]){60, 30, 60, 30});
        nk_style_push_color( pCtx, &pCtx->style.button.normal.data.color, nk_rgba(red, green, blue, alpha) );
		nk_button_label( pCtx, "Test" );
		nk_style_pop_color( pCtx );
        nk_tree_pop( pCtx );
    }
}

static const char *theme_names[] = {
	"Black",
	"White",
	"Red",
	"Blue",
	"Dark",
};
static int selected_theme = 0; // Initial theme selection

void ui_build_theme_chooser(struct nk_context *ctx)
{
    if (nk_tree_push(ctx, NK_TREE_TAB, "Theme Chooser", NK_MINIMIZED))
    {
        // Combo box to select a theme
        if (nk_combo_begin_label(ctx, theme_names[selected_theme], nk_vec2(200, 200)))
        {
            nk_layout_row_dynamic(ctx, 25, 1);
            for (int i = 0; i < sizeof(theme_names) / sizeof(theme_names[0]); i++)
            {
                if (nk_combo_item_label(ctx, theme_names[i], NK_TEXT_LEFT))
                {
                    selected_theme = i;
                    nk_combo_close(ctx);
                }
            }
            nk_combo_end(ctx);
        }

        // Button to apply the selected theme
        if (nk_button_label(ctx, "Apply Theme"))
        {
            nk_set_theme(ctx, selected_theme);
        }

        nk_tree_pop(ctx);
    }
}

#define RECTS_LEN 64
extern struct nk_rect rects[RECTS_LEN];
extern int rects_max;
extern int rects_max_bck;
extern int rects_ptr;
//extern int rects_pressed = 0;

int ui_menu = 0;
void ui_build2( tUi *pThis, struct nk_context *pCtx )
{
	if( nk_begin( pCtx, "STM32G4 Scope ", nk_rect(0, 0, 240, 320), NK_WINDOW_MINIMIZABLE | NK_WINDOW_NO_SCROLLBAR ) )
	{
		pThis->is_visible = 1;
		switch( ui_menu )
		{
			case 1: ui_build_acquire2( &pThis->acquire, pCtx ); break;
			case 2: ui_build_horizontal2( &pThis->horizontal, pCtx ); break;
			case 3: ui_build_vertical2( &pThis->vertical, pCtx ); break;
			case 4: ui_build_trigger2( &pThis->trigger, pCtx ); break;
			default: break;
		}
	}
	else
	{
		pThis->is_visible = 0;
	}
	nk_end( pCtx );
}

void ui_build( tUi *pThis, struct nk_context *pCtx )
{
    if( nk_begin( pCtx, "STM32G4 Scope ", nk_rect(0, 0, 240, 320), NK_WINDOW_MINIMIZABLE ) )
	{
    	pThis->is_visible = 1;

    	//ui_build_theme_chooser( pCtx );
    	//ui_build_color_picker( pThis, pCtx );
    	/*nk_layout_row( pCtx, NK_STATIC, 30, 3, (float[]){30, 30, 30});
    	if( nk_button_label( pCtx, "-" ) )
    	{
    		if( rects_ptr > 0 )
    		{
    			rects_ptr -= 1;
    		}
    		else
    		{
    			rects_ptr = rects_max_bck-1;
    		}
    	}
    	if( nk_button_label( pCtx, "X" ) )
    	{
    		rects_pressed = 1;
    	}
    	else
    	{
    		rects_pressed = 0;
    	}
    	if( nk_button_label( pCtx, "+" ) )
    	{
    		if( rects_ptr < rects_max_bck )
    		{
    			rects_ptr += 1;
    		}
    		else
    		{
        		rects_ptr = 0;
    		}
    	}*/

    	ui_build_acquire( &pThis->acquire, pCtx );
        ui_build_horizontal( &pThis->horizontal, pCtx );
        ui_build_vertical( &pThis->vertical, pCtx );
        ui_build_trigger( &pThis->trigger, pCtx );
        ui_build_wavegen( &pThis->wavegen, pCtx );
        //ui_build_cursor( pThis, pCtx );
        //ui_build_measurements( pThis, pCtx );
        ui_build_info( pThis, pCtx );
	}
    else
    {
    	pThis->is_visible = 0;
    }
	nk_end( pCtx );
}





void _ui_build_info( tUi *pThis, struct nk_context *pCtx )
{
    uint8_t buffer[32];
	pThis->info.is_visible = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Info", NK_MINIMIZED) )
    {
    	pThis->info.is_visible = 1;
        nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
        nk_label( pCtx, "FPS AVG", NK_TEXT_LEFT );
        static int a = 0;
        static int b = 0;
        int dt;
        static int dt_low = 0;
        b = a;
        a = HAL_GetTick();
        dt = 1000/(a-b);
        dt_low = dt_low*0.9f + dt*0.1f;
        snprintf( buffer, sizeof(buffer), "%d", dt_low );
        nk_label( pCtx, buffer, NK_TEXT_RIGHT );
        nk_label( pCtx, "CPU", NK_TEXT_LEFT );
        nk_label( pCtx, "0", NK_TEXT_RIGHT );
        nk_label( pCtx, "MEM", NK_TEXT_LEFT );
        nk_label( pCtx, "0", NK_TEXT_RIGHT );
        nk_tree_pop( pCtx );
    }
}

#include "scope_tasks.h"

void ui_build_info(tUi *pThis, struct nk_context *pCtx) {
    uint8_t buffer[32];
    pThis->info.is_visible = 0;

    if (nk_tree_push(pCtx, NK_TREE_TAB, "Info", NK_MINIMIZED)) {
        pThis->info.is_visible = 1;
        nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});

        nk_label(pCtx, "Version", NK_TEXT_LEFT);
        nk_label(pCtx, __DATE__, NK_TEXT_RIGHT);


        nk_label(pCtx, "FPS AVG", NK_TEXT_LEFT);
        static int a = 0;
        static int b = 0;
        int dt;
        static int dt_low = 0;
        b = a;
        a = HAL_GetTick();
        dt = 1000 / (a - b);
        dt_low = dt_low * 0.9f + dt * 0.1f;
        snprintf(buffer, sizeof(buffer), "%d", dt_low);
        nk_label(pCtx, buffer, NK_TEXT_RIGHT);

        // CPU Usage Calculation
        /*TaskStatus_t xTaskStatus;
        extern osThreadId_t taskTscHandle;
        extern osThreadId_t taskUiHandle;
        extern osThreadId_t taskScopeHandle;
        extern osThreadId_t taskWavegenHandle;
        vTaskGetInfo(taskTscHandle, &xTaskStatus, pdTRUE, eInvalid);
        float cpuUsageTaskTsc = ((float)xTaskStatus.ulRunTimeCounter / xTaskGetTickCount()) * 100;
        snprintf(buffer, sizeof(buffer), "%.2f%%", cpuUsageTaskTsc);
        nk_label(pCtx, "TaskTsc", NK_TEXT_LEFT);
        nk_label(pCtx, buffer, NK_TEXT_LEFT);

        vTaskGetInfo(taskUiHandle, &xTaskStatus, pdTRUE, eInvalid);
        float cpuUsageTaskUI = ((float)xTaskStatus.ulRunTimeCounter / xTaskGetTickCount()) * 100;
        snprintf(buffer, sizeof(buffer), "%.2f%%", cpuUsageTaskUI);
        nk_label(pCtx, "TaskUI", NK_TEXT_LEFT);
        nk_label(pCtx, buffer, NK_TEXT_LEFT);

        vTaskGetInfo(taskScopeHandle, &xTaskStatus, pdTRUE, eInvalid);
        float cpuUsageTaskScope = ((float)xTaskStatus.ulRunTimeCounter / xTaskGetTickCount()) * 100;
        snprintf(buffer, sizeof(buffer), "%.2f%%", cpuUsageTaskScope);
        nk_label(pCtx, "TaskScope", NK_TEXT_LEFT);
        nk_label(pCtx, buffer, NK_TEXT_LEFT);

        vTaskGetInfo(taskWavegenHandle, &xTaskStatus, pdTRUE, eInvalid);
        float cpuUsageTaskWavegen = ((float)xTaskStatus.ulRunTimeCounter / xTaskGetTickCount()) * 100;
        snprintf(buffer, sizeof(buffer), "%.2f%%", cpuUsageTaskWavegen);
        nk_label(pCtx, "TaskWavegen", NK_TEXT_LEFT);
        nk_label(pCtx, buffer, NK_TEXT_LEFT);

        // Calculate total CPU usage
        float totalCPUUsage = cpuUsageTaskTsc + cpuUsageTaskUI + cpuUsageTaskScope + cpuUsageTaskWavegen;
        snprintf(buffer, sizeof(buffer), "%.2f%%", totalCPUUsage);
        nk_label(pCtx, "CPU", NK_TEXT_LEFT);
        nk_label(pCtx, buffer, NK_TEXT_LEFT);*/

        //nk_label(pCtx, "MEM", NK_TEXT_LEFT);
        // You can add memory information here if needed
        //nk_label(pCtx, "0", NK_TEXT_RIGHT);

        nk_layout_row(pCtx, NK_STATIC, 60, 1, (float[]){94+94});
        static int property_int = 0;
        nk_property_int(pCtx, "Int:", 0, &property_int, 100, 1, 1);

        nk_tree_pop(pCtx);
    }
}
#include "lcd.h"
#define UI_COLOR_CURSOR_X0 LCD_COLOR_YELLOW
#define UI_COLOR_CURSOR_X1 LCD_COLOR_YELLOW
#define UI_COLOR_CURSOR_Y0 LCD_COLOR_YELLOW
#define UI_COLOR_CURSOR_Y1 LCD_COLOR_YELLOW
#define UI_COLOR_BACKGROUND LCD_COLOR_BLACK

void ui_draw_cursors( tUi_Cursors *pThis, tLcd *pLcd, int is_collapsed )
{
	ui_stroque_cursors( pThis, pLcd,
			UI_COLOR_CURSOR_X0, UI_COLOR_CURSOR_X1, UI_COLOR_CURSOR_Y0, UI_COLOR_CURSOR_Y1,
			is_collapsed );
}


void ui_erase_cursors( tUi_Cursors *pThis, tLcd *pLcd, int is_collapsed)
{
	ui_stroque_cursors( pThis, pLcd,
			UI_COLOR_BACKGROUND, UI_COLOR_BACKGROUND, UI_COLOR_BACKGROUND, UI_COLOR_BACKGROUND,
			is_collapsed );
}

#define MARKER_ORIENTATION_NORTH 0
#define MARKER_ORIENTATION_SOUTH 1
#define MARKER_ORIENTATION_EAST  2
#define MARKER_ORIENTATION_WEST  3

void ui_stroque_cursors( tUi_Cursors *pThis, tLcd *pLcd,
		uint16_t color1, uint16_t color2, uint16_t color3, uint16_t color4,
		int is_collapsed )
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    if( pThis->cursor_selected == 0 || pThis->cursor_selected == 1 )
    {
		if (is_collapsed) {
			x = -pThis->cursors[pThis->cursor_selected].offset/2 + pLcd->width/4;
		} else {
			x = -pThis->cursors[pThis->cursor_selected].offset + pLcd->width/2;
		}
		y = 0;
		h = pLcd->height;

		// Draw the horizontal stroke
		lcd_vline(pLcd, x, y, h, color1);

		// Draw an optional marker at the left end of the stroke
		draw_marker(pLcd, x, 0, MARKER_ORIENTATION_SOUTH, color1);
    }
    if( pThis->cursor_selected == 2 || pThis->cursor_selected == 3 )
	{
        if (is_collapsed) {
            w = pLcd->width / 2;
        } else {
            w = pLcd->width;
        }
        x = 0;
        y = pThis->cursors[pThis->cursor_selected].offset;
        lcd_hline(pLcd, x, y, w, color3);
        draw_marker(pLcd, x, y, MARKER_ORIENTATION_EAST, color3);
	}

}
