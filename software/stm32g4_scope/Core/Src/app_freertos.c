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
  .stack_size = 2048 * 4
};
/* Definitions for taskTsc */
osThreadId_t taskTscHandle;
const osThreadAttr_t taskTsc_attributes = {
  .name = "taskTsc",
  .priority = (osPriority_t) osPriorityNormal1,
  .stack_size = 256 * 4
};

/* Definitions for taskUI */
osThreadId_t taskUiHandle;
const osThreadAttr_t taskUi_attributes = {
  .name = "taskUI",
  .priority = (osPriority_t) osPriorityNormal1,
  .stack_size = 1024 * 4
};

osThreadId_t taskScopeHandle;
const osThreadAttr_t taskScope_attributes = {
  .name = "taskScope",
  .priority = (osPriority_t) osPriorityNormal4,
  .stack_size = 128
};
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
struct sQueueTscUi {
    uint16_t p;
    uint16_t x;
    uint16_t y;
};
osMessageQueueId_t queueTscUiHandle;
const osMessageQueueAttr_t queueTscUi_attributes = {
  .name = "queueTscUi"
};

struct sQueueUiLcd {
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
    uint16_t *buf;
};

osMessageQueueId_t queueUiLcdHandle;
const osMessageQueueAttr_t queueUiLcd_attributes = {
  .name = "queueUiLcd"
};

// declare and initialize the semaphoreLcdHandle
osSemaphoreId_t semaphoreLcdHandle;
const osSemaphoreAttr_t semaphoreLcd_attributes = {
  .name = "semaphoreLcd"
};

struct sQueueUiScope {
    uint16_t type;
    uint16_t data[8];
};

osMessageQueueId_t queueUiScopeHandle;
const osMessageQueueAttr_t queueUiScope_attributes = {
  .name = "queueUiScope"
};

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTaskTsc(void *argument);
void StartTaskUi(void *argument);
void StartTaskScope(void *argument);

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
	semaphoreLcdHandle = osSemaphoreNew(1, 1, &semaphoreLcd_attributes);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	queueTscUiHandle = osMessageQueueNew (4, sizeof(struct sQueueTscUi), &queueTscUi_attributes );
	//queueUiLcdHandle = osMessageQueueNew (1, sizeof(struct sQueueUiLcd), &queueUiLcd_attributes );
	queueUiScopeHandle = osMessageQueueNew (1, sizeof(struct sQueueUiScope), &queueUiScope_attributes );
 /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  //defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of taskTsc */
  taskTscHandle = osThreadNew(StartTaskTsc, NULL, &taskTsc_attributes);
  taskUiHandle = osThreadNew(StartTaskUi, NULL, &taskUi_attributes);
  taskScopeHandle = osThreadNew(StartTaskScope, NULL, &taskScope_attributes);

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

#include "tim.h"
#include "adc.h"
#include "dac.h"
#include "opamp.h"
#include "spi.h"

#include "Lcd.h"
#include "Tsc.h"
#include "Scope.h"
#include "tools.h"
#include "framebuf.h"
#include "FontUbuntuBookRNormal16.h"
#include "psram.h"
#include "wavegen.h"
#include "ui.h"


#include "nuklear.h"

#define ADC_BUFFER_LEN 	(512)
#define DAC_BUFFER_LEN 	(512)
#define FB_WIDTH 	(240)
#define FB_HEIGHT	(80)
#define FB2_WIDTH 	(160-1)
#define FB2_HEIGHT	(16)

uint16_t dac1_buffer[DAC_BUFFER_LEN];
uint16_t dac2_buffer[DAC_BUFFER_LEN];

uint16_t buffer1[ADC_BUFFER_LEN];
uint16_t buffer2[ADC_BUFFER_LEN];
uint16_t buffer3[ADC_BUFFER_LEN];
uint16_t buffer4[ADC_BUFFER_LEN];
uint16_t buffer5[ADC_BUFFER_LEN];
uint16_t buffer6[ADC_BUFFER_LEN];
uint16_t buffer7[ADC_BUFFER_LEN];
uint16_t buffer8[ADC_BUFFER_LEN];

uint16_t buffer_tmp[ADC_BUFFER_LEN];

uint16_t fb_buf[FB_WIDTH*FB_HEIGHT];
uint16_t fb2_buf[FB2_WIDTH*FB2_HEIGHT];

#define NK_BUFFER_CMDS_LEN 	(1024*6)
#define NK_BUFFER_POOL_LEN 	(1024*6)

uint8_t nk_buf_cmds[NK_BUFFER_CMDS_LEN] = {0};
uint8_t nk_buf_pool[NK_BUFFER_POOL_LEN] = {0};

int _write(int file, char *ptr, int len)
{
	int DataIdx;
	for(DataIdx = 0; DataIdx < len; DataIdx++)
	{
		ITM_SendChar(*ptr++);
	}
    return len;
}

void lcd_draw_cross( tLcd *pLcd, uint16_t x, uint16_t y, uint16_t color )
{
	lcd_rect( pLcd, x-4, y, 4, 2, color );
	lcd_rect( pLcd, x+2, y, 4, 2, color );
	lcd_rect( pLcd, x, y-4, 2, 4, color );
	lcd_rect( pLcd, x, y+2, 2, 4, color );
}

float text_width_f( nk_handle handle, float h, const char* text, int len )
{
	return font_text_width( &fontUbuntuBookRNormal16, text );
}

extern void nk_draw_fb(struct nk_context *ctx, tFramebuf *pfb, tLcd *pLcd, int16_t x0 );
extern void nk_draw_fb2(struct nk_context *ctx, tFramebuf *pfb, int16_t x0, int16_t y0 );
extern void nk_set_theme(struct nk_context *ctx, int theme);

