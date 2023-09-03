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

	pThis->trigger.level = 3096;
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


uint8_t nk_keypad( struct nk_context *pCtx, int32_t min, int32_t *pValue, int32_t max )
{
    int value = *pValue;
	int retval = 0;
	char buffer[32];

	if (nk_popup_begin( pCtx, NK_POPUP_STATIC, "Keypad", NK_WINDOW_CLOSABLE | NK_WINDOW_NO_SCROLLBAR, (struct nk_rect){0, -30, 108, 208} ) )
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

        //nk_layout_row( pCtx, NK_STATIC, 30, 1, (float[]){30+30+30});
        //nk_slider_int( pCtx, min, &value, max, 1 );

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

void ui_build( tUi *pThis, struct nk_context *pCtx )
{
    if( nk_begin( pCtx, "STM32G4 Scope", nk_rect(0, 0, 240, 320), NK_WINDOW_MINIMIZABLE ) )
	{
    	pThis->is_visible = 1;

    	ui_build_theme_chooser( pCtx );
    	ui_build_color_picker( pThis, pCtx );
        ui_build_info( pThis, pCtx );

    	ui_build_acquire( &pThis->acquire, pCtx );
        ui_build_horizontal( &pThis->horizontal, pCtx );
        ui_build_vertical( &pThis->vertical, pCtx );
        ui_build_trigger( &pThis->trigger, pCtx );
        ui_build_wavegen( &pThis->wavegen, pCtx );
        ui_build_cursor( pThis, pCtx );
        ui_build_measurements( pThis, pCtx );
    }
    else
    {
    	pThis->is_visible = 0;
    }
	nk_end( pCtx );
}





void ui_build_info( tUi *pThis, struct nk_context *pCtx )
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
