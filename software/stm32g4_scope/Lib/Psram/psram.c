
#include <stdlib.h>
#include "psram.h"

static uint8_t TxCpltCallback, RxCpltCallback;

static uint8_t qspi_command(
    psram_t *psram,
    uint8_t Instruction,
    uint32_t Address,
    uint8_t *pData,
    uint16_t NbData,
    uint32_t InstructionMode,
    uint32_t AddressMode,
    uint32_t DataMode,
    uint32_t DummyCycles,
    uint32_t SIOOMode,
	enum ePsram_Direction direction
);

void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
    TxCpltCallback = 1;
}
void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
    RxCpltCallback = 1;
}

uint8_t qspi_command(
    psram_t *psram,
    uint8_t Instruction,
    uint32_t Address,
    uint8_t *pData,
    uint16_t NbData,
    uint32_t InstructionMode,
    uint32_t AddressMode,
    uint32_t DataMode,
    uint32_t DummyCycles,
    uint32_t SIOOMode,
    enum ePsram_Direction direction )
{
    QSPI_CommandTypeDef sCommand = {
        .Instruction        = Instruction,
        .InstructionMode    = InstructionMode,
        .AddressMode        = AddressMode,
        .Address            = Address,
        .AddressSize        = QSPI_ADDRESS_24_BITS,
        .DataMode           = DataMode,
        .NbData             = NbData,
        .AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytes     = 0x00,
        .AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS,
        .DummyCycles        = DummyCycles,
        .DdrMode            = QSPI_DDR_MODE_DISABLE,
        .DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY,
        .SIOOMode           = SIOOMode
    };

    if( HAL_OK != HAL_QSPI_Command( psram->hqspi, &sCommand, HAL_MAX_DELAY ) ) {
        return 0;
    }

    if( direction == PSRAM_DIRECTION_TRANSMIT )
    {
        TxCpltCallback = 0;
        if( HAL_OK != HAL_QSPI_Transmit_DMA( psram->hqspi, pData ) ) {
            return 0;
        }
        while( !TxCpltCallback );
    }
    else if( direction == PSRAM_DIRECTION_RECEIVE )
    {
        RxCpltCallback = 0;
        if( HAL_OK != HAL_QSPI_Receive_DMA( psram->hqspi, pData ) ) {
            return 0;
        }
        while( !RxCpltCallback );
    }

    return 1;
}

void psram_init( psram_t *psram, QSPI_HandleTypeDef *hqspi )
{
    //MX_QUADSPI1_Init();
    psram->hqspi = hqspi;
}

void psram_deinit( psram_t *psram )
{
    //HAL_QSPI_DeInit( psram->hqspin );
    psram->hqspi = NULL;
}

uint8_t psram_reset( psram_t *psram )
{
    if( qspi_command(
        psram,
        PSRAM_CMD_RST_EN,
        0x00000000,
        NULL,
        0,
        QSPI_INSTRUCTION_1_LINE,
        QSPI_ADDRESS_NONE,
        QSPI_DATA_NONE,
        0x00,
        QSPI_SIOO_INST_EVERY_CMD,
        PSRAM_DIRECTION_NONE ) )
    {
        if( qspi_command(
            psram,
            PSRAM_CMD_RST,
            0x00000000,
            NULL,
            0,
            QSPI_INSTRUCTION_1_LINE,
            QSPI_ADDRESS_NONE,
            QSPI_DATA_NONE,
            0x00,
            QSPI_SIOO_INST_EVERY_CMD,
            PSRAM_DIRECTION_NONE ) )
        {
            return 1;
        }
    }

    return 0;
}

uint8_t psram_read_id( psram_t *psram )
{
    uint8_t data[2] = {0};

    qspi_command(
        psram,
        PSRAM_CMD_READ_ID,
        0x00000000,
        data,
        sizeof( data ),
        QSPI_INSTRUCTION_1_LINE,
        QSPI_ADDRESS_1_LINE,
        QSPI_DATA_1_LINE,
        0x00,
        QSPI_SIOO_INST_EVERY_CMD,
        PSRAM_DIRECTION_RECEIVE
    );

    if( data[0] == PSRAM_MANUFACTURER_ID &&
        data[1] == PSRAM_KNOWN_GOOD_DIE )
    {
        return 1;
    }

    return 0;
}

