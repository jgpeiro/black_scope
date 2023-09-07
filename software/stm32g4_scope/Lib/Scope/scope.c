/**
  ******************************************************************************
  * @file    scope.c
  * @author  jgpei
  * @brief   This file contains the implementation of the scope control module.
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                      ##### Scope Control Module #####
  ==============================================================================
  [..]
    (+) Initialization and configuration of the scope module.
    (+) Configuration of horizontal and vertical settings.
    (+) Configuration of trigger settings.
    (+) Start and stop scope operation.
    (+) Wait for scope operation to complete.
    (+) Check the status of scope operation.

                     ##### How to use this module #####
  ==============================================================================
  [..]
    (#) Include the "scope.h" header file in your project.

    (#) Create a tScope structure and initialize it using the "scope_init" function.
       This function sets up the hardware timers, DAC, OPAMPs, and ADCs required for the scope.

    (#) Configure the horizontal settings using the "scope_config_horizontal" function.
       This function sets the horizontal offset and scale.

    (#) Configure the vertical settings using the "scope_config_vertical" function.
       This function sets the vertical offset and gain for each channel.

    (#) Configure the trigger settings using the "scope_config_trigger" function.
       This function sets the trigger channel, mode, level, and slope.

    (#) Start the scope operation using the "scope_start" function.
       You can choose continuous or single-shot operation.

    (#) Wait for the scope operation to complete using the "scope_wait" function.

    (#) Check the status of scope operation using the provided status functions.

  @endverbatim
  ******************************************************************************
  */

#include "scope.h"

tScope *pScope = NULL;

/**
  * @brief Callback function invoked when an ADC conversion is completed.
  *
  * This function is called when an ADC conversion process is completed.
  * It is typically used for scope functionality to handle conversion
  * completion and control the scope's state transitions.
  *
  * @param hadc Pointer to the ADC handle that triggered the callback.
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    // Check if the scope is waiting for conversion completion
    if (pScope->state == SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE) {
        // Clear the ADC analog watchdog 1 flag
        __HAL_ADC_CLEAR_FLAG(pScope->trigger.hadc1, ADC_FLAG_AWD1);

        // Enable ADC analog watchdog 1 interrupt
        LL_ADC_EnableIT_AWD1(pScope->trigger.hadc1->Instance);

        // Transition the scope state to waiting for the arm signal
        pScope->state = SCOPE_STATE_WAIT_FOR_ARM;
    }
}

/**
  * @brief Callback function invoked when an ADC level goes out of the window.
  *
  * This function is called when an ADC level goes out of the window defined
  * by the analog watchdog 2. It is typically used in a scope application to
  * handle situations where the ADC input value falls out of the specified
  * window range.
  *
  * @param hadc Pointer to the ADC handle that triggered the callback.
  */
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc) {
    // Check if the scope is waiting for the arm signal
    if (pScope->state == SCOPE_STATE_WAIT_FOR_ARM) {
        // Disable ADC analog watchdog 1 interrupt
        LL_ADC_DisableIT_AWD1(pScope->trigger.hadc1->Instance);

        // Clear the ADC analog watchdog 2 flag
        __HAL_ADC_CLEAR_FLAG(pScope->trigger.hadc1, ADC_FLAG_AWD2);

        // Enable ADC analog watchdog 2 interrupt
        LL_ADC_EnableIT_AWD2(pScope->trigger.hadc1->Instance);

        // Transition the scope state to waiting for trigger
        pScope->state = SCOPE_STATE_WAIT_FOR_TRIGGER;
    }
}

/**
  * @brief Callback function invoked when ADC level is out of window 2.
  *
  * This function is called when the ADC level goes out of the specified window 2.
  * It is typically used for scope functionality to handle the trigger event and
  * control the scope's state transitions.
  *
  * @param hadc Pointer to the ADC handle that triggered the callback.
  */
