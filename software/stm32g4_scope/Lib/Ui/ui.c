/*
 * ui.c
 *
 *  Created on: Aug 1, 2023
 *      Author: jgpei
 */

#include <stdio.h>
#include "ui.h"

uint8_t _nk_keypad( struct nk_context *pCtx, int min, int *pValue, int max )
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
        *pShow_keypad = _nk_keypad( pCtx, min, pValue, max );
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

void ui_build( tUi *pThis, struct nk_context *pCtx )
{
    if( nk_begin( pCtx, "STM32G4 Scope", nk_rect(0, 0, 240, 320), NK_WINDOW_MINIMIZABLE ) )
	{
        ui_build_acquire( pThis, pCtx );
        ui_build_horizontal( pThis, pCtx );
        ui_build_vertical( pThis, pCtx );
        ui_build_trigger( pThis, pCtx );
        ui_build_waveform( pThis, pCtx );
        ui_build_cursor( pThis, pCtx );
        ui_build_measurements( pThis, pCtx );
        ui_build_info( pThis, pCtx );
    }
	nk_end( pCtx );
}

void ui_build_acquire( tUi *pThis, struct nk_context *pCtx )
{
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Acquire", NK_MAXIMIZED) )
    {
        nk_layout_row( pCtx, NK_STATIC, 30, 2, (float[]){95, 95});
        if( nk_button_label( pCtx, pThis->acquire.run ? "Run" : "Stop" ) ) pThis->acquire.run = !pThis->acquire.run;
        if( nk_button_label( pCtx, "Single" ) ) pThis->acquire.single = 1;
        nk_tree_pop( pCtx );
    }
}
#include "main.h"
void ui_build_horizontal( tUi *pThis, struct nk_context *pCtx )
{
    static uint8_t show_keypad_offset = 0;
    static uint8_t show_keypad_scale = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Horizontal", NK_MINIMIZED) )
    {
    	if( nk_property_keypad( pCtx, "Offset", -9999, &pThis->horizontal.offset, 9999, &show_keypad_offset ) )
    	{
    		extern TIM_HandleTypeDef htim3;
    		//htim3.Instance->CR1 &= 0xFFFFFFFE;
			//htim3.Instance->CCR1 = pThis->horizontal.offset;
    		//htim3.Instance->CR1 |= 0x00000001;
            // Configure via HAL
            //HAL_TIM_OC_Stop( &htim3, TIM_CHANNEL_1 );
    		//TIM_OC_InitTypeDef sConfigOC = {0};
    		//sConfigOC.OCMode = TIM_OCMODE_TIMING;
    		//sConfigOC.Pulse = 1023 + pThis->horizontal.offset;
    		//sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    		//sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    		//HAL_TIM_OC_ConfigChannel( &htim3, &sConfigOC, TIM_CHANNEL_1 );
    		//HAL_TIM_OC_Start( &htim3, TIM_CHANNEL_1 );
    	}
    	if( nk_property_keypad( pCtx, "Scale", 1, &pThis->horizontal.scale, 9999, &show_keypad_scale ) )
    	{

    		extern TIM_HandleTypeDef htim2;
    		extern TIM_HandleTypeDef htim3;
    		htim2.Instance->PSC = pThis->horizontal.scale;
    		htim3.Instance->PSC = pThis->horizontal.scale;
    	}
        nk_tree_pop( pCtx );
    }
}

