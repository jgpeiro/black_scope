/*
 * scope_draw.h
 *
 *  Created on: Sep 3, 2023
 *      Author: jgpei
 */

#ifndef _SCOPE_DRAW_H_
#define _SCOPE_DRAW_H_

#include "scope.h"

#define SCOPE_COLOR_ACQUIRE    	LCD_COLOR_RED
#define SCOPE_COLOR_HORIZONTAL 	LCD_COLOR_GREEN
#define SCOPE_COLOR_VERTICAL   	LCD_COLOR_BLUE
#define SCOPE_COLOR_TRIGGER    	LCD_COLOR_YELLOW

#define SCOPE_COLOR_CH1			LCD_COLOR_RED
#define SCOPE_COLOR_CH2			LCD_COLOR_GREEN
#define SCOPE_COLOR_CH3			LCD_COLOR_BLUE
#define SCOPE_COLOR_CH4			LCD_COLOR_MAGENTA

#define SCOPE_COLOR_GRID 		0x8410
#define SCOPE_COLOR_BACKGROUND  LCD_COLOR_BLACK

void scope_draw( tScope *pThis, tLcd *pLcd,
		uint8_t acquire_is_visible,
		uint8_t horizontal_is_visible,
		uint8_t vertical_is_visible,
		uint8_t trigger_is_visible,
		int is_collapsed );
void scope_erase( tScope *pThis, tLcd *pLcd,
		uint8_t acquire_is_visible,
		uint8_t horizontal_is_visible,
		uint8_t vertical_is_visible,
		uint8_t trigger_is_visible,
		int is_collapsed );

void scope_draw_acquire( tScope *pThis, tLcd *pLcd, int is_collapsed );
void scope_draw_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, int is_collapsed );
void scope_draw_vertical( tScope_Vertical *pThis, tLcd *pLcd, int is_collapsed );
void scope_draw_trigger( tScope_Trigger *pThis, tLcd *pLcd, int is_collapsed );

void scope_erase_acquire( tScope *pThis, tLcd *pLcd, int is_collapsed );
void scope_erase_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, int is_collapsed );
void scope_erase_vertical( tScope_Vertical *pThis, tLcd *pLcd, int is_collapsed );
void scope_erase_trigger( tScope_Trigger *pThis, tLcd *pLcd, int is_collapsed );

void scope_stroque_acquire( tScope *pThis, tLcd *pLcd, uint16_t color, int is_collapsed );
void scope_stroque_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, uint16_t color, int is_collapsed );
void scope_stroque_vertical( tScope_Vertical *pThis, tLcd *pLcd, uint16_t color,
		uint16_t color1, uint16_t color2, uint16_t color3, uint16_t color4,
		int is_collapsed );
void scope_stroque_trigger( tScope_Trigger *pThis, tLcd *pLcd, uint16_t color, int is_collapsed );

void scope_draw_signals( tScope *pThis, tLcd *pLcd, int is_collapsed );
void scope_draw_grid( tScope *pThis, tLcd *pLcd, int is_collapsed );
void draw_marker( tLcd *pLcd, uint16_t x, uint16_t y, uint16_t direction, uint16_t color );


#endif /* _SCOPE_DRAW_H_ */