#define MIN(a,b) ((a)<(b))?(a):(b)
#define MAX(a,b) ((a)>(b))?(a):(b)
#define ABS(a) ((a)>(0))?(a):-(a)


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

	int _ya = 0;
	int _yb = 0;
	int _yc = 0;
	int _yd = 0;
	int _ya2 = 0;
	int _yb2 = 0;
	int _yc2 = 0;
	int _yd2 = 0;

	uint16_t line_width = 1;
	uint16_t line_height = 1;

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

		x0 = collapsed? j : j/2;

		ya = height-((a_b?buffer5[n2]:buffer1[n2])*height)/4096;
		yb = height-((a_b?buffer6[n2]:buffer2[n2])*height)/4096;
		yc = height-((a_b?buffer7[n2]:buffer3[n2])*height)/4096;
		yd = height-((a_b?buffer8[n2]:buffer4[n2])*height)/4096;

		if( j > 0 )
		{
			lcd_rect( pLcd, x0+1, MIN(ya-1,_ya-1), line_width, MAX(ABS((ya-1)-(_ya-1)), line_height), 0x0000 );
			lcd_rect( pLcd, x0+1, MIN(yb-1,_yb-1), line_width, MAX(ABS((yb-1)-(_yb-1)), line_height), 0x0000 );
			lcd_rect( pLcd, x0+1, MIN(yc-1,_yc-1), line_width, MAX(ABS((yc-1)-(_yc-1)), line_height), 0x0000 );
			lcd_rect( pLcd, x0+1, MIN(yd-1,_yd-1), line_width, MAX(ABS((yd-1)-(_yd-1)), line_height), 0x0000 );
		}
		_ya = ya;
		_yb = yb;
		_yc = yc;
		_yd = yd;

		int jj = j - 4;
		if( j > 4 )
		{
			j2 = (jj*len)/width;
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

			x0 = collapsed? jj : jj/2;
			ya = height-((a_b?buffer1[n]:buffer5[n])*height)/4096;
			yb = height-((a_b?buffer2[n]:buffer6[n])*height)/4096;
			yc = height-((a_b?buffer3[n]:buffer7[n])*height)/4096;
			yd = height-((a_b?buffer4[n]:buffer8[n])*height)/4096;

			if( j > 5 )
			{
				lcd_rect( pLcd, x0+1, MIN(ya-1,_ya2-1), line_width, MAX(ABS((ya-1)-(_ya2-1)), line_height), 0x001F );
				lcd_rect( pLcd, x0+1, MIN(yb-1,_yb2-1), line_width, MAX(ABS((yb-1)-(_yb2-1)), line_height), 0x07E0 );
				lcd_rect( pLcd, x0+1, MIN(yc-1,_yc2-1), line_width, MAX(ABS((yc-1)-(_yc2-1)), line_height), 0xF800 );
				lcd_rect( pLcd, x0+1, MIN(yd-1,_yd2-1), line_width, MAX(ABS((yd-1)-(_yd2-1)), line_height), 0xF81F );
			}
			_ya2 = ya;
			_yb2 = yb;
			_yc2 = yc;
			_yd2 = yd;

		}

		buffer_tmp[j] = a_b?buffer5[n]:buffer1[n];
	}
}


uint16_t magnitude_to_rgb565( uint16_t value );
uint16_t rgb_to_rgb565( uint16_t r, uint16_t g, uint16_t b );
void hsv_to_rgb( uint16_t h, uint16_t s, uint16_t v, uint16_t *r, uint16_t *g, uint16_t *b );

void draw_buffers_fft(
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
		uint8_t a_b );
void fft( uint16_t *buffer, uint16_t *tmp, uint16_t len );

