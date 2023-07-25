/*
 * tools.h
 *
 *  Created on: Jul 25, 2023
 *      Author: jgpei
 */

#ifndef INC_TOOLS_H_
#define INC_TOOLS_H_

#include <stdint.h>

int16_t get_vmin( int16_t *buffer, int16_t len );
int16_t get_vmax( int16_t *buffer, int16_t len );
int16_t get_vavg( int16_t *buffer, int16_t len );
int16_t get_period( int16_t *buffer, int16_t len, int16_t mx, int16_t mn, int16_t avg );
int16_t get_duty( int16_t *buffer, int16_t len, int16_t mx, int16_t mn, int16_t avg );

#endif /* INC_TOOLS_H_ */
