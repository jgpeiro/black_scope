/*
 * scope_draw.h
 *
 *  Created on: Sep 3, 2023
 *      Author: jgpei
 */

#ifndef _SCOPE_DRAW_H_
#define _SCOPE_DRAW_H_

#include "scope.h"

// Define colors for various scope components
#define SCOPE_COLOR_ACQUIRE     LCD_COLOR_RED       // Color for the acquisition area
#define SCOPE_COLOR_HORIZONTAL  LCD_COLOR_GREEN     // Color for the horizontal scale
#define SCOPE_COLOR_VERTICAL    LCD_COLOR_BLUE      // Color for the vertical scale
#define SCOPE_COLOR_TRIGGER     LCD_COLOR_YELLOW    // Color for the trigger

#define SCOPE_COLOR_CH1         LCD_COLOR_RED       // Color for channel 1
#define SCOPE_COLOR_CH2         LCD_COLOR_GREEN     // Color for channel 2
#define SCOPE_COLOR_CH3         LCD_COLOR_BLUE      // Color for channel 3
#define SCOPE_COLOR_CH4         LCD_COLOR_MAGENTA   // Color for channel 4

#define SCOPE_COLOR_GRID        0x8410              // Color for the grid lines
#define SCOPE_COLOR_BACKGROUND  LCD_COLOR_BLACK     // Background color