void ui_build_vertical( tUi *pThis, struct nk_context *pCtx )
{
    static uint8_t show_keypad_offset = 0;
    static uint8_t show_keypad_scale = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Vertical", NK_MINIMIZED) )
    {
        nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
        pThis->channel_selected = nk_combo(pCtx, (const char*[]){"Ch1", "Ch2", "Ch3", "Ch4"}, UI_CHANNEL_COUNT, pThis->channel_selected, 30, nk_vec2(94, 160));
        if( nk_button_label( pCtx, pThis->channels[pThis->channel_selected].enabled ? "On" : "Off" ) )
        {
            pThis->channels[pThis->channel_selected].enabled = !pThis->channels[pThis->channel_selected].enabled;
        }
        if( pThis->channels[pThis->channel_selected].enabled )
        {
        	nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
            nk_label( pCtx, "Coupling", NK_TEXT_LEFT );
            pThis->channels[pThis->channel_selected].coupling = nk_combo( pCtx, (const char*[]){"DC", "Gnd"}, 2, pThis->channels[pThis->channel_selected].coupling, 30, nk_vec2(94, 120));
            nk_property_keypad( pCtx, "Offset", -9999, &pThis->vertical.offset, 9999, &show_keypad_offset );
			nk_property_keypad( pCtx, "Scale", 1, &pThis->vertical.scale, 9999, &show_keypad_scale );
        }
        nk_tree_pop( pCtx );
    }
}

void ui_build_trigger( tUi *pThis, struct nk_context *pCtx )
{
    static uint8_t show_keypad_level = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Trigger", NK_MINIMIZED) )
    {
        nk_layout_row(pCtx, NK_STATIC, 30, 1, (float[]){94});
        pThis->trigger.source = nk_combo(pCtx, (const char*[]){"Ch1", "Ch2", "Ch3", "Ch4"}, UI_CHANNEL_COUNT, pThis->trigger.source, 30, nk_vec2(94, 160));
        nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
        pThis->trigger.mode = nk_combo( pCtx, (const char*[]){"Auto", "Normal"}, 2, pThis->trigger.mode, 30, nk_vec2(94, 120));
        pThis->trigger.slope = nk_combo( pCtx, (const char*[]){"Rising", "Falling"}, 2, pThis->trigger.slope, 30, nk_vec2(94, 120));
        nk_property_keypad( pCtx, "Level", -9999, &pThis->trigger.level, 9999, &show_keypad_level );
        nk_tree_pop( pCtx );
    }
}
#include "wavegen.h"
extern tWaveGen wavegen;
/*
	wavegen_init( &wavegen,
		&hdac1,
		&htim4,
		dac1_buffer,
		dac2_buffer,
		100,
		1e6 );
    wavegen_build_sine( &wavegen, 0x01, 10e3, 2047, 1500 );
    wavegen_build_sine( &wavegen, 0x02, 20e3, 2047, 1500 );
    wavegen_start( &wavegen, 0x03 );
    //HAL_Delay( 1000 );
    //wavegen_stop( &wavegen, 0x03 );
*/

void ui_wavegen_build( tUi *pThis, tWaveGen *wavegen )
{
    if( wavegen->type == WAVEGEN_TYPE_DC )
    {
        wavegen_build_dc( wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].offset );
    }
    else if( wavegen->type == WAVEGEN_TYPE_SINE )
    {
        wavegen_build_sine( wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].scale, pThis->waveforms[pThis->waveform_selected].offset );
    }
    else if( wavegen->type == WAVEGEN_TYPE_SQUARE )
    {
        wavegen_build_square( wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].scale, pThis->waveforms[pThis->waveform_selected].offset );
    }
    else if( wavegen->type == WAVEGEN_TYPE_TRIANGLE )
    {
        wavegen_build_triangle( wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].scale, pThis->waveforms[pThis->waveform_selected].offset );
    }
    else if( wavegen->type == WAVEGEN_TYPE_SAWTOOTH )
    {
        wavegen_build_sawtooth( wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].scale, pThis->waveforms[pThis->waveform_selected].offset );
    }
    else if( wavegen->type == WAVEGEN_TYPE_PWM )
    {
        wavegen_build_pwm( wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].scale, pThis->waveforms[pThis->waveform_selected].offset, pThis->waveforms[pThis->waveform_selected].duty_cycle );
    }
    else if( wavegen->type == WAVEGEN_TYPE_NOISE )
    {
        wavegen_build_noise( wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].scale, pThis->waveforms[pThis->waveform_selected].offset );
    }
}

