/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 4096 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
#include <stdio.h>
#include <math.h>

#include "dac.h"
#include "tim.h"
#include "spi.h"

#include "Lcd.h"
#include "Tsc.h"
#include "Scope.h"
#include "framebuf.h"
#include "FontUbuntuBookRNormal16.h"
#include "psram.h"

#include "nuklear.h"

#define BUFFER_LEN 	(1024)
#define FB_WIDTH 	(240)
#define FB_HEIGHT	(80)

uint16_t dac1_buffer[BUFFER_LEN];
uint16_t dac2_buffer[BUFFER_LEN];

uint16_t buffer1[BUFFER_LEN];
uint16_t buffer2[BUFFER_LEN];
uint16_t buffer3[BUFFER_LEN];
uint16_t buffer4[BUFFER_LEN];
uint16_t buffer5[BUFFER_LEN];
uint16_t buffer6[BUFFER_LEN];
uint16_t buffer7[BUFFER_LEN];
uint16_t buffer8[BUFFER_LEN];

uint8_t fb_buf[FB_WIDTH*FB_HEIGHT*2];

#define NK_BUFFER_CMDS_LEN 	(1024*8)
#define NK_BUFFER_POOL_LEN 	(1024*8)

uint8_t nk_buf_cmds[NK_BUFFER_CMDS_LEN] = {0};
uint8_t nk_buf_pool[NK_BUFFER_POOL_LEN] = {0};


#define CHANNEL_COUNT 4
#define WAVEFORM_COUNT 2
#define CURSOR_COUNT 2
/*
enum TRIGGER_MODE {
    TRIGGER_MODE_AUTO,
    TRIGGER_MODE_NORMAL
};*/

enum TRIGGER_SLOPE {
    TRIGGER_SLOPE_RISING,
    TRIGGER_SLOPE_FALLING,
    TRIGGER_SLOPE_BOTH
};

enum CHANNEL_COUPLING {
    CHANNEL_COUPLING_AC,
    CHANNEL_COUPLING_DC,
    CHANNEL_COUPLING_GND
};

struct Oscilloscope {
    nk_bool acquire_run;
    nk_bool acquire_single;

    float horizontal_offset;
    float horizontal_scale;

    float trigger_offset;
    int trigger_mode;
    int trigger_slope;
    int trigger_source;
    nk_bool trigger_armed;

    int channel_selected;
    struct {
        nk_bool enabled;
        int coupling;
        int offset;
        int scale;
        struct nk_color color;
    } channels[CHANNEL_COUNT];

    int waveform_selected;
    struct {
        nk_bool enabled;
        int type;
        float offset;
        float scale;
        float duty_cycle;
        struct nk_color color;
    } waveforms[WAVEFORM_COUNT];

    int cursor_selected;
    struct {
        nk_bool enabled;
        nk_bool horizontal;
        float offset;
        float track;
        struct nk_color color;
    } cursors[CURSOR_COUNT];

    nk_bool draw_bg;
    int draw_signals;
};

