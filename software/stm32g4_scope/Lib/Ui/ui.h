/*
 * ui.h
 *
 *  Created on: Aug 1, 2023
 *      Author: jgpei
 */

#ifndef UI_H_
#define UI_H_

#include <stdint.h>
#include "nuklear.h"

#include "FreeRTOS.h"
#include "task.h"
//#include "main.h"
#include "cmsis_os.h"

#include "scope_tasks.h"

#define COLOR_BUTTON_ENABLED (struct nk_color){40,200,40, 255}
#define COLOR_BUTTON_DISABLED (struct nk_color){200,40,40, 255}

#define COLOR_CHANNEL1 (struct nk_color){255,0,0, 255}
#define COLOR_CHANNEL2 (struct nk_color){0,255,0, 255}
#define COLOR_CHANNEL3 (struct nk_color){0,0,255, 255}
#define COLOR_CHANNEL4 (struct nk_color){255,0,255, 255}

#define COLOR_WAVEGEN_CHANNEL1 (struct nk_color){0,255,255, 255}
#define COLOR_WAVEGEN_CHANNEL2 (struct nk_color){255,0,255, 255}

#define UI_CHANNEL_COUNT    (4)
#define UI_WAVEFORM_COUNT   (2)
#define UI_CURSOR_COUNT     (4)

enum UI_TRIGGER_MODE {
    UI_TRIGGER_MODE_AUTO,
    UI_TRIGGER_MODE_NORMAL
};

enum UI_TRIGGER_SLOPE {
    UI_TRIGGER_SLOPE_RISING,
    UI_TRIGGER_SLOPE_FALLING,
};

enum UI_VERTICAL_CHANNEL_COUPLING {
    UI_CHANNEL_COUPLING_AC,
    UI_CHANNEL_COUPLING_DC,
    UI_CHANNEL_COUPLING_GND
};

struct sUi_Acquire
{
    uint8_t run;
    uint8_t single;
    uint8_t is_visible;
    uint8_t is_visible_bck;
};
typedef struct sUi_Acquire tUi_Acquire;

struct sUi_Horizontal
{
    int32_t offset;
    int32_t scale;
    uint8_t is_visible;
    uint8_t is_visible_bck;
};
typedef struct sUi_Horizontal tUi_Horizontal;

struct sUi_Vertical_Channel
{
    uint8_t enabled;
    enum UI_VERTICAL_CHANNEL_COUPLING coupling;
    int32_t offset;
    int32_t gain;
    int32_t scale;
};
typedef struct sUi_Vertical_Channel tUi_Vertical_Channel;

struct sUi_Vertical
{
    int32_t offset;
    tUi_Vertical_Channel channels[UI_CHANNEL_COUNT];
    uint8_t channel_selected;
    uint8_t is_visible;
    uint8_t is_visible_bck;
};
typedef struct sUi_Vertical tUi_Vertical;

struct sUi_Trigger
{
    enum UI_TRIGGER_MODE mode;
    enum UI_TRIGGER_SLOPE slope;
    int32_t level;
    int32_t source;
    //uint8_t armed;
    uint8_t is_visible;
    uint8_t is_visible_bck;
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
    uint8_t is_visible;
    uint8_t is_visible_bck;
};
typedef struct sUi_Waveform tUi_Waveform;

struct sUi_Wavegen
{
    tUi_Waveform waveforms[UI_WAVEFORM_COUNT];
    uint8_t waveform_selected;
    uint8_t is_visible;
    uint8_t is_visible_bck;
};
typedef struct sUi_Wavegen tUi_Wavegen;

struct sUi_Cursor
{
    uint8_t enabled;
    //uint8_t horizontal;
    int32_t offset;
    //int32_t track;
    //struct nk_color color;
};
typedef struct sUi_Cursor tUi_Cursor;

struct sUi_Cursors
{
    tUi_Cursor cursors[UI_CURSOR_COUNT];
    uint8_t cursor_selected;
    uint8_t is_visible;
    uint8_t is_visible_bck;
};
typedef struct sUi_Cursors tUi_Cursors;

struct sUi_Measurements
{
    int32_t frequency;
    int32_t period;
    int32_t duty_cycle;
    int32_t vpp;
    int32_t vrms;
    int32_t vdc;
    int32_t vavg;
    int32_t vpp_to_vdc;
    int32_t vpp_to_vrms;
    int32_t vpp_to_vavg;
    int32_t vrms_to_vdc;
    int32_t vrms_to_vavg;
    int32_t vdc_to_vavg;
    uint8_t is_visible;
    uint8_t is_visible_bck;
};
typedef struct sUi_Measurements tUi_Measurements;

struct sUi_Info
{
    int32_t fps;
    int32_t cpu;
    int32_t mem;
    uint8_t is_visible;
    uint8_t is_visible_bck;
};
typedef struct sUi_Info tUi_Info;

struct sUi
{
    tUi_Acquire acquire;
    tUi_Horizontal horizontal;
    tUi_Vertical vertical;
    tUi_Trigger trigger;
    tUi_Wavegen wavegen;
    tUi_Cursors cursors;
    tUi_Measurements measurements;
    tUi_Info info;

    //int is_collapsed;
    uint8_t is_visible;
    uint8_t is_visible_bck;
};
typedef struct sUi tUi;

uint8_t nk_property_keypad( struct nk_context *pCtx, uint8_t *pLabel, int32_t min, int32_t *pValue, int32_t max, uint8_t *pShow_keypad );


void ui_init( tUi *pThis );
void ui_build( tUi *pThis, struct nk_context *pCtx );

void ui_build_acquire( tUi_Acquire *pThis, struct nk_context *pCtx );
void ui_build_horizontal( tUi_Horizontal *pThis, struct nk_context *pCtx );
void ui_build_vertical( tUi_Vertical *pThis, struct nk_context *pCtx );
void ui_build_trigger( tUi_Trigger *pThis, struct nk_context *pCtx );
void ui_build_wavegen( tUi_Wavegen *pThis, struct nk_context *pCtx );
void ui_build_cursor( tUi *pThis, struct nk_context *pCtx );
void ui_build_measurements( tUi *pThis, struct nk_context *pCtx );

void ui_build_info( tUi *pThis, struct nk_context *pCtx );
void ui_build_color_picker( tUi *pThis, struct nk_context *pCtx );
void ui_build_theme_chooser(struct nk_context *ctx);

#include "lcd.h"
void ui_draw_cursors( tUi_Cursors *pThis, tLcd *pLcd, int is_collapsed);
void ui_erase_cursors( tUi_Cursors *pThis, tLcd *pLcd, int is_collapsed);
void ui_stroque_cursors( tUi_Cursors *pThis, tLcd *pLcd,
		uint16_t color1, uint16_t color2, uint16_t color3, uint16_t color4,
		int is_collapsed );

#endif /* UI_H_ */
