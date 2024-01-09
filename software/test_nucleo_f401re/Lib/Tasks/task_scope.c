/*
 * task_scope.c
 *
 *  Created on: Sep 20, 2023
 *      Author: jgpei
 */


/*
 * task_tsc.c
 *
 *  Created on: Sep 18, 2023
 *      Author: jgpei
 */

#include "cmsis_os.h"

#include "main.h"
#include "adc.h"
#include "tim.h"
#include "spi.h"

#include "scope.h"

extern osMessageQueueId_t queueUiScopeHandle;
extern osSemaphoreId_t semSpiHandle;

#define SCOPE_BUFFER_LEN (512*2)
uint16_t bufferA[SCOPE_BUFFER_LEN];
uint16_t bufferB[SCOPE_BUFFER_LEN];

void StartTaskScope(void *argument)
{
	const TickType_t xFrequency = 10;
	TickType_t xLastWakeTime;
    uint8_t run = 1;
	tScope scope;
	extern tLcd lcd;
	struct sQueueUiScope msg = {0};

	__HAL_DBGMCU_FREEZE_TIM2();
	__HAL_DBGMCU_FREEZE_TIM3();

	scope_init( &scope,
		&hadc1,
		&htim2,
		&htim3,
		bufferA,
		bufferB,
		SCOPE_BUFFER_LEN
	);

	scope_config_horizontal(&scope, 0, 1000 );
	scope_config_vertical( &scope, 0, 1 );
	scope_config_trigger( &scope, 0, 2048, SCOPE_SLOPE_RISING );


    xLastWakeTime = xTaskGetTickCount();
	for(;;) {
		//vTaskDelayUntil(&xLastWakeTime, xFrequency);
		vTaskDelay(xFrequency);

		if( osMessageQueueGet(queueUiScopeHandle, &msg, NULL, 0) == osOK )
		{
			run = !run;
		}

		if( run )
		{
			scope_start( &scope );
			scope_wait( &scope, 100 );
			scope_stop( &scope );

			if( osSemaphoreAcquire( semSpiHandle, portMAX_DELAY ) == osOK )
			{
				scope_draw( &scope, &lcd );
				osSemaphoreRelease( semSpiHandle );
			}
		}
	}
}
