/*
 * tsc2046.c
 *
 *  Created on: 8 jul. 2023
 *      Author: Jose
 */

#include <stdint.h>

#include "tsc.h"


void tsc_init( tTsc* tsc, SPI_HandleTypeDef* spi, GPIO_TypeDef* cs_port, uint16_t cs_pin, float ax, float bx, float ay, float by, int avg )
{
	tsc->spi = spi;
	tsc->cs_port = cs_port;
	tsc->cs_pin = cs_pin;
	tsc->ax = ax;
	tsc->bx = bx;
	tsc->ay = ay;
	tsc->by = by;
	tsc->avg = avg;

    HAL_GPIO_WritePin(tsc->cs_port, tsc->cs_pin, GPIO_PIN_SET);
}

void tsc_read_ll( tTsc* tsc, uint16_t* x, uint16_t* y) {
	uint8_t buf_tx[3] = {0};
	uint8_t buf_rx[3] = {0};

    HAL_GPIO_WritePin(tsc->cs_port, tsc->cs_pin, GPIO_PIN_RESET);

    buf_tx[0] = TSC_CHANNEL_X;
    HAL_SPI_TransmitReceive(tsc->spi, buf_tx, buf_rx, 3, HAL_MAX_DELAY);
    *x = (buf_rx[1] << 4) | (buf_rx[2] >> 4);

    buf_tx[0] = TSC_CHANNEL_Y;
    HAL_SPI_TransmitReceive(tsc->spi, buf_tx, buf_rx, 3, HAL_MAX_DELAY);
    *y = (buf_rx[1] << 4) | (buf_rx[2] >> 4);

    HAL_GPIO_WritePin(tsc->cs_port, tsc->cs_pin, GPIO_PIN_SET);

    if (*x == 2047) {
        *x = 0;
    }
}

void tsc_read( tTsc* tsc, uint16_t* x, uint16_t* y) {
    int32_t x_acc = 0;
    int32_t y_acc = 0;

    for (int i = 0; i < tsc->avg; i++) {
    	tsc_read_ll(tsc, x, y);
        if (*x && *y) {
            x_acc += *x;
            y_acc += *y;
            tsc->cnt += 1;
        } else {
            *x = 0;
            *y = 0;
            tsc->cnt = 0;
            return;
        }
        //HAL_Delay(1);
    }

    if( tsc->cnt >= tsc->avg )
    {
        *x = x_acc / tsc->avg;
        *y = y_acc / tsc->avg;
        *x = tsc->ax * *x + tsc->bx;
        *y = tsc->ay * *y + tsc->by;
    }
    else
    {
        *x = 0;
        *y = 0;
    }
}

