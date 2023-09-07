/*
 * ui_wavegen.c
 *
 *  Created on: Sep 3, 2023
 *      Author: jgpei
 */

#include "ui.h"

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

void ui_build_wavegen( tUi_Wavegen *pThis, struct nk_context *pCtx )
{
    struct sQueueUiWavegen msgUiWavegen = {0};

    static uint8_t show_keypad_offset = 0;
    static uint8_t show_keypad_scale = 0;
    static uint8_t show_keypad_freq = 0;
    static uint8_t show_keypad_duty = 0;

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
			pThis->waveforms[pThis->waveform_selected].type = nk_combo( pCtx, (const char*[]){"Dc", "Sine", "Square", "Triangle", "Saw", "PWM", "Noise"}, 7, pThis->waveforms[pThis->waveform_selected].type, 30, nk_vec2(94, 120));
			nk_property_keypad( pCtx, "Offset", -4096, &pThis->waveforms[pThis->waveform_selected].offset, 4095, &show_keypad_offset );
			if( pThis->waveforms[pThis->waveform_selected].type != WAVEGEN_TYPE2_DC )
			{
				nk_property_keypad( pCtx, "Scale", 0, &pThis->waveforms[pThis->waveform_selected].scale, 4095, &show_keypad_scale );
				nk_property_keypad( pCtx, "Freq", 0, &pThis->waveforms[pThis->waveform_selected].frequency, 4095, &show_keypad_freq );
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
                osMessageQueuePut(queueUiWavegenHandle, &msgUiWavegen, 0U, portMAX_DELAY);
            }

            if( tmp.waveforms[tmp.waveform_selected].frequency != pThis->waveforms[pThis->waveform_selected].frequency )
            {
                msgUiWavegen.type = QUEUE_UI_WAVEGEN_TYPE_CONFIG_HORIZONTAL;
                msgUiWavegen.data[0] = pThis->waveform_selected;
                msgUiWavegen.data[1] = pThis->waveforms[pThis->waveform_selected].frequency;
                osMessageQueuePut(queueUiWavegenHandle, &msgUiWavegen, 0U, portMAX_DELAY);
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
                osMessageQueuePut(queueUiWavegenHandle, &msgUiWavegen, 0U, portMAX_DELAY);
            }

        }
		nk_tree_pop( pCtx );
	}
    else
    {
        pThis->is_visible = 0;
    }

    if( pThis->is_visible != pThis->is_visible_bck )
    {
    	msgUiWavegen.type = QUEUE_UI_WAVEGEN_TYPE_CHANGE_VISIBILITY;
    	msgUiWavegen.data[0] = pThis->is_visible;
    	msgUiWavegen.data[1] = 0; //
		osMessageQueuePut(queueUiWavegenHandle, &msgUiWavegen, 0U, portMAX_DELAY);
    }
    pThis->is_visible_bck = pThis->is_visible;
}