int visible = 0;
struct nk_rect keypad_size = {35, 35, 142, 208};
void oscilloscope_process(struct Oscilloscope *osc, struct nk_context *ctx, tScope *pScope)
{
	visible = 0;
	if( nk_begin(ctx, "STM32G4 Scope", nk_rect(0, 0, 240, 320), NK_WINDOW_MINIMIZABLE ) )
	{
		visible = 1;
        osc->draw_bg = nk_true;
        if( nk_tree_push( ctx, NK_TREE_TAB, "Acquire", NK_MAXIMIZED) ){
            osc->draw_bg = nk_false;
            nk_layout_row(ctx, NK_STATIC, 30, 4, (float[]){60, 60, 60, 60});
            nk_button_set_behavior(ctx, NK_BUTTON_DEFAULT);
            if (osc->acquire_run) {
                if (nk_button_label(ctx, "Stop")) {
                    osc->acquire_run = nk_false;
                }
            } else {
                if (nk_button_label(ctx, "Run")) {
                    osc->acquire_run = nk_true;
                }
            }
            //osc->acquire_run = nk_check_label( ctx, osc->acquire_run?"Stop":"Run", osc->acquire_run );

            if (nk_button_label(ctx, "Single")) {
                osc->acquire_single = nk_true;
            }

            if (nk_button_label(ctx, "Draw")) {
                if (osc->draw_signals + 1 < 3) {
                    osc->draw_signals += 1;
                } else {
                    osc->draw_signals = 0;
                }
            }
            nk_tree_pop(ctx);
        }

        {
        	if( nk_tree_push( ctx, NK_TREE_TAB, "Horizontal", NK_MINIMIZED) ){
        		nk_layout_row(ctx, NK_STATIC, 30, 4, (float[]){60, 30, 60, 30});
        		nk_label( ctx, "Offset", NK_TEXT_LEFT );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT) )
        		{
        			osc->horizontal_offset -= 1;
        		}
        		char combo_buffer[32];
        		sprintf(combo_buffer, "%.2f", osc->horizontal_offset);

        		static int show_app_about = 0;
        		show_app_about |= nk_button_label( ctx, combo_buffer );

                if (show_app_about)
                {
                    /* about popup */
                    //static struct nk_rect s = {20, 20, 200, 200};
                    if (nk_popup_begin(ctx, NK_POPUP_STATIC, "Keypad", NK_WINDOW_CLOSABLE | NK_WINDOW_NO_SCROLLBAR, keypad_size ) )
                    {
                    	nk_layout_row(ctx, NK_STATIC, 30, 1, (float[]){30+30+30+30});
                        nk_label(ctx, "0.00", NK_TEXT_RIGHT );
                        nk_layout_row(ctx, NK_STATIC, 30, 4, (float[]){30, 30, 30, 30});
                        nk_button_label( ctx, "7" );
                        nk_button_label( ctx, "8" );
                        nk_button_label( ctx, "9" );
                        nk_button_label( ctx, "k" );
                        nk_button_label( ctx, "4" );
                        nk_button_label( ctx, "5" );
                        nk_button_label( ctx, "6" );
                        nk_button_label( ctx, "m" );
                        nk_button_label( ctx, "1" );
                        nk_button_label( ctx, "2" );
                        nk_button_label( ctx, "3" );
                        nk_button_label( ctx, "u" );
                        nk_button_label( ctx, "C" );
                        nk_button_label( ctx, "0" );
                        nk_button_label( ctx, "." );
                        nk_button_symbol( ctx, NK_SYMBOL_X );
                        nk_popup_end(ctx);
                    } else show_app_about = nk_false;
                }

        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT) )
        		{
        			osc->horizontal_offset += 1;
        		}

        		nk_layout_row(ctx, NK_STATIC, 30, 2, (float[]){60,30+60+30});
        		nk_label( ctx, "Scale", NK_TEXT_LEFT );
        		nk_property_int(ctx, "", -100, &osc->horizontal_scale, 100, 1, 1);

        		/*nk_label( ctx, "Scale", NK_TEXT_LEFT );
				nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
				if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT) )
				{
					osc->horizontal_scale -= 1;
				}
				//char combo_buffer[32];
				sprintf(combo_buffer, "%.2f", osc->horizontal_scale);
				nk_label( ctx, combo_buffer, NK_TEXT_CENTERED );
				nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
				if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT) )
				{
					osc->horizontal_scale += 1;
				}*/
                nk_tree_pop(ctx);
            }

        	if( nk_tree_push( ctx, NK_TREE_TAB, "Vertical", NK_MINIMIZED) ){
        		nk_layout_row(ctx, NK_STATIC, 30, 1, (float[]){94});
        		//nk_style_push_style_item(&ctx, &ctx->style.combo.button.text_background, nk_style_item_color(nk_rgb(255,0,0)));
        		osc->channel_selected = nk_combo(ctx, (const char*[]){"Ch1", "Ch2", "Ch3", "Ch4"}, CHANNEL_COUNT, osc->channel_selected, 30, nk_vec2(94, 160));
        		//nk_style_pop_style_item(&ctx);
        		nk_layout_row(ctx, NK_STATIC, 30, 2, (float[]){94, 94});
                osc->channels[osc->channel_selected].enabled = nk_combo(ctx, (const char*[]){"Off", "On"}, 2, osc->channels[osc->channel_selected].enabled, 30, nk_vec2(94, 120));
                osc->channels[osc->channel_selected].coupling = nk_combo(ctx, (const char*[]){"DC", "AC", "Gnd"}, 3, osc->channels[osc->channel_selected].coupling, 30, nk_vec2(94, 120));
                //osc->channels[osc->channel_selected].offset = nk_slider_float(ctx, -10.0f, &osc->channels[osc->channel_selected].offset, 10.0f, 1.0f);
                //osc->channels[osc->channel_selected].scale = nk_slider_float(ctx, -10.0f, &osc->channels[osc->channel_selected].scale, 10.0f, 1.0f);

                nk_layout_row(ctx, NK_STATIC, 30, 4, (float[]){60, 30, 60, 30});
        		nk_label( ctx, "Offset", NK_TEXT_LEFT );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT) )
        		{
        			if( 0 <= osc->channels[0].offset - 1 )
        			{
        				osc->channels[0].offset -= 1;
        			}
        			HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, osc->channels[0].offset );
        		}
        		char combo_buffer[32];
        		sprintf(combo_buffer, "%d", osc->channels[0].offset);
        		nk_label( ctx, combo_buffer, NK_TEXT_CENTERED );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT) )
        		{
        			if( osc->channels[0].offset + 1 < 4096 )
					{
						osc->channels[0].offset += 1;
					}
        			HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, osc->channels[0].offset );
        		}

        		nk_label( ctx, "Scale", NK_TEXT_LEFT );
				nk_button_set_behavior(ctx, 0 );
				if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT) )
				{
					if( 0 <= osc->channels[osc->channel_selected].scale - 1 )
					{
						osc->channels[osc->channel_selected].scale -= 1;
					}
					if( osc->channel_selected == 0 )
					{
						//pScope->hopamp1->Instance->CSR &= ~OPAMP_CSR_PGGAIN_Msk;
						//pScope->hopamp1->Instance->CSR |= osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;
						extern OPAMP_HandleTypeDef hopamp1;
						HAL_OPAMP_Stop(&hopamp1);
						HAL_OPAMP_DeInit(&hopamp1);
						hopamp1.Instance = OPAMP1;
						hopamp1.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
						hopamp1.Init.Mode = OPAMP_PGA_MODE;
						hopamp1.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO2;
						hopamp1.Init.InternalOutput = ENABLE;
						hopamp1.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
						hopamp1.Init.PgaConnect = OPAMP_PGA_CONNECT_INVERTINGINPUT_IO0_BIAS;
						hopamp1.Init.PgaGain = osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;//OPAMP_PGA_GAIN_2_OR_MINUS_1;
						hopamp1.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
						HAL_OPAMP_Init(&hopamp1);
						HAL_OPAMP_SelfCalibrate(&hopamp1);
						HAL_OPAMP_Start(&hopamp1);
					}
					else if( osc->channel_selected == 1 )
					{
						//pScope->hopamp2->Instance->CSR &= ~OPAMP_CSR_PGGAIN_Msk;
						//pScope->hopamp2->Instance->CSR |= osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;
						extern OPAMP_HandleTypeDef hopamp3;
						HAL_OPAMP_Stop(&hopamp3);
						HAL_OPAMP_DeInit(&hopamp3);
						hopamp3.Instance = OPAMP3;
						hopamp3.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
						hopamp3.Init.Mode = OPAMP_PGA_MODE;
						hopamp3.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO2;
						hopamp3.Init.InternalOutput = ENABLE;
						hopamp3.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
						hopamp3.Init.PgaConnect = OPAMP_PGA_CONNECT_INVERTINGINPUT_IO0_BIAS;
						hopamp3.Init.PgaGain = osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;//OPAMP_PGA_GAIN_2_OR_MINUS_1;
						hopamp3.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
						HAL_OPAMP_Init(&hopamp3);
						HAL_OPAMP_SelfCalibrate(&hopamp3);
						HAL_OPAMP_Start(&hopamp3);
					}
					else if( osc->channel_selected == 2 )
					{
						//pScope->hopamp3->Instance->CSR &= ~OPAMP_CSR_PGGAIN_Msk;
						//pScope->hopamp3->Instance->CSR |= osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;
						extern OPAMP_HandleTypeDef hopamp5;
						HAL_OPAMP_Stop(&hopamp5);
						HAL_OPAMP_DeInit(&hopamp5);
						hopamp5.Instance = OPAMP5;
						hopamp5.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
						hopamp5.Init.Mode = OPAMP_PGA_MODE;
						hopamp5.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO2;
						hopamp5.Init.InternalOutput = ENABLE;
						hopamp5.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
						hopamp5.Init.PgaConnect = OPAMP_PGA_CONNECT_INVERTINGINPUT_IO0_BIAS;
						hopamp5.Init.PgaGain = osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;//OPAMP_PGA_GAIN_2_OR_MINUS_1;
						hopamp5.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
						HAL_OPAMP_Init(&hopamp5);
						HAL_OPAMP_SelfCalibrate(&hopamp5);
						HAL_OPAMP_Start(&hopamp5);
					}
					else if( osc->channel_selected == 3 )
					{
						//pScope->hopamp4->Instance->CSR &= ~OPAMP_CSR_PGGAIN_Msk;
						//pScope->hopamp4->Instance->CSR |= osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;
						extern OPAMP_HandleTypeDef hopamp6;
						HAL_OPAMP_Stop(&hopamp6);
						HAL_OPAMP_DeInit(&hopamp6);
						hopamp6.Instance = OPAMP6;
						hopamp6.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
						hopamp6.Init.Mode = OPAMP_PGA_MODE;
						hopamp6.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO2;
						hopamp6.Init.InternalOutput = ENABLE;
						hopamp6.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
						hopamp6.Init.PgaConnect = OPAMP_PGA_CONNECT_INVERTINGINPUT_IO0_BIAS;
						hopamp6.Init.PgaGain = osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;//OPAMP_PGA_GAIN_2_OR_MINUS_1;
						hopamp6.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
						HAL_OPAMP_Init(&hopamp6);
						HAL_OPAMP_SelfCalibrate(&hopamp6);
						HAL_OPAMP_Start(&hopamp6);
					}

				}
				//char combo_buffer[32];
				sprintf(combo_buffer, "%d", osc->channels[osc->channel_selected].scale);
				nk_label( ctx, combo_buffer, NK_TEXT_CENTERED );
				nk_button_set_behavior(ctx, 0);
				if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT) )
				{
					if( osc->channels[osc->channel_selected].scale + 1 <= 8 )
					{
						osc->channels[osc->channel_selected].scale += 1;
					}
					if( osc->channel_selected == 0 )
					{
						//pScope->hopamp1->Instance->CSR &= ~OPAMP_CSR_PGGAIN_Msk;
						//pScope->hopamp1->Instance->CSR |= osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;
						extern OPAMP_HandleTypeDef hopamp1;
						HAL_OPAMP_Stop(&hopamp1);
						HAL_OPAMP_DeInit(&hopamp1);
						hopamp1.Instance = OPAMP1;
						hopamp1.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
						hopamp1.Init.Mode = OPAMP_PGA_MODE;
						hopamp1.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO2;
						hopamp1.Init.InternalOutput = ENABLE;
						hopamp1.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
						hopamp1.Init.PgaConnect = OPAMP_PGA_CONNECT_INVERTINGINPUT_IO0_BIAS;
						hopamp1.Init.PgaGain = osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;//OPAMP_PGA_GAIN_2_OR_MINUS_1;
						hopamp1.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
						HAL_OPAMP_Init(&hopamp1);
						HAL_OPAMP_SelfCalibrate(&hopamp1);
						HAL_OPAMP_Start(&hopamp1);
					}
					else if( osc->channel_selected == 1 )
					{
						//pScope->hopamp2->Instance->CSR &= ~OPAMP_CSR_PGGAIN_Msk;
						//pScope->hopamp2->Instance->CSR |= osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;
						extern OPAMP_HandleTypeDef hopamp3;
						HAL_OPAMP_Stop(&hopamp3);
						HAL_OPAMP_DeInit(&hopamp3);
						hopamp3.Instance = OPAMP3;
						hopamp3.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
						hopamp3.Init.Mode = OPAMP_PGA_MODE;
						hopamp3.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO2;
						hopamp3.Init.InternalOutput = ENABLE;
						hopamp3.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
						hopamp3.Init.PgaConnect = OPAMP_PGA_CONNECT_INVERTINGINPUT_IO0_BIAS;
						hopamp3.Init.PgaGain = osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;//OPAMP_PGA_GAIN_2_OR_MINUS_1;
						hopamp3.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
						HAL_OPAMP_Init(&hopamp3);
						HAL_OPAMP_SelfCalibrate(&hopamp3);
						HAL_OPAMP_Start(&hopamp3);
					}
					else if( osc->channel_selected == 2 )
					{
						//pScope->hopamp3->Instance->CSR &= ~OPAMP_CSR_PGGAIN_Msk;
						//pScope->hopamp3->Instance->CSR |= osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;
						extern OPAMP_HandleTypeDef hopamp5;
						HAL_OPAMP_Stop(&hopamp5);
						HAL_OPAMP_DeInit(&hopamp5);
						hopamp5.Instance = OPAMP5;
						hopamp5.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
						hopamp5.Init.Mode = OPAMP_PGA_MODE;
						hopamp5.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO2;
						hopamp5.Init.InternalOutput = ENABLE;
						hopamp5.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
						hopamp5.Init.PgaConnect = OPAMP_PGA_CONNECT_INVERTINGINPUT_IO0_BIAS;
						hopamp5.Init.PgaGain = osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;//OPAMP_PGA_GAIN_2_OR_MINUS_1;
						hopamp5.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
						HAL_OPAMP_Init(&hopamp5);
						HAL_OPAMP_SelfCalibrate(&hopamp5);
						HAL_OPAMP_Start(&hopamp5);
					}
					else if( osc->channel_selected == 3 )
					{
						//pScope->hopamp4->Instance->CSR &= ~OPAMP_CSR_PGGAIN_Msk;
						//pScope->hopamp4->Instance->CSR |= osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;
						extern OPAMP_HandleTypeDef hopamp6;
						HAL_OPAMP_Stop(&hopamp6);
						HAL_OPAMP_DeInit(&hopamp6);
						hopamp6.Instance = OPAMP6;
						hopamp6.Init.PowerMode = OPAMP_POWERMODE_NORMALSPEED;
						hopamp6.Init.Mode = OPAMP_PGA_MODE;
						hopamp6.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO2;
						hopamp6.Init.InternalOutput = ENABLE;
						hopamp6.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
						hopamp6.Init.PgaConnect = OPAMP_PGA_CONNECT_INVERTINGINPUT_IO0_BIAS;
						hopamp6.Init.PgaGain = osc->channels[osc->channel_selected].scale << OPAMP_CSR_PGGAIN_Pos;//OPAMP_PGA_GAIN_2_OR_MINUS_1;
						hopamp6.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;
						HAL_OPAMP_Init(&hopamp6);
						HAL_OPAMP_SelfCalibrate(&hopamp6);
						HAL_OPAMP_Start(&hopamp6);
					}
				}
                nk_tree_pop(ctx);
            }

            if( nk_tree_push( ctx, NK_TREE_TAB, "Trigger", NK_MINIMIZED) ){
            	nk_layout_row(ctx, NK_STATIC, 30, 1, (float[]){94});
                osc->trigger_source = nk_combo(ctx, (const char*[]){"Ch1", "Ch2", "Ch3", "Ch4"}, CHANNEL_COUNT, osc->trigger_source, 30, nk_vec2(94, 160));
                nk_layout_row(ctx, NK_STATIC, 30, 2, (float[]){94, 94});
                osc->trigger_mode = nk_combo(ctx, (const char*[]){"Normal", "Auto"}, 2, osc->trigger_mode, 30, nk_vec2(94, 120));
                osc->trigger_slope = nk_combo(ctx, (const char*[]){"Rising", "Falling", "Both"}, 3, osc->trigger_slope, 30, nk_vec2(94, 120));
                //nk_layout_row(ctx, NK_STATIC, 30, 1, (float[]){200});
                //nk_slider_float(ctx, -10.0f, &osc->trigger_offset, 10.0f, 1.0f);


                nk_layout_row(ctx, NK_STATIC, 30, 4, (float[]){60, 30, 60, 30});
        		nk_label( ctx, "Offset", NK_TEXT_LEFT );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT) )
        		{
        			osc->trigger_offset -= 1;
        		}
        		char combo_buffer[32];
        		sprintf(combo_buffer, "%.2f", osc->trigger_offset);
        		nk_label( ctx, combo_buffer, NK_TEXT_CENTERED );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT) )
        		{
        			osc->trigger_offset += 1;
        		}


                nk_tree_pop(ctx);
            }

            if( nk_tree_push( ctx, NK_TREE_TAB, "Waveform", NK_MINIMIZED) ){
            	nk_layout_row(ctx, NK_STATIC, 30, 1, (float[]){94});
                osc->waveform_selected = nk_combo(ctx, (const char*[]){"Wf1", "Wf2"}, WAVEFORM_COUNT, osc->waveform_selected, 30, nk_vec2(94, 80));
                nk_layout_row(ctx, NK_STATIC, 30, 2, (float[]){94, 94});
                osc->waveforms[osc->waveform_selected].enabled = nk_combo(ctx, (const char*[]){"Off", "On"}, 2, osc->waveforms[osc->waveform_selected].enabled, 30, nk_vec2(94, 200));
                osc->waveforms[osc->waveform_selected].type = nk_combo(ctx, (const char*[]){"DC", "PWM", "Sine", "Tria", "Saw", "Noise"}, 6, osc->waveforms[osc->waveform_selected].type, 30, nk_vec2(94, 240));
                //osc->waveforms[osc->waveform_selected].offset = nk_slider_float(ctx, -10.0f, &osc->waveforms[osc->waveform_selected].offset, 10.0f, 1.0f);
                //osc->waveforms[osc->waveform_selected].scale = nk_slider_float(ctx, -10.0f, &osc->waveforms[osc->waveform_selected].scale, 10.0f, 1.0f);
                //osc->waveforms[osc->waveform_selected].duty_cycle = nk_slider_float(ctx, -10.0f, &osc->waveforms[osc->waveform_selected].duty_cycle, 10.0f, 1.0f);

                nk_layout_row(ctx, NK_STATIC, 30, 4, (float[]){60, 30, 60, 30});
        		nk_label( ctx, "Offset", NK_TEXT_LEFT );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT) )
        		{
        			osc->waveforms[osc->waveform_selected].offset -= 1;
        		}
        		char combo_buffer[32];
        		sprintf(combo_buffer, "%.2f", osc->waveforms[osc->waveform_selected].offset);
        		nk_label( ctx, combo_buffer, NK_TEXT_CENTERED );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT) )
        		{
        			osc->waveforms[osc->waveform_selected].offset += 1;
        		}

        		nk_label( ctx, "Scale", NK_TEXT_LEFT );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT) )
        		{
        			osc->waveforms[osc->waveform_selected].scale -= 1;
        		}
        		//char combo_buffer[32];
        		sprintf(combo_buffer, "%.2f", osc->waveforms[osc->waveform_selected].scale);
        		nk_label( ctx, combo_buffer, NK_TEXT_CENTERED );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT) )
        		{
        			osc->waveforms[osc->waveform_selected].scale += 1;
        		}

        		nk_label( ctx, "Duty", NK_TEXT_LEFT );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT) )
        		{
        			osc->waveforms[osc->waveform_selected].duty_cycle -= 1;
        		}
        		//char combo_buffer[32];
        		sprintf(combo_buffer, "%.2f", osc->waveforms[osc->waveform_selected].duty_cycle);
        		nk_label( ctx, combo_buffer, NK_TEXT_CENTERED );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT) )
        		{
        			osc->waveforms[osc->waveform_selected].duty_cycle += 1;
        		}
                nk_tree_pop(ctx);
            }

			if( nk_tree_push( ctx, NK_TREE_TAB, "Cursor", NK_MINIMIZED) ){
				nk_layout_row(ctx, NK_STATIC, 30, 1, (float[]){94});
                osc->cursor_selected = nk_combo(ctx, (const char*[]){"C1", "C2"}, CURSOR_COUNT, osc->cursor_selected, 20, nk_vec2(94, 100));
                nk_layout_row(ctx, NK_STATIC, 30, 2, (float[]){94, 94});
                osc->cursors[osc->cursor_selected].enabled = nk_combo(ctx, (const char*[]){"Off", "On"}, 2, osc->cursors[osc->cursor_selected].enabled, 30, nk_vec2(94, 100));
                osc->cursors[osc->cursor_selected].horizontal = nk_combo(ctx, (const char*[]){"Track", "Free"}, 2, osc->cursors[osc->cursor_selected].horizontal, 30, nk_vec2(94, 100));


                //osc->cursors[osc->cursor_selected].offset = nk_slider_float(ctx, -10.0f, &osc->cursors[osc->cursor_selected].offset, 10.0f, 1.0f);
                //osc->cursors[osc->cursor_selected].track = nk_slider_float(ctx, -10.0f, &osc->cursors[osc->cursor_selected].track, 10.0f, 1.0f);
                nk_layout_row(ctx, NK_STATIC, 30, 4, (float[]){60, 30, 60, 30});
        		nk_label( ctx, "Offset", NK_TEXT_LEFT );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT) )
        		{
        			osc->cursors[osc->cursor_selected].offset -= 1;
        		}
        		char combo_buffer[32];
        		sprintf(combo_buffer, "%.2f", osc->cursors[osc->cursor_selected].offset);
        		nk_label( ctx, combo_buffer, NK_TEXT_CENTERED );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT) )
        		{
        			osc->cursors[osc->cursor_selected].offset += 1;
        		}


        		nk_label( ctx, "Track", NK_TEXT_LEFT );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_LEFT) )
        		{
        			osc->cursors[osc->cursor_selected].track -= 1;
        		}
        		//char combo_buffer[32];
        		sprintf(combo_buffer, "%.2f", osc->cursors[osc->cursor_selected].track);
        		nk_label( ctx, combo_buffer, NK_TEXT_CENTERED );
        		nk_button_set_behavior(ctx, NK_BUTTON_REPEATER);
        		if( nk_button_symbol(ctx, NK_SYMBOL_TRIANGLE_RIGHT) )
        		{
        			osc->cursors[osc->cursor_selected].track += 1;
        		}


                nk_tree_pop(ctx);
            }

			if( nk_tree_push( ctx, NK_TREE_TAB, "Measurements", NK_MINIMIZED) ){
                nk_label(ctx, "Todo", NK_TEXT_LEFT);
                nk_tree_pop(ctx);
            }

			if( nk_tree_push( ctx, NK_TREE_TAB, "Info", NK_MINIMIZED) ){
                nk_layout_row(ctx, NK_STATIC, 30, 2, (float[]){60, 120});

                float fps = 0.0f; // Calculate fps here
                nk_label(ctx, "FPS", NK_TEXT_LEFT);

                static int a = 0;
                static int b = 0;
                b = a;
                a = HAL_GetTick();
                int d = a-b;
                char buffer[32];
                sprintf(buffer, "%f", 1000.0f/d);

                nk_label(ctx, buffer, NK_TEXT_LEFT); // Replace with actual fps

                float ratio = 0.0f; // Calculate memory ratio here
                nk_label(ctx, "Memory1", NK_TEXT_LEFT);

                sprintf(buffer, "%d%%", (100*ctx->memory.allocated)/ctx->memory.size );
                nk_label(ctx, buffer, NK_TEXT_LEFT); // Replace with actual memory ratio

                //float ratio = 0.0f; // Calculate memory ratio here
                nk_label(ctx, "Memory2", NK_TEXT_LEFT);

                sprintf(buffer, "%d%%", (100*ctx->pool.cap)/ctx->pool.size);
                nk_label(ctx, buffer, NK_TEXT_LEFT); // Replace with actual memory ratio

                nk_tree_pop(ctx);
            }
        }
        //nk_end(ctx);
    }
	nk_end(ctx);
}

