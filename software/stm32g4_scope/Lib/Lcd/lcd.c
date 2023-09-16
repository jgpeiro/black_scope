/*
 * ili9488.c
 *
 *  Created on: Jul 23, 2023
 *      Author: jgpei
 */

#include "lcd.h"

// ILI9488 register definitions
#define LCD_REG_SOFT_RESET              (0x01)
#define LCD_REG_SLEEP_OUT               (0x11)
#define LCD_REG_DISPLAY_ON              (0x29)
#define LCD_REG_MEMORY_ACCESS_CONTROL   (0x36)
#define LCD_REG_INTERFACE_PIXEL_FORMAT  (0x3A)
#define LCD_REG_COLUMN_ADDR             (0x2A)
#define LCD_REG_PAGE_ADDR               (0x2B)
#define LCD_REG_MEMORY_WRITE            (0x2C)
#define LCD_REG_MEMORY_READ             (0x2E)

// FMC memory-mapped address
#define LCD_CMD_ADDR	*(volatile uint16_t*)(0x60000000)
#define LCD_DATA_ADDR	*(volatile uint16_t*)(0x60000002)

#define LCD_CLAMP( x, min, max )    ( (x) < (min) ? (min) : ( (x) > (max) ? (max) : (x) ) )

void lcd_init( tLcd *pThis, GPIO_TypeDef *reset_port, uint16_t reset_pin, GPIO_TypeDef *bl_port, uint16_t bl_pin, uint16_t width, uint16_t height )
{
	pThis->reset_port = reset_port;
    pThis->reset_pin = reset_pin;
    pThis->bl_port = bl_port;
    pThis->bl_pin = bl_pin;
    pThis->width = width;
    pThis->height = height;

    lcd_reset( pThis );
    lcd_config( pThis );
    lcd_clear( pThis, 0x0000 );
    lcd_set_bl( pThis, 1 );
}

void lcd_reset( tLcd *pThis )
{
    HAL_GPIO_WritePin( pThis->reset_port, pThis->reset_pin, GPIO_PIN_RESET );
    HAL_Delay( 10 );
    HAL_GPIO_WritePin( pThis->reset_port, pThis->reset_pin, GPIO_PIN_SET );
    HAL_Delay( 100 );
}

void lcd_config( tLcd *pThis )
{
    LCD_CMD_ADDR = LCD_REG_SOFT_RESET;
    HAL_Delay( 100 );

    LCD_CMD_ADDR = LCD_REG_SLEEP_OUT;
    HAL_Delay( 10 );

    LCD_CMD_ADDR = LCD_REG_INTERFACE_PIXEL_FORMAT;
    LCD_DATA_ADDR = 0x05;

    LCD_CMD_ADDR = LCD_REG_MEMORY_ACCESS_CONTROL;
    LCD_DATA_ADDR = (0x01<<5)|(0x01<<6)|(0x01<<7);

    LCD_CMD_ADDR = LCD_REG_DISPLAY_ON;
    HAL_Delay( 10 );
}

void lcd_set_bl( tLcd *pThis, uint8_t on )
{
    HAL_GPIO_WritePin( pThis->bl_port, pThis->bl_pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET );
}

void _lcd_set_window( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t h )
{
    uint16_t x0 = LCD_CLAMP( x, 0, pThis->width - 1 );
    uint16_t y0 = LCD_CLAMP( y, 0, pThis->height - 1 );
    uint16_t x1 = LCD_CLAMP( x + w - 1, 0, pThis->width - 1 );
    uint16_t y1 = LCD_CLAMP( y + h - 1, 0, pThis->height - 1 );

    LCD_CMD_ADDR = LCD_REG_COLUMN_ADDR;
    LCD_DATA_ADDR =(x0>>8)&0xFF;
    LCD_DATA_ADDR = x0;
    LCD_DATA_ADDR =(x1>>8)&0xFF;
    LCD_DATA_ADDR = x1;

    LCD_CMD_ADDR = LCD_REG_PAGE_ADDR;
    LCD_DATA_ADDR =(y0>>8)&0xFF;
    LCD_DATA_ADDR = y0;
    LCD_DATA_ADDR =(y1>>8)&0xFF;
    LCD_DATA_ADDR = y1;
}

