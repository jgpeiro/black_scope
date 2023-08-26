/*
 * ui.c
 *
 *  Created on: Aug 1, 2023
 *      Author: jgpei
 */

#include <stdio.h>
#include "ui.h"

#define QUEUE_UI_SCOPE_TYPE_HORIZONTAL 0
#define QUEUE_UI_SCOPE_TYPE_VERTICAL 1
#define QUEUE_UI_SCOPE_TYPE_TRIGGER 2
#define QUEUE_UI_SCOPE_TYPE_START 3
#define QUEUE_UI_SCOPE_TYPE_STOP 4

#include "FreeRTOS.h"
#include "task.h"
//#include "main.h"
#include "cmsis_os.h"

struct sQueueUiScope {
    uint16_t type;
    uint16_t data[8];
};

struct sQueueUiWavegen {
    uint16_t type;
    uint16_t data[8];
};

extern osMessageQueueId_t queueUiScopeHandle;
extern osMessageQueueId_t queueUiWavegenHandle;


#define COLOR_BUTTON_ENABLED (struct nk_color){40,200,40, 255}
#define COLOR_BUTTON_DISABLED (struct nk_color){200,40,40, 255}

#define COLOR_CHANNEL1 (struct nk_color){255,0,0, 255}
#define COLOR_CHANNEL2 (struct nk_color){0,255,0, 255}
#define COLOR_CHANNEL3 (struct nk_color){0,0,255, 255}
#define COLOR_CHANNEL4 (struct nk_color){255,0,255, 255}

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

void ui_build_acquire2( tUi_Acquire *pThis, struct nk_context *pCtx )
{
	pThis->is_visible = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Acquire", NK_MAXIMIZED) )
    {
    	pThis->is_visible = 1;
        nk_layout_row( pCtx, NK_STATIC, 30, 2, (float[]){95, 95});

        nk_style_push_color_button( pCtx, pThis->run );
        if( nk_button_label( pCtx, pThis->run ? "Run" : "Stop" ) )
        {
        	pThis->run = !pThis->run;
        }
        nk_style_pop_color_button( pCtx );

        if( nk_button_label( pCtx, "Single" ) )
        {
        	pThis->single = 1;
        }
        nk_tree_pop( pCtx );
    }
}

void ui_build_horizontal2( tUi_Horizontal *pThis, struct nk_context *pCtx )
{
    static uint8_t show_keypad_offset = 0;
    static uint8_t show_keypad_scale = 0;

    struct sQueueUiScope msgUiScope = {0};

    pThis->is_visible = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Horizontal", NK_MINIMIZED) )
    {
    	pThis->is_visible = 1;
    	if( nk_property_keypad( pCtx, "Offset", -9999, &pThis->offset, 9999, &show_keypad_offset ) )
    	{
    		msgUiScope.type = QUEUE_UI_SCOPE_TYPE_HORIZONTAL;
    		msgUiScope.data[0] = pThis->scale*1000;
            msgUiScope.data[1] =-pThis->offset+512;
            osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, 0U);
        }
    	if( nk_property_keypad( pCtx, "Scale", 0, &pThis->scale, 9999, &show_keypad_scale ) )
    	{
    		msgUiScope.type = QUEUE_UI_SCOPE_TYPE_HORIZONTAL;
    		msgUiScope.data[0] = pThis->scale*1000;
            msgUiScope.data[1] =-pThis->offset+512;
            osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, 0U);
    	}
        nk_tree_pop( pCtx );
    }
}

