/*
 * tools.h
 *
 *  Created on: Jul 25, 2023
 *      Author: jgpei
 */

#ifndef INC_TOOLS_H_
#define INC_TOOLS_H_

#include <stdint.h>

uint16_t get_vmin( uint16_t *buffer, uint16_t len );
uint16_t get_vmax( uint16_t *buffer, uint16_t len );
uint16_t get_vavg( uint16_t *buffer, uint16_t len );
uint16_t get_period( uint16_t *buffer, uint16_t len, uint16_t mx, uint16_t mn, uint16_t avg );
uint16_t get_duty( uint16_t *buffer, uint16_t len, uint16_t mx, uint16_t mn, uint16_t avg );

#endif /* INC_TOOLS_H_ */
