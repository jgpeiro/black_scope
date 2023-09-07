/*
 * scope_tasks.h
 *
 *  Created on: Aug 31, 2023
 *      Author: jgpei
@startuml
actor User
participant TaskTsc
participant TaskUi
participant TaskScope
participant TaskWavegen
participant semLcdHandle

User -> TaskTsc: Touch Input
TaskTsc --> TaskUi: Touch Data

TaskUi --> TaskScope: UI Scope Messages

TaskScope --> semLcdHandle: Acquire LCD Semaphore
TaskScope --> semLcdHandle: Release LCD Semaphore

TaskUi --> TaskWavegen: UI Wavegen Messages

TaskWavegen --> semLcdHandle: Acquire LCD Semaphore
TaskWavegen --> semLcdHandle: Release LCD Semaphore

TaskUi -> semLcdHandle: Acquire LCD Semaphore
TaskUi --> semLcdHandle: Release LCD Semaphore
@enduml

 */

#ifndef TASKS_SCOPE_TASKS_H_
#define TASKS_SCOPE_TASKS_H_

#include "FreeRTOS.h"

/**
 * @brief Structure representing the data for the TSC (Touch Screen Controller) UI queue.
 */
struct sQueueTscUi {
    uint16_t x; /**< X-coordinate. */
    uint16_t y; /**< Y-coordinate. */
    uint16_t p; /**< Pressure value. */
};

/**
 * @brief Enum representing the different types of UI scope queue messages.
 */
enum eQueueUiScopeType {
    QUEUE_UI_SCOPE_TYPE_START,           /**< Start scope command. */
    QUEUE_UI_SCOPE_TYPE_STOP,            /**< Stop scope command. */
    QUEUE_UI_SCOPE_TYPE_HORIZONTAL,      /**< Horizontal configuration command. */
    QUEUE_UI_SCOPE_TYPE_VERTICAL,        /**< Vertical configuration command. */
    QUEUE_UI_SCOPE_TYPE_TRIGGER,         /**< Trigger configuration command. */
    QUEUE_UI_SCOPE_TYPE_CHANGE_VISIBILITY,/**< Change visibility command. */
    QUEUE_UI_SCOPE_TYPE_CHANGE_COLLAPSED  /**< Change collapsed state command. */
};

/**
 * @brief Structure representing the data for the UI scope queue.
 */
struct sQueueUiScope {
    uint16_t type; /**< Type of the message. */
    uint16_t data[8]; /**< Message data. */
};

/**
 * @brief Enum representing the different types of UI wavegen queue messages.
 */
enum eQueueUiWavegenType {
    QUEUE_UI_WAVEGEN_TYPE_START,           /**< Start wavegen command. */
    QUEUE_UI_WAVEGEN_TYPE_STOP,            /**< Stop wavegen command. */
    QUEUE_UI_WAVEGEN_TYPE_CONFIG_HORIZONTAL,/**< Horizontal configuration command. */
    QUEUE_UI_WAVEGEN_TYPE_CONFIG_VERTICAL,  /**< Vertical configuration command. */
    QUEUE_UI_WAVEGEN_TYPE_CHANGE_VISIBILITY,/**< Change visibility command. */
    QUEUE_UI_WAVEGEN_TYPE_CHANGE_COLLAPSED  /**< Change collapsed state command. */
};

/**
 * @brief Structure representing the data for the UI wavegen queue.
 */
struct sQueueUiWavegen {
    uint16_t type; /**< Type of the message. */
    uint16_t data[8]; /**< Message data. */
};

extern osMessageQueueId_t queueTscUiHandle;
extern osMessageQueueId_t queueUiScopeHandle;
extern osMessageQueueId_t queueUiWavegenHandle;
extern osSemaphoreId_t semaphoreLcdHandle;

extern osThreadId_t taskTscHandle;
extern osThreadId_t taskUiHandle;
extern osThreadId_t taskScopeHandle;
extern osThreadId_t taskWavegenHandle;

/**
 * @brief Start the TSC task.
 * @param argument Task argument.
 */
void StartTaskTsc(void *argument);

/**
 * @brief Start the UI task.
 * @param argument Task argument.
 */
void StartTaskUi(void *argument);

/**
 * @brief Start the Scope task.
 * @param argument Task argument.
 */
void StartTaskScope(void *argument);

/**
 * @brief Start the Wavegen task.
 * @param argument Task argument.
 */
void StartTaskWavegen(void *argument);

#endif /* TASKS_SCOPE_TASKS_H_ */
