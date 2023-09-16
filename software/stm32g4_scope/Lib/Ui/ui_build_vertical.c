/*
 * ui_vertical.c
 *
 *  Created on: Sep 3, 2023
 *      Author: jgpei
 */

#include "ui.h"

void ui_build_vertical( tUi_Vertical *pThis, struct nk_context *pCtx )
{
    static uint8_t show_keypad_offset1 = 0;
    static uint8_t show_keypad_offset2 = 0;
    static uint8_t show_keypad_gain = 0;
    static uint8_t show_keypad_scale = 0;

    struct sQueueUiScope msgUiScope = {0};

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
            msgUiScope.type = QUEUE_UI_SCOPE_TYPE_VERTICAL;
            msgUiScope.data[0] = pThis->offset;

            msgUiScope.data[1] = pThis->channels[0].gain;
            msgUiScope.data[2] = pThis->channels[1].gain;
            msgUiScope.data[3] = pThis->channels[2].gain;
            msgUiScope.data[4] = pThis->channels[3].gain;

            msgUiScope.data[5] = pThis->channels[0].offset;
            msgUiScope.data[6] = pThis->channels[1].offset;
            msgUiScope.data[7] = pThis->channels[2].offset;
            msgUiScope.data[8] = pThis->channels[3].offset;

            msgUiScope.data[9] = pThis->channels[0].enabled;
            msgUiScope.data[10]= pThis->channels[1].enabled;
            msgUiScope.data[11]= pThis->channels[2].enabled;
            msgUiScope.data[12]= pThis->channels[3].enabled;

            msgUiScope.data[13] = pThis->channels[0].scale;
            msgUiScope.data[14] = pThis->channels[1].scale;
            msgUiScope.data[15] = pThis->channels[2].scale;
            msgUiScope.data[16] = pThis->channels[3].scale;
            osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
        }
        nk_style_pop_color_button( pCtx );


        if( pThis->channels[pThis->channel_selected].enabled )
        {
        	nk_layout_row(pCtx, NK_STATIC, 30, 2, (float[]){94, 94});
            nk_label( pCtx, "Coupling", NK_TEXT_LEFT );
            pThis->channels[pThis->channel_selected].coupling = nk_combo( pCtx, (const char*[]){"DC", "Gnd"}, 2, pThis->channels[pThis->channel_selected].coupling, 30, nk_vec2(94, 120));
            if( nk_property_keypad( pCtx, "Offset", 0, &pThis->offset, 4095, &show_keypad_offset1 ) )
			{
                msgUiScope.type = QUEUE_UI_SCOPE_TYPE_VERTICAL;
                msgUiScope.data[0] = pThis->offset;

                msgUiScope.data[1] = pThis->channels[0].gain;
                msgUiScope.data[2] = pThis->channels[1].gain;
                msgUiScope.data[3] = pThis->channels[2].gain;
                msgUiScope.data[4] = pThis->channels[3].gain;

                msgUiScope.data[5] = pThis->channels[0].offset;
                msgUiScope.data[6] = pThis->channels[1].offset;
                msgUiScope.data[7] = pThis->channels[2].offset;
                msgUiScope.data[8] = pThis->channels[3].offset;

                msgUiScope.data[9] = pThis->channels[0].enabled;
                msgUiScope.data[10]= pThis->channels[1].enabled;
                msgUiScope.data[11]= pThis->channels[2].enabled;
                msgUiScope.data[12]= pThis->channels[3].enabled;

                msgUiScope.data[13] = pThis->channels[0].scale;
                msgUiScope.data[14] = pThis->channels[1].scale;
                msgUiScope.data[15] = pThis->channels[2].scale;
                msgUiScope.data[16] = pThis->channels[3].scale;

                osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
			}
            if( nk_property_keypad( pCtx, "Scale", 0, &pThis->channels[pThis->channel_selected].gain, 5, &show_keypad_gain ) )
			{
                msgUiScope.type = QUEUE_UI_SCOPE_TYPE_VERTICAL;
                msgUiScope.data[0] = pThis->offset;

                msgUiScope.data[1] = pThis->channels[0].gain;
                msgUiScope.data[2] = pThis->channels[1].gain;
                msgUiScope.data[3] = pThis->channels[2].gain;
                msgUiScope.data[4] = pThis->channels[3].gain;

                msgUiScope.data[5] = pThis->channels[0].offset;
                msgUiScope.data[6] = pThis->channels[1].offset;
                msgUiScope.data[7] = pThis->channels[2].offset;
                msgUiScope.data[8] = pThis->channels[3].offset;

                msgUiScope.data[9] = pThis->channels[0].enabled;
                msgUiScope.data[10]= pThis->channels[1].enabled;
                msgUiScope.data[11]= pThis->channels[2].enabled;
                msgUiScope.data[12]= pThis->channels[3].enabled;

                msgUiScope.data[13] = pThis->channels[0].scale;
                msgUiScope.data[14] = pThis->channels[1].scale;
                msgUiScope.data[15] = pThis->channels[2].scale;
                msgUiScope.data[16] = pThis->channels[3].scale;

                osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
			}
            if( nk_property_keypad( pCtx, "Offs.Sw", 0, &pThis->channels[pThis->channel_selected].offset, 4095, &show_keypad_offset2 ) )
			{
                msgUiScope.type = QUEUE_UI_SCOPE_TYPE_VERTICAL;
                msgUiScope.data[0] = pThis->offset;

                msgUiScope.data[1] = pThis->channels[0].gain;
                msgUiScope.data[2] = pThis->channels[1].gain;
                msgUiScope.data[3] = pThis->channels[2].gain;
                msgUiScope.data[4] = pThis->channels[3].gain;

                msgUiScope.data[5] = pThis->channels[0].offset;
                msgUiScope.data[6] = pThis->channels[1].offset;
                msgUiScope.data[7] = pThis->channels[2].offset;
                msgUiScope.data[8] = pThis->channels[3].offset;

                msgUiScope.data[9] = pThis->channels[0].enabled;
                msgUiScope.data[10]= pThis->channels[1].enabled;
                msgUiScope.data[11]= pThis->channels[2].enabled;
                msgUiScope.data[12]= pThis->channels[3].enabled;

                msgUiScope.data[13] = pThis->channels[0].scale;
                msgUiScope.data[14] = pThis->channels[1].scale;
                msgUiScope.data[15] = pThis->channels[2].scale;
                msgUiScope.data[16] = pThis->channels[3].scale;

                osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
			}
            if( nk_property_keypad( pCtx, "Sca.Sw", 0, &pThis->channels[pThis->channel_selected].scale, 1000, &show_keypad_scale ) )
			{
                msgUiScope.type = QUEUE_UI_SCOPE_TYPE_VERTICAL;
                msgUiScope.data[0] = pThis->offset;

                msgUiScope.data[1] = pThis->channels[0].gain;
                msgUiScope.data[2] = pThis->channels[1].gain;
                msgUiScope.data[3] = pThis->channels[2].gain;
                msgUiScope.data[4] = pThis->channels[3].gain;

                msgUiScope.data[5] = pThis->channels[0].offset;
                msgUiScope.data[6] = pThis->channels[1].offset;
                msgUiScope.data[7] = pThis->channels[2].offset;
                msgUiScope.data[8] = pThis->channels[3].offset;

                msgUiScope.data[9] = pThis->channels[0].enabled;
                msgUiScope.data[10]= pThis->channels[1].enabled;
                msgUiScope.data[11]= pThis->channels[2].enabled;
                msgUiScope.data[12]= pThis->channels[3].enabled;

                msgUiScope.data[13] = pThis->channels[0].scale;
                msgUiScope.data[14] = pThis->channels[1].scale;
                msgUiScope.data[15] = pThis->channels[2].scale;
                msgUiScope.data[16] = pThis->channels[3].scale;

                osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
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
    	msgUiScope.type = QUEUE_UI_SCOPE_TYPE_CHANGE_VISIBILITY;
		msgUiScope.data[0] = pThis->is_visible;
		msgUiScope.data[1] = 2; // VERTICAL
		osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
    }
    pThis->is_visible_bck = pThis->is_visible;
}
