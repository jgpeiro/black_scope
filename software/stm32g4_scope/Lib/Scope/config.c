#include <stdint.h>
#include "nuklear.h"
/*
#define UI_CHANNEL_COUNT    (4)
#define UI_WAVEGEN_COUNT    (2)
#define UI_CURSOR_COUNT     (2)

enum UI_TRIGGER_MODE {
    UI_TRIGGER_MODE_AUTO,
    UI_TRIGGER_MODE_NORMAL
};

enum UI_TRIGGER_SLOPE {
    UI_TRIGGER_SLOPE_RISING,
    UI_TRIGGER_SLOPE_FALLING,
};

enum UI_CHANNEL_COUPLING {
    UI_CHANNEL_COUPLING_AC,
    UI_CHANNEL_COUPLING_DC,
    UI_CHANNEL_COUPLING_GND
};

struct sUi_Acquire
{
    uint8_t run;
    uint8_t single;
};
typedef struct sUi_Acquire tUi_Acquire;

struct sUi_Horizontal
{
    int32_t offset;
    int32_t scale;
};
typedef struct sUi_Horizontal tUi_Horizontal;

struct sUi_Channel
{
    uint8_t enabled;
    enum UI_CHANNEL_COUPLING coupling;
    int32_t offset;
    int32_t scale;
    struct nk_color color;
};
typedef struct sUi_Channel tUi_Channel;

struct sUi_Vertical
{
    tUi_Channel channels[UI_CHANNEL_COUNT];
    uint8_t channel_selected;
};
typedef struct sUi_Vertical tUi_Vertical;

struct sUi_Trigger
{
    enum UI_TRIGGER_MODE mode;
    enum UI_TRIGGER_SLOPE slope;
    int32_t level;
    int32_t source;
    uint8_t armed;
};
typedef struct sUi_Trigger tUi_Trigger;

struct sUi_Waveform
{
    uint8_t enabled;
    int32_t type;
    int32_t offset;
    int32_t scale;
    int32_t duty_cycle;
    int32_t frequency;
};
typedef struct sUi_Waveform tUi_Waveform;

struct sUi_WaveGen
{
    tUi_Waveform waveforms[UI_WAVEGEN_COUNT];
    uint8_t waveform_selected;
};
typedef struct sUi_WaveGen tUI_WaveGen;


typedef struct sUi_Info tUi_Info;

struct sUi
{
    tUi_Acquire acquire;
    tUi_Horizontal horizontal;
    tUi_Vertical vertical;
    tUi_Trigger trigger;
    tUi_WaveGen wavegen;
};
typedef struct sUi tUi;
*/
#include <stdint.h>
#include "stm32g4xx_hal.h"

struct sScope_Horizontal
{
	TIM_HandleTypeDef *htim_clock; // ADC clock. TIMER2
	TIM_HandleTypeDef *htim_stop;  // Stop timer. TIMER3
};
typedef struct sScope_Horizontal tScope_Horizontal;

struct sScope_Vertical
{
    OPAMP_TypeDef *hopamp1; // CH1 OPAMP1
    OPAMP_TypeDef *hopamp2; // CH2 OPAMP3
    OPAMP_TypeDef *hopamp3; // CH3 OPAMP5
    OPAMP_TypeDef *hopamp4; // CH4 OPAMP6
    DAC_HandleTypeDef *hdac;      // Offset DAC
};
typedef struct sScope_Vertical tScope_Vertical;

struct sScope_Trigger
{
    ADC_HandleTypeDef *hadc1; // CH1 ADC1
    ADC_HandleTypeDef *hadc2; // CH2 ADC3
    ADC_HandleTypeDef *hadc3; // CH3 ADC5
    ADC_HandleTypeDef *hadc4; // CH4 ADC4
};
typedef struct sScope_Trigger tScope_Trigger;

struct sScope_Wavegen
{
    DAC_HandleTypeDef *hdac;  // DAC1 for CH1 and CH2
    TIM_HandleTypeDef *htim1; // CH1 clock. TIMER4
    TIM_HandleTypeDef *htim2; // CH2 clock. TIMER6
};
typedef struct sScope_Wavegen tScope_Wavegen;


enum eScopeState
{
	SCOPE_STATE_RESET = 0,
	SCOPE_STATE_WAIT_FOR_CONVERSION_COMPLETE = 1,
	SCOPE_STATE_WAIT_FOR_ARM = 2,
	SCOPE_STATE_WAIT_FOR_TRIGGER = 3,
	SCOPE_STATE_WAIT_FOR_STOP = 4,
	SCOPE_STATE_DONE = 5,
	SCOPE_STATE_MAX,
};
typedef enum eScopeState tScopeState;

struct sScope
{
    tScope_Horizontal horizontal;
    tScope_Vertical vertical;
    tScope_Trigger trigger;
    tScope_Wavegen wavegen;
    tScopeState state;
};
typedef struct sScope tScope;