static const float cos_table[256] = {
	1.0, 0.999698, 0.998795, 0.99729, 0.995185, 0.99248, 0.989177, 0.985278,
	0.980785, 0.975702, 0.970031, 0.963776, 0.95694, 0.949528, 0.941544, 0.932993,
	0.92388, 0.91421, 0.903989, 0.893224, 0.881921, 0.870087, 0.857729, 0.844854,
	0.83147, 0.817585, 0.803208, 0.788346, 0.77301, 0.757209, 0.740951, 0.724247,
	0.707107, 0.689541, 0.671559, 0.653173, 0.634393, 0.615232, 0.595699, 0.575808,
	0.55557, 0.534998, 0.514103, 0.492898, 0.471397, 0.449611, 0.427555, 0.405241,
	0.382683, 0.359895, 0.33689, 0.313682, 0.290285, 0.266713, 0.24298, 0.219101,
	0.19509, 0.170962, 0.14673, 0.122411, 0.0980171, 0.0735646, 0.0490677, 0.0245412,
	6.12323e-17, -0.0245412, -0.0490677, -0.0735646, -0.0980171, -0.122411, -0.14673, -0.170962,
	-0.19509, -0.219101, -0.24298, -0.266713, -0.290285, -0.313682, -0.33689, -0.359895,
	-0.382683, -0.405241, -0.427555, -0.449611, -0.471397, -0.492898, -0.514103, -0.534998,
	-0.55557, -0.575808, -0.595699, -0.615232, -0.634393, -0.653173, -0.671559, -0.689541,
	-0.707107, -0.724247, -0.740951, -0.757209, -0.77301, -0.788346, -0.803208, -0.817585,
	-0.83147, -0.844854, -0.857729, -0.870087, -0.881921, -0.893224, -0.903989, -0.91421,
	-0.92388, -0.932993, -0.941544, -0.949528, -0.95694, -0.963776, -0.970031, -0.975702,
	-0.980785, -0.985278, -0.989177, -0.99248, -0.995185, -0.99729, -0.998795, -0.999698,
	-1.0, -0.999698, -0.998795, -0.99729, -0.995185, -0.99248, -0.989177, -0.985278,
	-0.980785, -0.975702, -0.970031, -0.963776, -0.95694, -0.949528, -0.941544, -0.932993,
	-0.92388, -0.91421, -0.903989, -0.893224, -0.881921, -0.870087, -0.857729, -0.844854,
	-0.83147, -0.817585, -0.803208, -0.788346, -0.77301, -0.757209, -0.740951, -0.724247,
	-0.707107, -0.689541, -0.671559, -0.653173, -0.634393, -0.615232, -0.595699, -0.575808,
	-0.55557, -0.534998, -0.514103, -0.492898, -0.471397, -0.449611, -0.427555, -0.405241,
	-0.382683, -0.359895, -0.33689, -0.313682, -0.290285, -0.266713, -0.24298, -0.219101,
	-0.19509, -0.170962, -0.14673, -0.122411, -0.0980171, -0.0735646, -0.0490677, -0.0245412,
	-1.83697e-16, 0.0245412, 0.0490677, 0.0735646, 0.0980171, 0.122411, 0.14673, 0.170962,
	0.19509, 0.219101, 0.24298, 0.266713, 0.290285, 0.313682, 0.33689, 0.359895,
	0.382683, 0.405241, 0.427555, 0.449611, 0.471397, 0.492898, 0.514103, 0.534998,
	0.55557, 0.575808, 0.595699, 0.615232, 0.634393, 0.653173, 0.671559, 0.689541,
	0.707107, 0.724247, 0.740951, 0.757209, 0.77301, 0.788346, 0.803208, 0.817585,
	0.83147, 0.844854, 0.857729, 0.870087, 0.881921, 0.893224, 0.903989, 0.91421,
	0.92388, 0.932993, 0.941544, 0.949528, 0.95694, 0.963776, 0.970031, 0.975702,
	0.980785, 0.985278, 0.989177, 0.99248, 0.995185, 0.99729, 0.998795, 0.999698
};

#define MAX_N_LOG2 8
#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
void fft( uint16_t *buffer, uint16_t *tmp, uint16_t len )
{
	// Compute the FFT of the input signal.
	int i;
	for( i = 0 ; i < len ; i++ )
	{
		tmp[i] = buffer[i];
	}
#ifdef USE_CMSIS_DSP
	arm_cfft_radix4_instance_q15 S;
	arm_cfft_radix4_init_q15( &S, len, 0, 1 );
	arm_cfft_radix4_q15( &S, tmp );
	arm_cmplx_mag_q15( tmp, buffer, len );
#else
	// ANSI C implementation
	int n, k, j, N, N2;
	float theta, wr, wi, wpr, wpi, wtemp, tempr, tempi;

	// Reverse-binary reindexing
	N = len<<1;
	N2 = N>>1;
	j = 1;

	for( i = 1 ; i < N ; i += 2 )
	{
		if( j > i )
		{
			SWAP( tmp[j-1], tmp[i-1] );
			SWAP( tmp[j], tmp[i] );
		}
		k = N2;
		while( k < j )
		{
			j -= k;
			k >>= 1;
		}
		j += k;
	}

	// Compute the FFT
	float c1 = -1.0;
	float c2 = 0.0;
	int l2, l1, l, i1, i2;
	float t1, t2, u1, u2, z;
	
	l2 = 1;
	l1 = 0;
	l = 0;
	i1 = 0;
	i2 = 0;
	t1 = 0.0;
	t2 = 0.0;
	u1 = 0.0;
	u2 = 0.0;
	z = 0.0;
	for( l = 0 ; l < MAX_N_LOG2 ; l++ )
	{
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0;
		u2 = 0.0;
		for( j = 0 ; j < l1 ; j++ )
		{
			for( i = j ; i < N ; i += l2 )
			{
				i1 = i + l1;
				t1 = u1 * tmp[i1-1] - u2 * tmp[i1];
				t2 = u1 * tmp[i1] + u2 * tmp[i1-1];
				tmp[i1-1] = tmp[i-1] - t1;
				tmp[i1] = tmp[i] - t2;
				tmp[i-1] += t1;
				tmp[i] += t2;
			}
			z = u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}
		c2 = sqrtf((1.0 - c1) / 2.0);
		if( l2 != 1 )
		{
			c1 = sqrtf((1.0 + c1) / 2.0);
		}
	}

	// Compute the magnitude
	for( i = 0 ; i < len ; i++ )
	{
		buffer[i] = sqrtf( tmp[i*2]*tmp[i*2] + tmp[i*2+1]*tmp[i*2+1] );
	}
#endif
}
void draw_buffers_fft(
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

	int _ya = 0;
	int _yb = 0;
	int _yc = 0;
	int _yd = 0;

	static int y = 0;
	int h = pLcd->height/4;
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

		x0 = collapsed? j : j/2;

		ya = height-((a_b?buffer5[n2]:buffer1[n2])*height)/4096;
		yb = height-((a_b?buffer6[n2]:buffer2[n2])*height)/4096;
		yc = height-((a_b?buffer7[n2]:buffer3[n2])*height)/4096;
		yd = height-((a_b?buffer8[n2]:buffer4[n2])*height)/4096;

		lcd_set_pixel( pLcd, x0, y+h*0, magnitude_to_rgb565( ya ) );
		lcd_set_pixel( pLcd, x0, y+h*1, magnitude_to_rgb565( yb ) );
		lcd_set_pixel( pLcd, x0, y+h*2, magnitude_to_rgb565( yc ) );
		lcd_set_pixel( pLcd, x0, y+h*3, magnitude_to_rgb565( yd ) );
	}
	y = (y+1)%h;
}

