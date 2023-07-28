/*
 * nuklear_glue.c
 *
 *  Created on: Jul 27, 2023
 *      Author: jgpei
 */

#include "nuklear.h"
#include "framebuf.h"
#include "lcd.h"
#include "FontUbuntuBookRNormal16.h"

uint32_t nk_color_to_rgb565( struct nk_color color )
{
	uint32_t rgb565 = 0;

	color.r = (color.r*color.a)/256.0;
	color.g = (color.g*color.a)/256.0;
	color.b = (color.b*color.a)/256.0;

	color.r >>= 3;
	color.g >>= 2;
	color.b >>= 3;

	rgb565 = (color.b<<11) | (color.g<<5) | (color.r<<0);

	return rgb565;
}

void nk_draw_fb(struct nk_context *ctx, tFramebuf *pfb, tLcd *pLcd) {
	int y0;
	const struct nk_command *cmd = NULL;
	for (y0 = 0; y0 < pLcd->height; y0 += pfb->height) {
		framebuf_fill(pfb, 0x00000000);

		for (cmd = nk__begin(ctx); cmd != 0; cmd = nk__next(ctx, cmd)) {
			//printf( "cmd->type = %d\n", cmd->type );
			switch (cmd->type) {
			case NK_COMMAND_NOP:
				break;
			case NK_COMMAND_SCISSOR:
				break;
			case NK_COMMAND_LINE: {
				const struct nk_command_line *l =
						(const struct nk_command_line*) cmd;
				if (l->begin.y == l->end.y) {
					framebuf_hline(pfb, l->begin.x, l->begin.y - y0,
							l->end.x - l->begin.x,
							nk_color_to_rgb565(l->color));
				} else if (l->begin.x == l->end.x) {
					framebuf_vline(pfb, l->begin.x, l->begin.y - y0,
							l->end.y - l->begin.y,
							nk_color_to_rgb565(l->color));
				} else {
					framebuf_line(pfb, l->begin.x, l->begin.y - y0, l->end.x,
							l->end.y - y0, nk_color_to_rgb565(l->color));
				}
			}
				break;

			case NK_COMMAND_RECT: {
				const struct nk_command_rect *r =
						(const struct nk_command_rect*) cmd;
				int rad = 4; //r->rounding;
				framebuf_circle_quadrant(pfb, r->x + r->w - rad,
						r->y - y0 + r->h - rad, rad,
						nk_color_to_rgb565(r->color), QUADRANT_0);
				framebuf_circle_quadrant(pfb, r->x + rad,
						r->y - y0 + r->h - rad, rad,
						nk_color_to_rgb565(r->color), QUADRANT_90);
				framebuf_circle_quadrant(pfb, r->x + rad, r->y - y0 + rad, rad,
						nk_color_to_rgb565(r->color), QUADRANT_180);
				framebuf_circle_quadrant(pfb, r->x + r->w - rad,
						r->y - y0 + rad, rad, nk_color_to_rgb565(r->color),
						QUADRANT_270);

				framebuf_hline(pfb, r->x + rad, r->y - y0, r->w - rad - rad,
						nk_color_to_rgb565(r->color));
				framebuf_hline(pfb, r->x + rad, r->y - y0 + r->h,
						r->w - rad - rad, nk_color_to_rgb565(r->color));
				framebuf_vline(pfb, r->x, r->y - y0 + rad, r->h - rad - rad,
						nk_color_to_rgb565(r->color));
				framebuf_vline(pfb, r->x + r->w, r->y - y0 + rad,
						r->h - rad - rad, nk_color_to_rgb565(r->color));
			}
				break;
			case NK_COMMAND_RECT_FILLED: {
				const struct nk_command_rect_filled *r =
						(const struct nk_command_rect_filled*) cmd;
//framebuf_fill_rect( pfb, r->x, r->y-y0, r->w, r->h, nk_colot_to_rgb666( r->color ) );
				int rad = 4; //r->rounding;
				framebuf_fill_circle_quadrant(pfb, r->x + r->w - rad,
						r->y - y0 + r->h - rad, rad,
						nk_color_to_rgb565(r->color), QUADRANT_90);
				framebuf_fill_circle_quadrant(pfb, r->x + rad,
						r->y - y0 + r->h - rad, rad,
						nk_color_to_rgb565(r->color), QUADRANT_90);
				framebuf_fill_circle_quadrant(pfb, r->x + rad, r->y - y0 + rad,
						rad, nk_color_to_rgb565(r->color), QUADRANT_90);
				framebuf_fill_circle_quadrant(pfb, r->x + r->w - rad,
						r->y - y0 + rad, rad, nk_color_to_rgb565(r->color),
						QUADRANT_90);
// up and down
				framebuf_fill_rect(pfb, r->x + rad, r->y - y0, r->w - rad - rad,
						rad, nk_color_to_rgb565(r->color));
				framebuf_fill_rect(pfb, r->x + rad, r->y - y0 + r->h - rad,
						r->w - rad - rad, rad, nk_color_to_rgb565(r->color));
// middle
				framebuf_fill_rect(pfb, r->x, r->y - y0 + rad, r->w,
						r->h - rad - rad, nk_color_to_rgb565(r->color));
			}
				break;
			case NK_COMMAND_CIRCLE: {
				const struct nk_command_circle *c =
						(const struct nk_command_circle*) cmd;
				framebuf_circle(pfb, c->x + (c->w + c->h) / 4,
						c->y - y0 + (c->w + c->h) / 4, (c->w + c->h) / 4,
						nk_color_to_rgb565(c->color));
			}
				break;
			case NK_COMMAND_CIRCLE_FILLED: {
				const struct nk_command_circle_filled *c =
						(const struct nk_command_circle_filled*) cmd;
				framebuf_fill_circle(pfb, c->x + (c->w + c->h) / 4,
						c->y - y0 + (c->w + c->h) / 4, (c->w + c->h) / 4,
						nk_color_to_rgb565(c->color));
			}
				break;
			case NK_COMMAND_TEXT: {
				const struct nk_command_text *t =
						(const struct nk_command_text*) cmd;
				framebuf_text(pfb, &fontUbuntuBookRNormal16, t->x, t->y - y0,
						(char*) t->string, nk_color_to_rgb565(t->foreground));
			}
				break;
			case NK_COMMAND_IMAGE: {
				const struct nk_command_image *i =
						(const struct nk_command_image*) cmd;
				framebuf_fill_rect(pfb, i->x, i->y - y0, i->w, i->h,
						nk_color_to_rgb565(i->col));
			}
				break;
			case NK_COMMAND_TRIANGLE_FILLED: {
				const struct nk_command_triangle_filled *t =
						(const struct nk_command_triangle_filled*) cmd;
				framebuf_line(pfb, t->a.x, t->a.y - y0, t->b.x, t->b.y - y0,
						nk_color_to_rgb565(t->color));
				framebuf_line(pfb, t->b.x, t->b.y - y0, t->c.x, t->c.y - y0,
						nk_color_to_rgb565(t->color));
				framebuf_line(pfb, t->c.x, t->c.y - y0, t->a.x, t->a.y - y0,
						nk_color_to_rgb565(t->color));
			}
				break;
			default:
				break;
			}
		}
		lcd_bmp(pLcd, 0, y0, pfb->width, pfb->height, pfb->buf);
	}
}
