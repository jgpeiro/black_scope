/*
 * tsc2046.h
 *
 *  Created on: 8 jul. 2023
 *      Author: Jose
 */

#ifndef TSC2046_H_
#define TSC2046_H_

#include "stm32g4xx_hal.h"

#define TSC2046_CHANNEL_X	(0x90)
#define TSC2046_CHANNEL_Y	(0xD0)

typedef struct {
    SPI_HandleTypeDef* spi;
    GPIO_TypeDef* cs_port;
    uint16_t cs_pin;
    float ax;
    float bx;
    float ay;
    float by;
    int avg;
} tTsc2046;


void tsc2046_init( tTsc2046* tsc, SPI_HandleTypeDef* spi, GPIO_TypeDef* cs_port, uint16_t cs_pin, float ax, float bx, float ay, float by, int avg );
void tsc2046_read_ll( tTsc2046* tsc, uint16_t* x, uint16_t* y );
void tsc2046_read( tTsc2046* tsc, uint16_t* x, uint16_t* y );

#endif /* TSC2046_H_ */
