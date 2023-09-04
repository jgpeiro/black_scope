/*
 * scope_draw.c
 *
 *  Created on: Sep 3, 2023
 *      Author: jgpei
 */


#include "scope_draw.h"

#include "font.h"
#include	"FontUbuntuBookRNormal16.h"

#define MARKER_ORIENTATION_NORTH 0
#define MARKER_ORIENTATION_SOUTH 1
#define MARKER_ORIENTATION_EAST  2
#define MARKER_ORIENTATION_WEST  3

void draw_marker( tLcd *pLcd, uint16_t x, uint16_t y, uint16_t direction, uint16_t color )
{
	int w = 0;
	int h = 0;

	w = 10;
	h = 10;

	lcd_rect( pLcd, x-w/2, y-h/2, w, h, color );

	switch( direction )
	{
		case MARKER_ORIENTATION_NORTH:
			for( int i = 0; i < w/2; i++ )
			{
				lcd_hline( pLcd, x-w/2+i, y+h/2+i, 2*i, color );
			}
			break;
		case MARKER_ORIENTATION_SOUTH:
			for( int i = 0; i < w/2; i++ )
			{
				lcd_hline( pLcd, x-w/2+i, y-h/2-i, 2*i, color );
			}
			break;
		case MARKER_ORIENTATION_EAST:
			for( int i = 0; i < w/2; i++ )
			{
				lcd_vline( pLcd, x+w/2-i, y+h/2+i, 2*i, color );
			}
			break;
		case MARKER_ORIENTATION_WEST:
			for( int i = 0; i < w/2; i++ )
			{
				lcd_vline( pLcd, x+w/2-i, y-h/2-i, 2*i, color );
			}
			break;
		default:
			break;
	}
}


void scope_draw( tScope *pThis, tLcd *pLcd,
		uint8_t acquire_is_visible,
		uint8_t horizontal_is_visible,
		uint8_t vertical_is_visible,
		uint8_t trigger_is_visible,
		int is_collapsed )
{
	if( acquire_is_visible )
	{
		scope_draw_acquire( pThis, pLcd, is_collapsed );
	}
	if( horizontal_is_visible )
	{
		scope_draw_horizontal( &pThis->horizontal, pLcd, is_collapsed );
	}
	if( vertical_is_visible )
	{
		scope_draw_vertical( &pThis->vertical, pLcd, is_collapsed );
	}
	if( trigger_is_visible )
	{
		scope_draw_trigger( &pThis->trigger, pLcd, is_collapsed );
	}
}
void scope_erase( tScope *pThis, tLcd *pLcd,
		uint8_t acquire_is_visible,
		uint8_t horizontal_is_visible,
		uint8_t vertical_is_visible,
		uint8_t trigger_is_visible,
		int is_collapsed )
{
	if( acquire_is_visible )
	{
		scope_erase_acquire( pThis, pLcd, is_collapsed );
	}
	if( horizontal_is_visible )
	{
		scope_erase_horizontal( &pThis->horizontal, pLcd, is_collapsed );
	}
	if( vertical_is_visible )
	{
		scope_erase_vertical( &pThis->vertical, pLcd, is_collapsed );
	}
	if( trigger_is_visible )
	{
		scope_erase_trigger( &pThis->trigger, pLcd, is_collapsed );
	}
}


void scope_draw_acquire( tScope *pThis, tLcd *pLcd, int is_collapsed )
{
	scope_stroque_acquire( pThis, pLcd, SCOPE_COLOR_ACQUIRE, is_collapsed );
}
void scope_draw_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque_horizontal( pThis, pLcd, SCOPE_COLOR_HORIZONTAL, is_collapsed );

}

void scope_draw_vertical( tScope_Vertical *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque_vertical( pThis, pLcd, SCOPE_COLOR_VERTICAL, SCOPE_COLOR_CH1, SCOPE_COLOR_CH2, SCOPE_COLOR_CH3, SCOPE_COLOR_CH4, is_collapsed );

}

void scope_draw_trigger( tScope_Trigger *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque_trigger( pThis, pLcd, SCOPE_COLOR_TRIGGER, is_collapsed );

}


