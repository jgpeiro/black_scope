/*
 * st7789.h
 *
 *  Created on: 1 dic. 2021
 *      Author: Jose
 */

#ifndef ST7789_H_
#define ST7789_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32g4xx_hal.h"

struct st7789_t {
    uint8_t buf1[1];
    uint8_t buf2[2];
    uint8_t buf4[4];

    GPIO_TypeDef *port_cs;
    uint16_t pin_cs;
    GPIO_TypeDef *port_dc;
    uint16_t pin_dc;

    SPI_HandleTypeDef *spi;
    DMA_HandleTypeDef *dma;
};

void st7789_init(
	struct st7789_t *pThis,
	SPI_HandleTypeDef *spi,
	DMA_HandleTypeDef *dma,
	GPIO_TypeDef *port_cs,
	uint16_t pin_cs,
	GPIO_TypeDef *port_dc,
	uint16_t pin_dc
);
void st7789_config( struct st7789_t *pThis );
void st7789_write_register( struct st7789_t *pThis, uint8_t reg, uint8_t *buf, uint32_t len );
void st7789_write_register_dma( struct st7789_t *pThis, uint8_t reg, uint8_t *buf, uint32_t len, bool is_blocking );
void st7789_wait_dma_complete( struct st7789_t *pThis );
void st7789_set_window( struct st7789_t *pThis, uint16_t x, uint16_t y, uint16_t w, uint16_t h );
void st7789_draw_bitmap_dma( struct st7789_t *pThis, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *buf, bool is_blocking );
void st7789_clear( struct st7789_t *pThis, uint16_t color );

#endif /* ST7789_H_ */
