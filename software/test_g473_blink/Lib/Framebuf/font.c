/*
 * font.c
 *
 *  Created on: Jul 23, 2023
 *      Author: jgpei
 */

#include "font.h"

tRectangle get_char_rect( const tFont *pFont, uint8_t c )
{
    tRectangle rect = {0};

    rect.width = pFont->pGlyphs[c-32]->dwidthx;
    rect.height = pFont->bbxh;
    return rect;
}

tRectangle get_text_rect( const tFont *pFont, char *pString )
{
	uint8_t i;
	tRectangle rect = {0};

	rect.height = pFont->bbxh;
	for( i = 0 ; pString[i] != '\0' ; i++ )
	{
		rect.width += get_char_rect( pFont, pString[i] ).width;
    }

    return rect;
}
