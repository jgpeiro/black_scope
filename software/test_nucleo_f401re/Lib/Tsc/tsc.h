/*
  ******************************************************************************
  * @file    tsc.h
  * @author  Jose
  * @brief   This file contains the implementation of the TSC2046 touch controller driver.
  *
  ******************************************************************************
 */

#ifndef TSC_H_
#define TSC_H_

#include "stm32f4xx_hal.h"

// TSC2046 registers
#define TSC_CHANNEL_X	(0x90)
#define TSC_CHANNEL_Y	(0xD0)

/**
 * @brief Structure representing the TSC2046 touch controller configuration.
 */
typedef struct {
    SPI_HandleTypeDef* spi;     /**< Pointer to the SPI handle. */
    GPIO_TypeDef* cs_port;      /**< GPIO port for chip select pin. */
    uint16_t cs_pin;            /**< Chip select pin number. */
    float ax;                   /**< X-axis calibration coefficient 'ax'. */
    float bx;                   /**< X-axis calibration coefficient 'bx'. */
    float ay;                   /**< Y-axis calibration coefficient 'ay'. */
    float by;                   /**< Y-axis calibration coefficient 'by'. */
    int cnt;                    /**< Internal counter for stabilization. */
    int x_low;                  /**< Low-pass filtered X-axis value. */
    int y_low;                  /**< Low-pass filtered Y-axis value. */
    float tau;                  /**< Low-pass filter time constant. */
    uint16_t cnt_max;           /**< Maximum stabilization count. */
} tTsc;

/**
 * @brief Initialize the TSC2046 touch controller.
 *
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
void tsc_init(tTsc* pThis, SPI_HandleTypeDef* spi, GPIO_TypeDef* cs_port, uint16_t cs_pin, float ax, float bx, float ay, float by, float tau, uint16_t cnt_max);

/**
 * @brief Read raw X and Y coordinates from the TSC2046 touch controller.
 *
 * @param pThis Pointer to the tTsc structure.
 * @param x Pointer to store the X-coordinate.
 * @param y Pointer to store the Y-coordinate.
 */
void tsc_read_ll(tTsc* pThis, uint16_t* x, uint16_t* y);

/**
 * @brief Read calibrated X and Y coordinates from the TSC2046 touch controller.
 *
 * @param pThis Pointer to the tTsc structure.
 * @param x Pointer to store the calibrated X-coordinate.
 * @param y Pointer to store the calibrated Y-coordinate.
 * @param p Pointer to store the pressure value.
 */
void tsc_read(tTsc* pThis, uint16_t* x, uint16_t* y, uint16_t* p);

#endif /* TSC_H_ */