void _draw_buffers_fft(
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

	int _ya = 0;
	int _yb = 0;
	int _yc = 0;
	int _yd = 0;
	int _ya2 = 0;
	int _yb2 = 0;
	int _yc2 = 0;
	int _yd2 = 0;

	static uint16_t buffer_fft_temp[ADC_BUFFER_LEN];

	uint16_t line_width = 1;
	uint16_t line_height = 1;
	fft( buffer1, buffer_fft_temp, len );
	fft( buffer2, buffer_fft_temp, len );
	fft( buffer3, buffer_fft_temp, len );
	fft( buffer4, buffer_fft_temp, len );

	// Draw 4 waretfalls in the screen.
	static int y = 0; // This counts the last drawn line.
	int h = pLcd->height/4; // This is the height of each waterfall.
	int x;
	for( x = 0 ; x < len ; x++ ){
		lcd_set_pixel( pLcd, buffer1[x], y+h*0, magnitude_to_rgb565( buffer1[x] ) );
		lcd_set_pixel( pLcd,  buffer2[x], y+h*1, magnitude_to_rgb565( buffer2[x] ) );
		lcd_set_pixel( pLcd,  buffer3[x], y+h*2, magnitude_to_rgb565( buffer3[x] ) );
		lcd_set_pixel( pLcd,  buffer4[x], y+h*3, magnitude_to_rgb565( buffer4[x] ) );
	}
	y = (y+1)%h;
}

