/*
 * GravityMon Gateway
 * Copyright (c) 2021-2026 Magnus
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifndef SRC_UI_HELPERS_HPP_
#define SRC_UI_HELPERS_HPP_

#if defined(ENABLE_LVGL)
#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Theme mode enumeration
 */
typedef enum {
  UI_THEME_LIGHT,
  UI_THEME_DARK,
} ui_theme_t;

/**
 * Color theme definition
 */
typedef struct {
  lv_color_t bg;           // Background color
  lv_color_t text;         // Text color
  lv_color_t border;       // Border/accent color
  lv_color_t button_bg;    // Button background color
  lv_color_t button_text;  // Button text color
  // Additional palette entries for panels, status bar and battery states
  lv_color_t panel_bg;        // Card/panel background (e.g., gravity card)
  lv_color_t panel_alt_bg;    // Alternate panel background (e.g., temp)
  lv_color_t muted_bg;        // Muted background for small boxes (time/rssi)
  lv_color_t status_bg;       // Status bar background
  lv_color_t battery_good;    // Battery fill - good
  lv_color_t battery_mid;     // Battery fill - medium
  lv_color_t battery_low;     // Battery fill - low
  lv_color_t battery_border;  // Battery border color
} ui_theme_colors_t;

/**
 * Get color theme for specified mode
 */
ui_theme_colors_t ui_get_theme_colors(ui_theme_t theme);

/**
 * Apply theme colors to a UI object (label or button)
 */
void ui_apply_theme_to_object(lv_obj_t* obj, const ui_theme_colors_t* colors);

/**
 * Apply theme to all objects in a screen
 */
void ui_apply_theme_to_screen(lv_obj_t* scr, const ui_theme_colors_t* colors);

/**
 * Create a label with specific position, size, alignment, color, and font
 * @param parent Parent object
 * @param text Label text
 * @param x X position
 * @param y Y position
 * @param w Width
 * @param h Height
 * @param align Text alignment (LV_TEXT_ALIGN_LEFT, etc.)
 * @param color Text color
 * @param font Text font (e.g., &lv_font_montserrat_18)
 * @return Created label object
 */
lv_obj_t* ui_create_label(lv_obj_t* parent, const char* text, lv_coord_t x,
                          lv_coord_t y, lv_coord_t w, lv_coord_t h,
                          lv_text_align_t align, lv_color_t color,
                          const lv_font_t* font);

/**
 * Create a styled label with predefined style
 * @param parent Parent object
 * @param text Label text
 * @param x X position
 * @param y Y position
 * @param w Width
 * @param h Height
 * @param style Predefined lv_style_t with font, alignment, and color
 * @return Created label object
 */
lv_obj_t* ui_create_styled_label(lv_obj_t* parent, const char* text,
                                 lv_coord_t x, lv_coord_t y, lv_coord_t w,
                                 lv_coord_t h, lv_style_t* style);

/**
 * Create a status bar label
 * @param parent Parent object
 * @param text Label text
 * @param x X position
 * @param y Y position
 * @param w Width
 * @param h Height
 * @param align Text alignment
 * @param color Text color
 * @param font Text font (e.g., &lv_font_montserrat_12)
 * @return Created label object
 */
lv_obj_t* ui_create_status_label(lv_obj_t* parent, const char* text,
                                 lv_coord_t x, lv_coord_t y, lv_coord_t w,
                                 lv_coord_t h, lv_text_align_t align,
                                 lv_color_t color, const lv_font_t* font);

/**
 * Create a button with label
 * @param parent Parent object
 * @param label_text Button label text
 * @param x X position
 * @param y Y position
 * @param w Width
 * @param h Height
 * @param callback Event callback (NULL for no callback)
 * @param bg_color Background color
 * @param text_color Text color
 * @param font Text font (e.g., &lv_font_montserrat_18)
 * @return Created button object
 */
lv_obj_t* ui_create_button(lv_obj_t* parent, const char* label_text,
                           lv_coord_t x, lv_coord_t y, lv_coord_t w,
                           lv_coord_t h, lv_event_cb_t callback,
                           lv_color_t bg_color, lv_color_t text_color,
                           const lv_font_t* font);

/**
 * Create a styled button with predefined style
 * @param parent Parent object
 * @param label_text Button label text
 * @param x X position
 * @param y Y position
 * @param w Width
 * @param h Height
 * @param callback Event callback (NULL for no callback)
 * @param bg_color Background color
 * @param style Predefined lv_style_t for text styling
 * @return Created button object
 */
lv_obj_t* ui_create_styled_button(lv_obj_t* parent, const char* label_text,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t w,
                                  lv_coord_t h, lv_event_cb_t callback,
                                  lv_color_t bg_color, lv_style_t* style);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // ENABLE_LVGL

#endif  // SRC_UI_HELPERS_HPP_

// EOF
