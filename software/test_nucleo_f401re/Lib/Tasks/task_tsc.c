/*
 * task_tsc.c
 *
 *  Created on: Sep 18, 2023
 *      Author: jgpei
 */

#include "cmsis_os.h"

#include "main.h"
#include "spi.h"

#include "tsc.h"

extern osMessageQueueId_t queueTscUiHandle;
extern osSemaphoreId_t semSpiHandle;

void StartTaskTsc(void *argument)
{
	const TickType_t xFrequency = 10;
	TickType_t xLastWakeTime;

	struct sQueueTscUi msg = {0};
	int slow_cnt = 0;
	int msg_p_bck = 0;

	// Calibration coefficients
	float AX = 38.0/151.0;
	float BX = -1950.0/151.0;
	float AY = 11.0/62.0;
	float BY = -1157.0/62.0;

	tTsc tsc = {0};
    if( osSemaphoreAcquire( semSpiHandle, portMAX_DELAY ) == osOK )
    {
    	float x0 = 0;
    	float y0 = 0;
    	float x1 = 0;
    	float y1 = 0;

    	float AX = 0;
    	float BX = 0;
    	float AY = 0;
    	float BY = 0;

    	x0 = 1656;
    	x1 = 434;
    	y0 = 50;
    	y1 = 320-50;
    	AX = (y0-y1)/(x0-x1);
    	BX =-((x1*y0-x0*y1)/(x0-x1));

    	x0 = 519;
    	x1 = 1573;
    	y0 = 50;
    	y1 = 240-50;
    	AY = (y0-y1)/(x0-x1);
    	BY =-((x1*y0-x0*y1)/(x0-x1));

		tsc_init(&tsc,
			&hspi1,
			TSC_CS_GPIO_Port, TSC_CS_Pin,
			AX, BX, AY, BY,
			0.9, // tau
			10   // cnt_max
		);
        osSemaphoreRelease( semSpiHandle );
    }

    msg.x = 0;
    msg.y = 0;
    msg.p = 0;
    osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);

    xLastWakeTime = xTaskGetTickCount();
	for(;;) {
		// Wait for a fixed time interval
		vTaskDelayUntil(&xLastWakeTime, xFrequency);

		// Read touch data from the TSC2046
	    if (osSemaphoreAcquire(semSpiHandle, portMAX_DELAY) == osOK) {
	    	int CR1 = tsc.spi->Instance->CR1;
	    	tsc.spi->Instance->CR1 &= 0xFFFFFFC7;
			tsc.spi->Instance->CR1 |= SPI_BAUDRATEPRESCALER_16;
	    	tsc_read(&tsc, &msg.x, &msg.y, &msg.p);
			tsc.spi->Instance->CR1 = CR1;
	        osSemaphoreRelease(semSpiHandle);
	    }

		// Handle pressure detection and slow count
		if( msg.p && !msg_p_bck ) {
			// Pressure detected, send data with maximum delay
			osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
			slow_cnt = 0;
		} else if( msg.p ) {
			// Pressure detected, increment slow count
			if( slow_cnt < 10 ) {
				slow_cnt += 1;
			} else {
				// Slow count threshold reached, send data with no delay
				osMessageQueuePut(queueTscUiHandle, &msg, 0U, 0);
				slow_cnt = 0;
			}
		} else if( !msg.p && msg_p_bck ) {
			// Pressure not detected, send data with maximum delay
			osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
		}
		msg_p_bck = msg.p;
	}
}