void hsv_to_rgb( uint16_t h, uint16_t s, uint16_t v, uint16_t *r, uint16_t *g, uint16_t *b )
{
	uint16_t i;
	uint16_t f, p, q, t;
	if( s == 0 )
	{
		*r = *g = *b = v;
		return;
	}
	h = (h*6)/60;
	i = h;
	f = h - i;
	p = (v*(100-s))/100;
	q = (v*(100-(s*f)/100))/100;
	t = (v*(100-(s*(100-f))/100))/100;
	switch( i )
	{
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}
uint16_t rgb_to_rgb565( uint16_t r, uint16_t g, uint16_t b )
{
	uint16_t rgb565;
	rgb565 = (r&0xF8)<<8;
	rgb565 |= (g&0xFC)<<3;
	rgb565 |= (b&0xF8)>>3;
	return rgb565;
}

uint16_t magnitude_to_rgb565( uint16_t value )
{
	// Convert magnitude to HSV color, then to RGB then to RGB565.
	uint16_t h, s, v;
	uint16_t r, g, b;
	uint16_t rgb565;

	h = value*360/4096;
	s = 100;
	v = 100;

	hsv_to_rgb( h, s, v, &r, &g, &b );
	rgb565 = rgb_to_rgb565( r, g, b );
	return rgb565;
}


void draw_horizontal_offset( tLcd *pLcd, int32_t offset, uint32_t collapsed )
{
	static int last_offset = 0;
	offset = (offset*480)/512;
	if( collapsed )
	{
		lcd_rect( pLcd, last_offset/2+pLcd->width/2, 0, 1, pLcd->height, 0x0000 );
		lcd_rect( pLcd, offset/2+pLcd->width/2, 0, 1, pLcd->height, 0x07FF );
	}
	else
	{
		lcd_rect( pLcd, last_offset/4+pLcd->width/4, 0, 1, pLcd->height, 0x0000 );
		lcd_rect( pLcd, offset/4+pLcd->width/4, 0, 1, pLcd->height, 0x07FF );
	}
	last_offset = offset;
}

void draw_trigger( tLcd *pLcd, int32_t level, uint32_t collapsed )
{
	static int last_level = 0;
	if( collapsed )
	{ 
		lcd_rect( pLcd, 0, pLcd->height-((last_level)*pLcd->height)/4096, pLcd->width, 1, 0x0000 );
		lcd_rect( pLcd, 0, pLcd->height-((level)*pLcd->height)/4096, pLcd->width, 1, 0x07FF );
	}
	else
	{
		lcd_rect( pLcd, 0, pLcd->height-((last_level)*pLcd->height)/4096, pLcd->width/2, 1, 0x0000 );
		lcd_rect( pLcd, 0, pLcd->height-((level)*pLcd->height)/4096, pLcd->width/2, 1, 0x07FF );
	}
	last_level = level;
}

void draw_grid( tLcd *pLcd, uint32_t collapsed )
{
	static int i = 0;
	if( i < 10 )
	{
		i++;
		return;
	}
	else
	{
		i = 0;
	}
	if( collapsed )
	{
		for( int d = 0 ; d < pLcd->width ; d += 40 )
		{
			if( d > 480-40*6 )
			{
				lcd_rect( pLcd, d, 40, 1, pLcd->height-40, 0x8410 );
			}
			else
			{
				lcd_rect( pLcd, d, 0, 1, pLcd->height, 0x8410 );
			}
		}
		lcd_rect( pLcd, pLcd->width, 40, 1, pLcd->height-40, 0x8410 );

		for( int d = 0 ; d < pLcd->height ; d += 40 )
		{
			if( d == 0 )
			{
				lcd_rect( pLcd, 0, d, pLcd->width-40*6, 1, 0x8410 );
			}
			else
			{
				lcd_rect( pLcd, 0, d, pLcd->width, 1, 0x8410 );
			}
		}
		lcd_rect( pLcd, 0, pLcd->height, pLcd->width, 1, 0x8410 );
	}
	else
	{
		for( int d = 0 ; d < pLcd->width ; d += 40 )
		{
			lcd_rect( pLcd, d/2, 0, 1, pLcd->height, 0x8410 );
		}
		lcd_rect( pLcd, pLcd->width/2-1, 0, 1, pLcd->height, 0x8410 );

		for( int d = 0 ; d < pLcd->height ; d += 40 )
		{
			lcd_rect( pLcd, 0/2, d, pLcd->width/2, 1, 0x8410 );
		}
		lcd_rect( pLcd, 0/2, pLcd->height, pLcd->width/2, 1, 0x8410 );
	}

}


void draw_measurements( tLcd *pLcd, int collapsed )
{
	tFramebuf fb;
	uint16_t min, max, avg, period, duty;
	char buffer[32];

	min = get_vmin( buffer_tmp, 480 );
	max = get_vmax( buffer_tmp, 480 );
	avg = get_vavg( buffer_tmp, 480 );
	period = get_period( buffer_tmp, 480, max, min, avg );
	duty = get_duty( buffer_tmp, 480, max, min, avg );

	framebuf_init( &fb, FB2_WIDTH, FB2_HEIGHT, fb2_buf );
	framebuf_fill( &fb, 0x0000 );

	sprintf( buffer, "%04d %04d %04d %04d", min, max, period, duty );
	framebuf_text( &fb, &fontUbuntuBookRNormal16, 0, 0, buffer, 0xFFFF );
	if( collapsed )
	{
		lcd_bmp( pLcd, 0+40+1, 1, fb.width, fb.height, fb.buf );
	}
	else
	{
		lcd_bmp( pLcd, 0+40+1, 1, fb.width, fb.height, fb.buf );
	}
}

/*
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
*/
void draw_cursors( tLcd *pLcd, tUi_Cursors cursors )
{
	uint16_t color1 = 0xF00F;
	uint16_t color2 = 0x0FF0;

	if( cursors.is_visible )
	{
		if( cursors.cursors[0].enabled )
		{
			if( cursors.cursors[0].horizontal )
			{
				if( cursors.cursors[0].track )
				{
					lcd_rect( pLcd, cursors.cursors[0].offset/2+pLcd->width/2, 0, 1, pLcd->height, color1 );
				}
				else
				{
					lcd_rect( pLcd, cursors.cursors[0].offset/2+pLcd->width/2, 0, 1, pLcd->height, color2 );
				}
			}
			else
			{
				if( cursors.cursors[0].track )
				{
					lcd_rect( pLcd, 0, pLcd->height-((cursors.cursors[0].offset)*pLcd->height)/4096, pLcd->width/2, 1, color1 );
				}
				else
				{
					lcd_rect( pLcd, 0, pLcd->height-((cursors.cursors[0].offset)*pLcd->height)/4096, pLcd->width/2, 1, color2 );
				}
			}
		}
		if( cursors.cursors[1].enabled )
		{
			if( cursors.cursors[1].horizontal )
			{
				if( cursors.cursors[1].track )
				{
					lcd_rect( pLcd, cursors.cursors[1].offset/2+pLcd->width/2, 0, 1, pLcd->height, color1 );
				}
				else
				{
					lcd_rect( pLcd, cursors.cursors[1].offset/2+pLcd->width/2, 0, 1, pLcd->height, color2 );
				}
			}
			else
			{
				if( cursors.cursors[1].track )
				{
					lcd_rect( pLcd, 0, pLcd->height-((cursors.cursors[1].offset)*pLcd->height)/4096, pLcd->width/2, 1, color1 );
				}
				else
				{
					lcd_rect( pLcd, 0, pLcd->height-((cursors.cursors[1].offset)*pLcd->height)/4096, pLcd->width/2, 1, color2 );
				}
			}
		}
	}
}

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
uint16_t x_tsc = 0;
uint16_t y_tsc = 0;
uint16_t p_tsc = 0;

struct nk_context ctx = {0};
tWaveGen wavegen;
tScope scope = {0};

/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	tLcd lcd = {0};
	//tTsc tsc = {0};
	tFramebuf fb = {0};

	//struct nk_context ctx = {0};
	struct nk_buffer cmds = {0};
	struct nk_buffer pool = {0};
	struct nk_user_font font = {0};

	//struct Oscilloscope osc = {0};
	

	tUi ui = {0};


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

	framebuf_init( &fb, FB_WIDTH, FB_HEIGHT, fb_buf );

	nk_buffer_init_fixed( &cmds, nk_buf_cmds, NK_BUFFER_CMDS_LEN );
	nk_buffer_init_fixed( &pool, nk_buf_pool, NK_BUFFER_POOL_LEN);

	font.height = fontUbuntuBookRNormal16.bbxh;
	font.width = text_width_f;
	nk_init_custom( &ctx, &cmds, &pool, &font );
	nk_set_theme( &ctx, THEME_DARK );

	//for( i = 0 ; i < BUFFER_LEN ; i++ )
	//{
	//	dac1_buffer[i] = sinf(2*dac_freq*2*M_PI*i/BUFFER_LEN)*1023 + 2048;
	//	dac2_buffer[i] = sinf(dac_freq*2*M_PI*i/BUFFER_LEN)*1023 + 2048;
	//}

	__HAL_DBGMCU_FREEZE_TIM4();
	__HAL_DBGMCU_FREEZE_TIM6();
	wavegen_init( &wavegen,
		&hdac1,
		&htim4,
		&htim6,
		dac1_buffer,
		dac2_buffer,
		512,
		1e6 );
    wavegen_build_sine( &wavegen, 0x01, 10e3, 2047, 1500 );
    wavegen_build_sine( &wavegen, 0x02, 20e3, 2047, 1500 );
    wavegen_start( &wavegen, 0x03 );
    //HAL_Delay( 1000 );
    //wavegen_stop( &wavegen, 0x03 );

	//osc.acquire_run = 1;
    //osc.channels[0].offset = 2048;
    //osc.channels[1].offset = 2048;
    //osc.channels[2].offset = 2048;
    //osc.channels[3].offset = 2048;
    //HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_12B_R, osc.channels[0].offset);
    //HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);

	//scope.horizontal.htim_clock
    //osc.horizontal_offset = 0;
    //osc.horizontal_scale = 1000;
    //osc.waveforms[0].type = 1;
    //osc.waveforms[0].offset = 2000;
    //osc.waveforms[0].scale = 1000;

	scope.horizontal.htim_clock = &htim2;
	scope.horizontal.htim_stop =  &htim3;
	scope.vertical.hdac = &hdac2;
	scope.vertical.hopamp1 = &hopamp1;
	scope.vertical.hopamp2 = &hopamp3;
	scope.vertical.hopamp3 = &hopamp5;
	scope.vertical.hopamp4 = &hopamp6;
	scope.trigger.hadc1 = &hadc1;
	scope.trigger.hadc2 = &hadc3;
	scope.trigger.hadc3 = &hadc5;
	scope.trigger.hadc4 = &hadc4;
	scope.wavegen.htim1 = &htim4;
	scope.wavegen.htim2 = &htim6;

    ui.acquire.run = 1;
    ui.acquire.single = 0;

    ui.horizontal.offset = 0;
	ui.horizontal.scale = 1000;
	scope_config_horizontal( &scope, ui.horizontal.scale*1000, 512 );

	ui.vertical.offset = 2048;
	ui.vertical.channels[0].enabled = 1;
	ui.vertical.channels[0].coupling = 0;
	ui.vertical.channels[0].scale = 0;
	ui.vertical.channels[1].enabled = 1;
	ui.vertical.channels[1].coupling = 0;
	ui.vertical.channels[1].scale = 0;
	ui.vertical.channels[2].enabled = 1;
	ui.vertical.channels[3].coupling = 0;
	ui.vertical.channels[2].scale = 0;
	ui.vertical.channels[3].enabled = 1;
	ui.vertical.channels[3].coupling = 0;
	ui.vertical.channels[3].scale = 0;
	scope_config_vertical( &scope,
		ui.vertical.channels[0].scale,
		ui.vertical.channels[1].scale,
		ui.vertical.channels[2].scale,
		ui.vertical.channels[3].scale,
		ui.vertical.offset );

	ui.wavegen.waveforms[0].enabled = 1;
	ui.wavegen.waveforms[0].type = 1;
	ui.wavegen.waveforms[0].offset = 2048;
	ui.wavegen.waveforms[0].scale = 2000;
	ui.wavegen.waveforms[0].frequency = 4000;
	ui.wavegen.waveforms[0].duty_cycle = 0;

	ui.wavegen.waveforms[1].enabled = 1;
	ui.wavegen.waveforms[1].type = 1;
	ui.wavegen.waveforms[1].offset = 2048;
	ui.wavegen.waveforms[1].scale = 2000;
	ui.wavegen.waveforms[1].frequency = 4000;
	ui.wavegen.waveforms[1].duty_cycle = 0;

    wavegen_build_sine( &wavegen,
    	1 << 0,
    	ui.wavegen.waveforms[0].frequency*DAC_BUFFER_LEN,
		ui.wavegen.waveforms[0].scale,
		ui.wavegen.waveforms[0].offset );
    wavegen_build_sine( &wavegen,
    	1 << 1,
    	ui.wavegen.waveforms[1].frequency*DAC_BUFFER_LEN,
		ui.wavegen.waveforms[1].scale,
		ui.wavegen.waveforms[1].offset );

	ui.trigger.level = 2048+1024;
	ui.trigger.mode = UI_TRIGGER_MODE_NORMAL;
	ui.trigger.slope = UI_TRIGGER_SLOPE_RISING;
	ui.trigger.source = 0;
	scope_config_trigger( &scope,
		ui.trigger.source,
		ui.trigger.mode,
		ui.trigger.level,
		ui.trigger.slope );

	//scope_config_horizontal( &scope, ui.horizontal.scale*1000, 512 );
	//void scope_config_vertical( tScope *scope, int gain1, int gain2, int gain3, int gain4, int offset );
	//void scope_config_trigger( tScope *scope, int channel, int mode, int level, int slope );


	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 10;
	xLastWakeTime = xTaskGetTickCount();
	/* Infinite loop */
	for(;;)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		//vTaskDelay(1);

		x_bck = x;
		y_bck = y;
		//tsc_read( &tsc, &x, &y );
		x = x_tsc;
		y = y_tsc;

		//continue;
		pressed_bck = pressed;
		pressed = p_tsc && (0 <= (x-240) && (x-240) < fb.width);
		nk_input_begin( &ctx );
		if( pressed )
		{
			nk_input_motion( &ctx, x-240, y );
			nk_input_button( &ctx, 0, x-240, y, 1 );
		}
		else if( pressed_bck )
		{
			nk_input_motion( &ctx, x_bck-240, y_bck );
			nk_input_button( &ctx, 0, x_bck-240, y_bck, 0 );
		}
		nk_input_end( &ctx );

	    if( pressed || pressed_bck )
	    {
		    //oscilloscope_process( &osc, &ctx, &scope );
	    	ui_build( &ui, &ctx );
			collapsed = nk_window_is_collapsed( &ctx, "STM32G4 Scope" );
			if( collapsed != collapsed_bck )
			{
				collapsed_bck = collapsed;
				lcd_clear( &lcd, 0x0000 );
			}
			nk_draw_fb( &ctx, &fb, &lcd, lcd.width/2 );
			nk_clear(&ctx);
	    }

	    if( ui.acquire.run || ui.acquire.single )
	    {
	    	if( ui.acquire.single )
	    	{
		    	ui.acquire.run = 0;
		    	ui.acquire.single = 0;
	    	}
	    	scope_init( &scope, 2048, 1000000,
					(i&0x01)?buffer1:buffer5,
					(i&0x01)?buffer2:buffer6,
					(i&0x01)?buffer3:buffer7,
					(i&0x01)?buffer4:buffer8,
					ADC_BUFFER_LEN );
			scope_start( &scope );
			int t0 = HAL_GetTick();
			while( scope_is_busy( &scope ) && HAL_GetTick()-t0 < (1000/xFrequency) );
			scope_stop( &scope );

			trigger = scope_get_trigger( &scope ) - ADC_BUFFER_LEN/2;

			draw_grid( &lcd, collapsed );

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
				ADC_BUFFER_LEN,
				collapsed,
				i&0x01
			);

			if( ui.measurements.is_visible )
			{
				draw_measurements( &lcd, collapsed );
			}

			if( ui.horizontal.is_visible )
			{
				draw_horizontal_offset( &lcd, ui.horizontal.offset, collapsed );
			}

			if( ui.trigger.is_visible )
			{
				draw_trigger( &lcd, ui.trigger.level, collapsed );
			}

			if( ui.cursors.is_visible )
			{
				draw_cursors( &lcd, ui.cursors );
			}

			trigger_bck = trigger;
			i += 1;
	    }

		if( pressed && (0 <= x-240) && (x-240 < 240-5) )
		{
			lcd_draw_cross( &lcd, x, y, 0xFFFF );
		}

		//osDelay(1);


	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTaskTsc */
