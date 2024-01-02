/*
 * framebuf.c
 *
 *  Created on: 7 jul. 2023
 *      Author: Jose
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "framebuf.h"
#define CLAMP(x, a, b) ((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))

void framebuf_init(tFramebuf *fb, uint16_t width, uint16_t height, uint16_t *buf )
{
    fb->width = width;
    fb->height = height;
	fb->buf = buf;
}

void framebuf_fill( const tFramebuf *pThis, uint16_t color )
{
	uint32_t i;
	uint32_t *buf = (uint32_t*)pThis->buf;
	uint32_t color32 = (color<<16) | color;

	for( i=pThis->width*pThis->height/2 ; i ; i-- )
	{
		*buf++ = color32;
	}
}

void framebuf_pixel( const tFramebuf *pThis, int16_t x, int16_t y, uint16_t color )
{
    if( 0 <= x && x < pThis->width )
    {
        if( 0 <= y && y < pThis->height )
        {
        	pThis->buf[y*pThis->width+x] = color;
        }
    }
}

uint16_t framebuf_get_pixel( const tFramebuf *pThis, int16_t x, int16_t y )
{
	uint16_t color = 0;
    if( 0 <= x && x < pThis->width )
    {
        if( 0 <= y && y < pThis->height )
        {
        	color = pThis->buf[y*pThis->width+x];
        }
    }
    return color;
}
void framebuf_hline( const tFramebuf *pThis, int16_t x, int16_t y, int16_t width, uint16_t color )
{
	//|| x + width <= 0 ||  y >= pThis->height || x >= pThis->width)
	if( width < 1 ||
		x + width < 0 ||
		x >= pThis->width ||
		y < 0 ||
		y >= pThis->height )
	{
		return;
	}
    uint16_t *buf = pThis->buf + y*pThis->width + x;
    while( width-- )
    {
        *buf++ = color;
    }
}

void framebuf_vline( const tFramebuf *pThis, int16_t x, int16_t y, int16_t height, uint16_t color )
{
	if( height < 1 ||
		y + height < 0 ||
		y >= pThis->height ||
		x < 0 ||
		x >= pThis->width )
	{
		return;
	}
    uint16_t *buf = pThis->buf + y*pThis->width + x;
    uint16_t width = pThis->width;
    while( height-- )
    {
        *buf = color;
        buf += width;
    }
}
#define MIN(a,b) (a<b?a:b)
#define MAX(a,b) (a>b?a:b)
void _framebuf_fill_rect( const tFramebuf *pThis, int16_t x, int16_t y, int16_t width, int16_t height, uint16_t color )
{
	if (height < 1 || width < 1 || x + width <= 0 || y + height <= 0 || y >= pThis->height || x >= pThis->width) {
		return;
	}
	int xend = MIN(pThis->width, x + width);
	int yend = MIN(pThis->height, y + height);
	x = MAX(x, 0);
	y = MAX(y, 0);
	width = xend - x;
	height = yend - y;

	if( width > height )
	{

		while( height-- )
		{
		    uint16_t *buf = pThis->buf + y*pThis->width + x;
		    uint16_t width2 = width;
		    while( width2-- )
		    {
		        *buf++ = color;
		    }
			y++;
		}
	}
	else
	{
		while( width-- )
		{
		    uint16_t *buf = pThis->buf + y*pThis->width + x;
		    uint16_t pthis_width = pThis->width;
		    uint16_t height2 = height;
		    while( height2-- )
		    {
		        *buf = color;
		        buf += pthis_width;
		    }
			x++;
		}
	}
}



void framebuf_rect( const tFramebuf *pThis, int16_t x, int16_t y, int16_t width, int16_t height, uint8_t filled, uint16_t color )
{
	if( filled )
	{
		_framebuf_fill_rect( pThis, x, y, width, height, color );
	}
	else
	{
		_framebuf_fill_rect( pThis, x, y, width, 1, color );
		_framebuf_fill_rect( pThis, x, y+height-1, width, 1, color );
		_framebuf_fill_rect( pThis, x, y, 1, height, color );
		_framebuf_fill_rect( pThis, x+width-1, y, 1, height, color );
	}
}

void framebuf_line( const tFramebuf *fb, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color )
{
	int16_t dx = abs(x1-x0);
	int16_t sx = x0<x1 ? 1 : -1;
	int16_t dy = abs(y1-y0);
	int16_t sy = y0<y1 ? 1 : -1;
	int16_t err = (dx>dy ? dx : -dy)/2, e2;

	for(;;)
	{
		framebuf_pixel( fb, x0, y0, color );
		if( x0==x1 && y0==y1 )
		{
			break;
		} 
		e2 = err;
		if( e2 >-dx )
		{
			err -= dy; 
			x0 += sx;
		}
		if( e2 < dy )
		{
			err += dx; 
			y0 += sy;
		}
	}
}

void framebuf_circle( const tFramebuf *fb, int16_t x, int16_t y, uint16_t radius, uint16_t filled, uint8_t quadrant, uint16_t color )
{
	int16_t f = 1 - radius;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * radius;
	int16_t x0 = 0;
	int16_t y0 = radius;

	if( quadrant & FRAMEBUF_CIRCLE_Q1 )
	{
		framebuf_pixel( fb, x, y + radius, color );
		framebuf_pixel( fb, x, y - radius, color );
		framebuf_pixel( fb, x + radius, y, color );
		framebuf_pixel( fb, x - radius, y, color );
	}
	if( quadrant & FRAMEBUF_CIRCLE_Q2 )
	{
		framebuf_pixel( fb, x, y + radius, color );
		framebuf_pixel( fb, x, y - radius, color );
		framebuf_pixel( fb, x + radius, y, color );
		framebuf_pixel( fb, x - radius, y, color );
	}
	if( quadrant & FRAMEBUF_CIRCLE_Q3 )
	{
		framebuf_pixel( fb, x, y + radius, color );
		framebuf_pixel( fb, x, y - radius, color );
		framebuf_pixel( fb, x + radius, y, color );
		framebuf_pixel( fb, x - radius, y, color );
	}
	if( quadrant & FRAMEBUF_CIRCLE_Q4 )
	{
		framebuf_pixel( fb, x, y + radius, color );
		framebuf_pixel( fb, x, y - radius, color );
		framebuf_pixel( fb, x + radius, y, color );
		framebuf_pixel( fb, x - radius, y, color );
	}

	while( x0 < y0 )
	{
		if( f >= 0 )
		{
			y0--;
			ddF_y += 2;
			f += ddF_y;
		}
		x0++;
		ddF_x += 2;
		f += ddF_x;    
		if( quadrant & FRAMEBUF_CIRCLE_Q1 )
		{
			if( filled )
			{
				framebuf_hline( fb, x - x0, y + y0, 2*x0, color );
				framebuf_hline( fb, x - x0, y - y0, 2*x0, color );
			}
			else
			{
				framebuf_pixel( fb, x + x0, y + y0, color );
				framebuf_pixel( fb, x - x0, y + y0, color );
				framebuf_pixel( fb, x + x0, y - y0, color );
				framebuf_pixel( fb, x - x0, y - y0, color );
			}
		}
		if( quadrant & FRAMEBUF_CIRCLE_Q2 )
		{
			if( filled )
			{
				framebuf_hline( fb, x - y0, y + x0, 2*y0, color );
				framebuf_hline( fb, x - y0, y - x0, 2*y0, color );
			}
			else
			{
				framebuf_pixel( fb, x + y0, y + x0, color );
				framebuf_pixel( fb, x - y0, y + x0, color );
				framebuf_pixel( fb, x + y0, y - x0, color );
				framebuf_pixel( fb, x - y0, y - x0, color );
			}
		}
		if( quadrant & FRAMEBUF_CIRCLE_Q3 )
		{
			if( filled )
			{
				framebuf_hline( fb, x - x0, y + y0, 2*x0, color );
				framebuf_hline( fb, x - x0, y - y0, 2*x0, color );
			}
			else
			{
				framebuf_pixel( fb, x + x0, y + y0, color );
				framebuf_pixel( fb, x - x0, y + y0, color );
				framebuf_pixel( fb, x + x0, y - y0, color );
				framebuf_pixel( fb, x - x0, y - y0, color );
			}
		}
		if( quadrant & FRAMEBUF_CIRCLE_Q4 )
		{
			if( filled )
			{
				framebuf_hline( fb, x - y0, y + x0, 2*y0, color );
				framebuf_hline( fb, x - y0, y - x0, 2*y0, color );
			}
			else
			{
				framebuf_pixel( fb, x + y0, y + x0, color );
				framebuf_pixel( fb, x - y0, y + x0, color );
				framebuf_pixel( fb, x + y0, y - x0, color );
				framebuf_pixel( fb, x - y0, y - x0, color );
			}
		}
	}
}

void framebuf_text( const tFramebuf *pThis, const tFont *pFont, int16_t x, int16_t y, const uint8_t *text, uint16_t color )
{
	font_draw_text( pFont, x, y, text, color, framebuf_pixel, pThis );
}