void ui_build_vertical2( tUi_Vertical *pThis, struct nk_context *pCtx )
{
    static uint8_t show_keypad_offset = 0;
    static uint8_t show_keypad_scale = 0;

    struct sQueueUiScope msgUiScope = {0};

	pThis->is_visible = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Vertical", NK_MINIMIZED) )
    {
    	pThis->is_visible = 1;
        nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
        {
			nk_style_push_color_channel( pCtx, pThis->channel_selected );
			pThis->channel_selected = nk_combo(pCtx, (const char*[]){"Ch1", "Ch2", "Ch3", "Ch4"}, UI_CHANNEL_COUNT, pThis->channel_selected, 30, nk_vec2(94, 160));
			nk_style_pop_color_channel( pCtx );
        }

        nk_style_push_color_button( pCtx, pThis->channels[pThis->channel_selected].enabled );
        if( nk_button_label( pCtx, pThis->channels[pThis->channel_selected].enabled ? "On" : "Off" ) )
        {
            pThis->channels[pThis->channel_selected].enabled = !pThis->channels[pThis->channel_selected].enabled;
        }
        nk_style_pop_color_button( pCtx );


        if( pThis->channels[pThis->channel_selected].enabled )
        {
        	nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
            nk_label( pCtx, "Coupling", NK_TEXT_LEFT );
            pThis->channels[pThis->channel_selected].coupling = nk_combo( pCtx, (const char*[]){"DC", "Gnd"}, 2, pThis->channels[pThis->channel_selected].coupling, 30, nk_vec2(94, 120));
            if( nk_property_keypad( pCtx, "Offset", 0, &pThis->offset, 4095, &show_keypad_offset ) )
			{
                msgUiScope.type = QUEUE_UI_SCOPE_TYPE_VERTICAL;
                msgUiScope.data[0] = pThis->channels[0].scale;
                msgUiScope.data[1] = pThis->channels[1].scale;
                msgUiScope.data[2] = pThis->channels[2].scale;
                msgUiScope.data[3] = pThis->channels[3].scale;
                msgUiScope.data[4] = pThis->offset;
                osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, 0U);

			}
            if( nk_property_keypad( pCtx, "Scale", 0, &pThis->channels[pThis->channel_selected].scale, 5, &show_keypad_scale ) )
			{
                    msgUiScope.type = QUEUE_UI_SCOPE_TYPE_VERTICAL;
                    msgUiScope.data[0] = pThis->channels[0].scale;
                    msgUiScope.data[1] = pThis->channels[1].scale;
                    msgUiScope.data[2] = pThis->channels[2].scale;
                    msgUiScope.data[3] = pThis->channels[3].scale;
                    msgUiScope.data[4] = pThis->offset;
                    osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, 0U);
			}
        }
        nk_tree_pop( pCtx );
    }
}


void ui_build_trigger2( tUi_Trigger *pThis, struct nk_context *pCtx )
{
    struct sQueueUiScope msgUiScope = {0};

    static uint8_t show_keypad_level = 0;

    pThis->is_visible = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Trigger", NK_MINIMIZED) )
    {
    	pThis->is_visible = 1;
    	tUi_Trigger tmp = *pThis;
        nk_layout_row(pCtx, NK_STATIC, 30, 1, (float[]){94});

        pThis->mode = nk_combo( pCtx, (const char*[]){"Auto", "Normal"}, 2, pThis->mode, 30, nk_vec2(94, 120));

        if( pThis->mode == UI_TRIGGER_MODE_NORMAL )
        {
            nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});

            nk_style_push_color_channel( pCtx, pThis->source );
            pThis->source = nk_combo(pCtx, (const char*[]){"Ch1", "Ch2", "Ch3", "Ch4"}, UI_CHANNEL_COUNT, pThis->source, 30, nk_vec2(94, 160));
    		nk_style_pop_color_channel( pCtx );

            pThis->slope = nk_combo( pCtx, (const char*[]){"Rising", "Falling"}, 2, pThis->slope, 30, nk_vec2(94, 120));
            nk_property_keypad( pCtx, "Level", -9999, &pThis->level, 9999, &show_keypad_level );
        }

        if( memcmp( &tmp, pThis, sizeof(tUi_Trigger) ) )
        {
            msgUiScope.type = QUEUE_UI_SCOPE_TYPE_TRIGGER;
            msgUiScope.data[0] = pThis->source;
            msgUiScope.data[1] = pThis->mode;
            msgUiScope.data[2] = pThis->level;
            msgUiScope.data[3] = pThis->slope;
            osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, 0U);
        }
        nk_tree_pop( pCtx );
    }
}
enum eWaveGenType2
{
	WAVEGEN_TYPE2_DC,
	WAVEGEN_TYPE2_SINE,
	WAVEGEN_TYPE2_SQUARE,
	WAVEGEN_TYPE2_TRIANGLE,
	WAVEGEN_TYPE2_SAWTOOTH,
	WAVEGEN_TYPE2_PWM,
	WAVEGEN_TYPE2_NOISE
};

enum eQueueUiWavegenType
{
	QUEUE_UI_WAVEGEN_TYPE_START,
	QUEUE_UI_WAVEGEN_TYPE_STOP,
	QUEUE_UI_WAVEGEN_TYPE_CONFIG_HORIZONTAL,
	QUEUE_UI_WAVEGEN_TYPE_CONFIG_VERTICAL,
};

