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

#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))

void setpixel(const tFramebuf *fb, unsigned int x, unsigned int y, uint32_t col) {
	((uint16_t *)fb->buf)[x + y * fb->stride] = col;
}

void setpixel_checked(const tFramebuf *fb, int x, int y, int col, int mask) {
    if (mask && 0 <= x && x < fb->width && 0 <= y && y < fb->height) {
        setpixel(fb, x, y, col);
    }
}

uint32_t getpixel(const tFramebuf *fb, unsigned int x, unsigned int y) {
	return ((uint16_t *)fb->buf)[x + y * fb->stride];
}

void fill_rect(const tFramebuf *fb, int x, int y, int w, int h, uint32_t col) {
    if (h < 1 || w < 1 || x + w <= 0 || y + h <= 0 || y >= fb->height || x >= fb->width) {
        // No operation needed.
        return;
    }

    // clip to the framebuffer
    int xend = MIN(fb->width, x + w);
    int yend = MIN(fb->height, y + h);
    x = MAX(x, 0);
    y = MAX(y, 0);

    w = xend - x;
    h = yend - y;
    uint16_t *b = &((uint16_t *)fb->buf)[x + y * fb->stride];
    while (h--){
    	for (unsigned int ww = w; ww; --ww) {
            *b++ = col;
        }
        b += fb->stride - w;
    }
}

void framebuf_init(tFramebuf *fb, uint16_t width, uint16_t height, char *buf ) {
    fb->width = width;
    fb->height = height;
	fb->buf = buf;
	fb->stride = fb->width;
}

void framebuf_fill( const tFramebuf *fb, uint32_t col ) {
    fill_rect( fb, 0, 0, fb->width, fb->height, col);
}

void framebuf_fill_rect( const tFramebuf *fb, int x, int y, int w, int h, uint32_t col ) {
    fill_rect( fb, x, y, w, h, col );
}

void framebuf_pixel( const tFramebuf *fb, int x, int y, uint32_t col ) {
    if (0 <= x && x < fb->width && 0 <= y && y < fb->height) {
		setpixel( fb, x, y,  col );
    }
}

void framebuf_hline( const tFramebuf *fb, int x, int y, int w, uint32_t col ) {
	fill_rect( fb, x, y, w, 1, col );
}

void framebuf_vline( const tFramebuf *fb, int x, int y, int h, uint32_t col ) {
	fill_rect( fb, x, y, 1, h, col );
}

void framebuf_rect( const tFramebuf *fb, int x, int y, int w, int h, uint32_t col ) {
	fill_rect( fb, x, y, w, 1, col );
	fill_rect( fb, x, y + h - 1, w, 1, col );
	fill_rect( fb, x, y, 1, h, col );
	fill_rect( fb, x + w - 1, y, 1, h, col );
}

void line(const tFramebuf *fb, int x1, int y1, int x2, int y2, uint32_t col) {
	int dx = x2 - x1;
	int sx;
    if (dx > 0) {
        sx = 1;
    } else {
        dx = -dx;
        sx = -1;
    }

    int dy = y2 - y1;
    int sy;
    if (dy > 0) {
        sy = 1;
    } else {
        dy = -dy;
        sy = -1;
    }

    int steep;
    if (dy > dx) {
        int temp;
        temp = x1;
        x1 = y1;
        y1 = temp;
        temp = dx;
        dx = dy;
        dy = temp;
        temp = sx;
        sx = sy;
        sy = temp;
        steep = 1;
    } else {
        steep = 0;
    }

    int e = 2 * dy - dx;
    for (int i = 0; i < dx; ++i) {
        if (steep) {
            if (0 <= y1 && y1 < fb->width && 0 <= x1 && x1 < fb->height) {
                setpixel(fb, y1, x1, col);
            }
        } else {
            if (0 <= x1 && x1 < fb->width && 0 <= y1 && y1 < fb->height) {
                setpixel(fb, x1, y1, col);
            }
        }
        while (e >= 0) {
            y1 += sy;
            e -= 2 * dx;
        }
        x1 += sx;
        e += 2 * dy;
    }

    if (0 <= x2 && x2 < fb->width && 0 <= y2 && y2 < fb->height) {
        setpixel(fb, x2, y2, col);
    }
}

