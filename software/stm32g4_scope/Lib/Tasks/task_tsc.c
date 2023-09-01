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

struct sQueueTscUi {
    uint16_t p;
    uint16_t x;
    uint16_t y;
};

extern osMessageQueueId_t queueTscUiHandle;

void StartTaskTsc(void *argument)
{
	const TickType_t xFrequency = 1;
	TickType_t xLastWakeTime;

	struct sQueueTscUi msgTscUi = {0};

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
	uint16_t x;
	uint16_t y;
	uint16_t x_low = 0;
	uint16_t y_low = 0;
	int16_t cnt = 0;
	int released_sent = 0;
	int cnt2 = 0;
	for(;;)
  	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		//tsc_read( &tsc, &msgTscUi.p, &msgTscUi.x, &msgTscUi.y );
		tsc_read_ll( &tsc, &x, &y );
		if( x )
		{
			if( cnt < 50 )
			{
				cnt += 1;
			}
		}
		else
		{
			cnt = 0;
		}
		x_low = x_low*0.9 + x*0.1;
		y_low = y_low*0.9 + y*0.1;

		if( x )
		{

		}
		else
		{
			x_low = 0;
			y_low = 0;
		}

		msgTscUi.x = tsc.ax * x_low + tsc.bx;
		msgTscUi.y = tsc.ay * y_low + tsc.by;
		msgTscUi.p = x && (cnt>40);
		if( x )
		{
			if( cnt2 < 10 )
			{
				cnt2 += 1;
			}
			else
			{
				cnt2 = 0;
				osMessageQueuePut( queueTscUiHandle, &msgTscUi, 0U, 0U );
			}
			released_sent = 0;
		}
		else
		{
			if( released_sent == 0 )
			{
				osMessageQueuePut( queueTscUiHandle, &msgTscUi, 0U, portMAX_DELAY );
				released_sent = 1;
			}
		}
  	}
}

void __StartTaskTsc(void *argument) {
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
		tsc_read( &tsc, &msg.p, &msg.x, &msg.y );
		if( msg.p && !p_bck ) {
			osMessageQueuePut( queueTscUiHandle, &msg, 0U, portMAX_DELAY );
			cnt = 0;
		}else if( msg.p && !p_bck ){
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


