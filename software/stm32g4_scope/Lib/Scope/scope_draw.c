/*
 * scope_draw.c
 *
 *  Created on: Sep 3, 2023
 *      Author: jgpei
 */


#include "scope_draw.h"

#include "font.h"
#include    "FontUbuntuBookRNormal16.h"

#define MARKER_ORIENTATION_NORTH 0
#define MARKER_ORIENTATION_SOUTH 1
#define MARKER_ORIENTATION_EAST  2
#define MARKER_ORIENTATION_WEST  3

/**
  * @brief Draw a marker on the LCD at specified coordinates with a given color and orientation.
  *
  * This function draws a marker on the LCD screen at the specified (x, y) coordinates
  * with a specified color and orientation (angle in degrees). The marker is drawn as
  * an arrow-like shape pointing in the specified direction.
  *
  * @param pLcd Pointer to the LCD structure.
  * @param x The x-coordinate of the marker.
  * @param y The y-coordinate of the marker.
  * @param direction The orientation of the marker (angle in degrees).
  * @param color The color of the marker.
  */
void draw_marker(tLcd *pLcd, uint16_t x, uint16_t y, uint16_t direction, uint16_t color) {
    int w = 9;  // Marker width
    int h = 9;  // Marker height

    // Draw the marker body
    lcd_rect(pLcd, x, y, w / 2, h / 2, color);

    // Draw the arrowhead based on the specified direction
    switch (direction) {
        case MARKER_ORIENTATION_NORTH:
            for (int i = 0; i < w / 2; i++) {
                lcd_hline(pLcd, x - w / 2 + 1 + i, y - h / 2 - 1 - i, w - 2 - 2 * i, color);
            }
            break;
        case MARKER_ORIENTATION_SOUTH:
            for (int i = 0; i < w / 2; i++) {
                lcd_hline(pLcd, x - w / 2 + 1 + i, y + h / 2 + 1 + i, w - 2 - 2 * i, color);
            }
            break;
        case MARKER_ORIENTATION_EAST:
            for (int i = 0; i < w / 2; i++) {
                lcd_vline(pLcd, x + w / 2 + 1 + i, y - h / 2 + 1 + i, h - 2 - 2 * i, color);
            }
            break;
        case MARKER_ORIENTATION_WEST:
            for (int i = 0; i < w / 2; i++) {
                lcd_vline(pLcd, x - w / 2 - 1 - i, y - h / 2 + 1 + i, h - 2 - 2 * i, color);
            }
            break;
        default:
            break;
    }
}


/**
  * @brief Draw the entire scope display on the LCD.
  *
  * This function draws the complete scope display on the LCD screen, including
  * the acquisition waveform, horizontal scale, vertical scale, and trigger settings.
  * Each of these components can be individually toggled as visible or hidden based on
  * the input parameters.
  *
  * @param pThis Pointer to the scope structure.
  * @param pLcd Pointer to the LCD structure.
  * @param acquire_is_visible Flag indicating whether the acquisition waveform is visible.
  * @param horizontal_is_visible Flag indicating whether the horizontal scale is visible.
  * @param vertical_is_visible Flag indicating whether the vertical scale is visible.
  * @param trigger_is_visible Flag indicating whether the trigger settings are visible.
  * @param is_collapsed Flag indicating whether the scope display is in a collapsed state.
  *                    When collapsed, some components may be hidden or minimized.
  */
void scope_draw(tScope *pThis, tLcd *pLcd,
                uint8_t acquire_is_visible,
                uint8_t horizontal_is_visible,
                uint8_t vertical_is_visible,
                uint8_t trigger_is_visible,
                int is_collapsed) {
    if (acquire_is_visible) {
        // Draw the acquisition waveform
        scope_draw_acquire(pThis, pLcd, is_collapsed);
    }
    if (horizontal_is_visible) {
        // Draw the horizontal scale
        scope_draw_horizontal(&pThis->horizontal, pLcd, is_collapsed);
    }
    if (vertical_is_visible) {
        // Draw the vertical scale
        scope_draw_vertical(&pThis->vertical, pLcd, is_collapsed);
    }
    if (trigger_is_visible) {
        // Draw the trigger settings
        scope_draw_trigger(&pThis->trigger, pLcd, is_collapsed);
    }
}