void lcd_draw_cross( tLcd *pLcd, uint16_t x, uint16_t y, uint16_t color )
{
	lcd_rect( pLcd, x-4, y, 4, 2, color );
	lcd_rect( pLcd, x+2, y, 4, 2, color );
	lcd_rect( pLcd, x, y-4, 2, 4, color );
	lcd_rect( pLcd, x, y+2, 2, 4, color );
}

float text_width_f( nk_handle handle, float h, const char* t, int len )
{
	return font_text_width( &fontUbuntuBookRNormal16, t );
}

extern void nk_draw_fb(struct nk_context *ctx, tFramebuf *pfb, tLcd *pLcd);
extern void nk_set_theme(struct nk_context *ctx, int theme);

void draw_buffers(
		tLcd *pLcd,
		int32_t trigger,
		int32_t trigger_bck,
		uint16_t *buffer1,
		uint16_t *buffer2,
		uint16_t *buffer3,
		uint16_t *buffer4,
		uint16_t *buffer5,
		uint16_t *buffer6,
		uint16_t *buffer7,
		uint16_t *buffer8,
		uint16_t len,
		uint32_t collapsed,
		uint8_t a_b )
{
	int j, j2, n, n2;
	int x0, ya, yb, yc, yd;

	uint16_t width = pLcd->width;
	uint16_t height = pLcd->height;

	for( j = 0; j < width; j++ )
	{
		j2 = (j*len)/width;
		n = trigger + j2;
		if( n < 0 )
		{
			n += len;
		}
		else if( n >= len )
		{
			n -= len;
		}

		n2 = trigger_bck + j2;
		if( n2 < 0 )
		{
			n2 += len;
		}
		else if( n2 >= len )
		{
			n2 -= len;
		}

		x0 = collapsed? j : width/2 + j/2;

		ya = height-((a_b?buffer5[n2]:buffer1[n2])*height)/4096;
		yb = height-((a_b?buffer6[n2]:buffer2[n2])*height)/4096;
		yc = height-((a_b?buffer7[n2]:buffer3[n2])*height)/4096;
		yd = height-((a_b?buffer8[n2]:buffer4[n2])*height)/4096;
		lcd_set_pixel( pLcd, x0, ya, 0x0000 );
		lcd_set_pixel( pLcd, x0, yb, 0x0000 );
		lcd_set_pixel( pLcd, x0, yc, 0x0000 );
		lcd_set_pixel( pLcd, x0, yd, 0x0000 );

		ya = height-((a_b?buffer1[n]:buffer5[n])*height)/4096;
		yb = height-((a_b?buffer2[n]:buffer6[n])*height)/4096;
		yc = height-((a_b?buffer3[n]:buffer7[n])*height)/4096;
		yd = height-((a_b?buffer4[n]:buffer8[n])*height)/4096;
		lcd_set_pixel( pLcd, x0, ya, 0x001F );
		lcd_set_pixel( pLcd, x0, yb, 0x07E0 );
		lcd_set_pixel( pLcd, x0, yc, 0xF800 );
		lcd_set_pixel( pLcd, x0, yd, 0xF81F );
	}
}
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	tLcd lcd = {0};
	tTsc tsc = {0};
	tScope scope = {0};
	tFramebuf fb = {0};

	struct nk_context ctx = {0};
	struct nk_buffer cmds = {0};
	struct nk_buffer pool = {0};
	struct nk_user_font font = {0};

	struct Oscilloscope osc = {0};

	float AX = 38.0/151.0;
	float BX = -1950.0/151.0;
	float AY = 11.0/62.0;
	float BY = -1157.0/62.0;

	int i;
	float dac_freq = 3;
	int32_t trigger = 0;
	int trigger_bck = 0;
	uint16_t x = 0, y = 0;
	uint16_t x_bck = 0, y_bck = 0;
	int pressed = 1;
	int pressed_bck = 0;
	int nk_theme =1;
	int collapsed_bck = 0;
	int collapsed = 0;

	//psram_test();

	lcd_init( &lcd, LCD_nRST_GPIO_Port, LCD_nRST_Pin, LCD_BL_GPIO_Port, LCD_BL_Pin, 480, 320 );
	tsc_init( &tsc, &hspi3, TSC_nSS_GPIO_Port, TSC_nSS_Pin, AX, BX, AY, BY, 1 );

	framebuf_init( &fb, FB_WIDTH, FB_HEIGHT, fb_buf );

	nk_buffer_init_fixed( &cmds, nk_buf_cmds, NK_BUFFER_CMDS_LEN );
	nk_buffer_init_fixed( &pool, nk_buf_pool, NK_BUFFER_POOL_LEN);

	font.height = fontUbuntuBookRNormal16.bbxh;
	font.width = text_width_f;
	nk_init_custom( &ctx, &cmds, &pool, &font );
	nk_set_theme( &ctx, THEME_DARK );

	for( i = 0 ; i < BUFFER_LEN ; i++ )
	{
		dac1_buffer[i] = sinf(2*dac_freq*2*M_PI*i/BUFFER_LEN)*1023 + 2048;
		dac2_buffer[i] = sinf(dac_freq*2*M_PI*i/BUFFER_LEN)*1023 + 2048;
	}

	__HAL_DBGMCU_FREEZE_TIM4();
	HAL_DAC_Start_DMA( &hdac1, DAC_CHANNEL_1, (uint32_t*)dac1_buffer, BUFFER_LEN, DAC_ALIGN_12B_R );
	HAL_DAC_Start_DMA( &hdac1, DAC_CHANNEL_2, (uint32_t*)dac2_buffer, BUFFER_LEN, DAC_ALIGN_12B_R );
	HAL_TIM_Base_Start( &htim4 );

	osc.channels[0].offset = 2048;
	osc.channels[1].offset = 2048;
	osc.channels[2].offset = 2048;
	osc.channels[3].offset = 2048;
	HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, osc.channels[0].offset);
	HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);

	/* Infinite loop */
	for(;;)
	{
		x_bck = x;
		y_bck = y;
		tsc_read( &tsc, &x, &y );

		pressed_bck = pressed;
		pressed = 0 < x && x <fb.width;
		nk_input_begin( &ctx );
		if( pressed )
		{
			nk_input_motion( &ctx, x, y );
			nk_input_button( &ctx, 0, x, y, 1 );
		}
		else if( pressed_bck )
		{
			nk_input_motion( &ctx, x_bck, y_bck );
			nk_input_button( &ctx, 0, x_bck, y_bck, 0 );
		}
		nk_input_end( &ctx );

	    if( pressed || pressed_bck )
	    {
		    oscilloscope_process( &osc, &ctx, &scope );

			collapsed = nk_window_is_collapsed( &ctx, "STM32G4 Scope" );
			if( collapsed != collapsed_bck )
			{
				collapsed_bck = collapsed;
				lcd_clear( &lcd, 0x0000 );
			}

			nk_draw_fb( &ctx, &fb, &lcd );
			nk_clear(&ctx);
	    }

		scope_init( &scope, 2048, 1000000,
				(i&0x01)?buffer1:buffer5,
				(i&0x01)?buffer2:buffer6,
				(i&0x01)?buffer3:buffer7,
				(i&0x01)?buffer4:buffer8,
				BUFFER_LEN );
		scope_start( &scope );
		while( scope_is_busy( &scope ) );
		scope_stop( &scope );

		trigger = scope_get_trigger( &scope ) - BUFFER_LEN/2;






		draw_buffers(
			&lcd,
			trigger,
			trigger_bck,
			buffer1,
			buffer2,
			buffer3,
			buffer4,
			buffer5,
			buffer6,
			buffer7,
			buffer8,
			BUFFER_LEN,
			collapsed,
			i&0x01
		);

		if( pressed )
		{
			lcd_draw_cross( &lcd, x, y, 0xFFFF );
		}

		i += 1;
		trigger_bck = trigger;

	}
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