void HAL_ADCEx_LevelOutOfWindow2Callback(ADC_HandleTypeDef* hadc) {
    if (pScope->state == SCOPE_STATE_WAIT_FOR_TRIGGER) {
        // Clear relevant TIM interrupts and flags
        __HAL_TIM_CLEAR_IT(pScope->horizontal.htim_stop, TIM_IT_CC1);
        __HAL_TIM_CLEAR_IT(pScope->horizontal.htim_stop, TIM_IT_CC2);
        __HAL_TIM_CLEAR_IT(pScope->horizontal.htim_stop, TIM_IT_UPDATE);
        __HAL_TIM_CLEAR_FLAG(pScope->horizontal.htim_stop, TIM_FLAG_CC1);
        __HAL_TIM_CLEAR_FLAG(pScope->horizontal.htim_stop, TIM_FLAG_CC2);
        __HAL_TIM_CLEAR_FLAG(pScope->horizontal.htim_stop, TIM_FLAG_UPDATE);

        // Start the horizontal timer for triggering
        HAL_TIM_Base_Start(pScope->horizontal.htim_stop);
        HAL_TIM_OnePulse_Start_IT(pScope->horizontal.htim_stop, TIM_CHANNEL_1);

        // Disable ADC analog watchdog 2 interrupt
        LL_ADC_DisableIT_AWD2(pScope->trigger.hadc1->Instance);

        // Transition the scope state to waiting for stop
        pScope->state = SCOPE_STATE_WAIT_FOR_STOP;
    }
}

/**
  * @brief Callback function invoked when a TIM (Timer) output compare (OC) delay elapsed event occurs.
  *
  * This function is called when a TIM output compare delay elapsed event is triggered.
  * It is typically used for handling timing-related events, such as stopping timers
  * and stopping ADC conversions in a scope application when waiting for a stop condition.
  *
  * @param htim Pointer to the TIM handle that triggered the callback.
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef* htim) {
    if (pScope->state == SCOPE_STATE_WAIT_FOR_STOP) {
        // Stop the TIM clock for horizontal synchronization
        HAL_TIM_Base_Stop(pScope->horizontal.htim_clock);
        
        // Stop DMA for ADC channels 1, 2, 3, and 4
        HAL_ADC_Stop_DMA(pScope->trigger.hadc1);
        HAL_ADC_Stop_DMA(pScope->trigger.hadc2);
        HAL_ADC_Stop_DMA(pScope->trigger.hadc3);
        HAL_ADC_Stop_DMA(pScope->trigger.hadc4);
        
        // Stop the one-pulse mode for TIM stop signal
        HAL_TIM_OnePulse_Stop_IT(pScope->horizontal.htim_stop, TIM_CHANNEL_1);
        
        // Stop the TIM used for the horizontal synchronization stop signal
        HAL_TIM_Base_Stop(pScope->horizontal.htim_stop);
        
        // Transition the scope state to idle
        pScope->state = SCOPE_STATE_IDLE;
    }
}


/**
  * @brief Initializes a scope structure with various hardware handles.
  *
  * This function initializes a scope structure with handles to different
  * hardware peripherals such as timers, DACs, OPAMPs, and ADCs. These handles
  * are used for configuring and controlling the hardware components of the scope.
  *
  * @param pThis Pointer to the scope structure to be initialized.
  * @param htim_clock Pointer to the timer handle for controlling the clock signal.
  * @param htim_stop Pointer to the timer handle for controlling the stop signal.
  * @param hdac Pointer to the DAC handle for controlling the vertical signal.
  * @param hopamp1 Pointer to the OPAMP handle 1.
  * @param hopamp2 Pointer to the OPAMP handle 2.
  * @param hopamp3 Pointer to the OPAMP handle 3.
  * @param hopamp4 Pointer to the OPAMP handle 4.
  * @param hadc1 Pointer to the ADC handle 1 for triggering.
  * @param hadc2 Pointer to the ADC handle 2.
  * @param hadc3 Pointer to the ADC handle 3.
  * @param hadc4 Pointer to the ADC handle 4.
  */