void scope_erase_acquire( tScope *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque_acquire( pThis, pLcd, SCOPE_COLOR_BACKGROUND, is_collapsed );

}

void scope_erase_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque_horizontal( pThis, pLcd, SCOPE_COLOR_BACKGROUND, is_collapsed );

}

void scope_erase_vertical( tScope_Vertical *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque_vertical( pThis, pLcd, SCOPE_COLOR_BACKGROUND, SCOPE_COLOR_BACKGROUND, SCOPE_COLOR_BACKGROUND, SCOPE_COLOR_BACKGROUND, SCOPE_COLOR_BACKGROUND, is_collapsed );

}

void scope_erase_trigger( tScope_Trigger *pThis, tLcd *pLcd, int is_collapsed )
{
	   scope_stroque_trigger( pThis, pLcd, SCOPE_COLOR_BACKGROUND, is_collapsed );

}


void scope_stroque_acquire( tScope *pThis, tLcd *pLcd, uint16_t color, int is_collapsed )
{
    /*int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    x = pThis->state*10;
    y = 0;
    w = 10;
    h = 10;

    lcd_rect( pLcd, x, y, w, h, color );*/
}
void scope_stroque_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, uint16_t color, int is_collapsed )
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;

	if( is_collapsed )
	{
		x = -pThis->offset/4+120+10;
	}
	else
	{
		x = -pThis->offset/2+240+20;
	}
	y = 0;
	h = pLcd->height;

	lcd_vline( pLcd, x, y, h, color );

	// Draw a marker
	draw_marker( pLcd, x, 0, MARKER_ORIENTATION_SOUTH, color );
	font_draw_text( &fontUbuntuBookRNormal16, x+1, y+1, "H", color, lcd_set_pixel, pLcd );
}

void scope_stroque_vertical( tScope_Vertical *pThis, tLcd *pLcd, uint16_t color,
		uint16_t color1, uint16_t color2, uint16_t color3, uint16_t color4,
		int is_collapsed )
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;

	if( is_collapsed )
	{
		w = pLcd->width/2;
	}
	else
	{
		w = pLcd->width;
	}
	x = 0;
	y = pThis->offset;
	//w = pLcd->width/2;
	lcd_hline( pLcd, x, y, w, color );
	draw_marker( pLcd, x, y, MARKER_ORIENTATION_EAST, color );
	font_draw_text( &fontUbuntuBookRNormal16, x+1, y+1, "V", color, lcd_set_pixel, pLcd );

	x = 0;
	y = pThis->gain1;
	//w = pLcd->width/2;
	lcd_hline( pLcd, x, y, w, color1 );

	x = 0;
	y = pThis->gain1;
	//w = pLcd->width/2;
	lcd_hline( pLcd, x, y, w, color2 );

	x = 0;
	y = pThis->gain1;
	//w = pLcd->width/2;
	lcd_hline( pLcd, x, y, w, color3 );

	x = 0;
	y = pThis->gain1;
	//w = pLcd->width/2;
	lcd_hline( pLcd, x, y, w, color4 );
}

void scope_stroque_trigger( tScope_Trigger *pThis, tLcd *pLcd, uint16_t color, int is_collapsed )
{
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;

	x = 0;
	y = pLcd->height-pThis->level*pLcd->height/4096.0f;;
	if( is_collapsed )
	 {
		 w = pLcd->width/2;
	 }
	 else
	 {
		 w = pLcd->width;
	 }
	lcd_hline( pLcd, x, y, w, color );
	draw_marker( pLcd, x, y, MARKER_ORIENTATION_EAST, color );
	font_draw_text( &fontUbuntuBookRNormal16, x+1, y+1, "T", color, lcd_set_pixel, pLcd );
}