/**
 * @brief Draw the entire scope on the LCD.
 *
 * This function draws the complete scope, including its various components
 * such as the acquisition area, horizontal and vertical scales, trigger,
 * channel waveforms, grid lines, and markers on the provided LCD screen.
 *
 * @param pThis Pointer to the scope structure.
 * @param pLcd Pointer to the LCD structure.
 * @param acquire_is_visible Flag indicating whether the acquisition area is visible.
 * @param horizontal_is_visible Flag indicating whether the horizontal scale is visible.
 * @param vertical_is_visible Flag indicating whether the vertical scale is visible.
 * @param trigger_is_visible Flag indicating whether the trigger is visible.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_draw( tScope *pThis, tLcd *pLcd,
		uint8_t acquire_is_visible,
		uint8_t horizontal_is_visible,
		uint8_t vertical_is_visible,
		uint8_t trigger_is_visible,
		int is_collapsed );

/**
 * @brief Erase the entire scope from the LCD.
 *
 * This function erases the complete scope, including all its components,
 * from the provided LCD screen.
 *
 * @param pThis Pointer to the scope structure.
 * @param pLcd Pointer to the LCD structure.
 * @param acquire_is_visible Flag indicating whether the acquisition area is visible.
 * @param horizontal_is_visible Flag indicating whether the horizontal scale is visible.
 * @param vertical_is_visible Flag indicating whether the vertical scale is visible.
 * @param trigger_is_visible Flag indicating whether the trigger is visible.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_erase( tScope *pThis, tLcd *pLcd,
		uint8_t acquire_is_visible,
		uint8_t horizontal_is_visible,
		uint8_t vertical_is_visible,
		uint8_t trigger_is_visible,
		int is_collapsed );

/**
 * @brief Draw the acquisition area of the scope on the LCD.
 *
 * This function draws the acquisition area of the scope on the provided
 * LCD screen. It is typically called as part of the complete scope drawing
 * process.
 *
 * @param pThis Pointer to the scope structure.
 * @param pLcd Pointer to the LCD structure.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_draw_acquire( tScope *pThis, tLcd *pLcd, int is_collapsed );

/**
 * @brief Draw the horizontal scale of the scope on the LCD.
 *
 * This function draws the horizontal scale of the scope on the provided
 * LCD screen. It is typically called as part of the complete scope drawing
 * process.
 *
 * @param pThis Pointer to the horizontal scope configuration structure.
 * @param pLcd Pointer to the LCD structure.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_draw_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, int is_collapsed );

/**
 * @brief Draw the vertical scale of the scope on the LCD.
 *
 * This function draws the vertical scale of the scope on the provided
 * LCD screen. It is typically called as part of the complete scope drawing
 * process.
 *
 * @param pThis Pointer to the vertical scope configuration structure.
 * @param pLcd Pointer to the LCD structure.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_draw_vertical( tScope_Vertical *pThis, tLcd *pLcd, int is_collapsed );

/**
 * @brief Draw the trigger of the scope on the LCD.
 *
 * This function draws the trigger of the scope on the provided
 * LCD screen. It is typically called as part of the complete scope drawing
 * process.
 *
 * @param pThis Pointer to the trigger scope configuration structure.
 * @param pLcd Pointer to the LCD structure.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_draw_trigger( tScope_Trigger *pThis, tScope_Vertical *pVertical, tLcd *pLcd, int is_collapsed );

/**
 * @brief Erase the acquisition area of the scope from the LCD.
 *
 * This function erases the acquisition area of the scope from the provided
 * LCD screen. It is typically called as part of the complete scope erasing
 * process.
 *
 * @param pThis Pointer to the scope structure.
 * @param pLcd Pointer to the LCD structure.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_erase_acquire( tScope *pThis, tLcd *pLcd, int is_collapsed );

/**
 * @brief Erase the horizontal scale of the scope from the LCD.
 *
 * This function erases the horizontal scale of the scope from the provided
 * LCD screen. It is typically called as part of the complete scope erasing
 * process.
 *
 * @param pThis Pointer to the horizontal scope configuration structure.
 * @param pLcd Pointer to the LCD structure.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_erase_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, int is_collapsed );

/**
 * @brief Erase the vertical scale of the scope from the LCD.
 *
 * This function erases the vertical scale of the scope from the provided
 * LCD screen. It is typically called as part of the complete scope erasing
 * process.
 *
 * @param pThis Pointer to the vertical scope configuration structure.
 * @param pLcd Pointer to the LCD structure.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_erase_vertical( tScope_Vertical *pThis, tLcd *pLcd, int is_collapsed );

/**
 * @brief Erase the trigger of the scope from the LCD.
 *
 * This function erases the trigger of the scope from the provided
 * LCD screen. It is typically called as part of the complete scope erasing
 * process.
 *
 * @param pThis Pointer to the trigger scope configuration structure.
 * @param pLcd Pointer to the LCD structure.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_erase_trigger( tScope_Trigger *pThis, tScope_Vertical *pVertical, tLcd *pLcd, int is_collapsed );

/**
 * @brief Draw the acquisition area of the scope with a custom color on the LCD.
 *
 * This function draws the acquisition area of the scope with a specified color
 * on the provided LCD screen. It is typically called as part of the complete
 * scope drawing process.
 *
 * @param pThis Pointer to the scope structure.
 * @param pLcd Pointer to the LCD structure.
 * @param color The custom color to use for drawing.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_stroke_acquire( tScope *pThis, tLcd *pLcd, uint16_t color, int is_collapsed );

/**
 * @brief Draw the horizontal scale of the scope with a custom color on the LCD.
 *
 * This function draws the horizontal scale of the scope with a specified color
 * on the provided LCD screen. It is typically called as part of the complete
 * scope drawing process.
 *
 * @param pThis Pointer to the horizontal scope configuration structure.
 * @param pLcd Pointer to the LCD structure.
 * @param color The custom color to use for drawing.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_stroke_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, uint16_t color, int is_collapsed );

/**
 * @brief Draw the vertical scale of the scope with custom colors on the LCD.
 *
 * This function draws the vertical scale of the scope with specified colors for
 * each channel on the provided LCD screen. It is typically called as part of the
 * complete scope drawing process.
 *
 * @param pThis Pointer to the vertical scope configuration structure.
 * @param pLcd Pointer to the LCD structure.
 * @param color The custom color to use for drawing.
 * @param color1 The custom color for channel 1.
 * @param color2 The custom color for channel 2.
 * @param color3 The custom color for channel 3.
 * @param color4 The custom color for channel 4.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_stroke_vertical( tScope_Vertical *pThis, tLcd *pLcd, uint16_t color,
                           uint16_t color1, uint16_t color2, uint16_t color3, uint16_t color4,
                           int is_collapsed );

/**
 * @brief Draw the trigger of the scope with a custom color on the LCD.
 *
 * This function draws the trigger of the scope with a specified color
 * on the provided LCD screen. It is typically called as part of the complete
 * scope drawing process.
 *
 * @param pThis Pointer to the trigger scope configuration structure.
 * @param pLcd Pointer to the LCD structure.
 * @param color The custom color to use for drawing.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_stroke_trigger( tScope_Trigger *pThis, tScope_Vertical *pVertical, tLcd *pLcd, uint16_t color, int is_collapsed );

/**
 * @brief Draw the signals (waveforms) of the scope on the LCD.
 *
 * This function draws the signals (waveforms) of the scope's channels on the
 * provided LCD screen. It is typically called as part of the complete scope
 * drawing process.
 *
 * @param pThis Pointer to the scope structure.
 * @param pLcd Pointer to the LCD structure.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_draw_signals( tScope *pThis, tLcd *pLcd, int is_collapsed );

/**
 * @brief Draw the grid lines on the scope on the LCD.
 *
 * This function draws the grid lines on the scope's display on the provided
 * LCD screen. It is typically called as part of the complete scope drawing
 * process.
 *
 * @param pThis Pointer to the scope structure.
 * @param pLcd Pointer to the LCD structure.
 * @param is_collapsed Flag indicating whether the scope is in a collapsed state.
 */
void scope_draw_grid( tScope *pThis, tLcd *pLcd, int is_collapsed );

/**
 * @brief Draw a marker on the LCD at specified coordinates with a given color.
 *
 * This function draws a marker on the LCD screen at the specified (x, y)
 * coordinates with a specified color and direction.
 *
 * @param pLcd Pointer to the LCD structure.
 * @param x The x-coordinate of the marker.
 * @param y The y-coordinate of the marker.
 * @param direction The direction of the marker (angle in degrees).
 * @param color The color of the marker.
 */
void draw_marker( tLcd *pLcd, uint16_t x, uint16_t y, uint16_t direction, uint16_t color );

#endif /* _SCOPE_DRAW_H_ */
