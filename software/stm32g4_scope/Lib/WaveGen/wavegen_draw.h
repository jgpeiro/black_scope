/*
 * wavegen_draw.h
 *
 *  Created on: Sep 3, 2023
 *      Author: jgpei
 */

#ifndef _WAVEGEN_DRAW_H_
#define _WAVEGEN_DRAW_H_

#include "wavegen.h"
#include "lcd.h"

#define WAVEGEN_COLOR_CH1	LCD_COLOR_CYAN
#define WAVEGEN_COLOR_CH2	LCD_COLOR_MAGENTA

void wavegen_draw( tWaveGen *pThis, tLcd *pLcd, int is_collapsed );
void wavegen_erase( tWaveGen *pThis, tLcd *pLcd, int is_collapsed );
void wavegen_stroque( tWaveGen *pThis, tLcd *pLcd, uint16_t color1, uint16_t color2, int is_collapsed );

#endif /* _WAVEGEN_DRAW_H_ */