void ui_build_wavegen2( tUi_Wavegen *pThis, struct nk_context *pCtx )
{
    struct sQueueUiWavegen msgUiWavegen = {0};

    static uint8_t show_keypad_offset = 0;
    static uint8_t show_keypad_scale = 0;
    static uint8_t show_keypad_freq = 0;
    static uint8_t show_keypad_duty = 0;
	pThis->is_visible = 0;

    if( nk_tree_push( pCtx, NK_TREE_TAB, "Waveform", NK_MINIMIZED) )
    {
    	pThis->is_visible = 1;
    	tUi_Wavegen tmp = *pThis;

    	nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});

		nk_style_push_color_channel( pCtx, pThis->waveform_selected );
    	pThis->waveform_selected = nk_combo(pCtx, (const char*[]){"Wg1", "Wg2"}, UI_WAVEFORM_COUNT, pThis->waveform_selected, 30, nk_vec2(94, 160));
		nk_style_pop_color_channel( pCtx );

        nk_style_push_color_button( pCtx, pThis->waveforms[pThis->waveform_selected].enabled );
    	if( nk_button_label( pCtx, pThis->waveforms[pThis->waveform_selected].enabled ? "On" : "Off" ) )
		{
			pThis->waveforms[pThis->waveform_selected].enabled = !pThis->waveforms[pThis->waveform_selected].enabled;
		}
        nk_style_pop_color_button( pCtx );

		if( pThis->waveforms[pThis->waveform_selected].enabled )
		{
			nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
			pThis->waveforms[pThis->waveform_selected].type = nk_combo( pCtx, (const char*[]){"Dc", "Sine", "Square", "Triangle", "Sawtooth", "PWM", "Noise"}, 7, pThis->waveforms[pThis->waveform_selected].type, 30, nk_vec2(94, 120));
			nk_property_keypad( pCtx, "Offset", -9999, &pThis->waveforms[pThis->waveform_selected].offset, 9999, &show_keypad_offset );
			if( pThis->waveforms[pThis->waveform_selected].type != WAVEGEN_TYPE2_DC )
			{
				nk_property_keypad( pCtx, "Scale", 0, &pThis->waveforms[pThis->waveform_selected].scale, 9999, &show_keypad_scale );
				nk_property_keypad( pCtx, "Freq", 0, &pThis->waveforms[pThis->waveform_selected].frequency, 999999, &show_keypad_freq );
			}
			if( pThis->waveforms[pThis->waveform_selected].type == WAVEGEN_TYPE2_PWM )
			{
				nk_property_keypad( pCtx, "Duty", 0, &pThis->waveforms[pThis->waveform_selected].duty_cycle, 100, &show_keypad_duty );
			}
		}

		if( memcmp( &tmp, pThis, sizeof(tUi_Wavegen) ) )
		{
            // Send the new waveform configuration to the wavegen task.
            // if the channel changed from enabled to disabled, the wavegen task will stop the wave.
            
            // compare pThis with tmp to check enabled.
            if( pThis->waveforms[pThis->waveform_selected].enabled != tmp.waveforms[tmp.waveform_selected].enabled )
            {
                if( pThis->waveforms[pThis->waveform_selected].enabled )
                {
                    msgUiWavegen.type = QUEUE_UI_WAVEGEN_TYPE_START;
                }
                else
                {
                    msgUiWavegen.type = QUEUE_UI_WAVEGEN_TYPE_STOP;
                }
                msgUiWavegen.data[0] = pThis->waveform_selected;
                osMessageQueuePut(queueUiWavegenHandle, &msgUiWavegen, 0U, 0U);
            }

            if( tmp.waveforms[tmp.waveform_selected].frequency != pThis->waveforms[pThis->waveform_selected].frequency )
            {
                msgUiWavegen.type = QUEUE_UI_WAVEGEN_TYPE_CONFIG_HORIZONTAL;
                msgUiWavegen.data[0] = pThis->waveform_selected;
                msgUiWavegen.data[1] = pThis->waveforms[pThis->waveform_selected].frequency;
                osMessageQueuePut(queueUiWavegenHandle, &msgUiWavegen, 0U, 0U);
            }

            if( tmp.waveforms[tmp.waveform_selected].type != pThis->waveforms[pThis->waveform_selected].type ||
                tmp.waveforms[tmp.waveform_selected].offset != pThis->waveforms[pThis->waveform_selected].offset ||
                tmp.waveforms[tmp.waveform_selected].scale != pThis->waveforms[pThis->waveform_selected].scale ||
                tmp.waveforms[tmp.waveform_selected].duty_cycle != pThis->waveforms[pThis->waveform_selected].duty_cycle
            )
            {
                msgUiWavegen.type = QUEUE_UI_WAVEGEN_TYPE_CONFIG_VERTICAL;
                msgUiWavegen.data[0] = pThis->waveform_selected;
                msgUiWavegen.data[1] = pThis->waveforms[pThis->waveform_selected].type;
                msgUiWavegen.data[2] = pThis->waveforms[pThis->waveform_selected].offset;
                msgUiWavegen.data[3] = pThis->waveforms[pThis->waveform_selected].scale;
                msgUiWavegen.data[4] = pThis->waveforms[pThis->waveform_selected].duty_cycle;
                osMessageQueuePut(queueUiWavegenHandle, &msgUiWavegen, 0U, 0U);
            }
            
        }
		nk_tree_pop( pCtx );
	}
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