void scope_erase( tScope *pThis, tLcd *pLcd,
        uint8_t acquire_is_visible,
        uint8_t horizontal_is_visible,
        uint8_t vertical_is_visible,
        uint8_t trigger_is_visible,
        int is_collapsed )
{
    if( acquire_is_visible )
    {
        scope_erase_acquire( pThis, pLcd, is_collapsed );
    }
    if( horizontal_is_visible )
    {
        scope_erase_horizontal( &pThis->horizontal, pLcd, is_collapsed );
    }
    if( vertical_is_visible )
    {
        scope_erase_vertical( &pThis->vertical, pLcd, is_collapsed );
    }
    if( trigger_is_visible )
    {
        scope_erase_trigger( &pThis->trigger, pLcd, is_collapsed );
    }
}


void scope_draw_acquire( tScope *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque_acquire( pThis, pLcd, SCOPE_COLOR_ACQUIRE, is_collapsed );
}
void scope_draw_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque_horizontal( pThis, pLcd, SCOPE_COLOR_HORIZONTAL, is_collapsed );

}

void scope_draw_vertical( tScope_Vertical *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque_vertical( pThis, pLcd, SCOPE_COLOR_VERTICAL, SCOPE_COLOR_CH1, SCOPE_COLOR_CH2, SCOPE_COLOR_CH3, SCOPE_COLOR_CH4, is_collapsed );

}

void scope_draw_trigger( tScope_Trigger *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque_trigger( pThis, pLcd, SCOPE_COLOR_TRIGGER, is_collapsed );

}


void scope_erase_acquire( tScope *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque_acquire( pThis, pLcd, SCOPE_COLOR_BACKGROUND, is_collapsed );

}

void scope_erase_horizontal( tScope_Horizontal *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque_horizontal( pThis, pLcd, SCOPE_COLOR_BACKGROUND, is_collapsed );

}

void scope_erase_vertical( tScope_Vertical *pThis, tLcd *pLcd, int is_collapsed )
{
    scope_stroque_vertical( pThis, pLcd, SCOPE_COLOR_BACKGROUND, SCOPE_COLOR_BACKGROUND, SCOPE_COLOR_BACKGROUND, SCOPE_COLOR_BACKGROUND, SCOPE_COLOR_BACKGROUND, is_collapsed );

}

void scope_erase_trigger( tScope_Trigger *pThis, tLcd *pLcd, int is_collapsed )
{
       scope_stroque_trigger( pThis, pLcd, SCOPE_COLOR_BACKGROUND, is_collapsed );

}


void scope_stroque_acquire( tScope *pThis, tLcd *pLcd, uint16_t color, int is_collapsed )
{
    /*int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    x = pThis->state*10;
    y = 0;
    w = 10;
    h = 10;

    lcd_rect( pLcd, x, y, w, h, color );*/
}

/**
  * @brief Draw a horizontal stroke on the LCD with an optional marker.
  *
  * This function draws a horizontal stroke on the LCD screen with the specified color.
  * The stroke can be collapsed or expanded based on the `is_collapsed` parameter.
  * If collapsed, it is drawn at the top part of the screen; if expanded, it is drawn
  * lower on the screen. An optional marker is drawn at the left end of the stroke.
  *
  * @param pThis Pointer to the horizontal stroke structure.
  * @param pLcd Pointer to the LCD structure.
  * @param color The color of the stroke and marker.
  * @param is_collapsed A flag indicating whether the stroke should be collapsed (1) or expanded (0).
  */