/**
* @brief Function implementing the taskTsc thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskTsc */
void _StartTaskTsc(void *argument)
{
  /* USER CODE BEGIN StartTaskTsc */
	uint16_t x = 0;
	uint16_t y = 0;

	float x_low = 0;
	float y_low = 0;

	int cnt = 0;

	float AX = 38.0/151.0;
	float BX = -1950.0/151.0;
	float AY = 11.0/62.0;
	float BY = -1157.0/62.0;

	tTsc tsc = {0};
	tsc_init( &tsc, &hspi3, TSC_nSS_GPIO_Port, TSC_nSS_Pin, AX, BX, AY, BY, 32 );

	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1;
	xLastWakeTime = xTaskGetTickCount();
  /* Infinite loop */
  for(;;)
  {
	  vTaskDelayUntil( &xLastWakeTime, xFrequency );

	  while( scope_is_busy( &scope ) ){
		  vTaskDelay( 1 );
	  }

	  uint16_t xx;
	  uint16_t yy;
	  int acc_x = 0;
	  int acc_y = 0;
	  x = 0;
	  y = 0;
	  int i;
	  int N = 8;
	  for( i = 0 ; i < N ; i++ )
	  {
		  xx = 0;
		  yy = 0;
		  tsc_read_ll( &tsc, &xx, &yy );
		 // printf( "%d, %d, %d\n", HAL_GetTick(), (int)xx, (int)yy );
		  if( xx == 0 )
		  {
			  acc_x = 0;
			  acc_y = 0;
			  break;
		  }
		  acc_x += xx;
		  acc_y += yy;
	  }

	  if( acc_x )
	  {
		  cnt += 1;
	  }
	  else
	  {
		  cnt = 0;
	  }

	  // filter
	  if( acc_x )
	  {
		  x_low = acc_x/N*0.1 + x_low*0.9;
		  y_low = acc_y/N*0.1 + y_low*0.9;
	  }

	  // tsc to pixel coordinates
	  x = tsc.ax*x_low + tsc.bx;
	  y = tsc.ay*y_low + tsc.by;

	  if( cnt > 50 )
	  {
		  x_tsc = x;
		  y_tsc = y;
		  p_tsc = 1;
	  }
	  else
	  {
		  x_tsc = 0;
		  y_tsc = 0;
		  p_tsc = 0;
	  }
	  //osDelay(1);
  }
  /* USER CODE END StartTaskTsc */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void StartTaskTsc(void *argument)
{
	const TickType_t xFrequency = 1;
	TickType_t xLastWakeTime;

	struct sQueueTscUi msgTscUi = {0};

    float AX = 38.0/151.0;
	float BX = -1950.0/151.0;
	float AY = 11.0/62.0;
	float BY = -1157.0/62.0;

	tTsc tsc = {0};

	tsc_init( &tsc,
		&hspi3,
		TSC_nSS_GPIO_Port, TSC_nSS_Pin,
		AX, BX, AY, BY,
		32
	);

	xLastWakeTime = xTaskGetTickCount();
	for(;;)
  	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		tsc_read( &tsc, &msgTscUi.p, &msgTscUi.x, &msgTscUi.y );
        osMessageQueuePut( queueTscUiHandle, &msgTscUi, 0U, 0U );
	}
}