void ui_build( tUi *pThis, struct nk_context *pCtx )
{
    if( nk_begin( pCtx, "STM32G4 Scope", nk_rect(0, 0, 240, 320), NK_WINDOW_MINIMIZABLE ) )
	{
        ui_build_acquire2( &pThis->acquire, pCtx );
        ui_build_horizontal2( &pThis->horizontal, pCtx );
        ui_build_vertical2( &pThis->vertical, pCtx );
        ui_build_trigger2( &pThis->trigger, pCtx );
        ui_build_wavegen2( &pThis->wavegen, pCtx );
        ui_build_cursor( pThis, pCtx );
        ui_build_measurements( pThis, pCtx );
        ui_build_info( pThis, pCtx );
        ui_build_color_picker( pThis, pCtx );
    }
	nk_end( pCtx );
}

void ui_build_acquire( tUi *pThis, struct nk_context *pCtx )
{
	pThis->acquire.is_visible = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Acquire", NK_MAXIMIZED) )
    {
    	pThis->acquire.is_visible = 1;
        nk_layout_row( pCtx, NK_STATIC, 30, 2, (float[]){95, 95});

        if( pThis->acquire.run )
		{
            nk_style_push_color( pCtx, &pCtx->style.button.normal.data.color, nk_rgba(40,200,40, 255) );
            nk_style_push_color( pCtx, &pCtx->style.button.hover.data.color, nk_rgba(40,200,40, 255) );
            nk_style_push_color( pCtx, &pCtx->style.button.active.data.color, nk_rgba(40,200,40, 255) );
		}
        else
        {
            nk_style_push_color( pCtx, &pCtx->style.button.normal.data.color, nk_rgba(200,40,40, 255) );
            nk_style_push_color( pCtx, &pCtx->style.button.hover.data.color, nk_rgba(200,40,40, 255) );
            nk_style_push_color( pCtx, &pCtx->style.button.active.data.color, nk_rgba(200,40,40, 255) );
        }
        if( nk_button_label( pCtx, pThis->acquire.run ? "Run" : "Stop" ) )
        {
        	pThis->acquire.run = !pThis->acquire.run;
        }
		nk_style_pop_color( pCtx );
		nk_style_pop_color( pCtx );
		nk_style_pop_color( pCtx );

        if( nk_button_label( pCtx, "Single" ) )
        {
        	pThis->acquire.single = 1;
        }
        nk_tree_pop( pCtx );
    }
}
#include "main.h"
#include "scope.h"
extern tScope scope;
void ui_build_horizontal( tUi *pThis, struct nk_context *pCtx )
{
    static uint8_t show_keypad_offset = 0;
    static uint8_t show_keypad_scale = 0;
	pThis->horizontal.is_visible = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Horizontal", NK_MINIMIZED) )
    {
    	pThis->horizontal.is_visible = 1;
    	if( nk_property_keypad( pCtx, "Offset", -9999, &pThis->horizontal.offset, 9999, &show_keypad_offset ) )
    	{
    		scope_config_horizontal( &scope, pThis->horizontal.scale*1000, -pThis->horizontal.offset+512 );
    	}
    	if( nk_property_keypad( pCtx, "Scale", 0, &pThis->horizontal.scale, 9999, &show_keypad_scale ) )
    	{
    		scope_config_horizontal( &scope, pThis->horizontal.scale*1000, -pThis->horizontal.offset+512 );
    	}
        nk_tree_pop( pCtx );
    }
}

