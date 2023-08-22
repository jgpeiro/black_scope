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
    const tGlyph const **pGlyphs;
};
typedef struct sFont tFont;


int16_t font_char_width( const tFont *pFont, uint8_t c );
int16_t font_text_width( const tFont *pFont, char *pString );
void font_draw_text( const tFont *pFont, int16_t x0, int16_t y0, const uint8_t *str, uint16_t color, void (*cb)(void*, int, int, int ), const void *param );
void font_draw_char( const tFont *pFont, int16_t x0, int16_t y0, uint8_t c, uint16_t color, void (*cb)(void*, int, int, int ), const void *param );

#endif // #ifndef FONT_H_
