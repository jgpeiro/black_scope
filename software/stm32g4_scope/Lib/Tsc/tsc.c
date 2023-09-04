/*
 * tsc2046.c
 *
 *  Created on: 8 jul. 2023
 *      Author: Jose
 */

#include <stdint.h>

#include "tsc.h"


void tsc_init( tTsc* pThis, SPI_HandleTypeDef* spi, GPIO_TypeDef* cs_port, uint16_t cs_pin, float ax, float bx, float ay, float by, int avg )
{
	pThis->spi = spi;
	pThis->cs_port = cs_port;
	pThis->cs_pin = cs_pin;
	pThis->ax = ax;
	pThis->bx = bx;
	pThis->ay = ay;
	pThis->by = by;
	pThis->avg = avg;

    HAL_GPIO_WritePin(pThis->cs_port, pThis->cs_pin, GPIO_PIN_SET);
}

void tsc_read_ll( tTsc* pThis, uint16_t* x, uint16_t* y) {
	uint8_t buf_tx[3] = {0};
	uint8_t buf_rx[3] = {0};

    HAL_GPIO_WritePin(pThis->cs_port, pThis->cs_pin, GPIO_PIN_RESET);

    buf_tx[0] = TSC_CHANNEL_X;
    HAL_SPI_TransmitReceive(pThis->spi, buf_tx, buf_rx, 3, HAL_MAX_DELAY);
    *x = (buf_rx[1] << 4) | (buf_rx[2] >> 4);

    buf_tx[0] = TSC_CHANNEL_Y;
    HAL_SPI_TransmitReceive(pThis->spi, buf_tx, buf_rx, 3, HAL_MAX_DELAY);
    *y = (buf_rx[1] << 4) | (buf_rx[2] >> 4);

    HAL_GPIO_WritePin(pThis->cs_port, pThis->cs_pin, GPIO_PIN_SET);

    if (*x == 2047) {
        *x = 0;
    }
}

void tsc_read(tTsc* pThis, uint16_t* x, uint16_t* y, uint16_t* p) {
	tsc_read_ll(pThis, x, y);
	if (*x && *y) {
		pThis->cnt += 1;
	} else {
    	pThis->x_low = 0;
    	pThis->y_low = 0;
		pThis->cnt = 0;
	}

	pThis->x_low = pThis->x_low*0.9 + *x*0.1;
	pThis->y_low = pThis->y_low*0.9 + *y*0.1;

	if( pThis->cnt >= 40 ) {
        *x = pThis->ax * pThis->x_low + pThis->bx;
        *y = pThis->ay * pThis->y_low + pThis->by;
        *p = 1;
    } else {
        *x = 0;
        *y = 0;
        *p = 0; // Additional code to set pressure to 0
    }
}