void ui_build_vertical( tUi *pThis, struct nk_context *pCtx )
{
    static uint8_t show_keypad_offset = 0;
    static uint8_t show_keypad_scale = 0;
	pThis->vertical.is_visible = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Vertical", NK_MINIMIZED) )
    {
    	pThis->vertical.is_visible = 1;
        nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
        {
			if( pThis->vertical.channel_selected == 0 )
			{
				nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, nk_rgba(255,0,0, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, nk_rgba(255,0,0, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_active, nk_rgba(255,0,0, 255) );
			}
			else if( pThis->vertical.channel_selected == 1 )
			{
				nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, nk_rgba(0, 255,0, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, nk_rgba(0, 255,0, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_active, nk_rgba(0, 255,0, 255) );
			}
			else if( pThis->vertical.channel_selected == 2 )
			{
				nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, nk_rgba(0, 0, 255, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, nk_rgba(0, 0, 255, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_active, nk_rgba(0, 0, 255, 255) );
			}
			else if( pThis->vertical.channel_selected == 3 )
			{
				nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, nk_rgba(255,0,255, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, nk_rgba(255,0,255, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_active, nk_rgba(255,0,255, 255) );
			}
			pThis->vertical.channel_selected = nk_combo(pCtx, (const char*[]){"Ch1", "Ch2", "Ch3", "Ch4"}, UI_CHANNEL_COUNT, pThis->vertical.channel_selected, 30, nk_vec2(94, 160));
			nk_style_pop_color( pCtx );
			nk_style_pop_color( pCtx );
			nk_style_pop_color( pCtx );
        }

        if( pThis->vertical.channels[pThis->vertical.channel_selected].enabled )
		{
            nk_style_push_color( pCtx, &pCtx->style.button.normal.data.color, nk_rgba(40,200,40, 255) );
            nk_style_push_color( pCtx, &pCtx->style.button.hover.data.color, nk_rgba(40,200,40, 255) );
            nk_style_push_color( pCtx, &pCtx->style.button.active.data.color, nk_rgba(40,200,40, 255) );
		}
        else
        {
            nk_style_push_color( pCtx, &pCtx->style.button.normal.data.color, nk_rgba(200,40,40, 255) );
            nk_style_push_color( pCtx, &pCtx->style.button.hover.data.color, nk_rgba(200,40,40, 255) );
            nk_style_push_color( pCtx, &pCtx->style.button.active.data.color, nk_rgba(200,40,40, 255) );
        }
        if( nk_button_label( pCtx, pThis->vertical.channels[pThis->vertical.channel_selected].enabled ? "On" : "Off" ) )
        {
            pThis->vertical.channels[pThis->vertical.channel_selected].enabled = !pThis->vertical.channels[pThis->vertical.channel_selected].enabled;
        }
		nk_style_pop_color( pCtx );
		nk_style_pop_color( pCtx );
		nk_style_pop_color( pCtx );


        if( pThis->vertical.channels[pThis->vertical.channel_selected].enabled )
        {
        	nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
            nk_label( pCtx, "Coupling", NK_TEXT_LEFT );
            pThis->vertical.channels[pThis->vertical.channel_selected].coupling = nk_combo( pCtx, (const char*[]){"DC", "Gnd"}, 2, pThis->vertical.channels[pThis->vertical.channel_selected].coupling, 30, nk_vec2(94, 120));
            if( nk_property_keypad( pCtx, "Offset", 0, &pThis->vertical.offset, 4095, &show_keypad_offset ) )
			{
				scope_config_vertical( &scope,
					pThis->vertical.channels[0].scale,
					pThis->vertical.channels[1].scale,
					pThis->vertical.channels[2].scale,
					pThis->vertical.channels[3].scale,
					pThis->vertical.offset );
			}
            if( nk_property_keypad( pCtx, "Scale", 0, &pThis->vertical.channels[pThis->vertical.channel_selected].scale, 5, &show_keypad_scale ) )
			{
				scope_config_vertical( &scope,
					pThis->vertical.channels[0].scale,
					pThis->vertical.channels[1].scale,
					pThis->vertical.channels[2].scale,
					pThis->vertical.channels[3].scale,
					pThis->vertical.offset );
			}
        }
        nk_tree_pop( pCtx );
    }
}

void ui_build_trigger( tUi *pThis, struct nk_context *pCtx )
{
    static uint8_t show_keypad_level = 0;
	pThis->trigger.is_visible = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Trigger", NK_MINIMIZED) )
    {
    	pThis->trigger.is_visible = 1;
    	tUi_Trigger tmp = pThis->trigger;
        nk_layout_row(pCtx, NK_STATIC, 30, 1, (float[]){94});

        pThis->trigger.mode = nk_combo( pCtx, (const char*[]){"Auto", "Normal"}, 2, pThis->trigger.mode, 30, nk_vec2(94, 120));
        
        if( pThis->trigger.mode == UI_TRIGGER_MODE_NORMAL )
        {
            nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});

			if( pThis->trigger.source == 0 )
			{
				nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, nk_rgba(255,0,0, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, nk_rgba(255,0,0, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_active, nk_rgba(255,0,0, 255) );
			}
			else if( pThis->trigger.source == 1 )
			{
				nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, nk_rgba(0, 255,0, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, nk_rgba(0, 255,0, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_active, nk_rgba(0, 255,0, 255) );
			}
			else if( pThis->trigger.source == 2 )
			{
				nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, nk_rgba(0, 0, 255, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, nk_rgba(0, 0, 255, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_active, nk_rgba(0, 0, 255, 255) );
			}
			else if( pThis->trigger.source == 3 )
			{
				nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, nk_rgba(255,0,255, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, nk_rgba(255,0,255, 255) );
				nk_style_push_color( pCtx, &pCtx->style.combo.label_active, nk_rgba(255,0,255, 255) );
			}
            pThis->trigger.source = nk_combo(pCtx, (const char*[]){"Ch1", "Ch2", "Ch3", "Ch4"}, UI_CHANNEL_COUNT, pThis->trigger.source, 30, nk_vec2(94, 160));
    		nk_style_pop_color( pCtx );
    		nk_style_pop_color( pCtx );
    		nk_style_pop_color( pCtx );
            
            pThis->trigger.slope = nk_combo( pCtx, (const char*[]){"Rising", "Falling"}, 2, pThis->trigger.slope, 30, nk_vec2(94, 120));
            nk_property_keypad( pCtx, "Level", -9999, &pThis->trigger.level, 9999, &show_keypad_level );
        }
        if( memcmp( &tmp, &pThis->trigger, sizeof(tUi_Trigger) ) )
        {
        	scope_config_trigger( &scope, pThis->trigger.source, pThis->trigger.mode, pThis->trigger.level, pThis->trigger.slope );
        }
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

#include <stdio.h>

void get_period_prescaler(int clock, int desired_freq, int *period, int *prescaler)
{
    float scale = (float)clock / desired_freq;
    *prescaler = 0;
    *period = 0;

    for (int i = 0; i < 16; i++) {
        *prescaler = 1 << i;
        *period = (int)(scale / *prescaler);
        if (*period < 65535) {
            break;
        }
    }

    return clock / (*prescaler * *period);
}



void ui_wavegen_build( tUi *pThis, tWaveGen *wavegen )
{
    /*if( pThis->wavegen.waveform_selected == 0 )
    {
    	HAL_TIM_Base_Stop( scope.wavegen.htim1 );
	}
    if( pThis->wavegen.waveform_selected == 1 )
    {
		HAL_TIM_Base_Stop( scope.wavegen.htim2 );
	}*/

    wavegen_stop( wavegen,
    	((!pThis->wavegen.waveforms[0].enabled) << 0) |
    	((!pThis->wavegen.waveforms[1].enabled) << 1) );

    if( pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].type == WAVEGEN_TYPE_DC )
    {
        wavegen_build_dc( wavegen, 1 << pThis->wavegen.waveform_selected, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].offset );
    }
    else if( pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].type == WAVEGEN_TYPE_SINE )
    {
        wavegen_build_sine( wavegen, 1 << pThis->wavegen.waveform_selected, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].frequency, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].scale, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].offset );
    }
    else if( pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].type == WAVEGEN_TYPE_SQUARE )
    {
        wavegen_build_square( wavegen, 1 << pThis->wavegen.waveform_selected, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].frequency, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].scale, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].offset );
    }
    else if( pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].type == WAVEGEN_TYPE_TRIANGLE )
    {
        wavegen_build_triangle( wavegen, 1 << pThis->wavegen.waveform_selected, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].frequency, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].scale, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].offset );
    }
    else if( pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].type == WAVEGEN_TYPE_SAWTOOTH )
    {
        wavegen_build_sawtooth( wavegen, 1 << pThis->wavegen.waveform_selected, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].frequency, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].scale, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].offset );
    }
    else if( pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].type == WAVEGEN_TYPE_PWM )
    {
        wavegen_build_pwm( wavegen, 1 << pThis->wavegen.waveform_selected, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].frequency, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].scale, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].offset, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].duty_cycle );
    }
    else if( pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].type == WAVEGEN_TYPE_NOISE )
    {
        wavegen_build_noise( wavegen, 1 << pThis->wavegen.waveform_selected, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].frequency, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].scale, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].offset );
    }
    int tim_freq = 170e6;
    int prescaler = 0;
    int period = 0;
    int desired_freq = wavegen->len*pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].frequency;
    get_period_prescaler(tim_freq, desired_freq, &period, &prescaler);