void framebuf_line(const tFramebuf *fb, int x1, int y1, int x2, int y2, uint32_t col) {
    line( fb, x1, y1, x2, y2, col );
}
/*
// Q2 Q1
// Q3 Q4
#define ELLIPSE_MASK_FILL (0x10)
#define ELLIPSE_MASK_ALL (0x0f)
#define ELLIPSE_MASK_Q1 (0x01)
#define ELLIPSE_MASK_Q2 (0x02)
#define ELLIPSE_MASK_Q3 (0x04)
#define ELLIPSE_MASK_Q4 (0x08)

void draw_ellipse_points(const tFramebuf *fb, int cx, int cy, int x, int y, int col, int mask) {
    if (mask & ELLIPSE_MASK_FILL) {
        if (mask & ELLIPSE_MASK_Q1) {
            fill_rect(fb, cx, cy - y, x + 1, 1, col);
        }
        if (mask & ELLIPSE_MASK_Q2) {
            fill_rect(fb, cx - x, cy - y, x + 1, 1, col);
        }
        if (mask & ELLIPSE_MASK_Q3) {
            fill_rect(fb, cx - x, cy + y, x + 1, 1, col);
        }
        if (mask & ELLIPSE_MASK_Q4) {
            fill_rect(fb, cx, cy + y, x + 1, 1, col);
        }
    } else {
        setpixel_checked(fb, cx + x, cy - y, col, mask & ELLIPSE_MASK_Q1);
        setpixel_checked(fb, cx - x, cy - y, col, mask & ELLIPSE_MASK_Q2);
        setpixel_checked(fb, cx - x, cy + y, col, mask & ELLIPSE_MASK_Q3);
        setpixel_checked(fb, cx + x, cy + y, col, mask & ELLIPSE_MASK_Q4);
    }
}

void framebuf_ellipse(const tFramebuf *fb, int cx, int cy, int xr, int yr, int col, int mask) {
	int two_asquare = 2 * xr * xr;
	int two_bsquare = 2 * yr * yr;
	int x = xr;
	int y = 0;
	int xchange = yr * yr * (1 - 2 * xr);
	int ychange = xr * xr;
	int ellipse_error = 0;
	int stoppingx = two_bsquare * xr;
	int stoppingy = 0;
    while (stoppingx >= stoppingy) {   // 1st set of points,  y' > -1
        draw_ellipse_points(fb, cx, cy, x, y, col, mask);
        y += 1;
        stoppingy += two_asquare;
        ellipse_error += ychange;
        ychange += two_asquare;
        if ((2 * ellipse_error + xchange) > 0) {
            x -= 1;
            stoppingx -= two_bsquare;
            ellipse_error += xchange;
            xchange += two_bsquare;
        }
    }
    // 1st point set is done start the 2nd set of points
    x = 0;
    y =yr;
    xchange = yr * yr;
    ychange = xr * xr * (1 - 2 * yr);
    ellipse_error = 0;
    stoppingx = 0;
    stoppingy = two_asquare * yr;
    while (stoppingx <= stoppingy) {  // 2nd set of points, y' < -1
        draw_ellipse_points(fb, cx, cy, x, y, col, mask);
        x += 1;
        stoppingx += two_bsquare;
        ellipse_error += xchange;
        xchange += two_bsquare;
        if ((2 * ellipse_error + ychange) > 0) {
            y -= 1;
            stoppingy -= two_asquare;
            ellipse_error += ychange;
            ychange += two_asquare;
        }
    }
}
*/

