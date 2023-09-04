/*
 * tasks.h
 *
 *  Created on: Aug 31, 2023
 *      Author: jgpei
 */

#ifndef TASKS_SCOPE_TASKS_H_
#define TASKS_SCOPE_TASKS_H_

#include "FreeRTOS.h"

struct sQueueTscUi {
    uint16_t x;
    uint16_t y;
    uint16_t p;
};

enum eQueueUiScopeType
{
	QUEUE_UI_SCOPE_TYPE_START,
	QUEUE_UI_SCOPE_TYPE_STOP,
	QUEUE_UI_SCOPE_TYPE_HORIZONTAL,
	QUEUE_UI_SCOPE_TYPE_VERTICAL,
	QUEUE_UI_SCOPE_TYPE_TRIGGER,
	QUEUE_UI_SCOPE_TYPE_CHANGE_VISIBILITY,
	QUEUE_UI_SCOPE_TYPE_CHANGE_COLLAPSED
};

struct sQueueUiScope {
    uint16_t type;
    uint16_t data[8];
};

enum eQueueUiWavegenType
{
	QUEUE_UI_WAVEGEN_TYPE_START,
	QUEUE_UI_WAVEGEN_TYPE_STOP,
	QUEUE_UI_WAVEGEN_TYPE_CONFIG_HORIZONTAL,
	QUEUE_UI_WAVEGEN_TYPE_CONFIG_VERTICAL,
	QUEUE_UI_WAVEGEN_TYPE_CHANGE_VISIBILITY,
	QUEUE_UI_WAVEGEN_TYPE_CHANGE_COLLAPSED
};

struct sQueueUiWavegen {
    uint16_t type;
    uint16_t data[8];
};

extern osMessageQueueId_t queueTscUiHandle;
extern osMessageQueueId_t queueUiScopeHandle;
extern osMessageQueueId_t queueUiWavegenHandle;
extern osSemaphoreId_t semaphoreLcdHandle;

void StartTaskTsc(void *argument);
void StartTaskUi(void *argument);
void StartTaskScope(void *argument);
void StartTaskWavegen(void *argument);

#endif /* TASKS_SCOPE_TASKS_H_ */