//struct nk_context *ctx;
void StartTaskUi(void *argument)
{
    tFramebuf fb = {0};
    tLcd lcd = {0};
    static tUi ui = {0};

    struct sQueueTscUi msgTscUi = {0};
    //struct sQueueUiLcd msgUiLcd = {0};

	struct nk_buffer cmds = {0};
	struct nk_buffer pool = {0};
	struct nk_user_font font = {0};

	uint16_t x0, y0;

    lcd_init( &lcd,
    	LCD_nRST_GPIO_Port, LCD_nRST_Pin,
		LCD_BL_GPIO_Port, LCD_BL_Pin,
		480, 320
	);

    framebuf_init( &fb, FB_WIDTH, FB_HEIGHT, fb_buf );

	nk_buffer_init_fixed( &cmds, nk_buf_cmds, NK_BUFFER_CMDS_LEN );
	nk_buffer_init_fixed( &pool, nk_buf_pool, NK_BUFFER_POOL_LEN);

	font.height = fontUbuntuBookRNormal16.bbxh;
	font.width = text_width_f;
	nk_init_custom( &ctx, &cmds, &pool, &font );

    TickType_t xLastWakeTime;
	const TickType_t xFrequency = 10;
	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );

        if( osMessageQueueGet(queueTscUiHandle, &msgTscUi, NULL, 0U) == osOK )
        {
        	msgTscUi.x -= lcd.width/2;
            nk_input_begin( &ctx );
            nk_input_motion( &ctx, msgTscUi.x, msgTscUi.y );
            nk_input_button( &ctx, NK_BUTTON_LEFT, msgTscUi.x, msgTscUi.y, msgTscUi.p );
            nk_input_end( &ctx );
        }

        ui_build( &ui, &ctx );

        x0 = 0;
        for( y0 = 0; y0 < lcd.height; y0 += fb.height)
        {
	    	framebuf_fill( &fb, 0x0000 );
            nk_draw_fb2( &ctx, &fb, x0, y0 );
            //msgUiLcd.x = lcd.width/2;
            //msgUiLcd.y = y0;
            //msgUiLcd.w = fb.width;
            //msgUiLcd.h = fb.height;
            //msgUiLcd.buf = fb.buf;*/
            //osMessageQueuePut(queueUiLcdHandle, &msgUiLcd, 0U, 0U);
			if( osSemaphoreAcquire( semaphoreLcdHandle, 0 ) == osOK )
			{
				lcd_bmp( &lcd, lcd.width/2, y0, fb.width, fb.height, fb.buf);
				osSemaphoreRelease( semaphoreLcdHandle );
			}
        }
        nk_clear(&ctx);
    }
}
#define QUEUE_UI_SCOPE_TYPE_HORIZONTAL 0
#define QUEUE_UI_SCOPE_TYPE_VERTICAL 1
#define QUEUE_UI_SCOPE_TYPE_TRIGGER 2
#define QUEUE_UI_SCOPE_TYPE_START 3
#define QUEUE_UI_SCOPE_TYPE_STOP 4