void scope_stroque_horizontal(tScope_Horizontal *pThis, tLcd *pLcd, uint16_t color, int is_collapsed) {
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    if (is_collapsed) {
        x = -pThis->offset / 4 + 120 + 10;
    } else {
        x = -pThis->offset / 2 + 240 + 20;
    }
    y = 0;
    h = pLcd->height;

    // Draw the horizontal stroke
    lcd_vline(pLcd, x, y, h, color);

    // Draw an optional marker at the left end of the stroke
    draw_marker(pLcd, x, 0, MARKER_ORIENTATION_SOUTH, color);

    // You can uncomment the following line to draw the letter "H" as text within the marker
    // font_draw_text(&fontUbuntuBookRNormal16, x + 1, y + 1, "H", color, lcd_set_pixel, pLcd);
}


/**
  * @brief Draw a stroque (vertical lines with markers) on the LCD for a vertical scope element.
  *
  * This function draws a stroque on the LCD screen for a vertical scope element. The stroque
  * consists of multiple vertical lines, each having a corresponding marker at the top. This is
  * commonly used in oscilloscopes to represent signal levels. The function allows customization
  * of colors and the option to collapse the stroque to half-width.
  *
  * @param pThis Pointer to the vertical scope element structure.
  * @param pLcd Pointer to the LCD structure.
  * @param color The color of the main stroque lines.
  * @param color1 The color of the first gain level stroque line.
  * @param color2 The color of the second gain level stroque line.
  * @param color3 The color of the third gain level stroque line.
  * @param color4 The color of the fourth gain level stroque line.
  * @param is_collapsed Flag indicating whether to collapse the stroque to half-width (1 for collapsed, 0 for full width).
  */
void scope_stroque_vertical(tScope_Vertical *pThis, tLcd *pLcd, uint16_t color,
                            uint16_t color1, uint16_t color2, uint16_t color3, uint16_t color4,
                            int is_collapsed) {
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    if (is_collapsed) {
        w = pLcd->width / 2;
    } else {
        w = pLcd->width;
    }

    // Draw the main stroque line
    x = 0;
    y = pThis->offset;
    lcd_hline(pLcd, x, y, w, color);
    draw_marker(pLcd, x, y, MARKER_ORIENTATION_EAST, color);

    // Draw gain level stroque lines
    x = 0;
    y = pThis->gain1;
    lcd_hline(pLcd, x, y, w, color1);

    x = 0;
    y = pThis->gain2;
    lcd_hline(pLcd, x, y, w, color2);

    x = 0;
    y = pThis->gain3;
    lcd_hline(pLcd, x, y, w, color3);

    x = 0;
    y = pThis->gain4;
    lcd_hline(pLcd, x, y, w, color4);
}


/**
  * @brief Draw a trigger stroque on the LCD with optional marker.
  *
  * This function draws a trigger stroque on the LCD screen at a specified position and with
  * a specified color. It can also include a marker pointing eastward at the trigger position.
  * The stroque is typically used to indicate a trigger point on an oscilloscope display.
  *
  * @param pThis Pointer to the tScope_Trigger structure representing the trigger.
  * @param pLcd Pointer to the LCD structure.
  * @param color The color of the trigger stroque and marker.
  * @param is_collapsed Flag indicating whether the trigger stroque should be collapsed.
  *                     If true, it is drawn with half the width; otherwise, it spans the full width.
  */
void scope_stroque_trigger(tScope_Trigger *pThis, tLcd *pLcd, uint16_t color, int is_collapsed) {
    int x = 0;  // X-coordinate of the trigger stroque
    int y = 0;  // Y-coordinate of the trigger stroque
    int w = 0;  // Width of the trigger stroque
    int h = 0;  // Height of the trigger stroque

    x = 0;  // Initialize x-coordinate

    // Calculate the y-coordinate based on the trigger level
    y = pLcd->height - pThis->level * pLcd->height / 4096.0f;

    // Determine the width of the trigger stroque based on the 'is_collapsed' flag
    if (is_collapsed) {
        w = pLcd->width / 2;
    } else {
        w = pLcd->width;
    }

    // Draw the horizontal line representing the trigger stroque
    lcd_hline(pLcd, x, y, w, color);

    // Draw an eastward marker at the trigger position
    draw_marker(pLcd, x, y, MARKER_ORIENTATION_EAST, color);

    // Optionally, you can add text to label the trigger point:
    // font_draw_text(&fontUbuntuBookRNormal16, x + 1, y + 1, "T", color, lcd_set_pixel, pLcd);
}

