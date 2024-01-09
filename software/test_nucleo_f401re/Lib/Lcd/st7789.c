/*
 * st7789.c
 *
 *  Created on: 1 dic. 2021
 *      Author: Jose
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "st7789.h"

#define ST7789_CASET (0x2A)
#define ST7789_RASET (0x2B)
#define ST7789_RAMWR (0x2C)
#define ST7789_HRES  (320)
#define ST7789_VRES  (240)

void st7789_init(
	struct st7789_t *pThis,
	SPI_HandleTypeDef *spi,
    DMA_HandleTypeDef *dma,
	GPIO_TypeDef *port_cs,
    uint16_t pin_cs,
    GPIO_TypeDef *port_dc,
    uint16_t pin_dc
) {

    pThis->buf1[0] = 0;
    pThis->buf2[0] = 0;
    pThis->buf2[1] = 0;
    pThis->buf4[0] = 0;
    pThis->buf4[1] = 0;
    pThis->buf4[2] = 0;
    pThis->buf4[3] = 0;

    pThis->spi      = spi;
	pThis->dma      = dma;

	pThis->port_cs  = port_cs;
	pThis->pin_cs   = pin_cs;
	pThis->port_dc  = port_dc;
	pThis->pin_dc   = pin_dc;

    HAL_GPIO_WritePin( pThis->port_cs, pThis->pin_cs, GPIO_PIN_SET );

    //st7789_spi_init( pThis );
    //st7789_dma_init( pThis );
    st7789_config( pThis );
}

void st7789_config( struct st7789_t *pThis )
{
    st7789_write_register( pThis, 0x11, (uint8_t*)"", 0 );
    HAL_Delay( 100 );

    st7789_write_register( pThis, 0x36, (uint8_t*)"\x60", 1 );
    st7789_write_register( pThis, 0x3A, (uint8_t*)"\x55", 1 );
    st7789_write_register( pThis, 0x36, (uint8_t*)"\\x0C\x0C\x00\x33\x33", 5 );
    st7789_write_register( pThis, 0xB2, (uint8_t*)"\x0C\x0C\x00\x33\x33", 5 );
    st7789_write_register( pThis, 0xB7, (uint8_t*)"\x35", 1 );
    st7789_write_register( pThis, 0xBB, (uint8_t*)"\x28", 1 );
    st7789_write_register( pThis, 0xC0, (uint8_t*)"\x3C", 1 );
    st7789_write_register( pThis, 0xC2, (uint8_t*)"\x01", 1 );
    st7789_write_register( pThis, 0xC3, (uint8_t*)"\x0B", 1 );
    st7789_write_register( pThis, 0xC4, (uint8_t*)"\x20", 1 );
    st7789_write_register( pThis, 0xC6, (uint8_t*)"\x0F", 1 );
    st7789_write_register( pThis, 0xD0, (uint8_t*)"\xA4\xA1", 2 );
    st7789_write_register( pThis, 0xE0, (uint8_t*)"\xD0\x01\x08\x0F\x11\x2A\x36\x55\x44\x3A\x0B\x06\x11\x20", 14 );
    st7789_write_register( pThis, 0xE1, (uint8_t*)"\xD0\x02\x07\x0A\x0B\x18\x34\x43\x4A\x2B\x1B\x1C\x22\x1F", 14 );
    st7789_write_register( pThis, 0x55, (uint8_t*)"\xB0", 1 );
    st7789_write_register( pThis, 0x29, (uint8_t*)"", 0 );
    HAL_Delay( 100 );
}

void st7789_write_register( struct st7789_t *pThis, uint8_t reg, uint8_t *buf, uint32_t len )
{
    HAL_GPIO_WritePin( pThis->port_cs, pThis->pin_cs, GPIO_PIN_RESET );

    HAL_GPIO_WritePin( pThis->port_dc, pThis->pin_dc, GPIO_PIN_RESET );
    HAL_SPI_Transmit( pThis->spi, &reg, 1, HAL_MAX_DELAY );
    //HAL_Delay( 1 );

    HAL_GPIO_WritePin( pThis->port_dc, pThis->pin_dc, GPIO_PIN_SET );
    if( buf && len )
    {
		HAL_SPI_Transmit( pThis->spi, buf, len, HAL_MAX_DELAY );
		//HAL_Delay( 1 );
    }
    HAL_GPIO_WritePin( pThis->port_cs, pThis->pin_cs, GPIO_PIN_SET );
}

void st7789_write_register_dma( struct st7789_t *pThis, uint8_t reg, uint8_t *buf, uint32_t len, bool is_blocking )
{
    HAL_GPIO_WritePin( pThis->port_cs, pThis->pin_cs, GPIO_PIN_RESET );

    HAL_GPIO_WritePin( pThis->port_dc, pThis->pin_dc, GPIO_PIN_RESET );
    HAL_SPI_Transmit( pThis->spi, &reg, 1, HAL_MAX_DELAY );
    //HAL_Delay( 1 );

    HAL_GPIO_WritePin( pThis->port_dc, pThis->pin_dc, GPIO_PIN_SET );
    HAL_SPI_Transmit_DMA( pThis->spi, buf, len );
    //HAL_Delay( 1 );

    if( is_blocking ) {
        st7789_wait_dma_complete( pThis );
    }
}

void st7789_wait_dma_complete( struct st7789_t *pThis )
{
    while( HAL_DMA_GetState( pThis->dma ) != HAL_DMA_STATE_READY ) {
        HAL_Delay( 1 );
    }

    HAL_GPIO_WritePin( pThis->port_cs, pThis->pin_cs, GPIO_PIN_SET );
}

void st7789_set_window( struct st7789_t *pThis, uint16_t x, uint16_t y, uint16_t w, uint16_t h )
{
    uint16_t x0 = x;
    uint16_t y0 = y;
    uint16_t x1 = x0 + w - 1;
    uint16_t y1 = y0 + h - 1;
    pThis->buf4[0] = x0 >> 8;
    pThis->buf4[1] = x0 &  0xff;
    pThis->buf4[2] = x1 >> 8;
    pThis->buf4[3] = x1 &  0xff;
    st7789_write_register( pThis, ST7789_CASET, pThis->buf4, 4 );
    pThis->buf4[0] = y0 >> 8;
    pThis->buf4[1] = y0 &  0xff;
    pThis->buf4[2] = y1 >> 8;
    pThis->buf4[3] = y1 &  0xff;
    st7789_write_register( pThis, ST7789_RASET, pThis->buf4, 4 );
}

void st7789_draw_bitmap_dma( struct st7789_t *pThis, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *buf, bool is_blocking )
{
    st7789_set_window( pThis, x, y, w, h );
    st7789_write_register_dma( pThis, ST7789_RAMWR, buf, 2*w*h, is_blocking );
}

void st7789_clear( struct st7789_t *pThis, uint16_t color )
{
    int i, j;

    pThis->buf2[0] = color >> 8;
    pThis->buf2[1] = color &  0xff;

    st7789_set_window( pThis, 0, 0, ST7789_HRES, ST7789_VRES  );
    st7789_write_register( pThis, ST7789_RAMWR, NULL, 0 );

    HAL_GPIO_WritePin( pThis->port_cs, pThis->pin_cs, GPIO_PIN_RESET );
    HAL_GPIO_WritePin( pThis->port_dc, pThis->pin_dc, GPIO_PIN_SET );

    for( i = 0; i < ST7789_HRES; i++ )
    {
        for( j = 0; j < ST7789_VRES; j++ )
        {
            HAL_SPI_Transmit( pThis->spi, pThis->buf2, 2, 100 );
        }
    }
    HAL_GPIO_WritePin( pThis->port_cs, pThis->pin_cs, GPIO_PIN_SET );
}