void scope_init_ll(tScope* pThis,
                   TIM_HandleTypeDef* htim_clock,
                   TIM_HandleTypeDef* htim_stop,
                   DAC_HandleTypeDef* hdac,
                   OPAMP_HandleTypeDef* hopamp1,
                   OPAMP_HandleTypeDef* hopamp2,
                   OPAMP_HandleTypeDef* hopamp3,
                   OPAMP_HandleTypeDef* hopamp4,
                   ADC_HandleTypeDef* hadc1,
                   ADC_HandleTypeDef* hadc2,
                   ADC_HandleTypeDef* hadc3,
                   ADC_HandleTypeDef* hadc4) {
    // Initialize the horizontal section with timer handles
    pThis->horizontal.htim_clock = htim_clock;
    pThis->horizontal.htim_stop = htim_stop;

    // Initialize the vertical section with DAC and OPAMP handles
    pThis->vertical.hdac = hdac;
    pThis->vertical.hopamp1 = hopamp1;
    pThis->vertical.hopamp2 = hopamp2;
    pThis->vertical.hopamp3 = hopamp3;
    pThis->vertical.hopamp4 = hopamp4;

    // Initialize the trigger section with ADC handles
    pThis->trigger.hadc1 = hadc1;
    pThis->trigger.hadc2 = hadc2;
    pThis->trigger.hadc3 = hadc3;
    pThis->trigger.hadc4 = hadc4;
}


/**
  * @brief Initialize a scope structure with the provided parameters.
  *
  * This function initializes a scope structure with the specified parameters
  * and sets its initial state and variables.
  *
  * @param pThis Pointer to the scope structure to be initialized.
  * @param buffer1 Pointer to the first buffer for storing data.
  * @param buffer2 Pointer to the second buffer for storing data.
  * @param buffer3 Pointer to the third buffer for storing data.
  * @param buffer4 Pointer to the fourth buffer for storing data.
  * @param buffer5 Pointer to the fifth buffer for storing data.
  * @param buffer6 Pointer to the sixth buffer for storing data.
  * @param buffer7 Pointer to the seventh buffer for storing data.
  * @param buffer8 Pointer to the eighth buffer for storing data.
  * @param len The length of each buffer (number of elements).
  */
void scope_init(tScope *pThis,
                uint16_t *buffer1,
                uint16_t *buffer2,
                uint16_t *buffer3,
                uint16_t *buffer4,
                uint16_t *buffer5,
                uint16_t *buffer6,
                uint16_t *buffer7,
                uint16_t *buffer8,
                uint16_t len) {
    // Initialize the buffer pointers and length
    pThis->buffer1 = buffer1;
    pThis->buffer2 = buffer2;
    pThis->buffer3 = buffer3;
    pThis->buffer4 = buffer4;
    pThis->buffer5 = buffer5;
    pThis->buffer6 = buffer6;
    pThis->buffer7 = buffer7;
    pThis->buffer8 = buffer8;
    pThis->len = len;

    // Initialize other scope variables
    pThis->cnt = 0;
    pThis->state = SCOPE_STATE_IDLE;
    pThis->continuous = 0;
    pThis->dma_cndtr = 0;
    pThis->trigged = 0;
}


/**
  * @brief Configure the horizontal settings of the scope.
  *
  * This function configures the horizontal settings of the scope, including
  * the offset and scale. It also initializes the timer for timing and triggering
  * purposes.
  *
  * @param pThis Pointer to the scope structure.
  * @param offset The horizontal offset in time units.
  * @param scale The horizontal scale factor.
  */
void scope_config_horizontal(tScope *pThis, int offset, int scale) {
    // Set the horizontal offset and scale
    pThis->horizontal.offset = offset;
    pThis->horizontal.scale = scale;

    // Configure the timer for clock generation
    pThis->horizontal.htim_clock->Init.Prescaler = (170e6 / (scale * 1000)) / 2 - 1;
    pThis->horizontal.htim_clock->Init.Period = 2 - 1;
    HAL_TIM_Base_Init(pThis->horizontal.htim_clock);

    // Configure the timer for stop time calculation
    pThis->horizontal.htim_stop->Init.Prescaler = (170e6 / (scale * 1000)) / 2 - 1;
    pThis->horizontal.htim_stop->Init.Period = pThis->len + offset - 1;
    HAL_TIM_Base_Init(pThis->horizontal.htim_stop);

    // Configure the timer output compare channel for triggering
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_TIMING;
    sConfigOC.Pulse = pThis->len + offset - 1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_OC_ConfigChannel(pThis->horizontal.htim_stop, &sConfigOC, TIM_CHANNEL_1);
}

