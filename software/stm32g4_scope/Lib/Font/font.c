/*
 * font.c
 *
 *  Created on: Jul 23, 2023
 *      Author: jgpei
 */

#include "font.h"

int16_t font_char_width( const tFont *pFont, uint8_t c )
{
    return pFont->pGlyphs[c-32]->dwidthx;
}

int16_t font_text_width( const tFont *pFont, char *pString )
{
	uint8_t i;
	int16_t width = 0;

	for( i = 0 ; pString[i] != '\0' ; i++ )
	{
		width += font_char_width( pFont, pString[i] );
    }

    return width;
}

void font_draw_text( const tFont *pFont, int16_t x0, int16_t y0, uint8_t *str, uint16_t color, void (*cb)(void*, int, int, int ), const void *param )
{
	char chr;
	for( chr=*str; chr; chr=*(++str) )
	{
        //int chr = *(uint8_t *)str;
        if (chr < 32 || chr > 127) {
            chr = 127;
        }
        font_draw_char( pFont, x0, y0, chr, color, cb, param );
        x0 += font_char_width( pFont, chr );
	}
}

void font_draw_char( const tFont *pFont, int16_t x0, int16_t y0, uint8_t c, uint16_t color, void (*cb)(void*, int, int, int ), const void *param )
{
    int16_t x, y, w, bitmap, b;
    int16_t px, py;
    const tGlyph *pGlyph;

    pGlyph = pFont->pGlyphs[c-32];

    y0 += pFont->bbxh;
    y0 -= pFont->descent;
    y0 -= pGlyph->bbxh;
    y0 -= pGlyph->bbxy;

    for( y = 0 ; y < pGlyph->bbxh ; y++ )
    {
        py = y0+y;

        w = (pGlyph->bbxw-1)/8+1;

        int16_t yw = y*w;
        for( x = 0 ; x < pGlyph->bbxw ; x+=8 )
        {
            bitmap = pGlyph->pBitmap[yw+x/8];

            for( b = 0 ; b < 8 ; b++ )
            {
                if( x+b >= pGlyph->bbxw )
                {
                    break;
                }

                px = x0+x+b;

                if( bitmap & (0x80>>b) )
                {
                	cb( param, px, py, color );
                }
            }
        }
    }
}