void scope_init( tScope *scope, 
    tScope_Horizontal *horizontal, 
    tScope_Vertical *vertical, 
    tScope_Trigger *trigger, 
    tScope_Wavegen *wavegen );
void scope_reset( tScope *scope );
void scope_start( tScope *scope );
void scope_stop( tScope *scope );
uint8_t scope_is_busy( tScope *scope );
int32_t scope_get_trigger( tScope *scope );

void scope_config_horizontal( tScope *scope, int sample_rate, int buffer_len )
{
    // This function configures the timers so that the ADC is triggered at the frequency and to stop the conversion when the buffer is completed.
    int tim_freq = 170e6;
    int prescaler = 0;
    int period = 0;
    
    // Compute required Prescaler and Period for the ADC clock.
    scope->horizontal.htim_clock->Init.Prescaler = tim_freq / sample_rate - 1;
    scope->horizontal.htim_clock->Init.Period = 1;
    HAL_TIM_Base_Init( scope->horizontal.htim_clock );

    // Compute required Prescaler and Period for the Stop timer.
    scope->horizontal.htim_stop->Init.Prescaler = tim_freq / sample_rate - 1;
    scope->horizontal.htim_stop->Init.Period = buffer_len - 1;
    HAL_TIM_Base_Init( scope->horizontal.htim_stop );
}


int PgaGain_from_gain[6] =  // Table to convert gain to OPAMP PGA gain.{
    OPAMP_PGA_GAIN_2_OR_MINUS_1,
    OPAMP_PGA_GAIN_4_OR_MINUS_3,
    OPAMP_PGA_GAIN_8_OR_MINUS_7,
    OPAMP_PGA_GAIN_16_OR_MINUS_15,
    OPAMP_PGA_GAIN_32_OR_MINUS_31,
    OPAMP_PGA_GAIN_64_OR_MINUS_63,
};

int gain_to_pgagain( int gain )
{
    // convert gain from 1, 2, 5, 10, 20, 50 to a values from 0 t0 5.
    if( 0 <= gain && gain <= 1 )
    {
        return 0;
    }
    else if( 1 < gain && gain <= 5 )
    {
        return 1;
    }
    else if( 5 <= gain && gain <= 10 )
    {
        return 2;
    }
    else if( 10 <= gain && gain <= 20 )
    {
        return 3;
    }
    else if( 20 <= gain && gain <= 50 )
    {
        return 4;
    }
    else if( 50 <= gain )
    {
        return 5;
    }
    else
    {
        return 0;
    }
}

void scope_config_vertical( tScope *scope, int gain1, int gain2, int gain3, int gain4, int offset )
{
    // This function configures the OPAMPs and DACs for the vertical channels.
    gain1 = gain_to_pgagain( gain1 );
    gain2 = gain_to_pgagain( gain2 );
    gain3 = gain_to_pgagain( gain3 );
    gain4 = gain_to_pgagain( gain4 );

    scope->vertical.hopamp1->Init.PgaGain = PgaGain_from_gain[gain1];
    HAL_OPAMP_Init( scope->vertical.hopamp1 )
    scope->vertical.hopamp2->Init.PgaGain = PgaGain_from_gain[gain2];
    HAL_OPAMP_Init( scope->vertical.hopamp2 )
    scope->vertical.hopamp3->Init.PgaGain = PgaGain_from_gain[gain3];
    HAL_OPAMP_Init( scope->vertical.hopamp3 )
    scope->vertical.hopamp4->Init.PgaGain = PgaGain_from_gain[gain4];
    HAL_OPAMP_Init( scope->vertical.hopamp4 )

    HAL_OPAMP_SelfCalibrate( scope->vertical.hopamp1 );
    HAL_OPAMP_SelfCalibrate( scope->vertical.hopamp2 );
    HAL_OPAMP_SelfCalibrate( scope->vertical.hopamp3 );
    HAL_OPAMP_SelfCalibrate( scope->vertical.hopamp4 );
    HAL_OPAMP_Start( scope->vertical.hopamp1 );
    HAL_OPAMP_Start( scope->vertical.hopamp2 );
    HAL_OPAMP_Start( scope->vertical.hopamp3 );
    HAL_OPAMP_Start( scope->vertical.hopamp4 );

    HAL_DAC_SetValue( scope->vertical.hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, offset );
	HAL_DAC_Start( scope->vertical.hdac, DAC_CHANNEL_1 );
}