/**
  * @brief Map of gain values to OPAMP PGA gains.
  *
  * This array maps gain values to corresponding OPAMP PGA gains.
  * Modify this array according to your application requirements.
  */
const uint32_t PgaGain_from_gain[6] = {
    OPAMP_PGA_GAIN_2_OR_MINUS_1,
    OPAMP_PGA_GAIN_4_OR_MINUS_3,
    OPAMP_PGA_GAIN_8_OR_MINUS_7,
    OPAMP_PGA_GAIN_16_OR_MINUS_15,
    OPAMP_PGA_GAIN_32_OR_MINUS_31,
    OPAMP_PGA_GAIN_64_OR_MINUS_63,
};


/**
  * @brief Configures the vertical settings for a scope.
  *
  * This function configures the vertical settings, including the offset
  * and gain for each channel of a scope.
  *
  * @param pThis Pointer to the scope structure to configure.
  * @param offset The offset value to set for the vertical axis.
  * @param scale1 The scale factor (gain) for channel 1.
  * @param scale2 The scale factor (gain) for channel 2.
  * @param scale3 The scale factor (gain) for channel 3.
  * @param scale4 The scale factor (gain) for channel 4.
  */
void scope_config_vertical(tScope* pThis, int offset, int scale1, int scale2, int scale3, int scale4) {
    // Set the vertical offset
    pThis->vertical.offset = offset;

    // Set the gain for each channel
    pThis->vertical.gain1 = scale1;
    pThis->vertical.gain2 = scale2;
    pThis->vertical.gain3 = scale3;
    pThis->vertical.gain4 = scale4;

    // Stop the DAC and set the offset
    HAL_DAC_Stop(pThis->vertical.hdac, DAC_CHANNEL_1);
    HAL_DAC_SetValue(pThis->vertical.hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, offset);
    HAL_DAC_Start(pThis->vertical.hdac, DAC_CHANNEL_1);

    // Configure the OPAMP PGA gains
    pThis->vertical.hopamp1->Init.PgaGain = PgaGain_from_gain[scale1];
    pThis->vertical.hopamp2->Init.PgaGain = PgaGain_from_gain[scale2];
    pThis->vertical.hopamp3->Init.PgaGain = PgaGain_from_gain[scale3];
    pThis->vertical.hopamp4->Init.PgaGain = PgaGain_from_gain[scale4];

    // Stop and reinitialize OPAMPs with new gains
    HAL_OPAMP_Stop(pThis->vertical.hopamp1);
    HAL_OPAMP_Stop(pThis->vertical.hopamp2);
    HAL_OPAMP_Stop(pThis->vertical.hopamp3);
    HAL_OPAMP_Stop(pThis->vertical.hopamp4);

    HAL_OPAMP_Init(pThis->vertical.hopamp1);
    HAL_OPAMP_Init(pThis->vertical.hopamp2);
    HAL_OPAMP_Init(pThis->vertical.hopamp3);
    HAL_OPAMP_Init(pThis->vertical.hopamp4);

    // Start OPAMPs with new gains
    HAL_OPAMP_Start(pThis->vertical.hopamp1);
    HAL_OPAMP_Start(pThis->vertical.hopamp2);
    HAL_OPAMP_Start(pThis->vertical.hopamp3);
    HAL_OPAMP_Start(pThis->vertical.hopamp4);
}


/**
  * @brief Configures the trigger settings for the scope.
  *
  * This function configures the trigger settings for the scope, including
  * the channel, mode, level, and slope. It also sets up the analog watchdog
  * configurations for the ADC channels used for triggering.
  *
  * @param pThis Pointer to the scope structure.
  * @param channel The ADC channel to use for triggering.
  * @param mode The trigger mode (e.g., UI_TRIGGER_MODE_NORMAL).
  * @param level The trigger level.
  * @param slope The trigger slope (e.g., SCOPE_TRIGGER_SLOPE_RISING).
  */