/*#include "integer_factorization.h"
    uint32_t n = (tim_freq / desired_freq);
      uint32_t mx = 65536;
      uint32_t a = 0;
      uint32_t b = 0;
      integer_factorization( n, mx, &a, &b );
      period = a;
      prescaler = b;*/
    prescaler = (tim_freq / desired_freq)/2;
    period = 2;

    if( pThis->wavegen.waveform_selected == 0 )
    {
		//scope.wavegen.htim1->Init.Prescaler = (tim_freq / pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].frequency)/2 - 1;
		//scope.wavegen.htim1->Init.Period = 1;
    	scope.wavegen.htim1->Init.Prescaler = prescaler - 1;
    	scope.wavegen.htim1->Init.Period = period - 1;
    	HAL_TIM_Base_Init( scope.wavegen.htim1 );
		//HAL_TIM_Base_Start( scope.wavegen.htim1 );
	}
    if( pThis->wavegen.waveform_selected == 1 )
    {
		//scope.wavegen.htim2->Init.Prescaler = (tim_freq / pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].frequency)/2 - 1;
		//scope.wavegen.htim2->Init.Period = 1;
    	scope.wavegen.htim2->Init.Prescaler = prescaler - 1;
    	scope.wavegen.htim2->Init.Period = period - 1;
		HAL_TIM_Base_Init( scope.wavegen.htim2 );
		//HAL_TIM_Base_Start( scope.wavegen.htim2 );
	}

    wavegen_start( wavegen,
		((pThis->wavegen.waveforms[0].enabled) << 0) |
		((pThis->wavegen.waveforms[1].enabled) << 1) );


}

