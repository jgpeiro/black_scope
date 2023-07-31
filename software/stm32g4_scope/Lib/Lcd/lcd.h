/*
 * ili9488.h
 *
 *  Created on: Jul 23, 2023
 *      Author: jgpei
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>
#include "stm32g4xx_hal.h"

struct sLcd
{
    GPIO_TypeDef *reset_port;
    uint16_t reset_pin;
    GPIO_TypeDef *bl_port;
    uint16_t bl_pin;

    uint16_t width;
    uint16_t height;
};

typedef struct sLcd tLcd;

void lcd_init( tLcd *pThis, GPIO_TypeDef *reset_port, uint16_t reset_pin, GPIO_TypeDef *bl_port, uint16_t bl_pin, uint16_t width, uint16_t height );
void lcd_reset( tLcd *pThis );
void lcd_config( tLcd *pThis );
void lcd_set_bl( tLcd *pThis, uint8_t on );
void lcd_set_window( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t h );
void lcd_set_pixel( tLcd *pThis, int16_t x, int16_t y, uint16_t color );
void lcd_rect( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color );
void lcd_clear( tLcd *pThis, uint16_t color );
void lcd_bmp( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *buf );

#endif /* LCD_H_ */