void scope_config_trigger(tScope *pThis, int channel, int mode, int level, int slope) {
    // Configure trigger settings
    pThis->trigger.channel = channel;
    pThis->trigger.mode = mode;
    pThis->trigger.level = level;
    pThis->trigger.slope = slope;

    // Configure Analog Watchdog configurations for ADC channels
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_arm = {0};
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_trig = {0};

    if (channel == 0) {
        // Set the values for AnalogWDGConfig_arm and AnalogWDGConfig_trig for channel 0
        // based on the trigger mode, level, and slope

        AnalogWDGConfig_arm.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
        AnalogWDGConfig_arm.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
        AnalogWDGConfig_arm.Channel = 0;
        AnalogWDGConfig_arm.ITMode = ENABLE;
        AnalogWDGConfig_arm.HighThreshold = 4095;
        AnalogWDGConfig_arm.LowThreshold = 0;
        AnalogWDGConfig_arm.FilteringConfig = ADC_AWD_FILTERING_NONE;

        AnalogWDGConfig_trig.WatchdogNumber = ADC_ANALOGWATCHDOG_2;
        AnalogWDGConfig_trig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REGINJEC;
        AnalogWDGConfig_trig.Channel = 0;
        AnalogWDGConfig_trig.ITMode = ENABLE;
        AnalogWDGConfig_trig.HighThreshold = 4095;
        AnalogWDGConfig_trig.LowThreshold = 0;
        AnalogWDGConfig_trig.FilteringConfig = ADC_AWD_FILTERING_NONE;

        if (slope == SCOPE_TRIGGER_SLOPE_RISING) {
            AnalogWDGConfig_arm.HighThreshold = 4095;
            AnalogWDGConfig_arm.LowThreshold = level;
            AnalogWDGConfig_trig.HighThreshold = level;
            AnalogWDGConfig_trig.LowThreshold = 0;
        } else {
            AnalogWDGConfig_arm.HighThreshold = level;
            AnalogWDGConfig_arm.LowThreshold = 0;
            AnalogWDGConfig_trig.HighThreshold = 4095;
            AnalogWDGConfig_trig.LowThreshold = level;
        }
    }

    // Configure Analog Watchdog for VOPAMP1
    AnalogWDGConfig_arm.Channel = ADC_CHANNEL_VOPAMP1;
    AnalogWDGConfig_trig.Channel = ADC_CHANNEL_VOPAMP1;
    HAL_ADC_AnalogWDGConfig(pThis->trigger.hadc1, &AnalogWDGConfig_arm);
    HAL_ADC_AnalogWDGConfig(pThis->trigger.hadc1, &AnalogWDGConfig_trig);

    // Reset thresholds and disable ITMode for subsequent channels
    AnalogWDGConfig_arm.HighThreshold = 4095;
    AnalogWDGConfig_arm.LowThreshold = 0;
    AnalogWDGConfig_trig.HighThreshold = 4095;
    AnalogWDGConfig_trig.LowThreshold = 0;
    AnalogWDGConfig_arm.ITMode = DISABLE;
    AnalogWDGConfig_trig.ITMode = DISABLE;

    // Configure Analog Watchdog for VOPAMP3_ADC3
    AnalogWDGConfig_arm.Channel = ADC_CHANNEL_VOPAMP3_ADC3;
    AnalogWDGConfig_trig.Channel = ADC_CHANNEL_VOPAMP3_ADC3;
    HAL_ADC_AnalogWDGConfig(pThis->trigger.hadc2, &AnalogWDGConfig_arm);
    HAL_ADC_AnalogWDGConfig(pThis->trigger.hadc2, &AnalogWDGConfig_trig);

    // Configure Analog Watchdog for VOPAMP5
    AnalogWDGConfig_arm.Channel = ADC_CHANNEL_VOPAMP5;
    AnalogWDGConfig_trig.Channel = ADC_CHANNEL_VOPAMP5;
    HAL_ADC_AnalogWDGConfig(pThis->trigger.hadc3, &AnalogWDGConfig_arm);
    HAL_ADC_AnalogWDGConfig(pThis->trigger.hadc3, &AnalogWDGConfig_trig);

    // Configure Analog Watchdog for VOPAMP6
    AnalogWDGConfig_arm.Channel = ADC_CHANNEL_VOPAMP6;
    AnalogWDGConfig_trig.Channel = ADC_CHANNEL_VOPAMP6;
    HAL_ADC_AnalogWDGConfig(pThis->trigger.hadc4, &AnalogWDGConfig_arm);
    HAL_ADC_AnalogWDGConfig(pThis->trigger.hadc4, &AnalogWDGConfig_trig);
}