void ui_build_wavegen( tUi *pThis, struct nk_context *pCtx )
{
    static uint8_t show_keypad_offset = 0;
    static uint8_t show_keypad_scale = 0;
    static uint8_t show_keypad_freq = 0;
    static uint8_t show_keypad_duty = 0;
	pThis->wavegen.is_visible = 0;

    if( nk_tree_push( pCtx, NK_TREE_TAB, "Waveform", NK_MINIMIZED) )
    {
    	pThis->wavegen.is_visible = 1;
    	tUi_Wavegen tmp = pThis->wavegen;

    	nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});

		if( pThis->wavegen.waveform_selected == 0 )
		{
			nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, nk_rgba(255,255,0, 255) );
			nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, nk_rgba(255,255,0, 255) );
			nk_style_push_color( pCtx, &pCtx->style.combo.label_active, nk_rgba(255,255,0, 255) );
		}
		else if( pThis->wavegen.waveform_selected == 1 )
		{
			nk_style_push_color( pCtx, &pCtx->style.combo.label_normal, nk_rgba(0, 255,255, 255) );
			nk_style_push_color( pCtx, &pCtx->style.combo.label_hover, nk_rgba(0, 255,255, 255) );
			nk_style_push_color( pCtx, &pCtx->style.combo.label_active, nk_rgba(0, 255,255, 255) );
		}
    	pThis->wavegen.waveform_selected = nk_combo(pCtx, (const char*[]){"Wg1", "Wg2"}, UI_WAVEFORM_COUNT, pThis->wavegen.waveform_selected, 30, nk_vec2(94, 160));
		nk_style_pop_color( pCtx );
		nk_style_pop_color( pCtx );
		nk_style_pop_color( pCtx );

        if( pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].enabled )
		{
			nk_style_push_color( pCtx, &pCtx->style.button.normal.data.color, nk_rgba(40,200,40, 255) );
			nk_style_push_color( pCtx, &pCtx->style.button.hover.data.color, nk_rgba(40,200,40, 255) );
			nk_style_push_color( pCtx, &pCtx->style.button.active.data.color, nk_rgba(40,200,40, 255) );
		}
        else
        {
			nk_style_push_color( pCtx, &pCtx->style.button.normal.data.color, nk_rgba(200,40,40, 255) );
			nk_style_push_color( pCtx, &pCtx->style.button.hover.data.color, nk_rgba(200,40,40, 255) );
			nk_style_push_color( pCtx, &pCtx->style.button.active.data.color, nk_rgba(200,40,40, 255) );
        }

    	if( nk_button_label( pCtx, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].enabled ? "On" : "Off" ) )
		{
			pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].enabled = !pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].enabled;
		}
		nk_style_pop_color( pCtx );
		nk_style_pop_color( pCtx );
		nk_style_pop_color( pCtx );


		if( pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].enabled )
		{
			nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
			pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].type = nk_combo( pCtx, (const char*[]){"Dc", "Sine", "Square", "Triangle", "Sawtooth", "PWM", "Noise"}, 7, pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].type, 30, nk_vec2(94, 120));
			nk_property_keypad( pCtx, "Offset", -9999, &pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].offset, 9999, &show_keypad_offset );
			if( pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].type != WAVEGEN_TYPE_DC )
			{
				nk_property_keypad( pCtx, "Scale", 0, &pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].scale, 9999, &show_keypad_scale );
				nk_property_keypad( pCtx, "Freq", 0, &pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].frequency, 999999, &show_keypad_freq );
			}
			if( pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].type == WAVEGEN_TYPE_PWM )
			{
				nk_property_keypad( pCtx, "Duty", 0, &pThis->wavegen.waveforms[pThis->wavegen.waveform_selected].duty_cycle, 100, &show_keypad_duty );
			}
		}

		if( memcmp( &tmp, &pThis->wavegen, sizeof(tUi_Wavegen) ) )
		{
			ui_wavegen_build( pThis, &wavegen );
		}

		nk_tree_pop( pCtx );
	}
}

