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
struct sQueueTscUi
{
	uint16_t x;
	uint16_t y;
	uint8_t p;
	uint8_t padding[3]; // sizeof(tQueueTscUi) == 8
};
typedef struct sQueueTscUi tQueueTscUi;

struct sQueueUsbUi
{
	uint8_t btn;
	int8_t value;
	uint8_t padding[6]; // sizeof(tQueueUsbUi) == 8
};
typedef struct sQueueUsbUi tQueueUsbUi;

struct sQueueUiScope
{
    uint16_t type; /**< Type of the message. */
    uint16_t data[3]; /**< Message data. */
    // sizeof(tQueueUiScope) == 8
};
typedef struct sQueueUiScope tQueueUiScope;

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
  .priority = (osPriority_t) osPriorityBelowNormal,
  .stack_size = 128 * 4 * 2
};
/* Definitions for taskUi */
osThreadId_t taskUiHandle;
const osThreadAttr_t taskUi_attributes = {
  .name = "taskUi",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 1024 * 4
};
/* Definitions for taskTsc */
osThreadId_t taskTscHandle;
const osThreadAttr_t taskTsc_attributes = {
  .name = "taskTsc",
  .priority = (osPriority_t) osPriorityNormal2,
  .stack_size = 128 * 4 * 2
};
/* Definitions for taskUsb */
osThreadId_t taskUsbHandle;
const osThreadAttr_t taskUsb_attributes = {
  .name = "taskUsb",
  .priority = (osPriority_t) osPriorityNormal3,
  .stack_size = 128 * 4 * 2
};
/* Definitions for taskScope */
osThreadId_t taskScopeHandle;
const osThreadAttr_t taskScope_attributes = {
  .name = "taskScope",
  .priority = (osPriority_t) osPriorityNormal1,
  .stack_size = 128 * 4 * 2
};
/* Definitions for queueTscUi */
osMessageQueueId_t queueTscUiHandle;
const osMessageQueueAttr_t queueTscUi_attributes = {
  .name = "queueTscUi"
};
/* Definitions for queueUsbUi */
osMessageQueueId_t queueUsbUiHandle;
const osMessageQueueAttr_t queueUsbUi_attributes = {
  .name = "queueUsbUi"
};
/* Definitions for queueUiScope */
osMessageQueueId_t queueUiScopeHandle;
const osMessageQueueAttr_t queueUiScope_attributes = {
  .name = "queueUiScope"
};
/* Definitions for queueUsb2Ui */
osMessageQueueId_t queueUsb2UiHandle;
const osMessageQueueAttr_t queueUsb2Ui_attributes = {
  .name = "queueUsb2Ui"
};
/* Definitions for semLcd */
osSemaphoreId_t semLcdHandle;
const osSemaphoreAttr_t semLcd_attributes = {
  .name = "semLcd"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
extern void StartTaskUi(void *argument);
extern void StartTaskTsc(void *argument);
extern void StartTaskUsb(void *argument);
extern void StartTaskScope(void *argument);

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

  /* Create the semaphores(s) */
  /* creation of semLcd */
  semLcdHandle = osSemaphoreNew(1, 1, &semLcd_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  semLcdHandle = osSemaphoreNew(1, 0, &semLcd_attributes);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of queueTscUi */
  queueTscUiHandle = osMessageQueueNew (4, 8, &queueTscUi_attributes);

  /* creation of queueUsbUi */
  queueUsbUiHandle = osMessageQueueNew (4, 8, &queueUsbUi_attributes);

  /* creation of queueUiScope */
  queueUiScopeHandle = osMessageQueueNew (4, 8, &queueUiScope_attributes);

  /* creation of queueUsb2Ui */
  queueUsb2UiHandle = osMessageQueueNew (8, 8, &queueUsb2Ui_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  queueUsb2UiHandle = osMessageQueueNew (4, 8, &queueUsb2Ui_attributes);

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of taskUi */
  taskUiHandle = osThreadNew(StartTaskUi, NULL, &taskUi_attributes);

  /* creation of taskTsc */
  taskTscHandle = osThreadNew(StartTaskTsc, NULL, &taskTsc_attributes);

  /* creation of taskUsb */
  taskUsbHandle = osThreadNew(StartTaskUsb, NULL, &taskUsb_attributes);

  /* creation of taskScope */
  taskScopeHandle = osThreadNew(StartTaskScope, NULL, &taskScope_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */

#include "lvgl.h"
#include "lcd.h"
#include "tsc.h"
#include "spi.h"
#include "usbd_cdc_if.h"

#include "scope.h"

tLcd lcd = {0};
tScope scope = {0};

lv_indev_t * indev_encoder;

static lv_obj_t * list1;

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked: %s", lv_list_get_btn_text(list1, obj));
    }
}

void lv_example_list_1(lv_obj_t * parent, lv_group_t * group)
{
    /*Create a list*/
    list1 = lv_list_create(parent);
    lv_obj_set_size(list1, 180, 220);
    lv_obj_center(list1);

    /*Add buttons to the list*/
    lv_obj_t * btn;

    lv_list_add_text(list1, "File");
    btn = lv_list_add_btn(list1, LV_SYMBOL_FILE, "New");
    lv_group_add_obj(group, btn);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_DIRECTORY, "Open");
    lv_group_add_obj(group, btn);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_SAVE, "Save");
    lv_group_add_obj(group, btn);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_CLOSE, "Delete");
    lv_group_add_obj(group, btn);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_EDIT, "Edit");
    lv_group_add_obj(group, btn);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    lv_list_add_text(list1, "Connectivity");
    btn = lv_list_add_btn(list1, LV_SYMBOL_BLUETOOTH, "Bluetooth");
    lv_group_add_obj(group, btn);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_GPS, "Navigation");
    lv_group_add_obj(group, btn);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_USB, "USB");
    lv_group_add_obj(group, btn);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_BATTERY_FULL, "Battery");
    lv_group_add_obj(group, btn);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    lv_list_add_text(list1, "Exit");
    btn = lv_list_add_btn(list1, LV_SYMBOL_OK, "Apply");
    lv_group_add_obj(group, btn);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = lv_list_add_btn(list1, LV_SYMBOL_CLOSE, "Close");
    lv_group_add_obj(group, btn);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);


}

static lv_obj_t * meter;

static void set_value(void * indic, int32_t v)
{
    lv_meter_set_indicator_end_value(meter, indic, v);
}


/**
 * A meter with multiple arcs
 */
void lv_example_meter_2(lv_obj_t * parent, lv_group_t * group)
{
    meter = lv_meter_create(parent);
    lv_obj_center(meter);
    lv_obj_set_size(meter, 200, 200);

    /*Remove the circle from the middle*/
    lv_obj_remove_style(meter, NULL, LV_PART_INDICATOR);

    /*Add a scale first*/
    lv_meter_scale_t * scale = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale, 11, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter, scale, 1, 2, 30, lv_color_hex3(0xeee), 15);
    lv_meter_set_scale_range(meter, scale, 0, 100, 270, 90);

    /*Add a three arc indicator*/
    lv_meter_indicator_t * indic1 = lv_meter_add_arc(meter, scale, 10, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_indicator_t * indic2 = lv_meter_add_arc(meter, scale, 10, lv_palette_main(LV_PALETTE_GREEN), -10);
    lv_meter_indicator_t * indic3 = lv_meter_add_arc(meter, scale, 10, lv_palette_main(LV_PALETTE_BLUE), -20);

    /*Create an animation to set the value*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_value);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    lv_anim_set_time(&a, 2000);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_var(&a, indic1);
    lv_anim_start(&a);

    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 1000);
    lv_anim_set_var(&a, indic2);
    lv_anim_start(&a);

    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_var(&a, indic3);
    lv_anim_start(&a);
}

void lv_example_flex_1(lv_obj_t * parent, lv_group_t * group)
{
    /*Create a container with ROW flex direction*/
    lv_obj_t * cont_row = lv_obj_create(parent);
    lv_obj_set_size(cont_row, 300, 75);
    lv_obj_align(cont_row, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_flex_flow(cont_row, LV_FLEX_FLOW_ROW);

    /*Create a container with COLUMN flex direction*/
    lv_obj_t * cont_col = lv_obj_create(parent);
    lv_obj_set_size(cont_col, 200, 150);
    lv_obj_align_to(cont_col, cont_row, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);



    lv_obj_t * obj;
    lv_obj_t * label;
    uint32_t i;
    for(i = 0; i < 10; i++) {


        /*Add items to the row*/
        obj = lv_btn_create(cont_row);
        lv_obj_set_size(obj, 100, LV_PCT(100));

        label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "Item: %"LV_PRIu32, i);
        lv_obj_center(label);

        lv_group_add_obj(group, obj);
    }

    for(i = 0; i < 10; i++) {
		/*Add items to the column*/
        obj = lv_btn_create(cont_col);
        lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);

        label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "Item: %"LV_PRIu32, i);
        lv_obj_center(label);

        lv_group_add_obj(group, obj);
    }


}