void ui_build_waveform( tUi *pThis, struct nk_context *pCtx )
{
    static uint8_t show_keypad_offset = 0;
    static uint8_t show_keypad_scale = 0;
    static uint8_t show_keypad_freq = 0;
    static uint8_t show_keypad_duty = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Waveform", NK_MINIMIZED) )
    {
        nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
        pThis->waveform_selected = nk_combo(pCtx, (const char*[]){"Wg1", "Wg2"}, UI_WAVEGEN_COUNT, pThis->waveform_selected, 30, nk_vec2(94, 160));
        if( nk_button_label( pCtx, pThis->waveforms[pThis->waveform_selected].enabled ? "On" : "Off" ) )
        {
            pThis->waveforms[pThis->waveform_selected].enabled = !pThis->waveforms[pThis->waveform_selected].enabled;
            if( pThis->waveforms[pThis->waveform_selected].enabled )
            {
                wavegen_start( &wavegen, 1 << pThis->waveform_selected );
            }
            else
            {
                wavegen_stop( &wavegen, 1 << pThis->waveform_selected );
            }
        }
        if( pThis->waveforms[pThis->waveform_selected].enabled )
        {
            nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
            int type = nk_combo( pCtx, (const char*[]){"Dc", "Sine", "Square", "Triangle", "Sawtooth", "PWM", "Noise"}, 7, pThis->waveforms[pThis->waveform_selected].type, 30, nk_vec2(94, 120));
            if( type != pThis->waveforms[pThis->waveform_selected].type )
            {
                pThis->waveforms[pThis->waveform_selected].type = type;
                wavegen.type = type;
                ui_wavegen_build( pThis, &wavegen );
            }
            if( nk_property_keypad( pCtx, "Offset", -9999, &pThis->waveforms[pThis->waveform_selected].offset, 9999, &show_keypad_offset ) )
            {
                ui_wavegen_build( pThis, &wavegen );
            }
            if( pThis->waveforms[pThis->waveform_selected].type != WAVEGEN_TYPE_DC )
            {
                if( nk_property_keypad( pCtx, "Scale", 1, &pThis->waveforms[pThis->waveform_selected].scale, 9999, &show_keypad_scale ) )
                {
                    ui_wavegen_build( pThis, &wavegen );
                }
                if( nk_property_keypad( pCtx, "Freq", 1, &pThis->waveforms[pThis->waveform_selected].frequency, 9999, &show_keypad_freq ) )
                {
                    ui_wavegen_build( pThis, &wavegen );
                }
            }
            if( pThis->waveforms[pThis->waveform_selected].type == WAVEGEN_TYPE_PWM )
            {
                if( nk_property_keypad( pCtx, "Duty", 0, &pThis->waveforms[pThis->waveform_selected].duty_cycle, 100, &show_keypad_duty ) )
                {
                    ui_wavegen_build( pThis, &wavegen );
                }
            }
        }
        nk_tree_pop( pCtx );
    }
}

