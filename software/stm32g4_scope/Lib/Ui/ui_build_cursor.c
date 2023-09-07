/*
 * ui_cursor.c
 *
 *  Created on: Sep 3, 2023
 *      Author: jgpei
 */

#include "ui.h"

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
        	nk_property_keypad( pCtx, "Offset", 0, &pThis->cursors.cursors[pThis->cursors.cursor_selected].offset, 4095, &show_keypad_offset );
            nk_property_keypad( pCtx, "Track", 0, &pThis->cursors.cursors[pThis->cursors.cursor_selected].track, 4095, &show_keypad_track );
        }
        nk_tree_pop( pCtx );
    }
}
