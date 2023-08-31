/*
 * buffer.h
 *
 *  Created on: Aug 31, 2023
 *      Author: jgpei
 */

#ifndef TOOLS_BUFFER_H_
#define TOOLS_BUFFER_H_

#include <stdint.h>

struct sBuffer
{
    uint8_t *pBuffer;
    uint16_t size;
    uint16_t head;
    uint16_t tail;
};
typedef struct sBuffer tBuffer;

void buffer_init( tBuffer *pThis, uint8_t *pBuffer, uint16_t size );
uint16_t buffer_push( tBuffer *pThis, uint8_t *pBuffer, uint16_t size );
uint16_t buffer_pop( tBuffer *pThis, uint8_t *pBuffer, uint16_t size );
uint16_t buffer_size( tBuffer *pThis );
uint16_t buffer_free( tBuffer *pThis );

#endif /* TOOLS_BUFFER_H_ */
