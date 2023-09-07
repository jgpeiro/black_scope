/*
 * ui_trigger.c
 *
 *  Created on: Sep 3, 2023
 *      Author: jgpei
 */

#include "ui.h"

void ui_build_trigger( tUi_Trigger *pThis, struct nk_context *pCtx )
{
    struct sQueueUiScope msgUiScope = {0};

    static uint8_t show_keypad_level = 0;

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
            nk_property_keypad( pCtx, "Level", 0, &pThis->level, 4095, &show_keypad_level );
        }

        if( memcmp( &tmp, pThis, sizeof(tUi_Trigger) ) )
        {
            msgUiScope.type = QUEUE_UI_SCOPE_TYPE_TRIGGER;
            msgUiScope.data[0] = pThis->source;
            msgUiScope.data[1] = pThis->mode;
            msgUiScope.data[2] = pThis->level;
            msgUiScope.data[3] = pThis->slope;
            osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
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
		msgUiScope.data[1] = 3; //TRIGGER
		osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
    }
    pThis->is_visible_bck = pThis->is_visible;

}
