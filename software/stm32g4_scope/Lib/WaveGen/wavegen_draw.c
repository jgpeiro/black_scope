/*
 * wavegen_draw.c
 *
 *  Created on: Sep 3, 2023
 *      Author: jgpei
 */

#include "wavegen_draw.h"

void wavegen_draw( tWaveGen *pThis, tLcd *pLcd, int is_collapsed )
{
	wavegen_stroque( pThis, pLcd, WAVEGEN_COLOR_CH1, WAVEGEN_COLOR_CH2, is_collapsed );
}

void wavegen_erase( tWaveGen *pThis, tLcd *pLcd, int is_collapsed )
{
	wavegen_stroque( pThis, pLcd, LCD_COLOR_BLACK, LCD_COLOR_BLACK, is_collapsed );
}

void wavegen_stroque( tWaveGen *pThis, tLcd *pLcd, uint16_t color1, uint16_t color2, int is_collapsed )
{
	uint16_t i;
	int offset = pLcd->height;
	float scale = -320/4096.0f;

	for( i = 0; i < pThis->len; i++ )
	{
		int x;
	    if( is_collapsed )
	    {
	        x = (i/2 * pLcd->width) / pThis->len;
	    }
	    else
	    {
	        x = (i * pLcd->width) / pThis->len;
	    }
		lcd_set_pixel( pLcd, x, scale*pThis->buffer1[i]+offset, color1 );
		lcd_set_pixel( pLcd, x, scale*pThis->buffer2[i]+offset, color2 );
	}
}