/**
  * @brief Draw a grid on the LCD screen of the scope display.
  *
  * This function draws a grid on the LCD screen of the scope display. The grid consists
  * of evenly spaced horizontal and vertical lines. The appearance of the grid can change
  * based on whether the display is in a collapsed or expanded state.
  *
  * @param pThis Pointer to the scope structure.
  * @param pLcd Pointer to the LCD structure.
  * @param is_collapsed Flag indicating whether the scope display is collapsed (1) or expanded (0).
  *
  * @note The grid color is determined by the constant SCOPE_COLOR_GRID.
  */
void scope_draw_grid(tScope *pThis, tLcd *pLcd, int is_collapsed) {
    if (is_collapsed) {
        // Draw vertical grid lines for collapsed mode
        for (int d = 0; d < pLcd->width; d += 40) {
            lcd_rect(pLcd, d / 2, 0, 1, pLcd->height, SCOPE_COLOR_GRID);
        }
        lcd_rect(pLcd, pLcd->width / 2 - 1, 0, 1, pLcd->height, SCOPE_COLOR_GRID);

        // Draw horizontal grid lines for collapsed mode
        for (int d = 0; d < pLcd->height; d += 40) {
            lcd_rect(pLcd, 0, d, pLcd->width / 2, 1, SCOPE_COLOR_GRID);
        }
        lcd_rect(pLcd, 0, pLcd->height, pLcd->width / 2, 1, SCOPE_COLOR_GRID);
    } else {
        // Draw vertical grid lines for expanded mode
        for (int d = 0; d < pLcd->width; d += 40) {
            lcd_rect(pLcd, d, 0, 1, pLcd->height, SCOPE_COLOR_GRID);
        }
        lcd_rect(pLcd, pLcd->width - 1, 0, 1, pLcd->height, SCOPE_COLOR_GRID);

        // Draw horizontal grid lines for expanded mode
        for (int d = 0; d < pLcd->height; d += 40) {
            lcd_rect(pLcd, 0, d, pLcd->width, 1, SCOPE_COLOR_GRID);
        }
        lcd_rect(pLcd, 0, pLcd->height, pLcd->width, 1, SCOPE_COLOR_GRID);
    }
}

/**
  * @brief Draw the signals on the scope's LCD display.
  *
  * This function is responsible for drawing the waveform signals on the LCD display of the scope.
  * It takes into account the current state of the scope, including signal data, trigger position,
  * and the collapsed/expanded view mode.
  *
  * @param pThis Pointer to the scope structure.
  * @param pLcd Pointer to the LCD structure.
  * @param is_collapsed Flag indicating whether the display is in collapsed mode (1 for collapsed, 0 for expanded).
  */
#define ABS(a)   ((a>0)?(a):-(a))
#define MIN(a,b) ((a<b)?(a):(b))