void scope_draw_grid( tScope *pThis, tLcd *pLcd, int is_collapsed )
{
	if( is_collapsed )
	{
		for( int d = 0 ; d < pLcd->width ; d += 40 )
		{
			lcd_rect( pLcd, d/2, 0, 1, pLcd->height, SCOPE_COLOR_GRID );
		}
		lcd_rect( pLcd, pLcd->width/2-1, 0, 1, pLcd->height, SCOPE_COLOR_GRID );

		for( int d = 0 ; d < pLcd->height ; d += 40 )
		{
			lcd_rect( pLcd, 0, d, pLcd->width/2, 1, SCOPE_COLOR_GRID );
		}
		lcd_rect( pLcd, 0, pLcd->height, pLcd->width/2, 1, SCOPE_COLOR_GRID );

	}
	else
	{
		for( int d = 0 ; d < pLcd->width ; d += 40 )
		{
			lcd_rect( pLcd, d, 0, 1, pLcd->height, SCOPE_COLOR_GRID );
		}
		lcd_rect( pLcd, pLcd->width-1, 0, 1, pLcd->height, SCOPE_COLOR_GRID );

		for( int d = 0 ; d < pLcd->height ; d += 40 )
		{
			lcd_rect( pLcd, 0, d, pLcd->width, 1, SCOPE_COLOR_GRID );
		}
		lcd_rect( pLcd, 0, pLcd->height, pLcd->width, 1, SCOPE_COLOR_GRID );

	}
}

#define ABS(a)   ((a>0)?(a):-(a))
#define MIN(a,b) ((a<b)?(a):(b))

