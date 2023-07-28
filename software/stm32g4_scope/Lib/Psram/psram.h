
#ifndef _PSRAM_H_
#define _PSRAM_H_

#include <stdint.h>
#include "stm32g4xx_hal.h"

// APS6404L-3SQR-SN
// https://www.apmemory.com/wp-content/uploads/APS6404L-3SQR.pdf
#define PSRAM_CMD_RST_EN            (0x66)
#define PSRAM_CMD_RST               (0x99)
#define PSRAM_CMD_READ_ID           (0x9F)
#define PSRAM_CMD_ENTER_QUAD        (0x35)
#define PSRAM_CMD_EXIT_QUAD         (0xF5)
#define PSRAM_CMD_FAST_QUAD_READ    (0xEB)
#define PSRAM_CMD_QUAD_WRITE        (0x38)

#define PSRAM_MANUFACTURER_ID       (0x0D)
#define PSRAM_KNOWN_GOOD_DIE        (0x5D)

#define PSRAM_SIZE_BYTES			(0x00800000)

enum ePsram_Direction
{
    PSRAM_DIRECTION_NONE = 0,
    PSRAM_DIRECTION_TRANSMIT,
    PSRAM_DIRECTION_RECEIVE,
};

struct psram
{
    QSPI_HandleTypeDef *hqspi;
};
typedef struct psram psram_t;

void psram_init( psram_t *psram, QSPI_HandleTypeDef *hqspi );
void psram_deinit( psram_t *psram );
uint8_t psram_reset( psram_t *psram );
uint8_t psram_read_id( psram_t *psram );
uint8_t psram_quad_enable( psram_t *psram );
uint8_t psram_quad_disable( psram_t *psram );
uint8_t psram_read( psram_t *psram, uint32_t address, uint8_t *buff, uint32_t size );
uint8_t psram_write( psram_t *psram, uint32_t address, uint8_t *buff, uint32_t size );

uint8_t psram_test();

#endif // _PSRAM_H_
