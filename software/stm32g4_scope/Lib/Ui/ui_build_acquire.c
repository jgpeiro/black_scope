/*
 * ui_acquire.c
 *
 *  Created on: Sep 3, 2023
 *      Author: jgpei
 */

#include "ui.h"

void ui_build_acquire( tUi_Acquire *pThis, struct nk_context *pCtx )
{
    struct sQueueUiScope msgUiScope = {0};
    tUi_Acquire tmp;

    if( nk_tree_push( pCtx, NK_TREE_TAB, "Acquire", NK_MAXIMIZED) )
    {
    	pThis->is_visible = 1;
    	tmp = *pThis;

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
        else
        {
        	pThis->single = 0;
        }

        if( memcmp( &tmp, pThis, sizeof(tUi_Acquire) ) )
        {
        	if( pThis->run != tmp.run )
        	{
        		if( pThis->run )
                {
                    msgUiScope.type = QUEUE_UI_SCOPE_TYPE_START;
                    msgUiScope.data[0] = 1;
                    osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
                }
                else
                {
                    msgUiScope.type = QUEUE_UI_SCOPE_TYPE_STOP;
                    osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
                }
        	}

        	if( pThis->single != tmp.single )
        	{
        		if( pThis->single )
                {
                    msgUiScope.type = QUEUE_UI_SCOPE_TYPE_START;
                    msgUiScope.data[0] = 0; // continuous
                    osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);

                	pThis->run = 0;
                }
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
		msgUiScope.data[1] = 0; // ACQUIRE
		osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
    }
    pThis->is_visible_bck = pThis->is_visible;

}

void ui_build_acquire2( tUi_Acquire *pThis, struct nk_context *pCtx )
{
    struct sQueueUiScope msgUiScope = {0};
    tUi_Acquire tmp;

    //if( nk_tree_push( pCtx, NK_TREE_TAB, "Acquire", NK_MAXIMIZED) )
    {
    	pThis->is_visible = 1;
    	tmp = *pThis;

    	nk_layout_row( pCtx, NK_STATIC, 30+30, 2, (float[]){95, 95});

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
        else
        {
        	pThis->single = 0;
        }

        if( memcmp( &tmp, pThis, sizeof(tUi_Acquire) ) )
        {
        	if( pThis->run != tmp.run )
        	{
        		if( pThis->run )
                {
                    msgUiScope.type = QUEUE_UI_SCOPE_TYPE_START;
                    msgUiScope.data[0] = 1;
                    osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
                }
                else
                {
                    msgUiScope.type = QUEUE_UI_SCOPE_TYPE_STOP;
                    osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
                }
        	}

        	if( pThis->single != tmp.single )
        	{
        		if( pThis->single )
                {
                    msgUiScope.type = QUEUE_UI_SCOPE_TYPE_START;
                    msgUiScope.data[0] = 0; // continuous
                    osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);

                	pThis->run = 0;
                }
        	}
        }

        //nk_tree_pop( pCtx );
    }
    //else
    {
    //    pThis->is_visible = 0;
    }

    if( pThis->is_visible != pThis->is_visible_bck )
    {
    	msgUiScope.type = QUEUE_UI_SCOPE_TYPE_CHANGE_VISIBILITY;
		msgUiScope.data[0] = pThis->is_visible;
		msgUiScope.data[1] = 0; // ACQUIRE
		osMessageQueuePut(queueUiScopeHandle, &msgUiScope, 0U, portMAX_DELAY);
    }
    pThis->is_visible_bck = pThis->is_visible;

}
