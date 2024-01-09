/*
 * ui.c
 *
 *  Created on: Sep 29, 2023
 *      Author: jgpei
 */

#include "ui.h"

void ui_build( struct nk_context *pCtx )
{
	static int states[9] = {0};
    if( nk_begin( pCtx, "STM32G4 Scope ", nk_rect(0, 0, LCD_WIDTH, LCD_HEIGHT), NK_WINDOW_MINIMIZABLE ) )
	{
    	nk_layout_row( pCtx, NK_STATIC, 60, 3, (float[]){60, 60, 60});
    	for( int i = 0 ; i < 9 ; i++ )
    	{
			if( states[i] )
			{
				if( nk_button_label( pCtx, "ON" ) )
				{
					states[i] = 0;
				}

			}
			else
			{
				if( nk_button_label( pCtx, "OFF" ) )
				{
					states[i] = 1;
				}
			}
    	}
    }

	nk_end( pCtx );
}
