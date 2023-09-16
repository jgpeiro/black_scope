/*
 * ui_horizontal.c
 *
 *  Created on: Sep 3, 2023
 *      Author: jgpei
 */
#include "ui.h"


void ui_build_horizontal( tUi_Horizontal *pThis, struct nk_context *pCtx )
{
    static uint8_t show_keypad_offset = 0;
    static uint8_t show_keypad_scale = 0;

    struct sQueueUiScope msgUiScope = {0};

    if( nk_tree_push( pCtx, NK_TREE_TAB, "Horizontal", NK_MINIMIZED) )
    {
    	pThis->is_visible = 1;
    	if( nk_property_keypad( pCtx, "Offset", 0, &pThis->offset, 4095, &show_keypad_offset ) )
    	{
    		msgUiScope.type = QUEUE_UI_SCOPE_TYPE_HORIZONTAL;
            msgUiScope.data[0] = pThis->offset;
    		msgUiScope.data[1] = pThis->scale;
            osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
        }
    	if( nk_property_keypad( pCtx, "Scale", 0, &pThis->scale, 2500, &show_keypad_scale ) )
    	{
    		msgUiScope.type = QUEUE_UI_SCOPE_TYPE_HORIZONTAL;
            msgUiScope.data[0] = pThis->offset;
    		msgUiScope.data[1] = pThis->scale;
            osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
    	}

    	/*nk_layout_row( pCtx, NK_STATIC, 30, 1, (float[]){180});

    	int tmp = pThis->offset;
    	nk_property_int( pCtx, "Offset", 0, &pThis->offset, 4095, 1, 4096/240);
    	if( pThis->offset != tmp )
    	{
    		msgUiScope.type = QUEUE_UI_SCOPE_TYPE_HORIZONTAL;
            msgUiScope.data[0] = pThis->offset;
    		msgUiScope.data[1] = pThis->scale;
            osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
    	}
    	tmp = pThis->scale;
		nk_property_int( pCtx, "Scale", 0, &pThis->scale, 2500, 1, 2500/240);
		if( pThis->scale != tmp )
		{
			msgUiScope.type = QUEUE_UI_SCOPE_TYPE_HORIZONTAL;
			msgUiScope.data[0] = pThis->offset;
			msgUiScope.data[1] = pThis->scale;
			osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
		}
		*/

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
		msgUiScope.data[1] = 1; // HORIZONTAL
		osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
    }
    pThis->is_visible_bck = pThis->is_visible;
}

