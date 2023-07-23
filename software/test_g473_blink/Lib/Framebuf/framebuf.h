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

#define ELLIPSE_MASK_FILL (0x10)
#define ELLIPSE_MASK_ALL (0x0f)
#define ELLIPSE_MASK_Q1 (0x01)
#define ELLIPSE_MASK_Q2 (0x02)
#define ELLIPSE_MASK_Q3 (0x04)
#define ELLIPSE_MASK_Q4 (0x08)
extern const uint8_t font_petme128_8x8[];


struct sFramebuf
{
    void *buf;
    uint16_t width, height, stride;
};
typedef struct sFramebuf tFramebuf;


void setpixel(const tFramebuf *fb, unsigned int x, unsigned int y, uint32_t col);
void setpixel_checked(const tFramebuf *fb, int x, int y, int col, int mask);
uint32_t getpixel(const tFramebuf *fb, unsigned int x, unsigned int y);
void fill_rect(const tFramebuf *fb, int x, int y, int w, int h, uint32_t col);
void framebuf_init(tFramebuf *fb, uint16_t width, uint16_t height, char *buf );
void framebuf_fill( const tFramebuf *fb, uint32_t col );
void framebuf_fill_rect( const tFramebuf *fb, int x, int y, int w, int h, uint32_t col );
void framebuf_pixel( const tFramebuf *fb, int x, int y, uint32_t col );
void framebuf_hline( const tFramebuf *fb, int x, int y, int w, uint32_t col );
void framebuf_vline( const tFramebuf *fb, int x, int y, int h, uint32_t col );
void framebuf_rect( const tFramebuf *fb, int x, int y, int w, int h, uint32_t col );
void line(const tFramebuf *fb, int x1, int y1, int x2, int y2, uint32_t col);
void framebuf_line(const tFramebuf *fb, int x1, int y1, int x2, int y2, uint32_t col);
//void draw_ellipse_points(const tFramebuf *fb, int cx, int cy, int x, int y, int col, int mask);
//void framebuf_ellipse(const tFramebuf *fb, int cx, int cy, int xr, int yr, int col, int mask);
void framebuf_circle(const tFramebuf *fb, int xc, int yc, int radius, uint32_t col );
void framebuf_fill_circle(const tFramebuf *fb, int xc, int yc, int radius, uint32_t col );
//void framebuf_text( const tFramebuf *fb, int x0, int y0, char *str, uint32_t col );

#define QUADRANT_0   0x01
#define QUADRANT_90  0x02
#define QUADRANT_180 0x04
#define QUADRANT_270 0x08
void framebuf_circle_quadrant(const tFramebuf *fb, int xc, int yc, int radius, uint32_t col, uint8_t quadrant);
void framebuf_fill_circle_quadrant(const tFramebuf *fb, int xc, int yc, int radius, uint32_t col, uint8_t quadrant );

void framebuf_text( const tFramebuf *fb, const tFont *pFont, int x0, int y0, char *str, uint16_t color );
void framebuf_char( const tFramebuf *fb, const tFont *pFont, int16_t x0, int16_t y0, uint8_t c, uint16_t color );


#endif /* FRAMEBUF_H_ */
