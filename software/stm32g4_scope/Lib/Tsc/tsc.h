/*
 * tsc2046.h
 *
 *  Created on: 8 jul. 2023
 *      Author: Jose
 */

#ifndef TSC_H_
#define TSC_H_

#include "stm32g4xx_hal.h"

// TSC2046 registers
#define TSC_CHANNEL_X	(0x90)
#define TSC_CHANNEL_Y	(0xD0)
//#define TSC_CHANNEL_X	(0x93)
//#define TSC_CHANNEL_Y	(0xD3)

typedef struct {
    SPI_HandleTypeDef* spi;
    GPIO_TypeDef* cs_port;
    uint16_t cs_pin;
    float ax;
    float bx;
    float ay;
    float by;
    int cnt;
    int x_low;
    int y_low;
    int avg;
} tTsc;


void tsc_init( tTsc* tsc, SPI_HandleTypeDef* spi, GPIO_TypeDef* cs_port, uint16_t cs_pin, float ax, float bx, float ay, float by, int avg );
void tsc_read_ll( tTsc* tsc, uint16_t* x, uint16_t* y );
void tsc_read( tTsc* tsc, uint16_t* x, uint16_t* y );

#endif /* TSC_H_ */