void framebuf_circle(const tFramebuf *fb, int xc, int yc, int radius, uint32_t col) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y) {
        framebuf_pixel(fb, xc + x, yc + y, col);
        framebuf_pixel(fb, xc - x, yc + y, col);
        framebuf_pixel(fb, xc + x, yc - y, col);
        framebuf_pixel(fb, xc - x, yc - y, col);
        framebuf_pixel(fb, xc + y, yc + x, col);
        framebuf_pixel(fb, xc - y, yc + x, col);
        framebuf_pixel(fb, xc + y, yc - x, col);
        framebuf_pixel(fb, xc - y, yc - x, col);

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}
#define QUADRANT_0   0x01
#define QUADRANT_90  0x02
#define QUADRANT_180 0x04
#define QUADRANT_270 0x08
void framebuf_circle_quadrant(const tFramebuf *fb, int xc, int yc, int radius, uint32_t col, uint8_t quadrant) {
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y) {
        if (quadrant & QUADRANT_0) {
            framebuf_pixel(fb, xc + x, yc + y, col);
            framebuf_pixel(fb, xc + y, yc + x, col);
        }
        if (quadrant & QUADRANT_90) {
            framebuf_pixel(fb, xc - x, yc + y, col);
            framebuf_pixel(fb, xc - y, yc + x, col);
        }
        if (quadrant & QUADRANT_180) {
            framebuf_pixel(fb, xc - x, yc - y, col);
            framebuf_pixel(fb, xc - y, yc - x, col);
        }
        if (quadrant & QUADRANT_270) {
            framebuf_pixel(fb, xc + x, yc - y, col);
            framebuf_pixel(fb, xc + y, yc - x, col);
        }

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void framebuf_fill_circle(const tFramebuf *fb, int xc, int yc, int radius, uint32_t col) {
    framebuf_hline(fb, xc - radius, yc, 2 * radius, col);

    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y) {
        if (y > 0) {
            framebuf_hline(fb, xc - x, yc - y, 2 * x, col);
            framebuf_hline(fb, xc - x, yc + y, 2 * x, col);
        }

        if (x > 0) {
            framebuf_hline(fb, xc - y, yc - x, 2 * y, col);
            framebuf_hline(fb, xc - y, yc + x, 2 * y, col);
        }

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void framebuf_fill_circle_quadrant(const tFramebuf *fb, int xc, int yc, int radius, uint32_t col, uint8_t quadrant) {
    framebuf_hline(fb, xc - radius, yc, 2 * radius, col);

    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;

    while (x <= y) {
        if (quadrant & QUADRANT_0) {
            if (y > 0) {
                framebuf_hline(fb, xc + x, yc - y, 2 * x, col);
                framebuf_hline(fb, xc + x, yc + y, 2 * x, col);
            }
        }
        if (quadrant & QUADRANT_90) {
            if (x > 0) {
                framebuf_hline(fb, xc - y, yc - x, 2 * y, col);
                framebuf_hline(fb, xc - y, yc + x, 2 * y, col);
            }
        }
        if (quadrant & QUADRANT_180) {
            if (y > 0) {
                framebuf_hline(fb, xc - x, yc - y, 2 * x, col);
                framebuf_hline(fb, xc - x, yc + y, 2 * x, col);
            }
        }
        if (quadrant & QUADRANT_270) {
            if (x > 0) {
                framebuf_hline(fb, xc + y, yc - x, 2 * y, col);
                framebuf_hline(fb, xc + y, yc + x, 2 * y, col);
            }
        }

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

extern const uint8_t font_petme128_8x8[];

void framebuf_text( const tFramebuf *fb, int x0, int y0, char *str, uint32_t col ) {
    // loop over chars
    for (; *str; ++str) {
        // get char and make sure its in range of font
        int chr = *(uint8_t *)str;
        if (chr < 32 || chr > 127) {
            chr = 127;
        }
        // get char data
        const uint8_t *chr_data = &font_petme128_8x8[(chr - 32) * 8];
        // loop over char data
        for (int j = 0; j < 8; j++, x0++) {
            if (0 <= x0 && x0 < fb->width) { // clip x
                unsigned int vline_data = chr_data[j]; // each byte is a column of 8 pixels, LSB at top
                for (int y = y0; vline_data; vline_data >>= 1, y++) { // scan over vertical column
                    if (vline_data & 1) { // only draw if pixel set
                        if (0 <= y && y < fb->height) { // clip y
                            setpixel(fb, x0, y, col);
                        }
                    }
                }
            }
        }
    }
}
