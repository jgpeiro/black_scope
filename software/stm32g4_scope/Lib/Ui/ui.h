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

#define UI_CHANNEL_COUNT    (4)
#define UI_WAVEFORM_COUNT   (2)
#define UI_CURSOR_COUNT     (2)

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
};
typedef struct sUi_Acquire tUi_Acquire;

struct sUi_Horizontal
{
    int32_t offset;
    int32_t scale;
    uint8_t is_visible;
};
typedef struct sUi_Horizontal tUi_Horizontal;

struct sUi_Vertical_Channel
{
    uint8_t enabled;
    enum UI_VERTICAL_CHANNEL_COUPLING coupling;
    int32_t scale;
};
typedef struct sUi_Vertical_Channel tUi_Vertical_Channel;

struct sUi_Vertical
{
    int32_t offset;
    tUi_Vertical_Channel channels[UI_CHANNEL_COUNT];
    uint8_t channel_selected;
    uint8_t is_visible;
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
};
typedef struct sUi_Waveform tUi_Waveform;

struct sUi_Wavegen
{
    tUi_Waveform waveforms[UI_WAVEFORM_COUNT];
    uint8_t waveform_selected;
    uint8_t is_visible;
};
typedef struct sUi_Wavegen tUi_Wavegen;

struct sUi_Cursor
{
    uint8_t enabled;
    uint8_t horizontal;
    int32_t offset;
    int32_t track;
    struct nk_color color;
};
typedef struct sUi_Cursor tUi_Cursor;

struct sUi_Cursors
{
    tUi_Cursor cursors[UI_CURSOR_COUNT];
    uint8_t cursor_selected;
    uint8_t is_visible;
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
};
typedef struct sUi_Measurements tUi_Measurements;

struct sUi_Info
{
    int32_t fps;
    int32_t cpu;
    int32_t mem;
    uint8_t is_visible;
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
};
typedef struct sUi tUi;

void ui_build( tUi *pThis, struct nk_context *pCtx );
void ui_build_acquire( tUi *pThis, struct nk_context *pCtx );
void ui_build_horizontal( tUi *pThis, struct nk_context *pCtx );
void ui_build_vertical( tUi *pThis, struct nk_context *pCtx );
void ui_build_trigger( tUi *pThis, struct nk_context *pCtx );
void ui_build_wavegen( tUi *pThis, struct nk_context *pCtx );
void ui_build_cursor( tUi *pThis, struct nk_context *pCtx );
void ui_build_measurements( tUi *pThis, struct nk_context *pCtx );
void ui_build_info( tUi *pThis, struct nk_context *pCtx );
void ui_build_color_picker( tUi *pThis, struct nk_context *pCtx );


#endif /* UI_H_ */