uint8_t psram_quad_enable( psram_t *psram )
{
    return qspi_command(
        psram,
        PSRAM_CMD_ENTER_QUAD,
        0x00000000,
        NULL,
        0,
        QSPI_INSTRUCTION_1_LINE,
        QSPI_ADDRESS_NONE,
        QSPI_DATA_NONE,
        0x00,
        QSPI_SIOO_INST_EVERY_CMD,
        PSRAM_DIRECTION_NONE
    );
}

uint8_t psram_quad_disable( psram_t *psram )
{
    return qspi_command(
        psram,
        PSRAM_CMD_EXIT_QUAD,
        0x00000000,
        NULL,
        0,
        QSPI_INSTRUCTION_4_LINES,
        QSPI_ADDRESS_NONE,
        QSPI_DATA_NONE,
        0x00,
        QSPI_SIOO_INST_EVERY_CMD,
        PSRAM_DIRECTION_NONE
    );
}

uint8_t psram_read( psram_t *psram, uint32_t address, uint8_t *buff, uint32_t size )
{
    return qspi_command(
        psram,
        PSRAM_CMD_FAST_QUAD_READ,
        address,
        buff,
        size,
        QSPI_INSTRUCTION_4_LINES,
        QSPI_ADDRESS_4_LINES,
        QSPI_DATA_4_LINES,
        0x06,
        QSPI_SIOO_INST_EVERY_CMD,
        PSRAM_DIRECTION_RECEIVE
    );
}

uint8_t psram_write( psram_t *psram, uint32_t address, uint8_t *buff, uint32_t size )
{
    return qspi_command(
        psram,
        PSRAM_CMD_QUAD_WRITE,
        address,
        buff,
        size,
        QSPI_INSTRUCTION_4_LINES,
        QSPI_ADDRESS_4_LINES,
        QSPI_DATA_4_LINES,
        0x00,
        QSPI_SIOO_INST_EVERY_CMD,
        PSRAM_DIRECTION_TRANSMIT
    );
}

#define PSRAM_TEST_BUF_SIZE (1024)
#include <string.h>
uint8_t buf_tx[PSRAM_TEST_BUF_SIZE];
uint8_t buf_rx[PSRAM_TEST_BUF_SIZE];
uint8_t psram_test()
{
	// For PCU = 170MHz and prescaler = 6 and BUF_SIZE=1024:
	// tx_MBs = 10, rx_MBs = 10
	uint32_t i;

	extern QSPI_HandleTypeDef hqspi1;
	psram_t psram;
	uint32_t address = PSRAM_SIZE_BYTES - PSRAM_TEST_BUF_SIZE;

	uint32_t t0_cpu, t1_cpu;
	uint32_t t0_ms, t1_ms;
	float dt_us;
	float tx_MBs, rx_MBs;

	for( i = 0 ; i < sizeof( buf_tx ) ; i++ )
	{
		buf_tx[i] = i;
		buf_rx[i] = 0;
    }

	psram_init( &psram, &hqspi1 );
	psram_quad_disable( &psram );
	psram_reset( &psram );
	if( !psram_read_id( &psram ) )
	{
		return 0;
	}
	psram_quad_enable( &psram );

    t0_ms = HAL_GetTick();
    t0_cpu = SysTick->VAL;
	psram_write( &psram, address, buf_tx, sizeof( buf_tx ) );
	t1_cpu = SysTick->VAL;
	t1_ms = HAL_GetTick();
	if( t1_cpu > t0_cpu )
	{
		dt_us = (t0_cpu+SysTick->LOAD-t1_cpu)/170.0 + (t1_ms-t0_ms-1)*1000.0;
	}
	else
	{
		dt_us = (t0_cpu-t1_cpu)/170.0 + (t1_ms-t0_ms)*1000.0;
	}
	tx_MBs = sizeof( buf_tx )/(dt_us*1e-6)/(1024*1024);

    t0_ms = HAL_GetTick();
    t0_cpu = SysTick->VAL;
    psram_read( &psram, address, buf_rx, sizeof( buf_rx ) );
	t1_cpu = SysTick->VAL;
	t1_ms = HAL_GetTick();
	if( t1_cpu > t0_cpu )
	{
		dt_us = (t0_cpu+SysTick->LOAD-t1_cpu)/170.0 + (t1_ms-t0_ms-1)*1000.0;
	}
	else
	{
		dt_us = (t0_cpu-t1_cpu)/170.0 + (t1_ms-t0_ms)*1000.0;
	}
	rx_MBs = sizeof( buf_rx )/(dt_us*1e-6)/(1024*1024);

	psram_quad_disable( &psram );

	if( !memcmp( buf_tx, buf_rx, sizeof( buf_tx ) ) )
	{
		return 1;
	}
	return 0;
}