void ui_Screen1_screen_init(lv_obj_t * parent, lv_group_t * group)
{
	lv_obj_t *ui_Screen1;
	lv_obj_t *ui_Panel2;
	lv_obj_t *ui_Label1;
	lv_obj_t *ui_Roller1;
	lv_obj_t *ui_Panel3;
	lv_obj_t *ui_Label2;
	lv_obj_t *ui_Roller3;
	lv_obj_t *ui_Panel1;
	lv_obj_t *ui_Label3;
	lv_obj_t *ui_Roller2;

	ui_Screen1 = lv_obj_create(parent);
	lv_obj_set_width( ui_Screen1, 220);
	lv_obj_set_height( ui_Screen1, 140);
	lv_obj_clear_flag( ui_Screen1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
	lv_obj_set_flex_flow(ui_Screen1,LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(ui_Screen1, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
	lv_obj_set_style_pad_row(ui_Screen1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_column(ui_Screen1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

	ui_Panel2 = lv_obj_create(ui_Screen1);
	lv_obj_set_width( ui_Panel2, 184);
	lv_obj_set_height( ui_Panel2, LV_SIZE_CONTENT);   /// 108
	lv_obj_set_x( ui_Panel2, -3 );
	lv_obj_set_y( ui_Panel2, 74 );
	lv_obj_set_align( ui_Panel2, LV_ALIGN_CENTER );
	lv_obj_clear_flag( ui_Panel2, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
	lv_obj_set_style_pad_left(ui_Panel2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_Panel2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_Panel2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_Panel2, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

	ui_Label1 = lv_label_create(ui_Panel2);
	lv_obj_set_height( ui_Label1, 15);
	lv_obj_set_width( ui_Label1, LV_SIZE_CONTENT);  /// 1
	lv_obj_set_align( ui_Label1, LV_ALIGN_LEFT_MID );
	lv_label_set_text(ui_Label1,"Acquire");

	ui_Roller1 = lv_roller_create(ui_Panel2);
	lv_roller_set_options( ui_Roller1, "Option 1\nOption 2\nOption 3\nOption 4\nOption 5\nOption 6", LV_ROLLER_MODE_NORMAL );
	lv_obj_set_height( ui_Roller1, 30);
	lv_obj_set_width( ui_Roller1, LV_SIZE_CONTENT);  /// 1
	lv_obj_set_align( ui_Roller1, LV_ALIGN_RIGHT_MID );

	ui_Panel3 = lv_obj_create(ui_Screen1);
	lv_obj_set_width( ui_Panel3, 184);
	lv_obj_set_height( ui_Panel3, LV_SIZE_CONTENT);   /// 108
	lv_obj_set_x( ui_Panel3, -3 );
	lv_obj_set_y( ui_Panel3, 74 );
	lv_obj_set_align( ui_Panel3, LV_ALIGN_CENTER );
	lv_obj_clear_flag( ui_Panel3, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
	lv_obj_set_style_pad_left(ui_Panel3, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_Panel3, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_Panel3, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_Panel3, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

	ui_Label2 = lv_label_create(ui_Panel3);
	lv_obj_set_height( ui_Label2, 15);
	lv_obj_set_width( ui_Label2, LV_SIZE_CONTENT);  /// 1
	lv_obj_set_align( ui_Label2, LV_ALIGN_LEFT_MID );
	lv_label_set_text(ui_Label2,"Horizontal");

	ui_Roller3 = lv_roller_create(ui_Panel3);
	lv_roller_set_options( ui_Roller3, "Option 1\nOption 2\nOption 3\nOption 4\nOption 5\nOption 6", LV_ROLLER_MODE_NORMAL );
	lv_obj_set_height( ui_Roller3, 30);
	lv_obj_set_width( ui_Roller3, LV_SIZE_CONTENT);  /// 1
	lv_obj_set_align( ui_Roller3, LV_ALIGN_RIGHT_MID );

	ui_Panel1 = lv_obj_create(ui_Screen1);
	lv_obj_set_width( ui_Panel1, 184);
	lv_obj_set_height( ui_Panel1, LV_SIZE_CONTENT);   /// 108
	lv_obj_set_x( ui_Panel1, -3 );
	lv_obj_set_y( ui_Panel1, 74 );
	lv_obj_set_align( ui_Panel1, LV_ALIGN_CENTER );
	lv_obj_clear_flag( ui_Panel1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
	lv_obj_set_style_pad_left(ui_Panel1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui_Panel1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui_Panel1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui_Panel1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);

	ui_Label3 = lv_label_create(ui_Panel1);
	lv_obj_set_height( ui_Label3, 15);
	lv_obj_set_width( ui_Label3, LV_SIZE_CONTENT);  /// 1
	lv_obj_set_align( ui_Label3, LV_ALIGN_LEFT_MID );
	lv_label_set_text(ui_Label3,"Verital");

	ui_Roller2 = lv_roller_create(ui_Panel1);
	lv_roller_set_options( ui_Roller2, "Option 1\nOption 2\nOption 3\nOption 4\nOption 5\nOption 6", LV_ROLLER_MODE_NORMAL );
	lv_obj_set_height( ui_Roller2, 30);
	lv_obj_set_width( ui_Roller2, LV_SIZE_CONTENT);  /// 1
	lv_obj_set_align( ui_Roller2, LV_ALIGN_RIGHT_MID );

	lv_group_add_obj(group, ui_Roller1);
	lv_group_add_obj(group, ui_Roller2);
	lv_group_add_obj(group, ui_Roller3);
}

static lv_obj_t * chart;
/* Source: https://github.com/ankur219/ECG-Arrhythmia-classification/blob/642230149583adfae1e4bd26c6f0e1fd8af2be0e/sample.csv*/
static lv_coord_t data1[512];
static lv_coord_t data2[512];
static lv_coord_t data3[512];
static lv_coord_t data4[512];
/*
static void slider_x_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    int32_t v = lv_slider_get_value(obj);
    lv_chart_set_zoom_x(chart, v);
}

static void slider_y_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    int32_t v = lv_slider_get_value(obj);
    lv_chart_set_zoom_y(chart, v);
}*/

/**
 * Display 1000 data points with zooming and scrolling.
 * See how the chart changes drawing mode (draw only vertical lines) when
 * the points get too crowded.
 */
#include <math.h>
void lv_example_chart_5(lv_obj_t * parent, lv_group_t * group)
{
	int i;
	for( i = 0 ; i < 512 ; i++ )
	{
		data1[i] = sin(1*(2*M_PI*i)/512)*100+400;
		data2[i] = sin(2*(2*M_PI*i)/512)*200+300;
		data3[i] = sin(3*(2*M_PI*i)/512)*300+200;
		data4[i] = sin(4*(2*M_PI*i)/512)*400+100;
	}
    /*Create a chart*/
    chart = lv_chart_create(parent);
    lv_obj_set_size(chart, 480-100, 320-100);
    lv_obj_align(chart, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, -1000, 1000);

    /*Do not display points on the data*/
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);

    lv_chart_series_t * ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser3 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser4 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_PURPLE), LV_CHART_AXIS_PRIMARY_Y);

    uint32_t pcnt = sizeof(data1) / sizeof(data1[0]);
    lv_chart_set_point_count(chart, pcnt);
    lv_chart_set_ext_y_array(chart, ser1, (lv_coord_t *)data1);
    lv_chart_set_ext_y_array(chart, ser2, (lv_coord_t *)data2);
    lv_chart_set_ext_y_array(chart, ser3, (lv_coord_t *)data3);
    lv_chart_set_ext_y_array(chart, ser4, (lv_coord_t *)data4);

    /*lv_obj_t * slider;
    slider = lv_slider_create(parent);
    lv_slider_set_range(slider, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE * 10);
    lv_obj_add_event_cb(slider, slider_x_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_size(slider, 200, 10);
    lv_obj_align_to(slider, chart, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);

    slider = lv_slider_create(parent);
    lv_slider_set_range(slider, LV_IMG_ZOOM_NONE, LV_IMG_ZOOM_NONE * 10);
    lv_obj_add_event_cb(slider, slider_y_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_size(slider, 10, 150);
    lv_obj_align_to(slider, chart, LV_ALIGN_OUT_RIGHT_MID, 20, 0);*/
}

void lv_example_tabview_1(void)
{
    /*Create a Tab view object*/
    lv_obj_t * tabview;
    tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);

    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Acquire");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Horizontal");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Vertical");
    lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "Trigger");
    lv_obj_t * tab5 = lv_tabview_add_tab(tabview, "Plot");

    lv_group_t * group = lv_group_create();
    /*Add content to the tabs*/
    ui_Screen1_screen_init(tab1, group);
    lv_example_chart_5(tab2, group);
    lv_example_flex_1(tab3, group);
    lv_example_list_1(tab4, group);
    lv_example_meter_2(tab5, group);


    lv_indev_set_group(indev_encoder, group);
    //lv_obj_scroll_to_view_recursive(label, LV_ANIM_ON);

}
void build_chart( lv_obj_t *parent )
{
	lv_obj_t *chart;

	chart = lv_chart_create( parent );
	lv_obj_set_width( chart, LCD_WIDTH );
	lv_obj_set_height( chart, LCD_HEIGHT );
	lv_obj_set_align( chart, LV_ALIGN_CENTER );
	//lv_chart_set_div_line_count( chart, 8+1, 12+1 );
	lv_chart_set_div_line_count( chart, 0, 0 );
}

void build_vertical( lv_obj_t *parent, lv_group_t *group )
{
    lv_obj_t *chart;
    lv_obj_t *panel;
    lv_obj_t *label_title;
    lv_obj_t *label_channel;
    lv_obj_t *dropdown_channel;
    lv_obj_t *label_enabled;
    lv_obj_t *switch_enabled;
    lv_obj_t *label_coupling;
    lv_obj_t *checkbox_coupling;
    lv_obj_t *label_offset;
    lv_obj_t *slider_offset;
    lv_obj_t *label_scale;
    lv_obj_t *roller_scale;

    chart = lv_chart_create( parent );
    lv_obj_set_width( chart, LCD_WIDTH/2 );
    lv_obj_set_height( chart, LCD_HEIGHT );
    lv_obj_set_align( chart, LV_ALIGN_LEFT_MID );
    lv_chart_set_div_line_count( chart, 8+1, 12+1 );

    panel = lv_obj_create( parent );
    lv_obj_set_width( panel, LCD_WIDTH/2 );
    lv_obj_set_height( panel, LCD_HEIGHT-35 );
    lv_obj_set_align( panel, LV_ALIGN_RIGHT_MID );
    lv_obj_set_x( panel, 0 );
    lv_obj_set_y( panel, 15 );
    //lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_ROW);

    //static lv_coord_t col_dsc[] = {60, 120+10, LV_GRID_TEMPLATE_LAST};
    //static lv_coord_t row_dsc[] = {30, 30, 30, 30, 30, 30, 30, 30, LV_GRID_TEMPLATE_LAST};
    static const lv_coord_t col_dsc[] = {80, 120, LV_GRID_TEMPLATE_LAST};
    static const lv_coord_t row_dsc[] = {40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array( panel, col_dsc, row_dsc );

    label_title = lv_label_create( parent );
    //lv_obj_set_width( label_title, LV_SIZE_CONTENT );
    //lv_obj_set_height( label_title, LV_SIZE_CONTENT );
    //lv_obj_set_align( label_title, LV_ALIGN_TOP_MID );
    //lv_obj_set_x( label_title, 0 );
    //lv_obj_set_y( label_title, 10 );
    lv_obj_align_to( label_title, panel, LV_ALIGN_OUT_TOP_LEFT, 10, -5 );
    lv_label_set_text( label_title, "Vertical" );
    lv_obj_set_style_text_font(label_title, &lv_font_montserrat_18, LV_PART_MAIN);

    label_channel = lv_label_create( panel );
    //lv_obj_set_width( label_channel, LV_SIZE_CONTENT );
    //lv_obj_set_height( label_channel, LV_SIZE_CONTENT );
    //lv_obj_set_align( label_channel, LV_ALIGN_TOP_LEFT );
    //lv_obj_set_x( label_channel, 0 );
    //lv_obj_set_y( label_channel, 40 );
    lv_label_set_text( label_channel, "Channel" );

    dropdown_channel = lv_dropdown_create( panel );
    lv_dropdown_set_options( dropdown_channel, "CH1\nCH2\nCH3\nCH4" );
    lv_obj_set_width( dropdown_channel, 100 );
    lv_obj_set_height( dropdown_channel, 35 );
    //lv_obj_set_height( dropdown_channel, LV_SIZE_CONTENT );
    //lv_obj_set_align( dropdown_channel, LV_ALIGN_TOP_RIGHT );
    //lv_obj_set_x( dropdown_channel, 0 );
    //lv_obj_set_y( dropdown_channel, 40 );
    //lv_obj_add_flag( dropdown_channel, LV_OBJ_FLAG_SCROLL_ON_FOCUS );

    label_enabled = lv_label_create( panel );
    //lv_obj_set_width( label_enabled, LV_SIZE_CONTENT );
    //lv_obj_set_height( label_enabled, LV_SIZE_CONTENT );
    //lv_obj_set_align( label_enabled, LV_ALIGN_TOP_LEFT );
    // lv_obj_set_x( label_enabled, 0 );
    //lv_obj_set_y( label_enabled, 80 );
    lv_label_set_text( label_enabled, "Enable" );

    switch_enabled = lv_switch_create( panel );
    //lv_obj_set_width( switch_enabled, 50 );
    lv_obj_set_height( switch_enabled, 25 );
    //lv_obj_set_align( switch_enabled, LV_ALIGN_TOP_RIGHT );
    //lv_obj_set_x( switch_enabled, 0 );
    //lv_obj_set_y( switch_enabled, 80 );

    label_coupling = lv_label_create( panel );
    //lv_obj_set_width( label_coupling, LV_SIZE_CONTENT );
    //lv_obj_set_height( label_coupling, LV_SIZE_CONTENT );
    //lv_obj_set_align( label_coupling, LV_ALIGN_TOP_LEFT );
    //lv_obj_set_x( label_coupling, 0 );
    //lv_obj_set_y( label_coupling, 120 );
    lv_label_set_text( label_coupling, "Coupling" );

    checkbox_coupling = lv_checkbox_create( panel );
    //checkbox_coupling = lv_switch_create( panel );
    //lv_obj_set_width( checkbox_coupling, 50 );
    //lv_obj_set_height( checkbox_coupling, 25 );
    //lv_obj_set_align( checkbox_coupling, LV_ALIGN_TOP_RIGHT );
    //lv_obj_set_x( checkbox_coupling, 0 );
    //lv_obj_set_y( checkbox_coupling, 120 );
    lv_checkbox_set_text(checkbox_coupling,"AC");

    label_offset = lv_label_create( panel );
    //lv_obj_set_width( label_offset, LV_SIZE_CONTENT );
    //lv_obj_set_height( label_offset, LV_SIZE_CONTENT );
    //lv_obj_set_align( label_offset, LV_ALIGN_TOP_LEFT );
    //lv_obj_set_x( label_offset, 0 );
    //lv_obj_set_y( label_offset, 160 );
    lv_label_set_text( label_offset, "Offset" );

    slider_offset = lv_slider_create( panel );
    lv_obj_set_width( slider_offset, 100 );
    //lv_obj_set_height( slider_offset, 10 );
    //lv_obj_set_align( slider_offset, LV_ALIGN_TOP_RIGHT );
    //lv_obj_set_x( slider_offset, 0 );
    //lv_obj_set_y( slider_offset, 160 );
    lv_slider_set_range( slider_offset, 0, 50 );

    label_scale = lv_label_create( panel );
    //lv_obj_set_width( label_scale, LV_SIZE_CONTENT );
    //lv_obj_set_height( label_scale, LV_SIZE_CONTENT );
    //lv_obj_set_align( label_scale, LV_ALIGN_TOP_LEFT );
    //lv_obj_set_x( label_scale, 0 );
    //lv_obj_set_y( label_scale, 240 );
    lv_label_set_text( label_scale, "Scale" );

    roller_scale = lv_roller_create( panel );
    lv_roller_set_options( roller_scale, "5 V/Div\n2 V/Div\n1 V/Div\n500 mV/Div\n200 mV/Div\n100 mV/Div", LV_ROLLER_MODE_NORMAL );
    lv_obj_set_width( roller_scale, 100 );
    lv_obj_set_height( roller_scale, 50 );
    //lv_obj_set_align( roller_scale, LV_ALIGN_TOP_RIGHT );
    //lv_obj_set_x( roller_scale, 0 );
    //lv_obj_set_y( roller_scale, 200 );

    lv_obj_set_grid_cell( label_channel, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell( label_enabled, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell( label_coupling,LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell( label_offset,  LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell( label_scale,   LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    lv_obj_set_grid_cell( dropdown_channel, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell( switch_enabled,   LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell( checkbox_coupling,LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell( slider_offset,    LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell( roller_scale,     LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    lv_group_add_obj( group, dropdown_channel );
    lv_group_add_obj( group, switch_enabled );
    lv_group_add_obj( group, checkbox_coupling );
    lv_group_add_obj( group, slider_offset );
    lv_group_add_obj( group, roller_scale );



    //lv_example_meter_2( panel, group );
}

void cb_horizontal_offset(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *slider = lv_event_get_target(e);

    if( code == LV_EVENT_VALUE_CHANGED )
    {
    	int offset = lv_slider_get_value(slider);
    	//scope_config_horizontal( &scope, offset, scope.horizontal.scale );
		tQueueUiScope msg = {0};
		msg.type = 1;
		msg.data[0] = offset;
		osMessageQueuePut(queueUiScopeHandle, &msg, 0U, portMAX_DELAY);
    }
}

void cb_horizontal_scale(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *roller = lv_event_get_target(e);

    if( code == LV_EVENT_VALUE_CHANGED )
    {
    	int scale = lv_roller_get_selected(roller);
    	if( scale == 0 )
    	{
    		scale = 40;//1;
    	}
    	else if( scale == 1 )
    	{
    		scale = 80;//2;
    	}
    	else if( scale == 2 )
    	{
    		scale = 200;//5;
    	}
    	else if( scale == 3 )
    	{
    		scale = 400;//10;
    	}
    	else if( scale == 4 )
    	{
    		scale = 800;//20;
    	}
    	else if( scale == 5 )
    	{
    		scale = 2000;//50;
    	}
    	else if( scale == 6 )
    	{
    		scale = 4000;//100;
    	}
    	else if( scale == 7 )
    	{
    		scale = 8000;//200;
    	}
    	else if( scale == 8 )
    	{
    		scale = 20000;//500;
    	}
    	else if( scale == 9 )
    	{
    		scale = 40000;//1000;
    	}
    	else
    	{
    		scale = 40000;//2000;
    	}
    	//scope_config_horizontal( &scope, scope.horizontal.offset, scale );
		tQueueUiScope msg = {0};
		msg.type = 2;
		msg.data[0] = scale;
		osMessageQueuePut(queueUiScopeHandle, &msg, 0U, portMAX_DELAY);
    }
}

void build_horizontal( lv_obj_t *parent, lv_group_t *group )
{
    lv_obj_t *chart;
    lv_obj_t *panel;
    lv_obj_t *label_title;
    //lv_obj_t *label_channel;
    //lv_obj_t *dropdown_channel;
    //lv_obj_t *label_enabled;
    //lv_obj_t *switch_enabled;
    //lv_obj_t *label_coupling;
    //lv_obj_t *checkbox_coupling;
    lv_obj_t *label_offset;
    lv_obj_t *slider_offset;
    lv_obj_t *label_scale;
    lv_obj_t *roller_scale;

    chart = lv_chart_create( parent );
    lv_obj_set_width( chart, LCD_WIDTH/2 );
    lv_obj_set_height( chart, LCD_HEIGHT );
    lv_obj_set_align( chart, LV_ALIGN_LEFT_MID );
    //lv_chart_set_div_line_count( chart, 8+1, 12+1 );
	lv_chart_set_div_line_count( chart, 0, 0 );

    panel = lv_obj_create( parent );
    lv_obj_set_width( panel, LCD_WIDTH/2 );
    lv_obj_set_height( panel, LCD_HEIGHT-35 );
    lv_obj_set_align( panel, LV_ALIGN_RIGHT_MID );
    lv_obj_set_x( panel, 0 );
    lv_obj_set_y( panel, 15 );
    //lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_ROW);

    //static lv_coord_t col_dsc[] = {60, 120+10, LV_GRID_TEMPLATE_LAST};
    //static lv_coord_t row_dsc[] = {30, 30, 30, 30, 30, 30, 30, 30, LV_GRID_TEMPLATE_LAST};
    static const lv_coord_t col_dsc[] = {80, 120, LV_GRID_TEMPLATE_LAST};
    static const lv_coord_t row_dsc[] = {40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array( panel, col_dsc, row_dsc );

    label_title = lv_label_create( parent );
    //lv_obj_set_width( label_title, LV_SIZE_CONTENT );
    //lv_obj_set_height( label_title, LV_SIZE_CONTENT );
    //lv_obj_set_align( label_title, LV_ALIGN_TOP_MID );
    //lv_obj_set_x( label_title, 0 );
    //lv_obj_set_y( label_title, 10 );
    lv_obj_align_to( label_title, panel, LV_ALIGN_OUT_TOP_LEFT, 10, -5 );
    lv_label_set_text( label_title, "Horizontal" );
    lv_obj_set_style_text_font(label_title, &lv_font_montserrat_18, LV_PART_MAIN);

    label_offset = lv_label_create( panel );
    //lv_obj_set_width( label_offset, LV_SIZE_CONTENT );
    //lv_obj_set_height( label_offset, LV_SIZE_CONTENT );
    //lv_obj_set_align( label_offset, LV_ALIGN_TOP_LEFT );
    //lv_obj_set_x( label_offset, 0 );
    //lv_obj_set_y( label_offset, 160 );
    lv_label_set_text( label_offset, "Offset" );

    slider_offset = lv_slider_create( panel );
    lv_obj_set_width( slider_offset, 100 );
    //lv_obj_set_height( slider_offset, 10 );
    //lv_obj_set_align( slider_offset, LV_ALIGN_TOP_RIGHT );
    //lv_obj_set_x( slider_offset, 0 );
    //lv_obj_set_y( slider_offset, 160 );
    lv_slider_set_range( slider_offset, 0, 50 );
    lv_obj_add_event_cb( slider_offset, cb_horizontal_offset, LV_EVENT_VALUE_CHANGED, NULL );

    label_scale = lv_label_create( panel );
    //lv_obj_set_width( label_scale, LV_SIZE_CONTENT );
    //lv_obj_set_height( label_scale, LV_SIZE_CONTENT );
    //lv_obj_set_align( label_scale, LV_ALIGN_TOP_LEFT );
    //lv_obj_set_x( label_scale, 0 );
    //lv_obj_set_y( label_scale, 240 );
    lv_label_set_text( label_scale, "Scale" );

    roller_scale = lv_roller_create( panel );
    lv_roller_set_options( roller_scale, "1 S/Div\n500 mS/Div\n200 mS/Div\n100 mS/Div\n50 mS/Div\n20 mS/Div\n10 mS/Div\n5 mS/Div\n2 mS/Div\n1 mS/Div", LV_ROLLER_MODE_NORMAL );
    lv_roller_set_selected( roller_scale, 9, LV_ANIM_OFF );
    lv_obj_set_width( roller_scale, 100 );
    lv_obj_set_height( roller_scale, 50 );
    //lv_obj_set_align( roller_scale, LV_ALIGN_TOP_RIGHT );
    //lv_obj_set_x( roller_scale, 0 );
    //lv_obj_set_y( roller_scale, 200 );
    lv_obj_add_event_cb( roller_scale, cb_horizontal_scale, LV_EVENT_VALUE_CHANGED, NULL );

    lv_obj_set_grid_cell( label_offset,  LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell( label_scale,   LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    lv_obj_set_grid_cell( slider_offset,    LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell( roller_scale,     LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    lv_group_add_obj( group, slider_offset );
    lv_group_add_obj( group, roller_scale );
}

void _build_trigger( lv_obj_t *parent, lv_group_t *group )
{
    lv_obj_t *chart;
    lv_obj_t *panel;
    lv_obj_t *label_title;
    lv_obj_t *label_enable;
    lv_obj_t *switch_enable;
    lv_obj_t *label_source;
    lv_obj_t *dropdown_source;
    lv_obj_t *label_slope;
    lv_obj_t *checkbox_slope;
    lv_obj_t *label_level;
    lv_obj_t *slider_level;

    chart = lv_chart_create( parent );
    lv_obj_set_width( chart, LCD_WIDTH/2 );
    lv_obj_set_height( chart, LCD_HEIGHT );
    lv_obj_set_align( chart, LV_ALIGN_LEFT_MID );
    lv_chart_set_div_line_count( chart, 8+1, 12+1 );

    panel = lv_obj_create( parent );
    lv_obj_set_width( panel, LCD_WIDTH/2 );
    lv_obj_set_height( panel, LCD_HEIGHT-35 );
    lv_obj_set_align( panel, LV_ALIGN_RIGHT_MID );
    lv_obj_set_x( panel, 0 );
    lv_obj_set_y( panel, 15 );
    //lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_ROW);

    //static lv_coord_t col_dsc[] = {60, 120+10, LV_GRID_TEMPLATE_LAST};
    //static lv_coord_t row_dsc[] = {30, 30, 30, 30, 30, 30, 30, 30, LV_GRID_TEMPLATE_LAST};
    static const lv_coord_t col_dsc[] = {80, 120, LV_GRID_TEMPLATE_LAST};
    static const lv_coord_t row_dsc[] = {40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array( panel, col_dsc, row_dsc );

    label_title = lv_label_create( parent );
    //lv_obj_set_width( label_title, LV_SIZE_CONTENT );
    //lv_obj_set_height( label_title, LV_SIZE_CONTENT );
    //lv_obj_set_align( label_title, LV_ALIGN_TOP_MID );
    //lv_obj_set_x( label_title, 0 );
    //lv_obj_set_y( label_title, 10 );
    lv_obj_align_to( label_title, panel, LV_ALIGN_OUT_TOP_LEFT, 10, -5 );
    lv_label_set_text( label_title, "Trigger" );
    lv_obj_set_style_text_font(label_title, &lv_font_montserrat_18, LV_PART_MAIN);

    label_enable = lv_label_create( panel );
    //lv_obj_set_width( label_channel, LV_SIZE_CONTENT );
    //lv_obj_set_height( label_channel, LV_SIZE_CONTENT );
    //lv_obj_set_align( label_channel, LV_ALIGN_TOP_LEFT );
    //lv_obj_set_x( label_channel, 0 );
    //lv_obj_set_y( label_channel, 40 );
    lv_label_set_text( label_enable, "Enable" );

    switch_enable = lv_switch_create( panel );
    //lv_obj_set_width( switch_enabled, 50 );
    lv_obj_set_height( switch_enable, 25 );
    //lv_obj_set_align( switch_enabled, LV_ALIGN_TOP_RIGHT );
    //lv_obj_set_x( switch_enabled, 0 );
    //lv_obj_set_y( switch_enabled, 80 );

    label_source = lv_label_create( panel );
    //lv_obj_set_width( label_enabled, LV_SIZE_CONTENT );
    //lv_obj_set_height( label_enabled, LV_SIZE_CONTENT );
    //lv_obj_set_align( label_enabled, LV_ALIGN_TOP_LEFT );
    // lv_obj_set_x( label_enabled, 0 );
    //lv_obj_set_y( label_enabled, 80 );
    lv_label_set_text( label_source, "Source" );

    dropdown_source = lv_dropdown_create( panel );
	lv_dropdown_set_options( dropdown_source, "CH1\nCH2\nCH3\nCH4" );
	lv_obj_set_width( dropdown_source, 100 );
	lv_obj_set_height( dropdown_source, 35 );
	//lv_obj_set_height( dropdown_channel, LV_SIZE_CONTENT );
	//lv_obj_set_align( dropdown_channel, LV_ALIGN_TOP_RIGHT );
	//lv_obj_set_x( dropdown_channel, 0 );
	//lv_obj_set_y( dropdown_channel, 40 );
	//lv_obj_add_flag( dropdown_channel, LV_OBJ_FLAG_SCROLL_ON_FOCUS );


    label_slope = lv_label_create( panel );
    //lv_obj_set_width( label_coupling, LV_SIZE_CONTENT );
    //lv_obj_set_height( label_coupling, LV_SIZE_CONTENT );
    //lv_obj_set_align( label_coupling, LV_ALIGN_TOP_LEFT );
    //lv_obj_set_x( label_coupling, 0 );
    //lv_obj_set_y( label_coupling, 120 );
    lv_label_set_text( label_slope, "Slope" );

    checkbox_slope = lv_checkbox_create( panel );
    //checkbox_coupling = lv_switch_create( panel );
    //lv_obj_set_width( checkbox_coupling, 50 );
    //lv_obj_set_height( checkbox_coupling, 25 );
    //lv_obj_set_align( checkbox_coupling, LV_ALIGN_TOP_RIGHT );
    //lv_obj_set_x( checkbox_coupling, 0 );
    //lv_obj_set_y( checkbox_coupling, 120 );
    lv_checkbox_set_text( checkbox_slope,"Rising");

    label_level = lv_label_create( panel );
    //lv_obj_set_width( label_offset, LV_SIZE_CONTENT );
    //lv_obj_set_height( label_offset, LV_SIZE_CONTENT );
    //lv_obj_set_align( label_offset, LV_ALIGN_TOP_LEFT );
    //lv_obj_set_x( label_offset, 0 );
    //lv_obj_set_y( label_offset, 160 );
    lv_label_set_text( label_level, "Level" );

    slider_level = lv_slider_create( panel );
    lv_obj_set_width( slider_level, 100 );
    //lv_obj_set_height( slider_offset, 10 );
    //lv_obj_set_align( slider_offset, LV_ALIGN_TOP_RIGHT );
    //lv_obj_set_x( slider_offset, 0 );
    //lv_obj_set_y( slider_offset, 160 );
    lv_slider_set_range( slider_level, 0, 50 );


    lv_obj_set_grid_cell( label_enable, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell( label_source, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell( label_slope,  LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell( label_level,  LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    lv_obj_set_grid_cell( switch_enable,   LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell( dropdown_source, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell( checkbox_slope,  LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell( slider_level,    LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    lv_group_add_obj( group, switch_enable );
    lv_group_add_obj( group, dropdown_source );
    lv_group_add_obj( group, checkbox_slope );
    lv_group_add_obj( group, slider_level );
}

void build_trigger( lv_obj_t *parent, lv_group_t *group )
{
        lv_obj_t *chart;
    lv_obj_t *panel;
    lv_obj_t *label_title;
    lv_obj_t *arc1;
    lv_obj_t *arc2;
    lv_obj_t *arc3;
    lv_obj_t *arc4;
    lv_obj_t *arc5;
    lv_obj_t *arc6;
    lv_obj_t *arc7;
    lv_obj_t *arc8;

    chart = lv_chart_create( parent );
    lv_obj_set_width( chart, LCD_WIDTH/2 );
    lv_obj_set_height( chart, LCD_HEIGHT );
    lv_obj_set_align( chart, LV_ALIGN_LEFT_MID );
    lv_chart_set_div_line_count( chart, 8+1, 12+1 );

    panel = lv_obj_create( parent );
    lv_obj_set_width( panel, LCD_WIDTH/2 );
    lv_obj_set_height( panel, LCD_HEIGHT-35 );
    lv_obj_set_align( panel, LV_ALIGN_RIGHT_MID );
    lv_obj_set_x( panel, 0 );
    lv_obj_set_y( panel, 15 );
    //lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_ROW);

    //static lv_coord_t col_dsc[] = {60, 120+10, LV_GRID_TEMPLATE_LAST};
    //static lv_coord_t row_dsc[] = {30, 30, 30, 30, 30, 30, 30, 30, LV_GRID_TEMPLATE_LAST};
    static const lv_coord_t col_dsc[] = {80, 80, LV_GRID_TEMPLATE_LAST};
    static const lv_coord_t row_dsc[] = {40, 40, 40, 40, 40, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array( panel, col_dsc, row_dsc );

    label_title = lv_label_create( parent );
    //lv_obj_set_width( label_title, LV_SIZE_CONTENT );
    //lv_obj_set_height( label_title, LV_SIZE_CONTENT );
    //lv_obj_set_align( label_title, LV_ALIGN_TOP_MID );
    //lv_obj_set_x( label_title, 0 );
    //lv_obj_set_y( label_title, 10 );
    lv_obj_align_to( label_title, panel, LV_ALIGN_OUT_TOP_LEFT, 10, -5 );
    lv_label_set_text( label_title, "Arcs" );
    lv_obj_set_style_text_font(label_title, &lv_font_montserrat_18, LV_PART_MAIN);

    arc1 = lv_arc_create( panel );
    lv_obj_set_size( arc1, 50, 50 );
    // set width of the arc
    lv_obj_set_style_arc_width( arc1, 5, LV_PART_MAIN );
    lv_obj_set_style_arc_width( arc1, 5, LV_PART_INDICATOR );
    // set color of the arc
    lv_obj_set_style_bg_color( arc1, lv_palette_darken( LV_PALETTE_RED, 4 ), LV_PART_KNOB );
    lv_obj_set_style_arc_color( arc1, lv_palette_main( LV_PALETTE_RED ), LV_PART_INDICATOR );

    arc2 = lv_arc_create( panel );
    lv_obj_set_size( arc2, 50, 50 );
    // set width of the arc
    lv_obj_set_style_arc_width( arc2, 5, LV_PART_MAIN );
    lv_obj_set_style_arc_width( arc2, 5, LV_PART_INDICATOR );
    // set color of the arc
    lv_obj_set_style_bg_color( arc2, lv_palette_darken( LV_PALETTE_GREEN , 4 ), LV_PART_KNOB );
    lv_obj_set_style_arc_color( arc2, lv_palette_main( LV_PALETTE_GREEN ), LV_PART_INDICATOR );

    arc3 = lv_arc_create( panel );
    lv_obj_set_size( arc3, 50, 50 );
    // set width of the arc
    lv_obj_set_style_arc_width( arc3, 5, LV_PART_MAIN );
    lv_obj_set_style_arc_width( arc3, 5, LV_PART_INDICATOR );
    // set color of the arc
    lv_obj_set_style_bg_color( arc3, lv_palette_darken( LV_PALETTE_BLUE, 4 ), LV_PART_KNOB );
    lv_obj_set_style_arc_color( arc3, lv_palette_main( LV_PALETTE_BLUE ), LV_PART_INDICATOR );

    arc4 = lv_arc_create( panel );
    lv_obj_set_size( arc4, 50, 50 );
    // set width of the arc
    lv_obj_set_style_arc_width( arc4, 5, LV_PART_MAIN );
    lv_obj_set_style_arc_width( arc4, 5, LV_PART_INDICATOR );
    // set color of the arc
    lv_obj_set_style_bg_color( arc4, lv_palette_darken(( LV_PALETTE_PURPLE ), 4 ), LV_PART_KNOB );
    lv_obj_set_style_arc_color( arc4, lv_palette_main( LV_PALETTE_PURPLE ), LV_PART_INDICATOR );

    arc5 = lv_arc_create( panel );
    lv_obj_set_size( arc5, 50, 50 );
    // set width of the arc
    lv_obj_set_style_arc_width( arc5, 5, LV_PART_MAIN );
    lv_obj_set_style_arc_width( arc5, 5, LV_PART_INDICATOR );
    // set color of the arc
    lv_obj_set_style_bg_color( arc5, lv_palette_darken(( LV_PALETTE_RED ), 4 ), LV_PART_KNOB );
    lv_obj_set_style_arc_color( arc5, lv_palette_main( LV_PALETTE_RED ), LV_PART_INDICATOR );

    arc6 = lv_arc_create( panel );
    lv_obj_set_size( arc6, 50, 50 );
    // set width of the arc
    lv_obj_set_style_arc_width( arc6, 5, LV_PART_MAIN );
    lv_obj_set_style_arc_width( arc6, 5, LV_PART_INDICATOR );
    // set color of the arc
    lv_obj_set_style_bg_color( arc6, lv_palette_darken( ( LV_PALETTE_GREEN ), 4 ), LV_PART_KNOB );
    lv_obj_set_style_arc_color( arc6, lv_palette_main( LV_PALETTE_GREEN ), LV_PART_INDICATOR );

    arc7 = lv_arc_create( panel );
    lv_obj_set_size( arc7, 50, 50 );
    // set width of the arc
    lv_obj_set_style_arc_width( arc7, 5, LV_PART_MAIN );
    lv_obj_set_style_arc_width( arc7, 5, LV_PART_INDICATOR );
    // set color of the arc
    lv_obj_set_style_bg_color( arc7, lv_palette_darken( ( LV_PALETTE_BLUE ), 4 ), LV_PART_KNOB );
    lv_obj_set_style_arc_color( arc7, lv_palette_main( LV_PALETTE_BLUE ), LV_PART_INDICATOR );

    arc8 = lv_arc_create( panel );
    lv_obj_set_size( arc8, 50, 50 );
    // set width of the arc
    lv_obj_set_style_arc_width( arc8, 5, LV_PART_MAIN );
    lv_obj_set_style_arc_width( arc8, 5, LV_PART_INDICATOR );
    // set color of the arc
    lv_obj_set_style_bg_color( arc8, lv_palette_darken( ( LV_PALETTE_PURPLE ), 4 ), LV_PART_KNOB );
    lv_obj_set_style_arc_color( arc8, lv_palette_main( LV_PALETTE_PURPLE ), LV_PART_INDICATOR );

    lv_obj_set_grid_cell( arc1, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell( arc2, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell( arc3, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell( arc4, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    lv_obj_set_grid_cell( arc5, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_grid_cell( arc6, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell( arc7, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell( arc8, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    lv_group_add_obj( group, arc1 );
    lv_group_add_obj( group, arc2 );
    lv_group_add_obj( group, arc3 );
    lv_group_add_obj( group, arc4 );

    lv_group_add_obj( group, arc5 );
    lv_group_add_obj( group, arc6 );
    lv_group_add_obj( group, arc7 );
    lv_group_add_obj( group, arc8 );
}
lv_group_t *group = NULL;
lv_obj_t *last_scr = NULL;
void build_ui( int menu )
{

	if( group != NULL )
	{
		lv_group_del(group);
	}

    if( last_scr != NULL )
    {
    	lv_scr_load( lv_obj_create(NULL) );
    	lv_obj_del( last_scr );
    }
    lv_obj_t *scr = lv_obj_create(NULL);
	group = lv_group_create();
	lv_group_set_wrap( group, 0 );

	if( menu == 0 )
	{
		build_chart( scr );
	}
	else if( menu == 1 )
	{
		build_horizontal( scr, group );
	}
	else if( menu == 2 )
	{
		build_vertical( scr, group );
	}
	else if( menu == 3 )
	{
		build_trigger( scr, group );
	}

    lv_indev_set_group( indev_encoder, group );

    lv_scr_load( scr );
//    if( last_scr != NULL )
//    {
//    	lv_obj_del( last_scr );
//    }
    last_scr = scr;

}
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
int _write(int file, char *ptr, int len)
{
	int i;
	for(i = 0; i < len; i++)
	{
		ITM_SendChar(ptr[i]);
	}
    return len;
}

void StartTaskTsc(void *argument)
{
    const float AX = 38.0/151.0;
    const float BX = -1950.0/151.0;
    const float AY = 11.0/62.0;
    const float BY = -1157.0/62.0;

    const TickType_t xFrequency = 10;
    TickType_t xLastWakeTime;
    tQueueTscUi msg = {0};
    tTsc tsc = {0};

    tsc_init(&tsc,
        &hspi3,
        TSC_nSS_GPIO_Port, TSC_nSS_Pin,
        AX, BX, AY, BY,
        0.5, // tau
        10   // cnt_max
    );

    xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		uint16_t x, y, p;
		tsc_read(&tsc, &x, &y, &p);

		if( p || p != msg.p )
		{
			msg.x = x;
			msg.y = y;
			msg.p = p;
			osMessageQueuePut(queueTscUiHandle, &msg, 0U, portMAX_DELAY);
		}

		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void StartTaskUsb(void *argument)
{
	const lv_point_t keypad_points[5] = {
		{LCD_WIDTH/10*1, 25},
		{LCD_WIDTH/10*3, 25},
		{LCD_WIDTH/10*5, 25},
		{LCD_WIDTH/10*7, 25},
		{LCD_WIDTH/10*9, 25},
	};

	extern uint8_t usb_buf[8];
	extern uint8_t usb_flag;
	tQueueUsbUi msgUsbUi = {0};
	tQueueTscUi msgTscUi = {0};

	while(1)
	{
		if( usb_flag )
		{
			int btn = usb_buf[0]-1;
			int val = (int8_t)usb_buf[1];
			//printf( "received usb msg %d %d\n", btn, val );

			if( 0 <= btn && btn < 4 )
			{
				//msgTscUi.x = keypad_points[btn].x;
				//msgTscUi.y = keypad_points[btn].y;
				//msgTscUi.p = 1;
				//osMessageQueuePut(queueTscUiHandle, &msgTscUi, 0U, portMAX_DELAY);
				//msgTscUi.x = keypad_points[btn].x;
				//msgTscUi.y = keypad_points[btn].y;
				//msgTscUi.p = 0;
				//osMessageQueuePut(queueTscUiHandle, &msgTscUi, 0U, portMAX_DELAY);
				msgUsbUi.btn = btn;
				msgUsbUi.value = val;
				osMessageQueuePut(queueUsb2UiHandle, &msgUsbUi, 0U, portMAX_DELAY);
			}
			else if( btn == 4 )
			{
				msgUsbUi.btn = btn;
				msgUsbUi.value = val;
				osMessageQueuePut(queueUsbUiHandle, &msgUsbUi, 0U, portMAX_DELAY);
			}
			else if( btn == 5 )
			{
				msgUsbUi.btn = btn;
				msgUsbUi.value = val;
				osMessageQueuePut(queueUsbUiHandle, &msgUsbUi, 0U, portMAX_DELAY);
			}

			usb_flag = 0;
		}
		osDelay(10);
	}
}

void disp_flush_cb(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p) {
	if( osSemaphoreAcquire( semLcdHandle, portMAX_DELAY ) == osOK )
	{
		lcd_bmp( &lcd,
			area->x1,
			area->y1,
			area->x2 - area->x1 + 1,
			area->y2 - area->y1 + 1,
			(uint16_t*)color_p
		);
		osSemaphoreRelease( semLcdHandle );
	}
	lv_disp_flush_ready(disp_drv);
}

void touchpad_input_read(lv_indev_drv_t* drv, lv_indev_data_t* data) {
  static uint16_t x = 0;
  static uint16_t y = 0;
  static uint8_t p = 0;
  tQueueTscUi msg = {0};

  if( osMessageQueueGet( queueTscUiHandle, &msg, NULL, 0) == osOK )
  {
	  x = msg.x;
	  y = msg.y;
	  p = msg.p;
  }

  data->point.x = x;
  data->point.y = y;
  data->state = p;
}

void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
	static uint8_t last_btn = 0;
    int8_t btn_act = -1;
	tQueueUsbUi msg = {0};

    /*Get the pressed button's ID*/
    if( osMessageQueueGet( queueUsbUiHandle, &msg, NULL, 0) == osOK )
	{
    	if( msg.value )
    	{
    		btn_act = msg.btn-1;
    	}
	}

    if(btn_act >= 0)
    {
        data->state = LV_INDEV_STATE_PR;
        last_btn = btn_act;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Save the last pressed button's ID*/
    data->btn_id = last_btn;
}

static int32_t encoder_diff = 0;
static lv_indev_state_t encoder_state = LV_INDEV_STATE_REL;

static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
	tQueueUsbUi msg = {0};
	if( osMessageQueueGet( queueUsbUiHandle, &msg, NULL, 0) == osOK )
	{
		if( msg.btn == 4 )
		{
			if( msg.value )
			{
				encoder_state = LV_INDEV_STATE_PR;
			}
			else
			{
				encoder_state = LV_INDEV_STATE_REL;
			}
		}

		if( msg.btn == 5 )
		{
			if( msg.value > 0 )
			{
				encoder_diff -= 1;
			}
			else if ( msg.value < 0 )
			{
				encoder_diff += 1;
			}
			//encoder_state = LV_INDEV_STATE_REL;
		}
	}

    data->enc_diff = encoder_diff;
    data->state = encoder_state;
    encoder_diff = 0;
    //encoder_state = LV_INDEV_STATE_REL;
}
int menu = 0;
int run = 1;
static int ccc = 0;
static int ccc2 = 0;
void StartTaskUi(void *argument)
{
	static lv_color_t fb_buf_1[LCD_WIDTH * 20];
	//static lv_color_t fb_buf_2[LCD_WIDTH * 20];

	lv_disp_draw_buf_t disp_buf;
	lv_disp_drv_t disp_drv;
	lv_indev_drv_t indev_drv;
	lv_indev_drv_t indev_drv2;

	uint32_t delay = 0;

	tQueueUsbUi msg = {0};

	//if( osSemaphoreAcquire( semLcdHandle, portMAX_DELAY ) == osOK )
	{
		lcd_init( &lcd,
			LCD_nRST_GPIO_Port, LCD_nRST_Pin,
			LCD_BL_GPIO_Port, LCD_BL_Pin,
			LCD_WIDTH, LCD_HEIGHT
		);
		osSemaphoreRelease( semLcdHandle );
	}

    lv_init();

    // display driver
    lv_disp_drv_init( &disp_drv );
	disp_drv.hor_res = LCD_WIDTH;
	disp_drv.ver_res = LCD_HEIGHT;
	disp_drv.flush_cb = disp_flush_cb;

	lv_disp_draw_buf_init( &disp_buf, fb_buf_1, NULL, sizeof(fb_buf_1)/sizeof(lv_color_t) );
	disp_drv.draw_buf = &disp_buf;

	lv_disp_drv_register(&disp_drv);

	// touch screen driver
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = touchpad_input_read;
	lv_indev_drv_register(&indev_drv);

	// buttons driver
	/*lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_BUTTON;
    indev_drv.read_cb = button_read;
    lv_indev_t * indev_button = lv_indev_drv_register(&indev_drv);

    const lv_point_t keypad_points[4] = {
        {LCD_WIDTH/8*1, 25},
		{LCD_WIDTH/8*3, 25},
		{LCD_WIDTH/8*5, 25},
		{LCD_WIDTH/8*7, 25},
    };
    lv_indev_set_button_points(indev_button, keypad_points);*/

	// Encoder
    lv_indev_drv_init(&indev_drv2);
    indev_drv2.type = LV_INDEV_TYPE_ENCODER;
    indev_drv2.read_cb = encoder_read;
    indev_encoder = lv_indev_drv_register(&indev_drv2);

	//lv_example_tabview_1();

    //build_ui();
    build_ui(0);

	while(1)
	{
		delay = lv_timer_handler();
		vTaskDelay( delay );
		lv_tick_inc( delay );

		tQueueUsbUi msg = {0};
		if( osMessageQueueGet( queueUsb2UiHandle, &msg, NULL, 0) == osOK )
		{
			ccc = 20;
			ccc2 = 20;
			if( msg.btn == 0 )
			{
				run = !run;
			}
			else
			{
				if( msg.btn == 1 )
				{
					if( menu == 1 )
					{
						menu = 0;
					}
					else
					{
						menu = 1;
					}
				}
				if( msg.btn == 2 )
				{
					if( menu == 2 )
					{
						menu = 0;
					}
					else
					{
						menu = 2;
					}
				}
				if( msg.btn == 3 )
				{
					if( menu == 3 )
					{
						menu = 0;
					}
					else
					{
						menu = 3;
					}
				}
				build_ui(menu);
			}
		}
	}

	lv_deinit();
}

#include "wavegen.h"

#define ABS(a) (((a)>(0))?(a):-(a))
#define M_PIf		3.14159f

#define COLOR_PALETTE_BLACK (0x0000)
#define COLOR_PALETTE_WHITE (0xFFFF)
#define COLOR_PALETTE_RED 	(0xf206)
#define COLOR_PALETTE_GREEN	(0x4d6a)
#define COLOR_PALETTE_BLUE 	(0x24be)
#define COLOR_PALETTE_PURPLE (0x9936)

#define COLOR_PALETTE_RED2 		0xfe7a
#define COLOR_PALETTE_GREEN2	0xcf39
#define COLOR_PALETTE_BLUE2 	0xbeff
#define COLOR_PALETTE_PURPLE2 	0xe5fc

#define ADC_BUFFER_LEN (512)

#define SCOPE_COLOR_BACKGROUND  COLOR_PALETTE_WHITE
#define SCOPE_COLOR_CH1         COLOR_PALETTE_RED       // Color for channel 1
#define SCOPE_COLOR_CH2         COLOR_PALETTE_GREEN     // Color for channel 2
#define SCOPE_COLOR_CH3         COLOR_PALETTE_BLUE      // Color for channel 3
#define SCOPE_COLOR_CH4         COLOR_PALETTE_PURPLE    // Color for channel 4

#define SCOPE_COLOR_CH1_LIGHT         COLOR_PALETTE_RED2       // Color for channel 1
#define SCOPE_COLOR_CH2_LIGHT         COLOR_PALETTE_GREEN2     // Color for channel 2
#define SCOPE_COLOR_CH3_LIGHT         COLOR_PALETTE_BLUE2      // Color for channel 3
#define SCOPE_COLOR_CH4_LIGHT         COLOR_PALETTE_PURPLE2    // Color for channel 4



#define ABS(a)   ((a>0)?(a):-(a))
#define MIN(a,b) ((a<b)?(a):(b))
int kkk = 0;
void scope_draw_signals(tScope *pThis, tLcd *pLcd, int is_collapsed) {
    uint16_t i;
    //float scale = 320 / 4096.0f; // Scale factor for signal amplitudes
    uint16_t trigger;
    static uint16_t trigger_bck = 0;
    uint16_t n, n_bck;
    int pLcd_height = pLcd->height;
    static int is_collapsed_bck = 0;

    static int vertical_enable1_bck = 0;
    static int vertical_enable2_bck = 0;
    static int vertical_enable3_bck = 0;
    static int vertical_enable4_bck = 0;

    static int vertical_offset1_bck = 0;
    static int vertical_offset2_bck = 0;
    static int vertical_offset3_bck = 0;
    static int vertical_offset4_bck = 0;

    static int vertical_scale1_bck = 0;
    static int vertical_scale2_bck = 0;
    static int vertical_scale3_bck = 0;
    static int vertical_scale4_bck = 0;

    uint16_t x, w;
    int16_t y1, y2, y3, y4, y5, y6, y7, y8;
    int16_t y1_bck = 0;
    int16_t y2_bck = 0;
    int16_t y3_bck = 0;
    int16_t y4_bck = 0;
    int16_t y5_bck = 0;
    int16_t y6_bck = 0;
    int16_t y7_bck = 0;
    int16_t y8_bck = 0;

    // Calculate the trigger position
    volatile static int offset_trig = 0;
    trigger = pThis->len - pThis->dma_cndtr + offset_trig;
    trigger-= pThis->len/2;
    trigger-= pLcd->width/2;
    trigger%= pThis->len;

    // Check if the signal is in the first or second half of the buffer
    if (pThis->cnt & 0x01) {
    	for (i = 0; i < pLcd->width; i++) {
    	    if (1) {
                // Calculate the current sample index for the signal data
                n_bck = trigger_bck + i;
                n_bck = n_bck % pThis->len;

                // Determine the x-coordinate on the display based on the view mode
                if (is_collapsed_bck) {
                    x = (i) / 2;
                } else {
                    x = i;
                }

                // Calculate the y-coordinates for signal traces
                y5 = pLcd_height/2 + pLcd_height - (pThis->buffer5[n_bck]-2048) * vertical_scale1_bck/1000.0f + vertical_offset1_bck;
                y6 = pLcd_height/2 + pLcd_height - (pThis->buffer6[n_bck]-2048) * vertical_scale2_bck/1000.0f + vertical_offset2_bck;
                y7 = pLcd_height/2 + pLcd_height - (pThis->buffer7[n_bck]-2048) * vertical_scale3_bck/1000.0f + vertical_offset3_bck;
                y8 = pLcd_height/2 + pLcd_height - (pThis->buffer8[n_bck]-2048) * vertical_scale4_bck/1000.0f + vertical_offset4_bck;


                if( x > 0 )
				{
					// Draw vertical lines for signal traces
					if( vertical_enable1_bck )
					{
						lcd_vline(pLcd, x, MIN(y5, y5_bck), ABS(y5 - y5_bck) + 1, SCOPE_COLOR_BACKGROUND);
						//lcd_set_pixel(pLcd, x, MIN(y5, y5_bck)-1, SCOPE_COLOR_BACKGROUND);
						//lcd_set_pixel(pLcd, x, MAX(y5, y5_bck)+1, SCOPE_COLOR_BACKGROUND);
					}
					if( vertical_enable2_bck )
					{
						lcd_vline(pLcd, x, MIN(y6, y6_bck), ABS(y6 - y6_bck) + 1, SCOPE_COLOR_BACKGROUND);
						//lcd_set_pixel(pLcd, x, MIN(y6, y6_bck)-1, SCOPE_COLOR_BACKGROUND);
						//lcd_set_pixel(pLcd, x, MAX(y6, y6_bck)+1, SCOPE_COLOR_BACKGROUND);
					}
					if( vertical_enable3_bck )
					{
						lcd_vline(pLcd, x, MIN(y7, y7_bck), ABS(y7 - y7_bck) + 1, SCOPE_COLOR_BACKGROUND);
						//lcd_set_pixel(pLcd, x, MIN(y7, y7_bck)-1, SCOPE_COLOR_BACKGROUND);
						//lcd_set_pixel(pLcd, x, MAX(y7, y7_bck)+1, SCOPE_COLOR_BACKGROUND);
					}
					if( vertical_enable4_bck )
					{
						lcd_vline(pLcd, x, MIN(y8, y8_bck), ABS(y8 - y8_bck) + 1, SCOPE_COLOR_BACKGROUND);
						//lcd_set_pixel(pLcd, x, MIN(y8, y8_bck)-1, SCOPE_COLOR_BACKGROUND);
						//lcd_set_pixel(pLcd, x, MAX(y8, y8_bck)+1, SCOPE_COLOR_BACKGROUND);
					}
				}
                // Update previous values for next iteration
                y5_bck = y5;
                y6_bck = y6;
                y7_bck = y7;
                y8_bck = y8;
            }

            // Check if we have enough iterations to draw the second set of signals
            if (i > 2) {
                n = trigger + (i - 2);
                n = n % pThis->len;

                // Determine the x-coordinate on the display based on the view mode
                if (is_collapsed) {
                    x = (i - 2) / 2;
                } else {
                    x = (i - 2);
                }

                // Calculate the y-coordinates for signal traces
                y1 = pLcd_height/2 + pLcd_height - (pThis->buffer1[n]-2048) * pThis->vertical.scale1/1000.0f + pThis->vertical.offset1;
                y2 = pLcd_height/2 + pLcd_height - (pThis->buffer2[n]-2048) * pThis->vertical.scale2/1000.0f + pThis->vertical.offset2;
                y3 = pLcd_height/2 + pLcd_height - (pThis->buffer3[n]-2048) * pThis->vertical.scale3/1000.0f + pThis->vertical.offset3;
                y4 = pLcd_height/2 + pLcd_height - (pThis->buffer4[n]-2048) * pThis->vertical.scale4/1000.0f + pThis->vertical.offset4;

                if( x > 0 )
                {
					// Draw vertical lines for signal traces with channel-specific colors
					if( pThis->vertical.enable1 )
					{
						lcd_vline(pLcd, x, MIN(y1, y1_bck), ABS(y1 - y1_bck) + 1, SCOPE_COLOR_CH1);
						//lcd_set_pixel(pLcd, x, MIN(y1, y1_bck)-1, SCOPE_COLOR_CH1_LIGHT);
						//lcd_set_pixel(pLcd, x, MAX(y1, y1_bck)+1, SCOPE_COLOR_CH1_LIGHT);
					}
					if( pThis->vertical.enable2 )
					{
						lcd_vline(pLcd, x, MIN(y2, y2_bck), ABS(y2 - y2_bck) + 1, SCOPE_COLOR_CH2);
						//lcd_set_pixel(pLcd, x, MIN(y2, y2_bck)-1, SCOPE_COLOR_CH2_LIGHT);
						//lcd_set_pixel(pLcd, x, MAX(y2, y2_bck)+1, SCOPE_COLOR_CH2_LIGHT);
					}
					if( pThis->vertical.enable3 )
					{
						lcd_vline(pLcd, x, MIN(y3, y3_bck), ABS(y3 - y3_bck) + 1, SCOPE_COLOR_CH3);
						//lcd_set_pixel(pLcd, x, MIN(y3, y3_bck)-1, SCOPE_COLOR_CH3_LIGHT);
						//lcd_set_pixel(pLcd, x, MAX(y3, y3_bck)+1, SCOPE_COLOR_CH3_LIGHT);
					}
					if( pThis->vertical.enable4 )
					{
						lcd_vline(pLcd, x, MIN(y4, y4_bck), ABS(y4 - y4_bck) + 1, SCOPE_COLOR_CH4);
						//lcd_set_pixel(pLcd, x, MIN(y4, y4_bck)-1, SCOPE_COLOR_CH4_LIGHT);
						//lcd_set_pixel(pLcd, x, MAX(y4, y4_bck)+1, SCOPE_COLOR_CH4_LIGHT);
					}
                }

                // Update previous values for next iteration
                y1_bck = y1;
                y2_bck = y2;
                y3_bck = y3;
                y4_bck = y4;
            }
        }
    }
    else
    {
    	for( i = 0; i < pLcd->width; i++ )
    	{
            if( 1 )
            {
                // Calculate the current sample index for the signal data
                n_bck = trigger_bck + i;// - pThis->horizontal.offset/2;
                n_bck = n_bck % pThis->len;

                // Determine the x-coordinate on the display based on the view mode
                if( is_collapsed_bck )
                {
                    x = i/2;
                }
                else
                {
                    x = i;
                }

                // Calculate the y-coordinates for signal traces
                y1 = pLcd_height/2 + pLcd_height - (pThis->buffer1[n_bck]-2048) * vertical_scale1_bck/1000.0f + vertical_offset1_bck;
                y2 = pLcd_height/2 + pLcd_height - (pThis->buffer2[n_bck]-2048) * vertical_scale2_bck/1000.0f + vertical_offset2_bck;
                y3 = pLcd_height/2 + pLcd_height - (pThis->buffer3[n_bck]-2048) * vertical_scale3_bck/1000.0f + vertical_offset3_bck;
                y4 = pLcd_height/2 + pLcd_height - (pThis->buffer4[n_bck]-2048) * vertical_scale4_bck/1000.0f + vertical_offset4_bck;

                if( x > 0 )
				{
					// Draw vertical lines for signal traces
					if( vertical_enable1_bck )
					{
						lcd_vline( pLcd, x, MIN(y1,y1_bck), ABS(y1-y1_bck)+1, SCOPE_COLOR_BACKGROUND );
						//lcd_set_pixel(pLcd, x, MIN(y1, y1_bck)-1, SCOPE_COLOR_BACKGROUND);
						//lcd_set_pixel(pLcd, x, MAX(y1, y1_bck)+1, SCOPE_COLOR_BACKGROUND);
					}
					if( vertical_enable2_bck )
					{
						lcd_vline( pLcd, x, MIN(y2,y2_bck), ABS(y2-y2_bck)+1, SCOPE_COLOR_BACKGROUND );
						//lcd_set_pixel(pLcd, x, MIN(y2, y2_bck)-1, SCOPE_COLOR_BACKGROUND);
						//lcd_set_pixel(pLcd, x, MAX(y2, y2_bck)+1, SCOPE_COLOR_BACKGROUND);
					}
					if( vertical_enable3_bck )
					{
						lcd_vline( pLcd, x, MIN(y3,y3_bck), ABS(y3-y3_bck)+1, SCOPE_COLOR_BACKGROUND );
						//lcd_set_pixel(pLcd, x, MIN(y3, y3_bck)-1, SCOPE_COLOR_BACKGROUND);
						//lcd_set_pixel(pLcd, x, MAX(y3, y3_bck)+1, SCOPE_COLOR_BACKGROUND);
					}
					if( vertical_enable4_bck )
					{
						lcd_vline( pLcd, x, MIN(y4,y4_bck), ABS(y4-y4_bck)+1, SCOPE_COLOR_BACKGROUND );
						//lcd_set_pixel(pLcd, x, MIN(y4, y4_bck)-1, SCOPE_COLOR_BACKGROUND);
						//lcd_set_pixel(pLcd, x, MAX(y4, y4_bck)+1, SCOPE_COLOR_BACKGROUND);
					}
				}
                // Update previous values for next iteration
                y1_bck = y1;
                y2_bck = y2;
                y3_bck = y3;
                y4_bck = y4;
            }

            if( i > 2 )
            {
                // Calculate the current sample index for the signal data
                n = trigger + (i-2);// - pThis->horizontal.offset/2;
                n = n % pThis->len;

                // Determine the x-coordinate on the display based on the view mode
                if( is_collapsed )
                {
                    x = (i-2)/2;
                }
                else
                {
                    x = (i-2);
                }
                // Calculate the y-coordinates for signal traces
                y5 = pLcd_height/2 + pLcd_height - (pThis->buffer5[n]-2048) * pThis->vertical.scale1/1000.0f + pThis->vertical.offset1;
                y6 = pLcd_height/2 + pLcd_height - (pThis->buffer6[n]-2048) * pThis->vertical.scale2/1000.0f + pThis->vertical.offset2;
                y7 = pLcd_height/2 + pLcd_height - (pThis->buffer7[n]-2048) * pThis->vertical.scale3/1000.0f + pThis->vertical.offset3;
                y8 = pLcd_height/2 + pLcd_height - (pThis->buffer8[n]-2048) * pThis->vertical.scale4/1000.0f + pThis->vertical.offset4;

                if( x > 0 )
				{
					// Draw vertical lines for signal traces with channel-specific colors
					if( pThis->vertical.enable1 )
					{
						lcd_vline( pLcd, x, MIN(y5,y5_bck), ABS(y5-y5_bck)+1, SCOPE_COLOR_CH1 );
						//lcd_set_pixel(pLcd, x, MIN(y5, y5_bck)-1, SCOPE_COLOR_CH1_LIGHT);
						//lcd_set_pixel(pLcd, x, MAX(y5, y5_bck)+1, SCOPE_COLOR_CH1_LIGHT);
					}
					if( pThis->vertical.enable2 )
					{
						lcd_vline( pLcd, x, MIN(y6,y6_bck), ABS(y6-y6_bck)+1, SCOPE_COLOR_CH2 );
						//lcd_set_pixel(pLcd, x, MIN(y6, y6_bck)-1, SCOPE_COLOR_CH2_LIGHT);
						//lcd_set_pixel(pLcd, x, MAX(y6, y6_bck)+1, SCOPE_COLOR_CH2_LIGHT);
					}
					if( pThis->vertical.enable3 )
					{
						lcd_vline( pLcd, x, MIN(y7,y7_bck), ABS(y7-y7_bck)+1, SCOPE_COLOR_CH3 );
						//lcd_set_pixel(pLcd, x, MIN(y7, y7_bck)-1, SCOPE_COLOR_CH3_LIGHT);
						//lcd_set_pixel(pLcd, x, MAX(y7, y7_bck)+1, SCOPE_COLOR_CH3_LIGHT);
					}
					if( pThis->vertical.enable4 )
					{
						lcd_vline( pLcd, x, MIN(y8,y8_bck), ABS(y8-y8_bck)+1, SCOPE_COLOR_CH4 );
						//lcd_set_pixel(pLcd, x, MIN(y8, y8_bck)-1, SCOPE_COLOR_CH4_LIGHT);
						//lcd_set_pixel(pLcd, x, MAX(y8, y8_bck)+1, SCOPE_COLOR_CH4_LIGHT);
					}
				}
                // Update previous values for next iteration
                y5_bck = y5;
                y6_bck = y6;
                y7_bck = y7;
                y8_bck = y8;
            }
        }
    }

    // Update the background values and trigger position for the next draw
    trigger_bck = trigger;
    is_collapsed_bck = is_collapsed;


    vertical_enable1_bck = pThis->vertical.enable1;
    vertical_enable2_bck = pThis->vertical.enable2;
    vertical_enable3_bck = pThis->vertical.enable3;
    vertical_enable4_bck = pThis->vertical.enable4;

    vertical_offset1_bck = pThis->vertical.offset1;
    vertical_offset2_bck = pThis->vertical.offset2;
    vertical_offset3_bck = pThis->vertical.offset3;
    vertical_offset4_bck = pThis->vertical.offset4;

    vertical_scale1_bck = pThis->vertical.scale1;
    vertical_scale2_bck = pThis->vertical.scale2;
    vertical_scale3_bck = pThis->vertical.scale3;
    vertical_scale4_bck = pThis->vertical.scale4;

}


void StartTaskScope(void *argument)
{
	int i;
	static float buf_sine[ADC_BUFFER_LEN] = {0};

	static uint16_t buf0[ADC_BUFFER_LEN] = {0};
	static uint16_t buf1[ADC_BUFFER_LEN] = {0};
	static uint16_t buf2[ADC_BUFFER_LEN] = {0};
	static uint16_t buf3[ADC_BUFFER_LEN] = {0};

	static uint16_t buf4[ADC_BUFFER_LEN] = {0};
	static uint16_t buf5[ADC_BUFFER_LEN] = {0};
	static uint16_t buf6[ADC_BUFFER_LEN] = {0};
	static uint16_t buf7[ADC_BUFFER_LEN] = {0};

	int gain = 0;
	int step = 0;
	volatile int speed = 100;
	int y0, y1, h;
	int h0, h1, h2, h3;
	int h4, h5, h6, h7;

	//for( i = 0 ; i < 480 ; i++ )
	//{
	//	buf_sine[i] = sinf( 4*(2*M_PIf*i)/480 );
	//}

#define DAC_BUFFER_LEN (512)

// DAC buffer for channel 1 and channel 2
static uint16_t dac1_buffer[DAC_BUFFER_LEN];
static uint16_t dac2_buffer[DAC_BUFFER_LEN];

	tWaveGen wavegen = {0};
    // Initialize wavegen with DAC and timer handles
    wavegen_init_ll(&wavegen, &hdac1, &htim4, &htim6);

    // Initialize wavegen settings for both channels
    wavegen_init(&wavegen, dac1_buffer, dac2_buffer, DAC_BUFFER_LEN);
    wavegen_config_horizontal(&wavegen, WAVEGEN_CHANNEL_1, 2000);
    wavegen_config_vertical(&wavegen, WAVEGEN_CHANNEL_1, WAVEGEN_TYPE_SINE, 2048, 1000, 0);
    wavegen_config_horizontal(&wavegen, WAVEGEN_CHANNEL_2, 1000);
    wavegen_config_vertical(&wavegen, WAVEGEN_CHANNEL_2, WAVEGEN_TYPE_SINE, 2048, 2000, 0);
    wavegen_start(&wavegen, WAVEGEN_CHANNEL_1);
    wavegen_start(&wavegen, WAVEGEN_CHANNEL_2);



    // Initialize hardware components and buffers
    scope_init_ll(&scope,
        &htim2,  // horizontal.htim_clock
        &htim3,  // horizontal.htim_stop
        &hdac2,  // vertical.hdac
        &hopamp1, // vertical.hopamp1
        &hopamp3, // vertical.hopamp2
        &hopamp5, // vertical.hopamp3
        &hopamp6, // vertical.hopamp4
        &hadc1,  // trigger.hadc1
        &hadc3,  // trigger.hadc2
        &hadc5,  // trigger.hadc3
        &hadc4   // trigger.hadc4
    );

    scope_init(&scope,
		buf0,
		buf1,
		buf2,
		buf3,
		buf4,
		buf5,
		buf6,
		buf7,
        ADC_BUFFER_LEN
    );

    scope_config_horizontal(&scope,
    	0,		// offset
		80000 	// scale [KHz]
	);
    scope_config_vertical(&scope,
		2048+60,	// offset
    	0, 		// scale1
		0, 		// scale2
		0, 		// scale3
		0 		// scale4
	);

    scope.vertical.offset1 = 192;
	scope.vertical.offset2 = 192;
	scope.vertical.offset3 = 192;
	scope.vertical.offset4 = 192;

	scope.vertical.enable1 = 1;
	scope.vertical.enable2 = 1;
	scope.vertical.enable3 = 1;
	scope.vertical.enable4 = 1;

	scope.vertical.scale1 = 80;
	scope.vertical.scale2 = 80;
	scope.vertical.scale3 = 80;
	scope.vertical.scale4 = 80;

    scope_config_trigger(&scope,
		0, 							// channel
		1, //UI_TRIGGER_MODE_NORMAL, 	// mode
		2048+768,//3092, 						// level
		SCOPE_TRIGGER_SLOPE_RISING	// slope
	);
    volatile int ddd = 10;

	while(1)
	{
		int msg_get = 0;
		//if( run )
		{
			tQueueUiScope msg = {0};
			if( osMessageQueueGet(queueUiScopeHandle, &msg, NULL, 0U) == osOK)
			{
				msg_get = 1;
				if( osSemaphoreAcquire( semLcdHandle, portMAX_DELAY ) == osOK )
				{
					if( msg_get )
					{

								scope_erase(&scope, &lcd,
												1,
												1,
												1,
												1,
												(menu==0)?0:1);

					}
					osSemaphoreRelease( semLcdHandle );
				}
				if( msg.type == 1)
				{
					scope_config_horizontal( &scope, msg.data[0], scope.horizontal.scale );
				}
				else if( msg.type == 2 )
				{
					scope_config_horizontal( &scope, scope.horizontal.offset, msg.data[0] );
				}

				if( osSemaphoreAcquire( semLcdHandle, portMAX_DELAY ) == osOK )
				{
					if( msg_get )
					{

							scope_draw(&scope, &lcd,
											1,
											1,
											1,
											1,
											(menu==0)?0:1);

					}
					osSemaphoreRelease( semLcdHandle );
				}
			}
			if( run )
			{
				scope_start( &scope, 0 );
				int timeout_ms = (1000*480*5)/(scope.horizontal.scale);
				scope_wait( &scope, timeout_ms );
				scope_stop( &scope );
			}
		}
		if( osSemaphoreAcquire( semLcdHandle, portMAX_DELAY ) == osOK )
		{

			if( ccc < 10 )
			{
				ccc += 1;
			}
			else
			{
				scope_draw_grid(&scope, &lcd, (menu==0)?0:1);
				ccc = 0;
			}
			scope_draw_signals(&scope, &lcd, (menu==0)?0:1);


				scope_draw(&scope, &lcd,
					1,
					1,
					1,
					1,
					(menu==0)?0:1);
				ccc2 = 0;

			osSemaphoreRelease( semLcdHandle );
		}
		if( run )
		{
	        osDelay(ddd);
		}
		else
		{
			osDelay(50);
		}
        continue;
		/*gain = 32*ABS( sinf( (2*M_PIf*step)/speed ) );
		if( run )
		{
			step = (step+1)%speed;
		}

		for( i = 0 ; i < 480 ; i++ )
		{
			buf0[i] = 64*1+roundf( gain*buf_sine[i] );
			buf1[i] = 64*2+roundf( gain*buf_sine[i] );
			buf2[i] = 64*3+roundf( gain*buf_sine[i] );
			buf3[i] = 64*4+roundf( gain*buf_sine[i] );
		}*/

		if( osSemaphoreAcquire( semLcdHandle, portMAX_DELAY ) == osOK )
		{
			for( i = 4 ; i < 480-4 ; i++ )
			{
				int x = (menu==0)?i:i/2;
				/*y0 = buf4[i-1]; y1 = buf4[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );
				y0 = buf5[i-1]; y1 = buf5[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );
				y0 = buf6[i-1]; y1 = buf6[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );
				y0 = buf7[i-1]; y1 = buf7[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );

				y0 = buf0[i-1]; y1 = buf0[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_RED );
				y0 = buf1[i-1]; y1 = buf1[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_GREEN );
				y0 = buf2[i-1]; y1 = buf2[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_BLUE );
				y0 = buf3[i-1]; y1 = buf3[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_PURPLE );*/

				y0 = buf4[i-1]; y1 = buf4[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = ((MIN(y0,y1)*320)/4096*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );
				y0 = buf5[i-1]; y1 = buf5[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );
				y0 = buf6[i-1]; y1 = buf6[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );
				y0 = buf7[i-1]; y1 = buf7[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );

				y0 = buf4[i-1]; y1 = buf4[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_WHITE );
				y0 = buf5[i-1]; y1 = buf5[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_WHITE );
				y0 = buf6[i-1]; y1 = buf6[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_WHITE );
				y0 = buf7[i-1]; y1 = buf7[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_WHITE );

				y0 = buf4[i-1]; y1 = buf4[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_WHITE );
				y0 = buf5[i-1]; y1 = buf5[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_WHITE );
				y0 = buf6[i-1]; y1 = buf6[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_WHITE );
				y0 = buf7[i-1]; y1 = buf7[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_WHITE );



				y0 = buf0[i-1]; y1 = buf0[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_RED2 );
				y0 = buf1[i-1]; y1 = buf1[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_GREEN2 );
				y0 = buf2[i-1]; y1 = buf2[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_BLUE2 );
				y0 = buf3[i-1]; y1 = buf3[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_PURPLE2 );

				y0 = buf0[i-1]; y1 = buf0[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_RED2 );
				y0 = buf1[i-1]; y1 = buf1[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_GREEN2 );
				y0 = buf2[i-1]; y1 = buf2[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_BLUE2 );
				y0 = buf3[i-1]; y1 = buf3[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_PURPLE2 );


				y0 = buf0[i-1]; y1 = buf0[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_RED );
				y0 = buf1[i-1]; y1 = buf1[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_GREEN );
				y0 = buf2[i-1]; y1 = buf2[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_BLUE );
				y0 = buf3[i-1]; y1 = buf3[i]; h = ABS(y1-y0); h = MAX((h*320)/4096,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_PURPLE );
			}
			osSemaphoreRelease( semLcdHandle );
		}
		osDelay(20);
		continue;
		gain = 32*ABS(sinf( (2*M_PIf*step)/speed ) );
		if( run )
		{
			step = (step+1)%speed;
		}

		for( i = 0 ; i < 480 ; i++ )
		{
			buf4[i] = 64*1+roundf( gain*buf_sine[i] );
			buf5[i] = 64*2+roundf( gain*buf_sine[i] );
			buf6[i] = 64*3+roundf( gain*buf_sine[i] );
			buf7[i] = 64*4+roundf( gain*buf_sine[i] );
		}

		if( osSemaphoreAcquire( semLcdHandle, portMAX_DELAY ) == osOK )
		{
			for( i = 4 ; i < 480-4 ; i++ )
			{
				int x = (menu==0)?i:i/2;
				/*
				y0 = buf0[i-1]; y1 = buf0[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );
				y0 = buf1[i-1]; y1 = buf1[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );
				y0 = buf2[i-1]; y1 = buf2[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );
				y0 = buf3[i-1]; y1 = buf3[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );

				y0 = buf4[i-1]; y1 = buf4[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_RED );
				y0 = buf5[i-1]; y1 = buf5[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_GREEN );
				y0 = buf6[i-1]; y1 = buf6[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_BLUE );
				y0 = buf7[i-1]; y1 = buf7[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_PURPLE );
				*/
				y0 = buf0[i-1]; y1 = buf0[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );
				y0 = buf1[i-1]; y1 = buf1[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );
				y0 = buf2[i-1]; y1 = buf2[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );
				y0 = buf3[i-1]; y1 = buf3[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_WHITE );


				y0 = buf0[i-1]; y1 = buf0[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_WHITE );
				y0 = buf1[i-1]; y1 = buf1[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_WHITE );
				y0 = buf2[i-1]; y1 = buf2[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_WHITE );
				y0 = buf3[i-1]; y1 = buf3[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_WHITE );

				y0 = buf0[i-1]; y1 = buf0[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_WHITE );
				y0 = buf1[i-1]; y1 = buf1[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_WHITE );
				y0 = buf2[i-1]; y1 = buf2[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_WHITE );
				y0 = buf3[i-1]; y1 = buf3[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_WHITE );


				y0 = buf4[i-1]; y1 = buf4[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_RED2 );
				y0 = buf5[i-1]; y1 = buf5[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_GREEN2 );
				y0 = buf6[i-1]; y1 = buf6[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_BLUE2 );
				y0 = buf7[i-1]; y1 = buf7[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0+1, h, COLOR_PALETTE_PURPLE2 );

				y0 = buf4[i-1]; y1 = buf4[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_RED2 );
				y0 = buf5[i-1]; y1 = buf5[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_GREEN2 );
				y0 = buf6[i-1]; y1 = buf6[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_BLUE2 );
				y0 = buf7[i-1]; y1 = buf7[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0-1, h, COLOR_PALETTE_PURPLE2 );


				y0 = buf4[i-1]; y1 = buf4[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_RED );
				y0 = buf5[i-1]; y1 = buf5[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_GREEN );
				y0 = buf6[i-1]; y1 = buf6[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_BLUE );
				y0 = buf7[i-1]; y1 = buf7[i]; h = ABS(y1-y0); h = MAX(h,1); y0 = (MIN(y0,y1)*320)/4096;
				lcd_vline( &lcd, x, y0, h, COLOR_PALETTE_PURPLE );
			}
			osSemaphoreRelease( semLcdHandle );
		}
		osDelay(20);
	}
}
/* USER CODE END Application */