void scope_draw_signals(tScope *pThis, tLcd *pLcd, int is_collapsed) {
    uint16_t i;
    float scale = 320 / 4096.0f; // Scale factor for signal amplitudes
    int16_t trigger;
    static int16_t trigger_bck = 0;
    int16_t n, n_bck;
    int pLcd_height = pLcd->height;
    static int is_collapsed_bck = 0;

    uint16_t x;
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
    trigger = (pThis->len - pThis->dma_cndtr) - pThis->len / 2 + pThis->len / 2;
    trigger = trigger % pThis->len;

    // Check if the signal is in the first or second half of the buffer
    if (pThis->cnt & 0x01) {
        for (i = 0; i < pThis->len; i++) {
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
                y5 = pLcd_height - pThis->buffer5[n_bck] * scale;
                y6 = pLcd_height - pThis->buffer6[n_bck] * scale;
                y7 = pLcd_height - pThis->buffer7[n_bck] * scale;
                y8 = pLcd_height - pThis->buffer8[n_bck] * scale;
                
                // Draw vertical lines for signal traces
                lcd_vline(pLcd, x, MIN(y5, y5_bck), ABS(y5 - y5_bck) + 1, LCD_COLOR_BLACK);
                lcd_vline(pLcd, x, MIN(y6, y6_bck), ABS(y6 - y6_bck) + 1, LCD_COLOR_BLACK);
                lcd_vline(pLcd, x, MIN(y7, y7_bck), ABS(y7 - y7_bck) + 1, LCD_COLOR_BLACK);
                lcd_vline(pLcd, x, MIN(y8, y8_bck), ABS(y8 - y8_bck) + 1, LCD_COLOR_BLACK);
                
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
                y1 = pLcd_height - pThis->buffer1[n] * scale;
                y2 = pLcd_height - pThis->buffer2[n] * scale;
                y3 = pLcd_height - pThis->buffer3[n] * scale;
                y4 = pLcd_height - pThis->buffer4[n] * scale;
                
                // Draw vertical lines for signal traces with channel-specific colors
                lcd_vline(pLcd, x, MIN(y1, y1_bck), ABS(y1 - y1_bck) + 1, SCOPE_COLOR_CH1);
                lcd_vline(pLcd, x, MIN(y2, y2_bck), ABS(y2 - y2_bck) + 1, SCOPE_COLOR_CH2);
                lcd_vline(pLcd, x, MIN(y3, y3_bck), ABS(y3 - y3_bck) + 1, SCOPE_COLOR_CH3);
                lcd_vline(pLcd, x, MIN(y4, y4_bck), ABS(y4 - y4_bck) + 1, SCOPE_COLOR_CH4);
                
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
        for( i = 0; i < pThis->len; i++ )
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
                y1 = pLcd_height-pThis->buffer1[n_bck]*scale;
                y2 = pLcd_height-pThis->buffer2[n_bck]*scale;
                y3 = pLcd_height-pThis->buffer3[n_bck]*scale;
                y4 = pLcd_height-pThis->buffer4[n_bck]*scale;
                
                // Draw vertical lines for signal traces
                lcd_vline( pLcd, x, MIN(y1,y1_bck), ABS(y1-y1_bck)+1, LCD_COLOR_BLACK );
                lcd_vline( pLcd, x, MIN(y2,y2_bck), ABS(y2-y2_bck)+1, LCD_COLOR_BLACK );
                lcd_vline( pLcd, x, MIN(y3,y3_bck), ABS(y3-y3_bck)+1, LCD_COLOR_BLACK );
                lcd_vline( pLcd, x, MIN(y4,y4_bck), ABS(y4-y4_bck)+1, LCD_COLOR_BLACK );
                
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
                y5 = pLcd_height-pThis->buffer5[n]*scale;
                y6 = pLcd_height-pThis->buffer6[n]*scale;
                y7 = pLcd_height-pThis->buffer7[n]*scale;
                y8 = pLcd_height-pThis->buffer8[n]*scale;
                
                // Draw vertical lines for signal traces with channel-specific colors
                lcd_vline( pLcd, x, MIN(y5,y5_bck), ABS(y5-y5_bck)+1, SCOPE_COLOR_CH1 );
                lcd_vline( pLcd, x, MIN(y6,y6_bck), ABS(y6-y6_bck)+1, SCOPE_COLOR_CH2 );
                lcd_vline( pLcd, x, MIN(y7,y7_bck), ABS(y7-y7_bck)+1, SCOPE_COLOR_CH3 );
                lcd_vline( pLcd, x, MIN(y8,y8_bck), ABS(y8-y8_bck)+1, SCOPE_COLOR_CH4 );
                
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
}