void ui_build_cursor( tUi *pThis, struct nk_context *pCtx )
{
    static uint8_t show_keypad_offset = 0;
    static uint8_t show_keypad_track = 0;
    pThis->cursors.is_visible = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Cursor", NK_MINIMIZED) )
    {
    	pThis->cursors.is_visible = 1;
        nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
        pThis->cursors.cursor_selected = nk_combo(pCtx, (const char*[]){"C1", "C2"}, UI_CURSOR_COUNT, pThis->cursors.cursor_selected, 30, nk_vec2(94, 160));

		if( pThis->cursors.cursors[pThis->cursors.cursor_selected].enabled )
		{
			nk_style_push_color( pCtx, &pCtx->style.button.normal.data.color, nk_rgba(40,200,40, 255) );
			nk_style_push_color( pCtx, &pCtx->style.button.hover.data.color, nk_rgba(40,200,40, 255) );
			nk_style_push_color( pCtx, &pCtx->style.button.active.data.color, nk_rgba(40,200,40, 255) );
		}
        else
        {
			nk_style_push_color( pCtx, &pCtx->style.button.normal.data.color, nk_rgba(200,40,40, 255) );
			nk_style_push_color( pCtx, &pCtx->style.button.hover.data.color, nk_rgba(200,40,40, 255) );
			nk_style_push_color( pCtx, &pCtx->style.button.active.data.color, nk_rgba(200,40,40, 255) );
        }

		if( nk_button_label( pCtx, pThis->cursors.cursors[pThis->cursors.cursor_selected].enabled ? "On" : "Off" ) )
		{
			pThis->cursors.cursors[pThis->cursors.cursor_selected].enabled = !pThis->cursors.cursors[pThis->cursors.cursor_selected].enabled;
		}
		nk_style_pop_color( pCtx );
		nk_style_pop_color( pCtx );
		nk_style_pop_color( pCtx );

        if( pThis->cursors.cursors[pThis->cursors.cursor_selected].enabled )
        {
        	pThis->cursors.cursors[pThis->cursors.cursor_selected].horizontal = nk_combo( pCtx, (const char*[]){"Hori", "Vert"}, 2, pThis->cursors.cursors[pThis->cursors.cursor_selected].horizontal, 30, nk_vec2(94, 120));
        	nk_property_keypad( pCtx, "Offset", -9999, &pThis->cursors.cursors[pThis->cursors.cursor_selected].offset, 9999, &show_keypad_offset );
            nk_property_keypad( pCtx, "Track", -9999, &pThis->cursors.cursors[pThis->cursors.cursor_selected].track, 9999, &show_keypad_track );
        }
        nk_tree_pop( pCtx );
    }
}

void ui_build_measurements( tUi *pThis, struct nk_context *pCtx )
{
	pThis->measurements.is_visible = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Measurements", NK_MINIMIZED) )
    {
    	pThis->measurements.is_visible = 1;
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
	pThis->info.is_visible = 0;
    if( nk_tree_push( pCtx, NK_TREE_TAB, "Info", NK_MINIMIZED) )
    {
    	pThis->info.is_visible = 1;
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
