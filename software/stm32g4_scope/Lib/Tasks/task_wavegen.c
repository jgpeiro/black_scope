/*
 * task_wavegen.c
 *
 *  Created on: Aug 31, 2023
 *      Author: jgpei
 */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "tim.h"
#include "adc.h"
#include "dac.h"
#include "opamp.h"
#include "spi.h"

#include "lcd.h"
#include "wavegen.h"
#include "ui.h"

#include "scope_tasks.h"

#define DAC_BUFFER_LEN (512)

// DAC buffer for channel 1 and channel 2
uint16_t dac1_buffer[DAC_BUFFER_LEN];
uint16_t dac2_buffer[DAC_BUFFER_LEN];

// External variables for UI and LCD
extern tUi ui;
extern tLcd lcd;

/**
 * @brief Task to handle the wave generator functionality.
 *
 * This task manages the wave generator by configuring its parameters, starting and stopping it,
 * and updating the LCD display. It also responds to UI queue messages to adjust wave generator settings.
 *
 * Pseudocode:
 *
 * Initialize wavegen structure with DAC and timer handles.
 * Configure wavegen settings for channel 1 and channel 2.
 * Start wave generation for both channels.
 *
 * Enter the main loop:
 *   - Wait for a fixed time interval (xFrequency).
 *   - Check for UI queue messages to update wavegen settings or visibility.
 *   - If visible, erase the previous wavegen display on the LCD.
 *   - Update wavegen settings based on received messages (e.g., start, stop, configure).
 *   - If visible, draw the updated wavegen display on the LCD.
 *
 * Additionally, a slow count is used to periodically redraw the wavegen display for visibility.
 *
@startuml
start
:Initialize wavegen structure with DAC and timer handles;
:Configure wavegen settings for channel 1 and channel 2;
:Start wave generation for both channels;
while (true) is (true)
    :Wait for a fixed time interval (xFrequency);
    if (UI wavegen messages are available) then (yes)
        :Erase the previous wavegen display on the LCD;
        :Handle UI messages to adjust wavegen settings;
        :Draw the updated wavegen display on the LCD;
    endif
endwhile
@enduml
 * @param argument Task argument.
 */
void StartTaskWavegen(void *argument) {
    static tWaveGen wavegen = {0};
    int is_visible = 0;
    int is_collapsed = 0;
    int slow_cnt = 0;

    struct sQueueUiWavegen msgUiWavegen = {0};

    // Initialize wavegen with DAC and timer handles
    wavegen_init_ll(&wavegen, &hdac1, &htim4, &htim6);

    // Initialize wavegen settings for both channels
    wavegen_init(&wavegen, dac1_buffer, dac2_buffer, DAC_BUFFER_LEN);
    wavegen_config_horizontal(&wavegen, WAVEGEN_CHANNEL_1, 1000);
    wavegen_config_vertical(&wavegen, WAVEGEN_CHANNEL_1, WAVEGEN_TYPE_SINE, 2048, 2000, 0);
    wavegen_config_horizontal(&wavegen, WAVEGEN_CHANNEL_2, 1000);
    wavegen_config_vertical(&wavegen, WAVEGEN_CHANNEL_2, WAVEGEN_TYPE_SINE, 2048, 2000, 0);
    wavegen_start(&wavegen, WAVEGEN_CHANNEL_1);
    wavegen_start(&wavegen, WAVEGEN_CHANNEL_2);

    // Configure task frequency
    const TickType_t xFrequency = 1;
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        // Wait for a fixed time interval
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // Check for UI queue messages
        if (osMessageQueueGet(queueUiWavegenHandle, &msgUiWavegen, NULL, 0U) == osOK) {
            if (is_visible) {
                if (osSemaphoreAcquire(semaphoreLcdHandle, portMAX_DELAY) == osOK) {
                    // Erase the previous wavegen display on the LCD
                    wavegen_erase(&wavegen, &lcd, is_collapsed);
                    osSemaphoreRelease(semaphoreLcdHandle);
                }
            }

            // Handle UI messages to adjust wavegen settings
            switch (msgUiWavegen.type) {
                case QUEUE_UI_WAVEGEN_TYPE_START:
                    wavegen_start(&wavegen, msgUiWavegen.data[0]); // Start wave generation for a channel
                    break;
                case QUEUE_UI_WAVEGEN_TYPE_STOP:
                    wavegen_stop(&wavegen, msgUiWavegen.data[0]); // Stop wave generation for a channel
                    break;
                case QUEUE_UI_WAVEGEN_TYPE_CONFIG_HORIZONTAL:
                    wavegen_config_horizontal(&wavegen, msgUiWavegen.data[0], msgUiWavegen.data[1]); // Configure horizontal settings
                    break;
                case QUEUE_UI_WAVEGEN_TYPE_CONFIG_VERTICAL:
                    wavegen_config_vertical(&wavegen, msgUiWavegen.data[0], msgUiWavegen.data[1],
                                            msgUiWavegen.data[2], msgUiWavegen.data[3], msgUiWavegen.data[4]); // Configure vertical settings
                    break;
                case QUEUE_UI_WAVEGEN_TYPE_CHANGE_VISIBILITY:
                    is_visible = msgUiWavegen.data[0]; // Change visibility
                    break;
                case QUEUE_UI_WAVEGEN_TYPE_CHANGE_COLLAPSED:
                    is_collapsed = msgUiWavegen.data[0]; // Change collapsed state
                    break;
                default:
                    break;
            }

            if (is_visible) {
                if (osSemaphoreAcquire(semaphoreLcdHandle, portMAX_DELAY) == osOK) {
                    // Draw the updated wavegen display on the LCD
                    wavegen_draw(&wavegen, &lcd, is_collapsed);
                    osSemaphoreRelease(semaphoreLcdHandle);
                }
            }
        }

        // Periodically redraw the wavegen display for visibility
        if (slow_cnt == 0) {
            if (is_visible) {
                if (osSemaphoreAcquire(semaphoreLcdHandle, portMAX_DELAY) == osOK) {
                    // Redraw the wavegen display on the LCD
                    wavegen_draw(&wavegen, &lcd, is_collapsed);
                    osSemaphoreRelease(semaphoreLcdHandle);
                }
            }
        }
        slow_cnt = (slow_cnt + 1) % 10;
    }
}
