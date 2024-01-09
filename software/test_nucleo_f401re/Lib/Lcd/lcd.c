/*
 * ili9488.c
 *
 *  Created on: Jul 23, 2023
 *      Author: jgpei
 */

#include "lcd.h"

// ILI9488/ST7789 register definitions
#define LCD_REG_SOFT_RESET              (0x01)
#define LCD_REG_SLEEP_OUT               (0x11)
#define LCD_REG_DISPLAY_ON              (0x29)
#define LCD_REG_MEMORY_ACCESS_CONTROL   (0x36)
#define LCD_REG_INTERFACE_PIXEL_FORMAT  (0x3A)
#define LCD_REG_COLUMN_ADDR             (0x2A)
#define LCD_REG_PAGE_ADDR               (0x2B)
#define LCD_REG_MEMORY_WRITE            (0x2C)


#define LCD_CLAMP( x, min, max )    ( (x) < (min) ? (min) : ( (x) > (max) ? (max) : (x) ) )

void lcd_init( tLcd *pThis,
	SPI_HandleTypeDef *spi,
	GPIO_TypeDef *port_cs,
	uint16_t pin_cs,
	GPIO_TypeDef *port_dc,
	uint16_t pin_dc,
	GPIO_TypeDef *bl_port,
	uint16_t bl_pin,
	uint16_t width,
	uint16_t height
){
	pThis->spi = spi;
	pThis->port_cs = port_cs;
    pThis->pin_cs = pin_cs;
	pThis->port_dc = port_dc;
    pThis->pin_dc = pin_dc;
    pThis->bl_port = bl_port;
    pThis->bl_pin = bl_pin;
    pThis->width = width;
    pThis->height = height;

    HAL_GPIO_WritePin( pThis->port_cs, pThis->pin_cs, GPIO_PIN_SET );

    lcd_reset( pThis );
    lcd_config( pThis );
    lcd_clear( pThis, 0x0000 );
    lcd_set_bl( pThis, 1 );
}

void lcd_write_register( tLcd *pThis, uint8_t reg, uint8_t *buf, uint32_t len )
{
	//volatile uint32_t *DR = &(pThis->spi->Instance->DR);
    HAL_GPIO_WritePin( pThis->port_cs, pThis->pin_cs, GPIO_PIN_RESET );

    HAL_GPIO_WritePin( pThis->port_dc, pThis->pin_dc, GPIO_PIN_RESET );
    HAL_SPI_Transmit( pThis->spi, &reg, 1, HAL_MAX_DELAY );
    //*DR = reg;
    //HAL_Delay( 1 );

    if( buf && len )
    {
    	HAL_GPIO_WritePin( pThis->port_dc, pThis->pin_dc, GPIO_PIN_SET );
		HAL_SPI_Transmit( pThis->spi, buf, len, HAL_MAX_DELAY );
    	//for( int i = len ; i > 0 ; i-- )
    	//{
    	//	*DR = *buf++;
    	//}
    	//HAL_Delay( 1 );
		HAL_GPIO_WritePin( pThis->port_dc, pThis->pin_dc, GPIO_PIN_RESET );
    }

    HAL_GPIO_WritePin( pThis->port_cs, pThis->pin_cs, GPIO_PIN_SET );
}

void lcd_reset( tLcd *pThis )
{
}