void _ui_build_waveform( tUi *pThis, struct nk_context *pCtx )
{
    static uint8_t show_keypad_offset = 0;
    static uint8_t show_keypad_scale = 0;
    static uint8_t show_keypad_freq = 0;
    static uint8_t show_keypad_duty = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Waveform", NK_MINIMIZED) )
    {
        nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
        pThis->waveform_selected = nk_combo(pCtx, (const char*[]){"Wg1", "Wg2"}, UI_WAVEGEN_COUNT, pThis->waveform_selected, 30, nk_vec2(94, 160));
        if( nk_button_label( pCtx, pThis->waveforms[pThis->waveform_selected].enabled ? "On" : "Off" ) )
        {
            pThis->waveforms[pThis->waveform_selected].enabled = !pThis->waveforms[pThis->waveform_selected].enabled;
            if( pThis->waveforms[pThis->waveform_selected].enabled )
            {
                wavegen_start( &wavegen, 1 << pThis->waveform_selected );
            }
            else
            {
                wavegen_stop( &wavegen, 1 << pThis->waveform_selected );
            }
        }
        if( pThis->waveforms[pThis->waveform_selected].enabled )
        {
        	nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
			int type = nk_combo( pCtx, (const char*[]){"Sine", "Square", "Triangle", "Sawtooth"}, 4, pThis->waveforms[pThis->waveform_selected].type, 30, nk_vec2(94, 120));
			if( type != pThis->waveforms[pThis->waveform_selected].type )
            {
                pThis->waveforms[pThis->waveform_selected].type = type;
                if( type == WAVEGEN_TYPE_DC )
                {
                    wavegen_build_dc( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].offset );
                }
                else if( type == WAVEGEN_TYPE_SINE )
                {
                    wavegen_build_sine( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].offset, pThis->waveforms[pThis->waveform_selected].scale );
                }
                else if( type == WAVEGEN_TYPE_SQUARE )
                {
                    wavegen_build_square( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].offset, pThis->waveforms[pThis->waveform_selected].scale );
                }
                else if( type == WAVEGEN_TYPE_TRIANGLE )
                {
                    wavegen_build_triangle( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].offset, pThis->waveforms[pThis->waveform_selected].scale );
                }
            }
            if( nk_property_keypad( pCtx, "Offset", -9999, &pThis->waveforms[pThis->waveform_selected].offset, 9999, &show_keypad_offset ) )
            {
                int type = pThis->waveforms[pThis->waveform_selected].type;
                if( type == WAVEGEN_TYPE_DC )
                {
                    wavegen_build_dc( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].offset );
                }
                else if( type == WAVEGEN_TYPE_SINE )
                {
                    wavegen_build_sine( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].offset, pThis->waveforms[pThis->waveform_selected].scale );
                }
                else if( type == WAVEGEN_TYPE_SQUARE )
                {
                    wavegen_build_square( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].offset, pThis->waveforms[pThis->waveform_selected].scale );
                }
                else if( type == WAVEGEN_TYPE_TRIANGLE )
                {
                    wavegen_build_triangle( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].offset, pThis->waveforms[pThis->waveform_selected].scale );
                }
            }
            if( nk_property_keypad( pCtx, "Scale", 1, &pThis->waveforms[pThis->waveform_selected].scale, 9999, &show_keypad_scale ) )
            {
                int type = pThis->waveforms[pThis->waveform_selected].type;
                if( type == WAVEGEN_TYPE_DC )
                {
                    wavegen_build_dc( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].offset );
                }
                else if( type == WAVEGEN_TYPE_SINE )
                {
                    wavegen_build_sine( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].offset, pThis->waveforms[pThis->waveform_selected].scale );
                }
                else if( type == WAVEGEN_TYPE_SQUARE )
                {
                    wavegen_build_square( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].offset, pThis->waveforms[pThis->waveform_selected].scale );
                }
                else if( type == WAVEGEN_TYPE_TRIANGLE )
                {
                    wavegen_build_triangle( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].offset, pThis->waveforms[pThis->waveform_selected].scale );
                }
            }
            if( nk_property_keypad( pCtx, "Freq", 1, &pThis->waveforms[pThis->waveform_selected].frequency, 9999, &show_keypad_freq ) )
            {
                int type = pThis->waveforms[pThis->waveform_selected].type;
                if( type == WAVEGEN_TYPE_DC )
                {
                    wavegen_build_dc( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].offset );
                }
                else if( type == WAVEGEN_TYPE_SINE )
                {
                    wavegen_build_sine( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].offset, pThis->waveforms[pThis->waveform_selected].scale );
                }
                else if( type == WAVEGEN_TYPE_SQUARE )
                {
                    wavegen_build_square( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].offset, pThis->waveforms[pThis->waveform_selected].scale );
                }
                else if( type == WAVEGEN_TYPE_TRIANGLE )
                {
                    wavegen_build_triangle( &wavegen, 1 << pThis->waveform_selected, pThis->waveforms[pThis->waveform_selected].frequency, pThis->waveforms[pThis->waveform_selected].offset, pThis->waveforms[pThis->waveform_selected].scale );
                }
            }
			if( pThis->waveforms[pThis->waveform_selected].type == 1 )
			{
				nk_property_keypad( pCtx, "Duty", 0, &pThis->waveforms[pThis->waveform_selected].duty_cycle, 100, &show_keypad_duty );
			}
        }
        nk_tree_pop( pCtx );
    }
}

