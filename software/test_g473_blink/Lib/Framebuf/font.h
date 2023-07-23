/*
 * Font.h
 *
 *  Created on: Jul 23, 2023
 *      Author: jgpei
 */

#ifndef FONT_H_
#define FONT_H_

#include <stdint.h>

struct sGlyph{
    uint8_t *pName;
    uint8_t encoding;
    uint8_t swidthx, swidthy;
    uint8_t dwidthx, dwidthy;
    int8_t bbxw, bbxh, bbxx, bbxy;
    const uint8_t *pBitmap;
};
typedef struct sGlyph tGlyph;

struct sFont{
    uint8_t *pName;
    uint8_t start;
    uint8_t end;
    uint8_t ascent;
    uint8_t descent;
    int8_t bbxw, bbxh, bbxx, bbxy;
    const tGlyph **pGlyphs;
};
typedef struct sFont tFont;

struct sRectangle
{
	int16_t x;
	int16_t y;
	int16_t width;
	int16_t height;
};
typedef struct sRectangle tRectangle;

tRectangle get_char_rect( const tFont *pFont, uint8_t c );
tRectangle get_text_rect( const tFont *pFont, char *pString );

#endif // #ifndef FONT_H_
