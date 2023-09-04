/*
 * task_tsc.c
 *
 *  Created on: Aug 31, 2023
 *      Author: jgpei
 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "Tsc.h"
#include "spi.h"

#include "scope_tasks.h"


void StartTaskTsc(void *argument) {
	const TickType_t xFrequency = 1;
	TickType_t xLastWakeTime;

	struct sQueueTscUi msg = {0};
	int cnt = 0;
	int p_bck = 0;

    float AX = 38.0/151.0;
	float BX = -1950.0/151.0;
	float AY = 11.0/62.0;
	float BY = -1157.0/62.0;

	tTsc tsc = {0};

	tsc_init( &tsc,
		&hspi3,
		TSC_nSS_GPIO_Port, TSC_nSS_Pin,
		AX, BX, AY, BY,
		32
	);

	xLastWakeTime = xTaskGetTickCount();
	for(;;) {
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		tsc_read( &tsc, &msg.x, &msg.y, &msg.p );
		if( msg.p && !p_bck ) {
			osMessageQueuePut( queueTscUiHandle, &msg, 0U, portMAX_DELAY );
			cnt = 0;
		}else if( msg.p ){
			if( cnt < 10 ){
				cnt += 1;
			}else{
				osMessageQueuePut( queueTscUiHandle, &msg, 0U, 0 );
				cnt = 0;
			}
		}else if( !msg.p && p_bck ){
			osMessageQueuePut( queueTscUiHandle, &msg, 0U, portMAX_DELAY );
		}
		p_bck = msg.p;
  	}
}


