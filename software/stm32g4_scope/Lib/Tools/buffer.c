/*
 * buffer.c
 *
 *  Created on: Aug 31, 2023
 *      Author: jgpei
 */

#ifndef TOOLS_BUFFER_C_
#define TOOLS_BUFFER_C_

#include "buffer.h"

void buffer_init( tBuffer *pThis, uint8_t *pBuffer, uint16_t size )
{
    pThis->pBuffer = pBuffer;
    pThis->size = size;
    pThis->head = 0;
    pThis->tail = 0;
}

uint16_t buffer_push( tBuffer *pThis, uint8_t *pBuffer, uint16_t size )
{
    uint16_t i;
    uint16_t free = buffer_free( pThis );
    uint16_t toCopy = ( size < free ) ? size : free;

    for( i = 0; i < toCopy; i++ )
    {
        pThis->pBuffer[pThis->head] = pBuffer[i];
        pThis->head = ( pThis->head + 1 ) % pThis->size;
    }

    return toCopy;
}

uint16_t buffer_pop( tBuffer *pThis, uint8_t *pBuffer, uint16_t size )
{
    uint16_t i;
    uint16_t used = buffer_size( pThis );
    uint16_t toCopy = ( size < used ) ? size : used;

    for( i = 0; i < toCopy; i++ )
    {
        pBuffer[i] = pThis->pBuffer[pThis->tail];
        pThis->tail = ( pThis->tail + 1 ) % pThis->size;
    }

    return toCopy;
}

uint16_t buffer_size( tBuffer *pThis )
{
    return ( pThis->head - pThis->tail + pThis->size ) % pThis->size;
}

uint16_t buffer_free( tBuffer *pThis )
{
    return pThis->size - buffer_size( pThis ) - 1;
}


#endif /* TOOLS_BUFFER_C_ */
