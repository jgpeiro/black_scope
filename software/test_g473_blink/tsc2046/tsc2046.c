/*
 * tsc2046.c
 *
 *  Created on: 8 jul. 2023
 *      Author: Jose
 */

#include <stdint.h>
#include "tsc2046.h"


void tsc2046_init(tTsc2046* tsc, SPI_HandleTypeDef* spi, GPIO_TypeDef* cs_port, uint16_t cs_pin, float ax, float bx, float ay, float by )
{
	tsc->spi = spi;
	tsc->cs_port = cs_port;
	tsc->cs_pin = cs_pin;
	tsc->ax = ax;
	tsc->bx = bx;
	tsc->ay = ay;
	tsc->by = by;
    HAL_GPIO_WritePin(tsc->cs_port, tsc->cs_pin, GPIO_PIN_SET);
}

void tsc2046_read_ll( tTsc2046* tsc, uint16_t* x, uint16_t* y) {
	uint8_t buf_tx[3] = {0};
	uint8_t buf_rx[3] = {0};

    HAL_GPIO_WritePin(tsc->cs_port, tsc->cs_pin, GPIO_PIN_RESET);

    buf_tx[0] = TSC2046_CHANNEL_X;
    HAL_SPI_TransmitReceive(tsc->spi, buf_tx, buf_rx, 3, HAL_MAX_DELAY);
    *x = (buf_rx[1] << 4) | (buf_rx[2] >> 4);

    buf_tx[0] = TSC2046_CHANNEL_Y;
    HAL_SPI_TransmitReceive(tsc->spi, buf_tx, buf_rx, 3, HAL_MAX_DELAY);
    *y = (buf_rx[1] << 4) | (buf_rx[2] >> 4);

    HAL_GPIO_WritePin(tsc->cs_port, tsc->cs_pin, GPIO_PIN_SET);

    if (*x == 2047) {
        *x = 0;
    }
}

void tsc2046_read( tTsc2046* tsc, uint16_t* x, uint16_t* y) {
    uint16_t x_acc = 0;
    uint16_t y_acc = 0;

    for (int i = 0; i < 4; i++) {
    	tsc2046_read_ll(tsc, x, y);
        if (*x && *y) {
            x_acc += *x;
            y_acc += *y;
        } else {
            *x = 0;
            *y = 0;
            return;
        }
    }
    *x = x_acc / 4;
    *y = y_acc / 4;
    *x = tsc->ax * *x + tsc->bx;
    *y = tsc->ay * *y + tsc->by;
}