void lcd_config( tLcd *pThis )
{
	uint8_t buffer[1];

	lcd_write_register( pThis, LCD_REG_SOFT_RESET, NULL, 0 );
	HAL_Delay( 100 );

	lcd_write_register( pThis, LCD_REG_SLEEP_OUT, NULL, 0 );
	HAL_Delay( 10 );

#define LCD_REG_DISPLAY_INV_OFF             (0x20)
#define LCD_REG_DISPLAY_INV_ON              (0x21)
	lcd_write_register( pThis, LCD_REG_DISPLAY_INV_ON, NULL, 0 );

	buffer[0] = 0x05;
	lcd_write_register( pThis, LCD_REG_INTERFACE_PIXEL_FORMAT, buffer, 1 );

#define MH 	(1<<2)
#define RGB (1<<3)
#define ML 	(1<<4)
#define MV 	(1<<5)
#define MX 	(1<<6)
#define MY 	(1<<7)

	buffer[0] = MX|MV|ML;//
	lcd_write_register( pThis, LCD_REG_MEMORY_ACCESS_CONTROL, buffer, 1 );

	lcd_write_register( pThis, LCD_REG_DISPLAY_ON, NULL, 0 );
	HAL_Delay( 10 );

	return;


	lcd_write_register( pThis, 0x11, (uint8_t*)"", 0 );
    HAL_Delay( 100 );

    lcd_write_register( pThis, 0x36, (uint8_t*)"\x60", 1 );
    lcd_write_register( pThis, 0x3A, (uint8_t*)"\x55", 1 );
    lcd_write_register( pThis, 0x36, (uint8_t*)"\\x0C\x0C\x00\x33\x33", 5 );
    lcd_write_register( pThis, 0xB2, (uint8_t*)"\x0C\x0C\x00\x33\x33", 5 );
    lcd_write_register( pThis, 0xB7, (uint8_t*)"\x35", 1 );
    lcd_write_register( pThis, 0xBB, (uint8_t*)"\x28", 1 );
    lcd_write_register( pThis, 0xC0, (uint8_t*)"\x3C", 1 );
    lcd_write_register( pThis, 0xC2, (uint8_t*)"\x01", 1 );
    lcd_write_register( pThis, 0xC3, (uint8_t*)"\x0B", 1 );
    lcd_write_register( pThis, 0xC4, (uint8_t*)"\x20", 1 );
    lcd_write_register( pThis, 0xC6, (uint8_t*)"\x0F", 1 );
    lcd_write_register( pThis, 0xD0, (uint8_t*)"\xA4\xA1", 2 );
    lcd_write_register( pThis, 0xE0, (uint8_t*)"\xD0\x01\x08\x0F\x11\x2A\x36\x55\x44\x3A\x0B\x06\x11\x20", 14 );
    lcd_write_register( pThis, 0xE1, (uint8_t*)"\xD0\x02\x07\x0A\x0B\x18\x34\x43\x4A\x2B\x1B\x1C\x22\x1F", 14 );
    lcd_write_register( pThis, 0x55, (uint8_t*)"\xB0", 1 );
    lcd_write_register( pThis, 0x29, (uint8_t*)"", 0 );
    HAL_Delay( 100 );
}

void lcd_set_bl( tLcd *pThis, uint8_t on )
{
    HAL_GPIO_WritePin( pThis->bl_port, pThis->bl_pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET );
}

void lcd_set_window( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t h )
{
    uint16_t x0 = x;
    uint16_t y0 = y;
    uint16_t x1 = x0 + w - 1;
    uint16_t y1 = y0 + h - 1;
    uint8_t buffer[4];

    buffer[0] = x0 >> 8;
    buffer[1] = x0 &  0xff;
    buffer[2] = x1 >> 8;
    buffer[3] = x1 &  0xff;
    lcd_write_register( pThis, LCD_REG_COLUMN_ADDR, buffer, 4 );

    buffer[0] = y0 >> 8;
    buffer[1] = y0 &  0xff;
    buffer[2] = y1 >> 8;
    buffer[3] = y1 &  0xff;
    lcd_write_register( pThis, LCD_REG_PAGE_ADDR, buffer, 4 );
}

void lcd_set_pixel( tLcd *pThis, int16_t x, int16_t y, uint16_t color )
{
	lcd_set_window( pThis, x, y, 1, 1 );
    lcd_write_register( pThis, LCD_REG_MEMORY_WRITE, (uint8_t*)&color, 2 );
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

    //for( i = w*h ; i ; i-- )
    //{
    //	lcd_write_register( pThis, LCD_REG_MEMORY_WRITE, &color, 2 );
    //}

    uint8_t reg = LCD_REG_MEMORY_WRITE;

    HAL_GPIO_WritePin( pThis->port_cs, pThis->pin_cs, GPIO_PIN_RESET );

    HAL_GPIO_WritePin( pThis->port_dc, pThis->pin_dc, GPIO_PIN_RESET );
    HAL_SPI_Transmit( pThis->spi, &reg, 1, HAL_MAX_DELAY );
    //HAL_Delay( 1 );

	HAL_GPIO_WritePin( pThis->port_dc, pThis->pin_dc, GPIO_PIN_SET );
	for( i = w*h ; i ; i-- )
	{
		HAL_SPI_Transmit( pThis->spi, (uint8_t*)&color, 2, HAL_MAX_DELAY );
	}
	//HAL_Delay( 1 );
	HAL_GPIO_WritePin( pThis->port_dc, pThis->pin_dc, GPIO_PIN_RESET );

    HAL_GPIO_WritePin( pThis->port_cs, pThis->pin_cs, GPIO_PIN_SET );

}

void lcd_clear( tLcd *pThis, uint16_t color )
{
    lcd_rect( pThis, 0, 0, pThis->width, pThis->height, color );
}

void lcd_bmp( tLcd *pThis, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *buf )
{
    lcd_set_window( pThis, x, y, w, h );
	lcd_write_register( pThis, LCD_REG_MEMORY_WRITE, buf, 2*w*h );
}
