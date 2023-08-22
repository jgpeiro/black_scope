/*
 * framebuf.h
 *
 *  Created on: 10 jul. 2023
 *      Author: Jose
 */

#ifndef FRAMEBUF_H_
#define FRAMEBUF_H_

#include <stdint.h>
#include "font.h"

#define FRAMEBUF_CIRCLE_Q1 (0x01)
#define FRAMEBUF_CIRCLE_Q2 (0x02)
#define FRAMEBUF_CIRCLE_Q3 (0x04)
#define FRAMEBUF_CIRCLE_Q4 (0x08)
#define FRAMEBUF_CIRCLE_QALL (0x0F)

struct sFramebuf
{
	uint16_t *buf;
    uint16_t width, height;
};
typedef struct sFramebuf tFramebuf;

void framebuf_init(tFramebuf *fb, uint16_t width, uint16_t height, uint16_t *buf );
void framebuf_fill( const tFramebuf *pThis, uint16_t color );
void framebuf_pixel( const tFramebuf *pThis, int16_t x, int16_t y, uint16_t color );
void framebuf_hline( const tFramebuf *pThis, int16_t x, int16_t y, int16_t width, uint16_t color );
void framebuf_vline( const tFramebuf *pThis, int16_t x, int16_t y, int16_t height, uint16_t color );
void _framebuf_fill_rect( const tFramebuf *pThis, int16_t x, int16_t y, int16_t width, int16_t height, uint16_t color );
void framebuf_rect( const tFramebuf *pThis, int16_t x, int16_t y, int16_t width, int16_t height, uint8_t filled, uint16_t color );
void framebuf_line( const tFramebuf *pThis, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color );
void framebuf_circle( const tFramebuf *pThis, int16_t x, int16_t y, uint16_t radius, uint16_t filled, uint8_t quadrant, uint16_t color );
void framebuf_text( const tFramebuf *pThis, const tFont *pFont, int16_t x, int16_t y, const uint8_t *text, uint16_t color );

#endif /* FRAMEBUF_H_ */
