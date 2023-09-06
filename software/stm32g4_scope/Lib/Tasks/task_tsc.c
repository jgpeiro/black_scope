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

/**
 * @brief Task to handle the TSC2046 touch controller.
 *
 * This task periodically reads data from the TSC2046 touch controller, performs
 * filtering and stabilization, and sends the data to the UI queue for further
 * processing.
 *
 * Pseudocode:
 *
 * Initialize TSC2046 touch controller with calibration coefficients.
 *
 * while(1) {
 *     Wait for a fixed time interval (xFrequency).
 *     Read raw touch data (X, Y, and pressure) from the TSC2046.
 *     If pressure is detected and it was not detected in the previous iteration:
 *         Send the data to the UI queue with maximum delay.
 *         Reset the slow count.
 *     Else if pressure is detected:
 *         Increment the slow count.
 *         If the slow count reaches a threshold (e.g., 10):
 *             Send the data to the UI queue with no delay.
 *             Reset the slow count.
 *     Else (pressure is not detected):
 *         If pressure was detected in the previous iteration:
 *             Send the data to the UI queue with maximum delay.
 * }
 *
@startuml
start
:Initialize TSC2046 touch controller;
while (true) is (true)
    :Wait for a fixed time interval (xFrequency);
    :Read touch data from the TSC2046;
    if (pressure is detected) then (yes)
        :Send data to the UI queue;
    endif
endwhile
@enduml
 * @param argument Task argument.
 */
void StartTaskTsc(void *argument) {
    const TickType_t xFrequency = 1;
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

    tsc_init(&tsc,
        &hspi3,
        TSC_nSS_GPIO_Port, TSC_nSS_Pin,
        AX, BX, AY, BY,
        0.9, // tau
        40   // cnt_max
    );

    xLastWakeTime = xTaskGetTickCount();
    for(;;) {
        // Wait for a fixed time interval
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // Read touch data from the TSC2046
        tsc_read(&tsc, &msg.x, &msg.y, &msg.p);

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