void scope_draw_signals( tScope *pThis, tLcd *pLcd, int is_collapsed )
{
	uint16_t i;
	float scale = 320/4096.0f;
	int16_t trigger;
	static int16_t trigger_bck = 0;
	int16_t n, n_bck;
	int pLcd_height = pLcd->height;
	static int is_collapsed_bck = 0;

	uint16_t x;
	int16_t y1, y2, y3, y4, y5, y6, y7, y8;
	int16_t y1_bck = 0;
	int16_t y2_bck = 0;
	int16_t y3_bck = 0;
	int16_t y4_bck = 0;
	int16_t y5_bck = 0;
	int16_t y6_bck = 0;
	int16_t y7_bck = 0;
	int16_t y8_bck = 0;

	//lcd_rect( pLcd, 0, 0, 240, 320, 0 );
	trigger = (pThis->len - pThis->dma_cndtr);// - pThis->len/2;
	trigger = (pThis->len - pThis->dma_cndtr) - pThis->len/2 + pThis->len/2;
	trigger = trigger%pThis->len;
	if( pThis->cnt & 0x01 )
	{
		for( i = 0; i < pThis->len; i++ )
		{
			if( 1 )
			{
				n_bck = trigger_bck + i;// - pThis->horizontal.offset/2;
				if( n_bck < 0 )
				{
					n_bck += pThis->len;
				}
				else if( n_bck >= pThis->len )
				{
					n_bck -= pThis->len;
				}

				if( is_collapsed_bck )
				{
					x = (i)/2;
				}
				else
				{
					x = i;
				}
				y5 = pLcd_height-pThis->buffer5[n_bck]*scale;
				y6 = pLcd_height-pThis->buffer6[n_bck]*scale;
				y7 = pLcd_height-pThis->buffer7[n_bck]*scale;
				y8 = pLcd_height-pThis->buffer8[n_bck]*scale;

				lcd_rect( pLcd, x, MIN(y5,y5_bck), 1, ABS(y5-y5_bck)+1, LCD_COLOR_BLACK );
				lcd_rect( pLcd, x, MIN(y6,y6_bck), 1, ABS(y6-y6_bck)+1, LCD_COLOR_BLACK );
				lcd_rect( pLcd, x, MIN(y7,y7_bck), 1, ABS(y7-y7_bck)+1, LCD_COLOR_BLACK );
				lcd_rect( pLcd, x, MIN(y8,y8_bck), 1, ABS(y8-y8_bck)+1, LCD_COLOR_BLACK );

				y5_bck = y5;
				y6_bck = y6;
				y7_bck = y7;
				y8_bck = y8;
			}

			if( i > 2 )
			{
				n = trigger + (i-2);// - pThis->horizontal.offset/2;
				if( n < 0 )
				{
					n += pThis->len;
				}
				else if( n >= pThis->len )
				{
					n -= pThis->len;
				}

				if( is_collapsed )
				{
					x = (i-2)/2;
				}
				else
				{
					x = (i-2);
				}

				y1 = pLcd_height-pThis->buffer1[n]*scale;
				y2 = pLcd_height-pThis->buffer2[n]*scale;
				y3 = pLcd_height-pThis->buffer3[n]*scale;
				y4 = pLcd_height-pThis->buffer4[n]*scale;

				lcd_rect( pLcd, x, MIN(y1,y1_bck), 1, ABS(y1-y1_bck)+1, SCOPE_COLOR_CH1 );
				lcd_rect( pLcd, x, MIN(y2,y2_bck), 1, ABS(y2-y2_bck)+1, SCOPE_COLOR_CH2 );
				lcd_rect( pLcd, x, MIN(y3,y3_bck), 1, ABS(y3-y3_bck)+1, SCOPE_COLOR_CH3 );
				lcd_rect( pLcd, x, MIN(y4,y4_bck), 1, ABS(y4-y4_bck)+1, SCOPE_COLOR_CH4 );

				y1_bck = y1;
				y2_bck = y2;
				y3_bck = y3;
				y4_bck = y4;
			}
		}
	}
	else
	{
		for( i = 0; i < pThis->len; i++ )
		{
			if( 1 )
			{
				n_bck = trigger_bck + i;// - pThis->horizontal.offset/2;
				if( n_bck < 0 )
				{
					n_bck += pThis->len;
				}
				else if( n_bck >= pThis->len )
				{
					n_bck -= pThis->len;
				}

				if( is_collapsed_bck )
				{
					x = i/2;
				}
				else
				{
					x = i;
				}
				y1 = pLcd_height-pThis->buffer1[n_bck]*scale;
				y2 = pLcd_height-pThis->buffer2[n_bck]*scale;
				y3 = pLcd_height-pThis->buffer3[n_bck]*scale;
				y4 = pLcd_height-pThis->buffer4[n_bck]*scale;

				lcd_rect( pLcd, x, MIN(y1,y1_bck), 1, ABS(y1-y1_bck)+1, LCD_COLOR_BLACK );
				lcd_rect( pLcd, x, MIN(y2,y2_bck), 1, ABS(y2-y2_bck)+1, LCD_COLOR_BLACK );
				lcd_rect( pLcd, x, MIN(y3,y3_bck), 1, ABS(y3-y3_bck)+1, LCD_COLOR_BLACK );
				lcd_rect( pLcd, x, MIN(y4,y4_bck), 1, ABS(y4-y4_bck)+1, LCD_COLOR_BLACK );

				y1_bck = y1;
				y2_bck = y2;
				y3_bck = y3;
				y4_bck = y4;
			}

			if( i > 2 )
			{
				n = trigger + (i-2);// - pThis->horizontal.offset/2;
				if( n < 0 )
				{
					n += pThis->len;
				}
				else if( n >= pThis->len )
				{
					n -= pThis->len;
				}

				if( is_collapsed )
				{
					x = (i-2)/2;
				}
				else
				{
					x = (i-2);
				}
				y5 = pLcd_height-pThis->buffer5[n]*scale;
				y6 = pLcd_height-pThis->buffer6[n]*scale;
				y7 = pLcd_height-pThis->buffer7[n]*scale;
				y8 = pLcd_height-pThis->buffer8[n]*scale;

				lcd_rect( pLcd, x, MIN(y5,y5_bck), 1, ABS(y5-y5_bck)+1, SCOPE_COLOR_CH1 );
				lcd_rect( pLcd, x, MIN(y6,y6_bck), 1, ABS(y6-y6_bck)+1, SCOPE_COLOR_CH2 );
				lcd_rect( pLcd, x, MIN(y7,y7_bck), 1, ABS(y7-y7_bck)+1, SCOPE_COLOR_CH3 );
				lcd_rect( pLcd, x, MIN(y8,y8_bck), 1, ABS(y8-y8_bck)+1, SCOPE_COLOR_CH4 );

				y5_bck = y5;
				y6_bck = y6;
				y7_bck = y7;
				y8_bck = y8;
			}
		}
	}
	trigger_bck = trigger;
	is_collapsed_bck = is_collapsed;
}