/**
  * @brief Starts the scope for data acquisition.
  *
  * This function initializes and starts the scope for data acquisition. It
  * performs the following steps:
  * 1. Configures the scope's operating mode (continuous or single-shot).
  * 2. Performs self-calibration for the OP-AMPs used in the vertical channels.
  * 3. Starts the OP-AMPs for the vertical channels.
  * 4. Performs calibration for the ADCs used in the trigger channels.
  * 5. Disables analog watchdog interrupts for the ADCs.
  * 6. Clears ADC flags and sets the ADC DMA counters to zero.
  * 7. Starts ADC conversions using DMA based on the buffer selection.
  * 8. Starts the timer for horizontal synchronization.
  * 9. Sets the scope state to waiting for conversion completion.
  *
  * @param pThis Pointer to the scope structure.
  * @param continuous Flag indicating whether the scope operates in continuous mode (1) or single-shot mode (0).
  */
void scope_start(tScope* pThis, uint8_t continuous) {
    // Configure the operating mode
    pThis->continuous = continuous;
    pThis->dma_cndtr = 0;
    pThis->trigged = 0;

    // Self-calibrate the OP-AMPs for vertical channels
    HAL_OPAMP_SelfCalibrate(pThis->vertical.hopamp1);
    HAL_OPAMP_SelfCalibrate(pThis->vertical.hopamp2);
    HAL_OPAMP_SelfCalibrate(pThis->vertical.hopamp3);
    HAL_OPAMP_SelfCalibrate(pThis->vertical.hopamp4);

    // Start the OP-AMPs for vertical channels
    HAL_OPAMP_Start(pThis->vertical.hopamp1);
    HAL_OPAMP_Start(pThis->vertical.hopamp2);
    HAL_OPAMP_Start(pThis->vertical.hopamp3);
    HAL_OPAMP_Start(pThis->vertical.hopamp4);

    // Perform calibration for the ADCs in the trigger channels
    HAL_ADCEx_Calibration_Start(pThis->trigger.hadc1, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(pThis->trigger.hadc2, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(pThis->trigger.hadc3, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(pThis->trigger.hadc4, ADC_SINGLE_ENDED);

    // Disable analog watchdog interrupts for the ADCs
    LL_ADC_DisableIT_AWD1(pThis->trigger.hadc1->Instance);
    LL_ADC_DisableIT_AWD2(pThis->trigger.hadc1->Instance);

    // Clear ADC flags and set DMA counters to zero
    __HAL_ADC_CLEAR_FLAG(pThis->trigger.hadc1, ADC_FLAG_AWD1);
    __HAL_ADC_CLEAR_FLAG(pThis->trigger.hadc1, ADC_FLAG_AWD2);

    // Determine which buffer to use based on the count
    pThis->cnt += 1;
    if (pThis->cnt & 0x01) {
        HAL_ADC_Start_DMA(pThis->trigger.hadc1, (uint32_t*)pThis->buffer1, pThis->len);
        HAL_ADC_Start_DMA(pThis->trigger.hadc2, (uint32_t*)pThis->buffer2, pThis->len);
        HAL_ADC_Start_DMA(pThis->trigger.hadc3, (uint32_t*)pThis->buffer3, pThis->len);
        HAL_ADC_Start_DMA(pThis->trigger.hadc4, (uint32_t*)pThis->buffer4, pThis->len);
    } else {
        HAL_ADC_Start_DMA(pThis->trigger.hadc1, (uint32_t*)pThis->buffer5, pThis->len);
        HAL_ADC_Start_DMA(pThis->trigger.hadc2, (uint32_t*)pThis->buffer6, pThis->len);
        HAL_ADC_Start_DMA(pThis->trigger.hadc3, (uint32_t*)pThis->buffer7, pThis->len);
        HAL_ADC_Start_DMA(pThis->trigger.hadc4, (uint32_t*)pThis->buffer8, pThis->len);
    }

    // Start the timer for horizontal synchronization
    HAL_TIM_Base_Start(pThis->horizontal.htim_clock);

    // Set the scope state to waiting for conversion completion
    pThis->state = SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE;
}


/**
  * @brief Stops the scope and disables relevant peripherals.
  *
  * This function stops the scope by disabling relevant timers, ADCs, DACs,
  * and operational amplifiers. It also sets the scope state to IDLE.
  *
  * @param pThis Pointer to the scope structure.
  */
void scope_stop(tScope* pThis) {
    // Stop horizontal timers
    HAL_TIM_Base_Stop(pThis->horizontal.htim_clock);
    HAL_TIM_Base_Stop(pThis->horizontal.htim_stop);
    HAL_TIM_OnePulse_Stop_IT(pThis->horizontal.htim_stop, TIM_CHANNEL_1);

    // Stop ADCs
    HAL_ADC_Stop_DMA(pThis->trigger.hadc1);
    HAL_ADC_Stop_DMA(pThis->trigger.hadc2);
    HAL_ADC_Stop_DMA(pThis->trigger.hadc3);
    HAL_ADC_Stop_DMA(pThis->trigger.hadc4);

    // Stop DACs (if needed)
    // HAL_DAC_Stop(pThis->vertical.hdac, DAC_CHANNEL_1);
    // HAL_DAC_Stop(pThis->vertical.hdac, DAC_CHANNEL_2);

    // Stop operational amplifiers (if needed)
    // HAL_OPAMP_Stop(pThis->vertical.hopamp1);
    // HAL_OPAMP_Stop(pThis->vertical.hopamp2);
    // HAL_OPAMP_Stop(pThis->vertical.hopamp3);
    // HAL_OPAMP_Stop(pThis->vertical.hopamp4);

    // Set the scope state to IDLE
    pThis->state = SCOPE_STATE_IDLE;
}

#include "cmsis_os.h"

/**
  * @brief Waits for the scope to become idle or until a timeout occurs.
  *
  * This function waits for the scope to become idle or until a timeout
  * occurs. It checks whether the scope is busy and waits for it to become
  * idle.
  *
  * @param pThis Pointer to the scope structure.
  * @param timeout_ms The maximum time, in milliseconds, to wait for the scope to become idle.
  *
  * @return 1 if the scope becomes idle within the timeout, 0 if a timeout occurs.
  */
uint8_t scope_wait(tScope* pThis, uint32_t timeout_ms) {
    uint32_t start = HAL_GetTick();
    while (scope_is_busy(pThis)) {
        // Use osDelay(1) to yield to other tasks
        osDelay(1);
        if (HAL_GetTick() - start > timeout_ms) {
            return 0; // Timeout occurred
        }
    }
    return 1; // Scope is idle
}

/**
  * @brief Checks if the scope is in continuous mode.
  *
  * This function checks whether the scope is in continuous mode.
  *
  * @param pThis Pointer to the scope structure.
  *
  * @return 1 if the scope is in continuous mode, 0 otherwise.
  */
uint8_t scope_is_continuous(tScope* pThis) {
    return pThis->continuous;
}

/**
  * @brief Checks if the scope is currently running.
  *
  * This function checks if the scope is currently running, which means it is
  * not in the "done" or "idle" state.
  *
  * @param pThis Pointer to the scope structure.
  *
  * @return 1 if the scope is running, 0 otherwise.
  */
uint8_t scope_is_running(tScope* pThis) {
    return pThis->state != SCOPE_STATE_DONE && pThis->state != SCOPE_STATE_IDLE;
}

/**
  * @brief Checks if the scope is busy.
  *
  * This function checks if the scope is busy, meaning it is not in the "idle"
  * state.
  *
  * @param pThis Pointer to the scope structure.
  *
  * @return 1 if the scope is busy, 0 if it is idle.
  */
uint8_t scope_is_busy(tScope* pThis) {
    return pThis->state != SCOPE_STATE_IDLE;
}