void ui_build_cursor( tUi *pThis, struct nk_context *pCtx )
{
    static uint8_t show_keypad_offset = 0;
    static uint8_t show_keypad_track = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Cursor", NK_MINIMIZED) )
    {
        nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
        pThis->cursor_selected = nk_combo(pCtx, (const char*[]){"C1", "C2"}, UI_CURSOR_COUNT, pThis->cursor_selected, 30, nk_vec2(94, 160));
        if( nk_button_label( pCtx, pThis->cursors[pThis->cursor_selected].enabled ? "On" : "Off" ) )
        {
            pThis->cursors[pThis->cursor_selected].enabled = !pThis->cursors[pThis->cursor_selected].enabled;
        }
        if( pThis->cursors[pThis->cursor_selected].enabled )
        {
        	pThis->cursors[pThis->cursor_selected].horizontal = nk_combo( pCtx, (const char*[]){"Hori", "Vert"}, 2, pThis->cursors[pThis->cursor_selected].horizontal, 30, nk_vec2(94, 120));
        	nk_property_keypad( pCtx, "Offset", -9999, &pThis->cursors[pThis->cursor_selected].offset, 9999, &show_keypad_offset );
            nk_property_keypad( pCtx, "Track", -9999, &pThis->cursors[pThis->cursor_selected].track, 9999, &show_keypad_track );
        }
        nk_tree_pop( pCtx );
    }
}

void ui_build_measurements( tUi *pThis, struct nk_context *pCtx )
{
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Measurements", NK_MINIMIZED) )
    {
        nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
        nk_label( pCtx, "Frequency", NK_TEXT_LEFT );
        nk_label( pCtx, "0", NK_TEXT_RIGHT );
        nk_label( pCtx, "Period", NK_TEXT_LEFT );
        nk_label( pCtx, "0", NK_TEXT_RIGHT );
        nk_label( pCtx, "Duty Cycle", NK_TEXT_LEFT );
        nk_label( pCtx, "0", NK_TEXT_RIGHT );
        nk_label( pCtx, "Vpp", NK_TEXT_LEFT );
        nk_label( pCtx, "0", NK_TEXT_RIGHT );
        nk_label( pCtx, "Vrms", NK_TEXT_LEFT );
        nk_label( pCtx, "0", NK_TEXT_RIGHT );
        nk_label( pCtx, "Vdc", NK_TEXT_LEFT );
        nk_label( pCtx, "0", NK_TEXT_RIGHT );
        nk_label( pCtx, "Vavg", NK_TEXT_LEFT );
        nk_label( pCtx, "0", NK_TEXT_RIGHT );
        nk_tree_pop( pCtx );
    }
}

void ui_build_info( tUi *pThis, struct nk_context *pCtx )
{
    uint8_t buffer[32];
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Info", NK_MINIMIZED) )
    {
        nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
        nk_label( pCtx, "FPS", NK_TEXT_LEFT );
        static int a = 0;
        static int b = 0;
        b = a;
        a = HAL_GetTick();
        snprintf( buffer, sizeof(buffer), "%d", 1000/(a-b) );
        nk_label( pCtx, buffer, NK_TEXT_RIGHT );
        nk_label( pCtx, "CPU", NK_TEXT_LEFT );
        nk_label( pCtx, "0", NK_TEXT_RIGHT );
        nk_label( pCtx, "MEM", NK_TEXT_LEFT );
        nk_label( pCtx, "0", NK_TEXT_RIGHT );
        nk_tree_pop( pCtx );
    }
}