void lcd_set_window( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t h )
{
    uint16_t x0 = x;
    uint16_t y0 = y;
    uint16_t x1 = x + w - 1;
    uint16_t y1 = y + h - 1;

    LCD_CMD_ADDR = LCD_REG_COLUMN_ADDR;
    LCD_DATA_ADDR =(x0>>8)&0xFF;
    LCD_DATA_ADDR = x0;
    LCD_DATA_ADDR =(x1>>8)&0xFF;
    LCD_DATA_ADDR = x1;

    LCD_CMD_ADDR = LCD_REG_PAGE_ADDR;
    LCD_DATA_ADDR =(y0>>8)&0xFF;
    LCD_DATA_ADDR = y0;
    LCD_DATA_ADDR =(y1>>8)&0xFF;
    LCD_DATA_ADDR = y1;
}

void lcd_set_pixel( tLcd *pThis, int16_t x, int16_t y, uint16_t color )
{
    LCD_CMD_ADDR = LCD_REG_COLUMN_ADDR;
    LCD_DATA_ADDR =(x>>8)&0xFF;
    LCD_DATA_ADDR = x;
    LCD_DATA_ADDR =(x>>8)&0xFF;
    LCD_DATA_ADDR = x;

    LCD_CMD_ADDR = LCD_REG_PAGE_ADDR;
    LCD_DATA_ADDR =(y>>8)&0xFF;
    LCD_DATA_ADDR = y;
    LCD_DATA_ADDR =(y>>8)&0xFF;
    LCD_DATA_ADDR = y;

    LCD_CMD_ADDR = LCD_REG_MEMORY_WRITE;
    LCD_DATA_ADDR = color;
}

uint16_t lcd_get_pixel( tLcd *pThis, int16_t x, int16_t y )
{
	uint16_t color = 0;

	LCD_CMD_ADDR = LCD_REG_COLUMN_ADDR;
    LCD_DATA_ADDR =(x>>8)&0xFF;
    LCD_DATA_ADDR = x;
    LCD_DATA_ADDR =(x>>8)&0xFF;
    LCD_DATA_ADDR = x;

    LCD_CMD_ADDR = LCD_REG_PAGE_ADDR;
    LCD_DATA_ADDR =(y>>8)&0xFF;
    LCD_DATA_ADDR = y;
    LCD_DATA_ADDR =(y>>8)&0xFF;
    LCD_DATA_ADDR = y;

    LCD_CMD_ADDR = LCD_REG_MEMORY_READ;
    color = LCD_DATA_ADDR;

    return color;
}
void lcd_hline( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t color )
{
	lcd_rect( pThis, x, y, w, 1, color );
}
void lcd_vline( tLcd *pThis, int16_t x, int16_t y, uint16_t h, uint16_t color )
{
	lcd_rect( pThis, x, y, 1, h, color );
}

void lcd_rect( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color )
{
    uint32_t i = 0;

    lcd_set_window( pThis, x, y, w, h );

    LCD_CMD_ADDR = LCD_REG_MEMORY_WRITE;
    for( i = w*h ; i ; i-- )
    {
    	LCD_DATA_ADDR = color;
    }
}

void lcd_clear( tLcd *pThis, uint16_t color )
{
    lcd_rect( pThis, 0, 0, pThis->width, pThis->height, color );
}

void lcd_bmp( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *buf )
{
    uint32_t i = 0;

    lcd_set_window( pThis, x, y, w, h );

    LCD_CMD_ADDR = LCD_REG_MEMORY_WRITE;
    for( i = w*h ; i ; i-- )
    {
        LCD_DATA_ADDR = *buf++;
    }
}

void lcd_get_bmp( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *buf )
{
    uint32_t i = 0;

    lcd_set_window( pThis, x, y, w, h );

    LCD_CMD_ADDR = LCD_REG_MEMORY_READ;
    for( i = w*h ; i ; i-- )
    {
    	*buf++ = LCD_DATA_ADDR;
    }


    for( i = 0 ; i < 100 ; i++ )
    {
    	buf[i] = lcd_get_pixel( pThis, x, i );
    }
}

uint16_t lcd_get_chip_id( tLcd *pThis )
{
    uint16_t id = 0;

    LCD_CMD_ADDR = 0x04;
    id = LCD_DATA_ADDR;

    return id;
}