void scope_config_trigger( tScope *scope, int channel, in mode, int level, int slope )
{
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_arm_1 = {0};
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_arm_2 = {0};
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_arm_3 = {0};
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_arm_4 = {0};
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_trig_1 = {0};
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_trig_2 = {0};
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_trig_3 = {0};
    ADC_AnalogWDGConfTypeDef AnalogWDGConfig_trig_4 = {0};

    // reset all
    AnalogWDGConfig_arm_1.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    AnalogWDGConfig_arm_2.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    AnalogWDGConfig_arm_3.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    AnalogWDGConfig_arm_4.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    AnalogWDGConfig_trig_1.WatchdogNumber = ADC_ANALOGWATCHDOG_2;
    AnalogWDGConfig_trig_2.WatchdogNumber = ADC_ANALOGWATCHDOG_2;
    AnalogWDGConfig_trig_3.WatchdogNumber = ADC_ANALOGWATCHDOG_2;
    AnalogWDGConfig_trig_4.WatchdogNumber = ADC_ANALOGWATCHDOG_2;

    AnalogWDGConfig_arm_1.HighThreshold = 4095;
    AnalogWDGConfig_arm_1.LowThreshold = 0;
    AnalogWDGConfig_arm_2.HighThreshold = 4095;
    AnalogWDGConfig_arm_2.LowThreshold = 0;
    AnalogWDGConfig_arm_3.HighThreshold = 4095;
    AnalogWDGConfig_arm_3.LowThreshold = 0;
    AnalogWDGConfig_arm_4.HighThreshold = 4095;
    AnalogWDGConfig_arm_4.LowThreshold = 0;

    AnalogWDGConfig_trig_1.HighThreshold = 4095;
    AnalogWDGConfig_trig_1.LowThreshold = 0;
    AnalogWDGConfig_trig_2.HighThreshold = 4095;
    AnalogWDGConfig_trig_2.LowThreshold = 0;
    AnalogWDGConfig_trig_3.HighThreshold = 4095;
    AnalogWDGConfig_trig_3.LowThreshold = 0;
    AnalogWDGConfig_trig_4.HighThreshold = 4095;
    AnalogWDGConfig_trig_4.LowThreshold = 0;

    // Configure the required for arm the oscilloscope.
    if( mode == SCOPE_MODE_NORMAL )
    {
        if( slope == SCOPE_SLOPE_RISING )
        {
            if( channel == 0 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_1.HighThreshold = 4095;
                AnalogWDGConfig_arm_1.LowThreshold = level;
                AnalogWDGConfig_trig_1.HighThreshold = level;
                AnalogWDGConfig_trig_1.LowThreshold = 0;
            }
            else if( channel == 1 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_2.HighThreshold = 4095;
                AnalogWDGConfig_arm_2.LowThreshold = level;
                AnalogWDGConfig_trig_2.HighThreshold = level;
                AnalogWDGConfig_trig_2.LowThreshold = 0;
            }
            else if( channel == 2 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_3.HighThreshold = 4095;
                AnalogWDGConfig_arm_3.LowThreshold = level;
                AnalogWDGConfig_trig_3.HighThreshold = level;
                AnalogWDGConfig_trig_3.LowThreshold = 0;
            }
            else if( channel == 3 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_4.HighThreshold = 4095;
                AnalogWDGConfig_arm_4.LowThreshold = level;
                AnalogWDGConfig_trig_4.HighThreshold = level;
                AnalogWDGConfig_trig_4.LowThreshold = 0;
            }
        }
        else
        {
            if( channel == 0 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_1.HighThreshold = level;
                AnalogWDGConfig_arm_1.LowThreshold = 0;
                AnalogWDGConfig_trig_1.HighThreshold = 4095;
                AnalogWDGConfig_trig_1.LowThreshold = level;
            }
            else if( channel == 1 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_2.HighThreshold = level;
                AnalogWDGConfig_arm_2.LowThreshold = 0;
                AnalogWDGConfig_trig_2.HighThreshold = 4095;
                AnalogWDGConfig_trig_2.LowThreshold = level;
            }
            else if( channel == 2 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_3.HighThreshold = level;
                AnalogWDGConfig_arm_3.LowThreshold = 0;
                AnalogWDGConfig_trig_3.HighThreshold = 4095;
                AnalogWDGConfig_trig_3.LowThreshold = level;
            }
            else if( channel == 3 )
            {
                // Configure arm and trigger high and low threshold values.
                AnalogWDGConfig_arm_4.HighThreshold = level;
                AnalogWDGConfig_arm_4.LowThreshold = 0;
                AnalogWDGConfig_trig_4.HighThreshold = 4095;
                AnalogWDGConfig_trig_4.LowThreshold = level;
            }
        }
    }

    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc1, &AnalogWDGConfig_arm_1 );
    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc2, &AnalogWDGConfig_arm_2 );
    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc3, &AnalogWDGConfig_arm_3 );
    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc4, &AnalogWDGConfig_arm_4 );
    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc1, &AnalogWDGConfig_trig_1 );
    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc2, &AnalogWDGConfig_trig_2 );
    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc3, &AnalogWDGConfig_trig_3 );
    HAL_ADC_AnalogWDGConfig( scope->trigger.hadc4, &AnalogWDGConfig_trig_4 );

    // disable all ADC irqs. Leave only the selected channel enabled.
    __HAL_ADC_DISABLE_IT( scope->trigger.hadc1, ADC_IT_EOC );
