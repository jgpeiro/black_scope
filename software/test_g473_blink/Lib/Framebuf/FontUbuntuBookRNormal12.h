/****************************************************************************
 * File:	FontUbuntuBookRNormal12.h
 * Brief:		
 * Author:		
 ***************************************************************************/
#ifndef	_FONTUBUNTUBOOKRNORMAL12_H_
#define	_FONTUBUNTUBOOKRNORMAL12_H_
/****************************************************************************
 * Public Includes.
 ***************************************************************************/
//#include	"Font.h"
#include	<stdint.h>

// Font.h structure definitions
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


/****************************************************************************
 * Public Defines.
 ***************************************************************************/

/****************************************************************************
 * Public Types.
 ***************************************************************************/

/****************************************************************************
 * Public Variables.
 ***************************************************************************/
extern const tFont fontUbuntuBookRNormal12;

/****************************************************************************
 * Public Functions.
 ***************************************************************************/

#endif	//#ifndef	_FONTUBUNTUBOOKRNORMAL12_H_
