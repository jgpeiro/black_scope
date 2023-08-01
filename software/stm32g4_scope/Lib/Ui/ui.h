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

struct sUi_Vertical
{
    int32_t offset;
    int32_t scale;
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

struct sUi_Channel
{
    uint8_t enabled;
    enum UI_CHANNEL_COUPLING coupling;
    int32_t offset;
    int32_t scale;
    struct nk_color color;
};
typedef struct sUi_Channel tUi_Channel;

struct sUi_Waveform
{
    uint8_t enabled;
    int32_t type;
    int32_t offset;
    int32_t scale;
    int32_t duty_cycle;
};
typedef struct sUi_Waveform tUi_Waveform;

struct sUi_Cursor
{
    uint8_t enabled;
    uint8_t horizontal;
    int32_t offset;
    int32_t track;
    struct nk_color color;
};
typedef struct sUi_Cursor tUi_Cursor;

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
};
typedef struct sUi_Measurements tUi_Measurements;

struct sUi_Info
{
    int32_t fps;
    int32_t cpu;
    int32_t mem;
};
typedef struct sUi_Info tUi_Info;

struct sUi
{
    tUi_Acquire acquire;
    tUi_Horizontal horizontal;
    tUi_Vertical vertical;
    tUi_Trigger trigger;
    tUi_Channel channels[UI_CHANNEL_COUNT];
    uint8_t channel_selected;
    tUi_Waveform waveforms[UI_WAVEGEN_COUNT];
    uint8_t waveform_selected;
    tUi_Cursor cursors[UI_CURSOR_COUNT];
    uint8_t cursor_selected;
    tUi_Measurements measurements;
    tUi_Info info;
};
typedef struct sUi tUi;

void ui_build( tUi *pThis, struct nk_context *pCtx );
void ui_build_acquire( tUi *pThis, struct nk_context *pCtx );
void ui_build_horizontal( tUi *pThis, struct nk_context *pCtx );
void ui_build_vertical( tUi *pThis, struct nk_context *pCtx );
void ui_build_trigger( tUi *pThis, struct nk_context *pCtx );
void ui_build_waveform( tUi *pThis, struct nk_context *pCtx );
void ui_build_cursor( tUi *pThis, struct nk_context *pCtx );
void ui_build_measurements( tUi *pThis, struct nk_context *pCtx );
void ui_build_info( tUi *pThis, struct nk_context *pCtx );



#endif /* UI_H_ */
