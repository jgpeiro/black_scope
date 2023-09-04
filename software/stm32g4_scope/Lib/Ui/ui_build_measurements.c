/*
 * ui_build_measurements.c
 *
 *  Created on: Sep 3, 2023
 *      Author: jgpei
 */

#include "ui.h"

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
