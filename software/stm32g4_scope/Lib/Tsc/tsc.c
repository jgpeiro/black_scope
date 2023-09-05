/**
  ******************************************************************************
  * @file    tsc.c
  * @author  Jose
  * @brief   This file contains the implementation of the TSC2046 touch controller driver.
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                    ##### TSC driver features #####
  ==============================================================================
  [..]
    (+) Initialization and configuration of the TSC2046 touch controller.
    (+) Reading raw X and Y coordinates from the touch controller.
    (+) Reading calibrated X and Y coordinates with pressure information.
    (+) Integration with STM32 HAL library for SPI communication.

                     ##### How to use this driver #####
  ==============================================================================
  [..]
    (#) Include the "tsc.h" header file in your project.

    (#) Create a tTsc structure and initialize it using the "tsc_init" function.
       This function sets up the SPI communication, GPIO for chip select, and
       calibration coefficients.

    (#) Call the "tsc_read" function to read calibrated touch coordinates and
       pressure values. This function also performs low-pass filtering and
       stabilization.

    (#) Use the obtained touch coordinates and pressure values in your application.

  @endverbatim
  ******************************************************************************
  */

#include <stdint.h>
#include "tsc.h"

// Implementation of the tsc_init function
/**
  * @brief Initialize the TSC2046 touch controller.
  * @param pThis Pointer to the tTsc structure.
  * @param spi Pointer to the SPI handle.
  * @param cs_port GPIO port for chip select pin.
  * @param cs_pin Chip select pin number.
  * @param ax X-axis calibration coefficient 'ax'.
  * @param bx X-axis calibration coefficient 'bx'.
  * @param ay Y-axis calibration coefficient 'ay'.
  * @param by Y-axis calibration coefficient 'by'.
  * @param tau Low-pass filter time constant.
  * @param cnt_max Maximum stabilization count.
  */
void tsc_init(tTsc* pThis, SPI_HandleTypeDef* spi, GPIO_TypeDef* cs_port, uint16_t cs_pin, float ax, float bx, float ay, float by, float tau, uint16_t cnt_max) {
    pThis->spi = spi;
    pThis->cs_port = cs_port;
    pThis->cs_pin = cs_pin;
    pThis->ax = ax;
    pThis->bx = bx;
    pThis->ay = ay;
    pThis->by = by;
    pThis->tau = tau;
    pThis->cnt_max = cnt_max;

    // Initialize the chip select pin to high (inactive)
    HAL_GPIO_WritePin(pThis->cs_port, pThis->cs_pin, GPIO_PIN_SET);
}

// Implementation of the tsc_read_ll function
/**
  * @brief Read raw X and Y coordinates from the TSC2046 touch controller.
  * @param pThis Pointer to the tTsc structure.
  * @param x Pointer to store the raw X-coordinate.
  * @param y Pointer to store the raw Y-coordinate.
  */
void tsc_read_ll(tTsc* pThis, uint16_t* x, uint16_t* y) {
    uint8_t buf_tx[3] = {0};
    uint8_t buf_rx[3] = {0};

    // Pull the chip select pin low to start the communication
    HAL_GPIO_WritePin(pThis->cs_port, pThis->cs_pin, GPIO_PIN_RESET);

    // Read X-coordinate
    buf_tx[0] = TSC_CHANNEL_X;
    HAL_SPI_TransmitReceive(pThis->spi, buf_tx, buf_rx, 3, HAL_MAX_DELAY);
    *x = (buf_rx[1] << 4) | (buf_rx[2] >> 4);

    // Read Y-coordinate
    buf_tx[0] = TSC_CHANNEL_Y;
    HAL_SPI_TransmitReceive(pThis->spi, buf_tx, buf_rx, 3, HAL_MAX_DELAY);
    *y = (buf_rx[1] << 4) | (buf_rx[2] >> 4);

    // Release the chip select pin
    HAL_GPIO_WritePin(pThis->cs_port, pThis->cs_pin, GPIO_PIN_SET);

    // Check if X-coordinate is at maximum value
    if (*x == 2047) {
        *x = 0;
    }
}

// Implementation of the tsc_read function
/**
  * @brief Read calibrated X and Y coordinates with pressure information
  *        from the TSC2046 touch controller.
  * @param pThis Pointer to the tTsc structure.
  * @param x Pointer to store the calibrated X-coordinate.
  * @param y Pointer to store the calibrated Y-coordinate.
  * @param p Pointer to store the pressure value.
  */
void tsc_read(tTsc* pThis, uint16_t* x, uint16_t* y, uint16_t* p) {
    // Read raw X and Y coordinates
    tsc_read_ll(pThis, x, y);

    // Check if X and Y coordinates are non-zero
    if (*x && *y) {
        pThis->cnt += 1;
    } else {
        pThis->x_low = 0;
        pThis->y_low = 0;
        pThis->cnt = 0;
    }

    // Low-pass filtering for X and Y coordinates
    pThis->x_low = pThis->x_low * pThis->tau + *x * (1 - pThis->tau);
    pThis->y_low = pThis->y_low * pThis->tau + *y * (1 - pThis->tau);

    // Check if stabilization count is reached
    if (pThis->cnt >= pThis->cnt_max) {
        *x = pThis->ax * pThis->x_low + pThis->bx;
        *y = pThis->ay * pThis->y_low + pThis->by;
        *p = 1; // Pressure value is non-zero
    } else {
        *x = 0;
        *y = 0;
        *p = 0; // Pressure value is zero
    }
}