void StartTaskScope(void *argument)
{
    static tScope scope = {0};

    struct sQueueUiScope msgScope = {0};
    //struct sQueueUiLcd msgLcd = {0};
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 1;
	
    scope_init_ll( &scope,
        &htim2, // horizontal.htim_clock
        &htim3, // horizontal.htim_stop
        &hdac2, // vertical.hdac
        &hopamp1, // vertical.hopamp1
        &hopamp3, // vertical.hopamp2
        &hopamp5, // vertical.hopamp3
        &hopamp6, // vertical.hopamp4
        &hadc1, // trigger.hadc1
        &hadc3, // trigger.hadc2
        &hadc5, // trigger.hadc3
        &hadc4 // trigger.hadc4
    );


    xLastWakeTime = xTaskGetTickCount();
    for(;;)
    {
        vTaskDelayUntil( &xLastWakeTime, xFrequency );

        if( osMessageQueueGet(queueUiScopeHandle, &msgScope, NULL, 0U) == osOK )
        {
            switch( msgScope.type )
            {
                case QUEUE_UI_SCOPE_TYPE_HORIZONTAL:
                    scope_config_horizontal( &scope, msgScope.data[0], msgScope.data[1] );
                    break;
                case QUEUE_UI_SCOPE_TYPE_VERTICAL:
                    scope_config_vertical( &scope,
                        msgScope.data[0],
                        msgScope.data[1],
                        msgScope.data[2],
                        msgScope.data[3],
                        msgScope.data[4] );
                    break;
                case QUEUE_UI_SCOPE_TYPE_TRIGGER:
                    scope_config_trigger( &scope,
                        msgScope.data[0],
                        msgScope.data[1],
                        msgScope.data[2],
                        msgScope.data[3] );
                    break;
                case QUEUE_UI_SCOPE_TYPE_START:
                    scope_start( &scope );
                    break;
                case QUEUE_UI_SCOPE_TYPE_STOP:
                    scope_stop( &scope );
                    break;
            }
        }

        if( 0 && scope_is_running( &scope ) )
        {
            scope_wait( &scope, xFrequency );
            //osMessageQueuePut(queueScopeLcdHandle, &msgLcd, 0U, 0U);
			// take the lcd semaphore and draw.
			if( osSemaphoreAcquire( semaphoreLcdHandle, 0 ) == osOK )
			{
				//scope_draw( &scope, &lcd );
				osSemaphoreRelease( semaphoreLcdHandle );
			}
        }
    }
}

/* USER CODE END Application */

