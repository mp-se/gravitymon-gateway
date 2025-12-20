/*
MIT License

Copyright (c) 2024-2025 Magnus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#if defined(ENABLE_LVGL)

#include <math.h>
#include <string.h>

#include <cstdio>
#include <ctime>
#include <log.hpp>
#include <ui_gravitymon_gateway.hpp>

/**
 * UI state structure - matching hardware layout
 */
typedef struct {
  // Display labels
  lv_obj_t* lbl_name;
  lv_obj_t* lbl_index;
  lv_obj_t* lbl_temp;
  lv_obj_t* lbl_measurement;  // Gravity or Pressure
  lv_obj_t* lbl_battery;
  lv_obj_t* lbl_time;
  lv_obj_t* lbl_status;
  lv_obj_t* lbl_rssi;
  lv_obj_t* lbl_type;
  lv_obj_t* lbl_source;
  lv_obj_t* lbl_history[5];

  lv_obj_t* wid_battery_canvas;  // Battery indicator canvas

  // Data buffers
  char data_name[30];
  char data_index[10];
  char data_temp[30];
  char data_measurement[20];  // Gravity or Pressure
  int16_t measurement_min;    // Gravity or Pressure
  int16_t measurement_max;    // Gravity or Pressure
  int32_t measurement;        // Gravity or Pressure
  char data_time[30];
  char data_time_ago[20];
  char data_rssi[20];
  char data_type[20];
  char data_source[30];
  char data_status[64];
  char data_history[5][64];
  char data_battery_voltage[10];
  char data_battery_percentage[10];
  int8_t battery_percentage;

  char data_measurement_min[32];  // Gravity or Pressure
  char data_measurement_max[32];

  // Theme state
  bool flg_darkmode;
  bool flg_last_darkmode;

  // Style storage - only for complex objects that need persistent styles
  struct {
    /* layout 1 styles removed */
  } styles;
} gravitymon_state_t;

static gravitymon_state_t g_state = {0};
static lv_disp_t* g_disp = NULL;

/**
 * Layout manager
 */
typedef struct {
  uint8_t current_layout;
  uint8_t total_layouts;
} gravitymon_layout_mgr_t;

static gravitymon_layout_mgr_t layout_mgr = {
    .current_layout = 0,
    .total_layouts = 0,
};

/**
 * Create a label with specific position, size, alignment, color and font.
 *
 * @param parent Parent LVGL object to attach the label to.
 * @param text Initial text for the label (may be an empty string).
 * @param x X position (pixels) relative to parent.
 * @param y Y position (pixels) relative to parent.
 * @param w Width (pixels).
 * @param h Height (pixels).
 * @param align Text alignment (LV_TEXT_ALIGN_CENTER, _LEFT, _RIGHT).
 * @param color Text color as an `lv_color_t`.
 * @param font Pointer to an LVGL font (or NULL to use default font).
 * @return Pointer to the created `lv_obj_t` label.
 */
static lv_obj_t* create_label(lv_obj_t* parent, const char* text, int32_t x,
                              int32_t y, int32_t w, int32_t h,
                              lv_text_align_t align, lv_color_t color,
                              const lv_font_t* font) {
  lv_obj_t* lbl = lv_label_create(parent);
  lv_label_set_text(lbl, text);
  lv_obj_set_pos(lbl, x, y);
  lv_obj_set_size(lbl, w, h);
  lv_obj_set_style_text_align(lbl, align, LV_PART_MAIN);
  lv_obj_set_style_text_color(lbl, color, LV_PART_MAIN);
  if (font) {
    lv_obj_set_style_text_font(lbl, font, LV_PART_MAIN);
  }
  return lbl;
}

/**
 * Create a button and its centered label.
 *
 * This helper creates a standard LVGL button at the given position/size
 * and places a centered label inside it.
 *
 * @param parent Parent LVGL object.
 * @param label_text Text for the button label.
 * @param x X position.
 * @param y Y position.
 * @param w Width.
 * @param h Height.
 * @param callback Event callback for `LV_EVENT_PRESSED`.
 * @return The created `lv_obj_t*` button.
 */
static lv_obj_t* create_button(lv_obj_t* parent, const char* label_text,
                               int32_t x, int32_t y, int32_t w, int32_t h,
                               lv_event_cb_t callback) {
  lv_obj_t* btn = lv_btn_create(parent);
  lv_obj_set_size(btn, w, h);
  lv_obj_set_pos(btn, x, y);
  lv_obj_add_event_cb(btn, callback, LV_EVENT_PRESSED, NULL);

  lv_obj_t* lbl = lv_label_create(btn);
  lv_label_set_text(lbl, label_text);
  lv_obj_center(lbl);
  return btn;
}

/**
 * Draw a 5-segment battery indicator with dynamic coloring.
 *
 * The returned container holds five child rectangles representing battery
 * segments; filled segments are colored according to the active theme.
 *
 * @param parent Parent LVGL object.
 * @param x X position (top-left of battery outline).
 * @param y Y position (top-left of battery outline).
 * @param percentage Battery percentage (0-100).
 * @return Container `lv_obj_t*` that contains the segment children.
 */
static lv_obj_t* create_battery_indicator(lv_obj_t* parent, int32_t x,
                                          int32_t y, uint8_t percentage) {
  if (percentage >= 200) {
    Log.error(F("UI: Invalid battery value %d" CR), percentage);
    return NULL;  // Invalid battery level, do not draw anything
  }

  // Create container for battery indicator (5 segments: 9px each + 1px gaps =
  // 55px wide, 20px tall)
  lv_obj_t* container = lv_obj_create(parent);
  lv_obj_set_pos(container, x, y);
  lv_obj_set_size(container, 55, 20);
  lv_obj_set_style_bg_opa(container, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(container, 2, 0);
  ui_theme_colors_t theme_colors = ui_get_theme_colors(
      g_state.flg_darkmode ? UI_THEME_DARK : UI_THEME_LIGHT);
  lv_obj_set_style_border_color(container, theme_colors.battery_border, 0);
  lv_obj_set_style_pad_all(container, 0, 0);

  // Determine fill color based on battery level
  lv_color_t fill_color;
  if (percentage <= 20) {
    fill_color = theme_colors.battery_low;
  } else if (percentage <= 40) {
    fill_color = theme_colors.battery_mid;
  } else {
    fill_color = theme_colors.battery_good;
  }

  // Calculate how many segments should be filled (each segment = 20%)
  uint8_t filled_segments =
      (percentage + 19) / 20;  // Round up: 1-20% = 1 seg, 21-40% = 2 seg, etc.

  // Create 5 rectangles with 1px gaps (positioned inside the container).
  // Add small inner margins so the container border remains visible.
  for (int i = 0; i < 5; i++) {
    // Leave 2px padding from left/top so container border shows, then 1px gaps
    lv_obj_t* segment = lv_obj_create(container);
    lv_obj_set_pos(segment, 2 + (i * 10),
                   2);  // 9px width + 1px gap, 3px top pad
    lv_obj_set_size(segment, 7,
                    12);  // Leave gap at top and bottom inside border
    lv_obj_set_style_bg_opa(segment, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(segment, 0, 0);
    lv_obj_set_style_border_color(segment, theme_colors.battery_border, 0);
    lv_obj_set_style_pad_all(segment, 0, 0);

    // Fill only segments that should be filled
    if (i < filled_segments) {
      lv_obj_set_style_bg_color(segment, fill_color, 0);
      lv_obj_set_style_bg_opa(segment, LV_OPA_COVER, 0);
    } else {
      // Make empty segments transparent so gaps show through
      lv_obj_set_style_bg_opa(segment, LV_OPA_TRANSP, 0);
    }
  }

  return container;
}

/**
 * Delete any UI objects created by the active layout and clear pointers.
 *
 * This frees LVGL objects referenced in the `g_state` structure and sets
 * those pointers to NULL so a fresh layout can be created.
 */
static void gravitymon_gateway_cleanup_layout(void) {
  if (g_state.lbl_name) {
    lv_obj_del(g_state.lbl_name);
    g_state.lbl_name = NULL;
  }
  if (g_state.lbl_index) {
    lv_obj_del(g_state.lbl_index);
    g_state.lbl_index = NULL;
  }
  if (g_state.lbl_temp) {
    lv_obj_del(g_state.lbl_temp);
    g_state.lbl_temp = NULL;
  }
  if (g_state.lbl_measurement) {
    lv_obj_del(g_state.lbl_measurement);
    g_state.lbl_measurement = NULL;
  }
  if (g_state.lbl_battery) {
    lv_obj_del(g_state.lbl_battery);
    g_state.lbl_battery = NULL;
  }
  if (g_state.lbl_time) {
    lv_obj_del(g_state.lbl_time);
    g_state.lbl_time = NULL;
  }
  if (g_state.lbl_status) {
    lv_obj_del(g_state.lbl_status);
    g_state.lbl_status = NULL;
  }
  for (int i = 0; i < 5; i++) {
    if (g_state.lbl_history[i]) {
      lv_obj_del(g_state.lbl_history[i]);
      g_state.lbl_history[i] = NULL;
    }
  }
  if (g_state.lbl_rssi) {
    lv_obj_del(g_state.lbl_rssi);
    g_state.lbl_rssi = NULL;
  }
  if (g_state.lbl_type) {
    lv_obj_del(g_state.lbl_type);
    g_state.lbl_type = NULL;
  }
  if (g_state.lbl_source) {
    lv_obj_del(g_state.lbl_source);
    g_state.lbl_source = NULL;
  }

  /* layout 1 widgets cleaned up as part of removal */
  if (g_state.wid_battery_canvas) {
    lv_obj_del(g_state.wid_battery_canvas);
    g_state.wid_battery_canvas = NULL;
  }
}

/**
 * Common initialization for the Gravitymon Gateway UI.
 *
 * Performs one-time setup such as storing the display reference, applying
 * the screen background color from the active theme, and initializing
 * default data buffers and the layout manager.
 *
 * @param disp Display pointer used by LVGL.
 * @param darkmode True to select dark theme defaults, false for light.
 */
static void gravitymon_gateway_init_common(lv_disp_t* disp, bool darkmode) {
  g_disp = disp;
  g_state.flg_darkmode = darkmode;

  // Get active screen
  lv_obj_t* scr = lv_scr_act();
  if (!scr) {
    Log.error(F("UI: No active screen" CR));
    return;
  }

  // Get theme colors
  ui_theme_colors_t theme_colors =
      ui_get_theme_colors(darkmode ? UI_THEME_DARK : UI_THEME_LIGHT);

  // Apply background
  lv_obj_set_style_bg_color(scr, theme_colors.bg, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

  // Initialize gravity range (default: 1000-1100)
  g_state.measurement_min = 1000;
  g_state.measurement_max = 1100;

  // Initialize layout manager
  layout_mgr.current_layout = 0;
  layout_mgr.total_layouts = 2;

  // Initialize last-darkmode tracking so theme is applied on first loop
  g_state.flg_last_darkmode = darkmode;

  Log.info(F("UI: Gravitymon Gateway common init complete (%s mode)" CR),
           darkmode ? "dark" : "light");
}

/**
 * Setup layout 0 (compact): creates a dense layout with many text elements.
 *
 * This function constructs the labels, history list, and navigation
 * buttons used for the compact view.
 */
static void gravitymon_gateway_setup_layout_0(void) {
  lv_obj_t* scr = lv_scr_act();
  if (!scr) return;

  ui_theme_colors_t theme_colors = ui_get_theme_colors(
      g_state.flg_darkmode ? UI_THEME_DARK : UI_THEME_LIGHT);

  // Device name (5, 5, 250, 36) - left aligned, 20pt font
  g_state.lbl_name = create_label(scr, "", 5, 5, 250, 34, LV_TEXT_ALIGN_CENTER,
                                  theme_colors.text, &lv_font_montserrat_20);
  lv_obj_set_style_border_color(g_state.lbl_name, theme_colors.border, 0);
  lv_obj_set_style_border_width(g_state.lbl_name, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_name, 8, 0);
  lv_obj_set_style_bg_color(g_state.lbl_name, theme_colors.panel_alt_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_name, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_pad_top(g_state.lbl_name, 6, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(g_state.lbl_name, 4, LV_PART_MAIN);

  // Device index (260, 5, 54, 36) - center aligned, 16pt font
  g_state.lbl_index =
      create_label(scr, "", 260, 5, 54, 34, LV_TEXT_ALIGN_CENTER,
                   theme_colors.text, &lv_font_montserrat_16);
  lv_obj_set_style_border_color(g_state.lbl_index, theme_colors.border, 0);
  lv_obj_set_style_border_width(g_state.lbl_index, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_index, 6, 0);
  lv_obj_set_style_bg_color(g_state.lbl_index, theme_colors.muted_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_index, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_pad_top(g_state.lbl_index, 6, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(g_state.lbl_index, 4, LV_PART_MAIN);

  // First row: Gravity (left) and Temperature (right) - larger display
  g_state.lbl_measurement =
      create_label(scr, "", 5, 44, 150, 30, LV_TEXT_ALIGN_CENTER,
                   theme_colors.text, &lv_font_montserrat_16);
  lv_obj_set_style_pad_top(g_state.lbl_measurement, 6, LV_PART_MAIN);
  lv_obj_set_style_bg_color(g_state.lbl_measurement, theme_colors.panel_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_measurement, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_border_color(g_state.lbl_measurement, theme_colors.border,
                                0);
  lv_obj_set_style_border_width(g_state.lbl_measurement, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_measurement, 12, 0);

  g_state.lbl_temp =
      create_label(scr, "", 165, 44, 150, 30, LV_TEXT_ALIGN_CENTER,
                   theme_colors.text, &lv_font_montserrat_16);
  lv_obj_set_style_pad_top(g_state.lbl_temp, 6, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(g_state.lbl_temp, 4, LV_PART_MAIN);
  lv_obj_set_style_bg_color(g_state.lbl_temp, theme_colors.panel_alt_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_temp, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_border_color(g_state.lbl_temp, theme_colors.border, 0);
  lv_obj_set_style_border_width(g_state.lbl_temp, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_temp, 10, 0);

  // Second row: Timestamp (left) and Voltage/Battery (right)
  g_state.lbl_time = create_label(scr, "", 5, 79, 150, 30, LV_TEXT_ALIGN_CENTER,
                                  theme_colors.text, &lv_font_montserrat_14);
  lv_obj_set_style_pad_top(g_state.lbl_time, 8, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(g_state.lbl_time, 4, LV_PART_MAIN);
  lv_obj_set_style_bg_color(g_state.lbl_time, theme_colors.muted_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_time, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_border_color(g_state.lbl_time, theme_colors.border, 0);
  lv_obj_set_style_border_width(g_state.lbl_time, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_time, 6, 0);

  g_state.lbl_battery =
      create_label(scr, "", 165, 79, 150, 30, LV_TEXT_ALIGN_CENTER,
                   theme_colors.text, &lv_font_montserrat_16);
  lv_obj_set_style_pad_top(g_state.lbl_battery, 4, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(g_state.lbl_battery, 2, LV_PART_MAIN);
  lv_obj_set_style_bg_color(g_state.lbl_battery, theme_colors.panel_alt_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_battery, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_border_color(g_state.lbl_battery, theme_colors.border, 0);
  lv_obj_set_style_border_width(g_state.lbl_battery, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_battery, 6, 0);

  // Status bar (5, 219, 310, 18) - center aligned, 12pt font
  g_state.lbl_status =
      create_label(scr, "", 5, 219, 310, 18, LV_TEXT_ALIGN_CENTER,
                   theme_colors.text, &lv_font_montserrat_12);
  lv_obj_set_style_bg_color(g_state.lbl_status, theme_colors.status_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_status, LV_OPA_60, LV_PART_MAIN);
  lv_obj_set_style_border_color(g_state.lbl_status, theme_colors.border, 0);
  lv_obj_set_style_border_width(g_state.lbl_status, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_status, 0, 0);

  // History items (5, 114+i*21, 310, 18) - left aligned, 12pt font
  for (int i = 0; i < 5; i++) {
    g_state.lbl_history[i] =
        create_label(scr, "", 5, 114 + (i * 21), 310, 18, LV_TEXT_ALIGN_LEFT,
                     theme_colors.text, &lv_font_montserrat_12);
  }
}

/**
 * Setup layout 1
 */
static void gravitymon_gateway_setup_layout_1(void) {
  lv_obj_t* scr = lv_scr_act();
  if (!scr) return;

  ui_theme_colors_t theme_colors = ui_get_theme_colors(
      g_state.flg_darkmode ? UI_THEME_DARK : UI_THEME_LIGHT);

  // Font styles are set directly on labels like chamber controller

  // Header: Device name on left (5, 5, 250, 36) - left aligned, 20pt font
  // Make header span from top of battery (y=5) to bottom of index (y=52) =>
  // height=47
  g_state.lbl_name = create_label(scr, "", 5, 5, 245, 47, LV_TEXT_ALIGN_LEFT,
                                  theme_colors.text, &lv_font_montserrat_20);
  // Center text vertically inside the taller header
  lv_obj_set_style_pad_top(g_state.lbl_name, 12, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(g_state.lbl_name, 12, LV_PART_MAIN);
  lv_obj_set_style_border_color(g_state.lbl_name, theme_colors.border, 0);
  lv_obj_set_style_border_width(g_state.lbl_name, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_name, 8, 0);
  lv_obj_set_style_bg_color(g_state.lbl_name, theme_colors.panel_alt_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_name, LV_OPA_80, LV_PART_MAIN);

  // Battery indicator (top-right, 260, 5) - 55px wide
  g_state.wid_battery_canvas =
      create_battery_indicator(scr, 260, 5, g_state.battery_percentage);
  if (g_state.wid_battery_canvas) {
    lv_obj_set_style_border_color(g_state.wid_battery_canvas,
                                  theme_colors.border, 0);
    lv_obj_set_style_border_width(g_state.wid_battery_canvas, 0, 0);
    lv_obj_set_style_radius(g_state.wid_battery_canvas, 6, 0);
  }

  // Device index/page indicator (295, 5, 25, 36) - center aligned, 12pt font
  g_state.lbl_index =
      create_label(scr, "", 260, 32, 55, 20, LV_TEXT_ALIGN_CENTER,
                   theme_colors.text, &lv_font_montserrat_14);
  lv_obj_set_style_border_color(g_state.lbl_index, theme_colors.border, 0);
  lv_obj_set_style_border_width(g_state.lbl_index, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_index, 6, 0);
  lv_obj_set_style_bg_color(g_state.lbl_index, theme_colors.muted_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_index, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_pad_top(g_state.lbl_index, 2, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(g_state.lbl_index, 2, LV_PART_MAIN);

  // Large gravity display (centered, 60-130px vertically)
  g_state.lbl_measurement =
      create_label(scr, "", 10, 62, 300, 50, LV_TEXT_ALIGN_CENTER,
                   theme_colors.text, &lv_font_montserrat_20);
  lv_obj_set_style_pad_top(g_state.lbl_measurement, 10, LV_PART_MAIN);
  lv_obj_set_style_bg_color(g_state.lbl_measurement, theme_colors.panel_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_measurement, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_border_color(g_state.lbl_measurement, theme_colors.border,
                                0);
  lv_obj_set_style_border_width(g_state.lbl_measurement, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_measurement, 12, 0);

  // Temperature row (140-165px, full width centered) - center aligned, 20pt
  // font
  g_state.lbl_temp =
      create_label(scr, "", 40, 126, 240, 29, LV_TEXT_ALIGN_CENTER,
                   theme_colors.text, &lv_font_montserrat_20);
  lv_obj_set_style_pad_top(g_state.lbl_temp, 4, LV_PART_MAIN);
  lv_obj_set_style_bg_color(g_state.lbl_temp, theme_colors.panel_alt_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_temp, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_border_color(g_state.lbl_temp, theme_colors.border, 0);
  lv_obj_set_style_border_width(g_state.lbl_temp, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_temp, 10, 0);

  // Updated layout with two rows for the fields
  // Row 1: Time (left) and RSSI (center)
  g_state.lbl_time =
      create_label(scr, "", 5, 165, 150, 22, LV_TEXT_ALIGN_CENTER,
                   theme_colors.text, &lv_font_montserrat_12);
  lv_obj_set_style_pad_top(g_state.lbl_time, 2, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(g_state.lbl_time, 2, LV_PART_MAIN);
  lv_obj_set_style_bg_color(g_state.lbl_time, theme_colors.muted_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_time, LV_OPA_70, LV_PART_MAIN);
  lv_obj_set_style_border_color(g_state.lbl_time, theme_colors.border, 0);
  lv_obj_set_style_border_width(g_state.lbl_time, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_time, 8, 0);

  g_state.lbl_rssi =
      create_label(scr, "", 160, 165, 150, 22, LV_TEXT_ALIGN_CENTER,
                   theme_colors.text, &lv_font_montserrat_12);
  lv_obj_set_style_pad_top(g_state.lbl_rssi, 2, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(g_state.lbl_rssi, 2, LV_PART_MAIN);
  lv_obj_set_style_bg_color(g_state.lbl_rssi, theme_colors.muted_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_rssi, LV_OPA_70, LV_PART_MAIN);
  lv_obj_set_style_border_color(g_state.lbl_rssi, theme_colors.border, 0);
  lv_obj_set_style_border_width(g_state.lbl_rssi, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_rssi, 8, 0);

  // Row 2: API type (left) and Source (right)
  g_state.lbl_type =
      create_label(scr, "", 5, 192, 150, 22, LV_TEXT_ALIGN_CENTER,
                   theme_colors.text, &lv_font_montserrat_12);
  lv_obj_set_style_pad_top(g_state.lbl_type, 2, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(g_state.lbl_type, 2, LV_PART_MAIN);
  lv_obj_set_style_bg_color(g_state.lbl_type, theme_colors.muted_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_type, LV_OPA_70, LV_PART_MAIN);
  lv_obj_set_style_border_color(g_state.lbl_type, theme_colors.border, 0);
  lv_obj_set_style_border_width(g_state.lbl_type, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_type, 8, 0);

  g_state.lbl_source =
      create_label(scr, "", 160, 192, 150, 22, LV_TEXT_ALIGN_CENTER,
                   theme_colors.text, &lv_font_montserrat_12);
  lv_obj_set_style_pad_top(g_state.lbl_source, 2, LV_PART_MAIN);
  lv_obj_set_style_pad_bottom(g_state.lbl_source, 2, LV_PART_MAIN);
  lv_obj_set_style_bg_color(g_state.lbl_source, theme_colors.muted_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_source, LV_OPA_70, LV_PART_MAIN);
  lv_obj_set_style_border_color(g_state.lbl_source, theme_colors.border, 0);
  lv_obj_set_style_border_width(g_state.lbl_source, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_source, 8, 0);

  // Measurement and Temperature fields moved up by 4px
  g_state.lbl_measurement =
      create_label(scr, "", 40, 61, 240, 50, LV_TEXT_ALIGN_CENTER,
                   theme_colors.text, &lv_font_montserrat_28);
  lv_obj_set_style_pad_top(g_state.lbl_measurement, 10, LV_PART_MAIN);
  lv_obj_set_style_bg_color(g_state.lbl_measurement, theme_colors.panel_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_measurement, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_border_color(g_state.lbl_measurement, theme_colors.border,
                                0);
  lv_obj_set_style_border_width(g_state.lbl_measurement, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_measurement, 12, 0);

  g_state.lbl_temp =
      create_label(scr, "", 40, 126, 240, 29, LV_TEXT_ALIGN_CENTER,
                   theme_colors.text, &lv_font_montserrat_20);
  lv_obj_set_style_pad_top(g_state.lbl_temp, 4, LV_PART_MAIN);
  lv_obj_set_style_bg_color(g_state.lbl_temp, theme_colors.panel_alt_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_temp, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_border_color(g_state.lbl_temp, theme_colors.border, 0);
  lv_obj_set_style_border_width(g_state.lbl_temp, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_temp, 10, 0);

  // Status bar (fill bottom of screen) - center aligned, 12pt font
  g_state.lbl_status =
      create_label(scr, "", 0, 219, 320, 21, LV_TEXT_ALIGN_CENTER,
                   theme_colors.text, &lv_font_montserrat_12);
  lv_obj_set_style_bg_color(g_state.lbl_status, theme_colors.status_bg,
                            LV_PART_MAIN);
  lv_obj_set_style_bg_opa(g_state.lbl_status, LV_OPA_60, LV_PART_MAIN);
  lv_obj_set_style_border_color(g_state.lbl_status, theme_colors.border, 0);
  lv_obj_set_style_border_width(g_state.lbl_status, 0, 0);
  lv_obj_set_style_radius(g_state.lbl_status, 0,
                          0);  // No radius for full bottom bar
}

/**
 * Fetch the current active layout
 *
 * @return layout_id Layout index
 */
uint8_t gravitymon_gateway_get_layout() { return layout_mgr.current_layout; }

/**
 * Switch to the specified layout ID.
 *
 * Cleans up the current layout, resets state, and constructs the chosen
 * layout. If the requested `layout_id` is invalid it will wrap to 0.
 *
 * @param layout_id Layout index to activate.
 */
void gravitymon_gateway_set_layout(uint8_t layout_id) {
  if (layout_id >= layout_mgr.total_layouts) {
    layout_id = 0;
  }

  // If the requested layout is already active, do nothing
  if (layout_id == layout_mgr.current_layout) {
    Log.verbose(F("UI: Layout %d already active" CR), layout_id);
    return;
  }

  // Cleanup current layout objects
  gravitymon_gateway_cleanup_layout();

  // Clear entire state for clean new layout
  memset(&g_state, 0, sizeof(g_state));

  // Update layout ID
  layout_mgr.current_layout = layout_id;

  // Setup new layout
  if (layout_id == 0) {
    gravitymon_gateway_setup_layout_0();
  } else if (layout_id == 1) {
    gravitymon_gateway_setup_layout_1();
  }

  Log.verbose(F("UI: Layout switched to %d" CR), layout_id);
}

/**
 * Initialize the Gravitymon Gateway UI subsystem.
 *
 * Performs common initialization and then creates the requested layout.
 * This is the entry point used by the application to bring up the UI.
 *
 * @param disp LVGL display pointer.
 * @param darkmode Select dark theme when true.
 * @param layout_id Initial layout index to use.
 */
void gravitymon_gateway_init(lv_disp_t* disp, bool darkmode,
                             uint8_t layout_id) {
  if (!disp) {
    Log.error(F("UI: gravitymon_gateway_init: NULL display" CR));
    return;
  }

  memset(&g_state, 0, sizeof(g_state));

  // Common initialization (one-time setup)
  gravitymon_gateway_init_common(disp, darkmode);

  // Clamp layout_id to valid range (0..1)
  if (layout_id >= 2) {
    layout_id = 0;
  }
  layout_mgr.current_layout = layout_id;

  // Setup requested layout
  if (layout_id == 0) {
    gravitymon_gateway_setup_layout_0();
  } else if (layout_id == 1) {
    gravitymon_gateway_setup_layout_1();
  }

  Log.info(F("UI: Gravitymon Gateway UI initialized (320x240 landscape, %s "
             "mode) - layout %d active" CR),
           darkmode ? "dark" : "light", layout_id);
}

/**
 * Update the cached device name shown in the UI.
 *
 * The new value is copied into `g_state.data_name` and will be applied
 * to visible labels on the next UI loop iteration.
 *
 * @param name Null-terminated string with the device name.
 */
void gravitymon_gateway_set_name(const char* name) {
  if (!name) return;
  snprintf(g_state.data_name, sizeof(g_state.data_name), " %s", name);
}

/**
 * Set the device index text (e.g. "1/3").
 *
 * The value is formatted into `g_state.data_index` and will be applied
 * during the next UI update.
 *
 * @param current Current device number.
 * @param total Total number of devices.
 */
void gravitymon_gateway_set_index(uint8_t current, uint8_t total) {
  snprintf(g_state.data_index, sizeof(g_state.data_index), "%d/%d", current,
           total);
}

/**
 * Configure the measurement gravity min and max range.
 *
 * Updates both the stored min/max values and the gauge range if the
 * gauge object is present.
 *
 * @param min Minimum value.
 * @param max Maximum value.
 * @param unit 'G' for SG gravity formatting, otherwise pressure formatting.
 */
void gravitymon_gateway_set_gravity_range(float min, float max, char unit) {
  /* layout 1 gauge removed; only update stored numeric range and strings */

  if (unit == 'G') {
    g_state.measurement_min = min * 1000.0f;
    g_state.measurement_max = max * 1000.0f;
    snprintf(g_state.data_measurement_min, sizeof(g_state.data_measurement_min),
             "%.2f", min);
    snprintf(g_state.data_measurement_max, sizeof(g_state.data_measurement_max),
             "%.2f", max);
  } else {
    g_state.measurement_min = min * 10.0f;
    g_state.measurement_max = max * 10.0f;
    snprintf(g_state.data_measurement_min, sizeof(g_state.data_measurement_min),
             "%.1f", min);
    snprintf(g_state.data_measurement_max, sizeof(g_state.data_measurement_max),
             "%.1f", max);
  }
}

/**
 * Update the current measurement gravity string.
 *
 * Formats the numeric value into `g_state.data_measurement` according to
 * the provided unit and updates the internal numeric representation used
 * by the gauge.
 *
 * @param gravity Numeric measurement value.
 * @param unit 'G' for SG gravity formatting, otherwise pressure.
 */
void gravitymon_gateway_set_gravity(float gravity, char unit) {
  if (unit == 'G') {
    snprintf(g_state.data_measurement, sizeof(g_state.data_measurement),
             "%.2f SG", gravity);
    g_state.measurement = gravity * 100.0f;
  } else if (unit == 'P') {
    snprintf(g_state.data_measurement, sizeof(g_state.data_measurement),
             "%.1f P", gravity);
    g_state.measurement = gravity * 10.0f;
  } else {
    snprintf(g_state.data_measurement, sizeof(g_state.data_measurement), "");
    g_state.measurement = 0;
  }
}

/**
 * Configure the measurement pressure min and max range.
 *
 * Updates both the stored min/max values and the gauge range if the
 * gauge object is present.
 *
 * @param min Minimum value.
 * @param max Maximum value.
 * @param unit 'P' for PSI, 'B' for Bar amd 'k' for KPa
 */
void gravitymon_gateway_set_pressure_range(float min, float max,
                                           const char* unit) {
  /* layout 1 gauge removed; only update stored numeric range and strings */

  if (strcmp(unit, "PSI") == 0) {
    g_state.measurement_min = min * 100.0f;
    g_state.measurement_max = max * 100.0f;
    snprintf(g_state.data_measurement_min, sizeof(g_state.data_measurement_min),
             "%.2f", min);
    snprintf(g_state.data_measurement_max, sizeof(g_state.data_measurement_max),
             "%.2f", max);
  } else if (strcmp(unit, "Bar") == 0) {
    g_state.measurement_min = min * 10.0f;
    g_state.measurement_max = max * 10.0f;
    snprintf(g_state.data_measurement_min, sizeof(g_state.data_measurement_min),
             "%.1f", min);
    snprintf(g_state.data_measurement_max, sizeof(g_state.data_measurement_max),
             "%.1f", max);
  } else if (strcmp(unit, "kPa") == 0) {
    g_state.measurement_min = min;
    g_state.measurement_max = max;
    snprintf(g_state.data_measurement_min, sizeof(g_state.data_measurement_min),
             "%.0f", min);
    snprintf(g_state.data_measurement_max, sizeof(g_state.data_measurement_max),
             "%.0f", max);
  } else {
    g_state.measurement_min = min;
    g_state.measurement_max = max;
    snprintf(g_state.data_measurement_min, sizeof(g_state.data_measurement_min),
             "");
    snprintf(g_state.data_measurement_max, sizeof(g_state.data_measurement_max),
             "");
  }
}

/**
 * Update the current measurement pressure string.
 *
 * Formats the numeric value into `g_state.data_measurement` according to
 * the provided unit and updates the internal numeric representation used
 * by the gauge.
 *
 * @param pressure Numeric measurement value.
 * @param unit 'G' for SG gravity formatting, otherwise pressure.
 */
void gravitymon_gateway_set_pressure(float pressure, float pressure2,
                                     const char* unit) {
  if (isnan(pressure) && isnan(pressure2)) {
    snprintf(g_state.data_measurement, sizeof(g_state.data_measurement), "--");
    g_state.measurement = 0;
  } else if (isnan(pressure2)) {
    if (strcmp(unit, "PSI") == 0) {
      g_state.measurement = pressure * 100.0f;
      snprintf(g_state.data_measurement, sizeof(g_state.data_measurement),
               "%.2f PSI", pressure);
    } else if (strcmp(unit, "Bar") == 0) {
      g_state.measurement = pressure * 10.0f;
      snprintf(g_state.data_measurement, sizeof(g_state.data_measurement),
               "%.1f Bar", pressure);
    } else if (strcmp(unit, "kPa") == 0) {
      g_state.measurement = pressure;
      snprintf(g_state.data_measurement, sizeof(g_state.data_measurement),
               "%.0f kPa", pressure);
    }
  } else {
    if (strcmp(unit, "PSI") == 0) {
      g_state.measurement = pressure * 100.0f;
      snprintf(g_state.data_measurement, sizeof(g_state.data_measurement),
               "%.2f / %.2f PSI", pressure, pressure2);
    } else if (strcmp(unit, "Bar") == 0) {
      g_state.measurement = pressure * 10.0f;
      snprintf(g_state.data_measurement, sizeof(g_state.data_measurement),
               "%.1f / %.1f Bar", pressure, pressure2);
    } else if (strcmp(unit, "kPa") == 0) {
      g_state.measurement = pressure;
      snprintf(g_state.data_measurement, sizeof(g_state.data_measurement),
               "%.0f / %.0f kPa", pressure, pressure2);
    }
  }
}

/**
 * Update the cached battery percentage string and numeric value.
 *
 * @param percent Battery percentage (0..100).
 */
void gravitymon_gateway_set_battery_percentage(float percent) {
  if (isnan(percent)) {
    snprintf(g_state.data_battery_percentage,
             sizeof(g_state.data_battery_percentage), "--");
    g_state.battery_percentage = (uint8_t)255;
  } else {
    snprintf(g_state.data_battery_percentage,
             sizeof(g_state.data_battery_percentage), "%.0f%%", percent);
    g_state.battery_percentage = (uint8_t)percent;
  }
}

/**
 * Update the cached battery voltage string (e.g. "3.70V").
 *
 * @param voltage Battery voltage in volts.
 */
void gravitymon_gateway_set_battery_voltage(float voltage) {
  if (isnan(voltage)) {
    snprintf(g_state.data_battery_voltage, sizeof(g_state.data_battery_voltage),
             "--");
  } else {
    snprintf(g_state.data_battery_voltage, sizeof(g_state.data_battery_voltage),
             "%.2fV", voltage);
  }
}

/**
 * Update the connection/device type string.
 *
 * Stores the provided `type` into `g_state.data_type` so it will be
 * reflected on-screen during the next UI loop iteration.
 *
 * @param type Null-terminated type string (e.g. "WiFi", "BLE").
 */
void gravitymon_gateway_set_type(const char* type) {
  snprintf(g_state.data_type, sizeof(g_state.data_type), "%s", type);
}

/**
 * Update the device/source identifier string.
 *
 * Stores the provided `source` into `g_state.data_source` so it will be
 * reflected on-screen during the next UI loop iteration.
 *
 * @param source Null-terminated source string (e.g. MAC address, location).
 */
void gravitymon_gateway_set_source(const char* source) {
  snprintf(g_state.data_source, sizeof(g_state.data_source), "%s", source);
}

/**
 * Set the temperature display string.
 *
 * Formats temperature according to the unit and stores it in the
 * `g_state.data_temp` buffer. If `temp` is NaN the display shows "--".
 *
 * @param temp Temperature value.
 * @param temp2 Secondary temperature value
 * @param unit 'C' for Celsius, otherwise Fahrenheit.
 */
void gravitymon_gateway_set_temp(float temp, float temp2, char unit) {
  if (isnan(temp) && isnan(temp2)) {
    snprintf(g_state.data_temp, sizeof(g_state.data_temp), "--");
  } else if (isnan(temp2)) {
    if (unit == 'C')
      snprintf(g_state.data_temp, sizeof(g_state.data_temp), "%.2f°C", temp);
    else
      snprintf(g_state.data_temp, sizeof(g_state.data_temp), "%.1f°F", temp);
  } else {
    if (unit == 'C')
      snprintf(g_state.data_temp, sizeof(g_state.data_temp), "%.2f / %.2f°C",
               temp, temp2);
    else
      snprintf(g_state.data_temp, sizeof(g_state.data_temp), "%.1f / %.1f°F",
               temp, temp2);
  }
}

/**
 * Update the RSSI text shown in layout 2.
 *
 * @param rssi_dbm RSSI in dBm.
 */
void gravitymon_gateway_set_rssi(int8_t rssi_dbm) {
  snprintf(g_state.data_rssi, sizeof(g_state.data_rssi), "%d dBm", rssi_dbm);
}

/**
 * Set device timestamp and compute elapsed time string.
 *
 * Parses an incoming timestamp of the form "YYYY-MM-DD HH:MM:SS" and
 * stores a human-friendly "time ago" string into `g_state.data_time_ago`.
 * If parsing fails the field is set to "--".
 *
 * @param time_str Null-terminated timestamp string.
 */
void gravitymon_gateway_set_time(const char* time_str) {
  if (!time_str) return;

  // Store raw timestamp
  snprintf(g_state.data_time, sizeof(g_state.data_time), "%s", time_str);

  // Parse incoming time string: "YYYY-MM-DD HH:MM:SS"
  struct tm parsed_time = {};
  if (sscanf(time_str, "%d-%d-%d %d:%d:%d", &parsed_time.tm_year,
             &parsed_time.tm_mon, &parsed_time.tm_mday, &parsed_time.tm_hour,
             &parsed_time.tm_min, &parsed_time.tm_sec) != 6) {
    snprintf(g_state.data_time_ago, sizeof(g_state.data_time_ago), "--");
    return;
  }

  // Adjust parsed values to struct tm format (year since 1900, month 0-11)
  parsed_time.tm_year -= 1900;
  parsed_time.tm_mon -= 1;
  parsed_time.tm_isdst = -1;  // Let mktime determine DST

  // Convert parsed time to time_t
  time_t parsed_timestamp = mktime(&parsed_time);
  if (parsed_timestamp == -1) {
    snprintf(g_state.data_time_ago, sizeof(g_state.data_time_ago), "--");
    return;
  }

  // Get current time
  time_t now = std::time(NULL);

  // Calculate elapsed time in seconds
  int64_t elapsed_seconds = difftime(now, parsed_timestamp);
  if (elapsed_seconds < 0) {
    snprintf(g_state.data_time_ago, sizeof(g_state.data_time_ago), "--");
    return;
  }

  // Format as "Xh Xm Xs" or shorter variants
  int64_t hours = elapsed_seconds / 3600;
  int64_t minutes = (elapsed_seconds % 3600) / 60;
  int64_t seconds = elapsed_seconds % 60;

  if (hours > 0) {
    snprintf(g_state.data_time_ago, sizeof(g_state.data_time_ago),
             "%ldh %ldm %lds", hours, minutes, seconds);
  } else if (minutes > 0) {
    snprintf(g_state.data_time_ago, sizeof(g_state.data_time_ago), "%ldm %lds",
             minutes, seconds);
  } else {
    snprintf(g_state.data_time_ago, sizeof(g_state.data_time_ago), "%lds",
             seconds);
  }
}

/**
 * Store a historical measurement value for the given index (0..4).
 *
 * The string is formatted and placed into the `g_state.data_history`
 * buffer for later display.
 *
 * @param index History index (0-4).
 * @param value Numeric value to store.
 */
void gravitymon_gateway_set_history(uint8_t index, const char* value) {
  if (index >= 5 || !value) return;
  snprintf(g_state.data_history[index], sizeof(g_state.data_history[index]),
           "%s", value);
}

/**
 * Set the status bar message text.
 *
 * @param status Null-terminated status message string.
 */
void gravitymon_gateway_set_status(const char* status) {
  if (!status) return;
  snprintf(g_state.data_status, sizeof(g_state.data_status), "%s", status);
}

/**
 * Toggle the internal dark/light theme flag used when drawing UI elements.
 *
 * The actual UI update to apply the theme occurs during the next loop
 * (or when layouts are rebuilt).
 *
 * @param darkmode True to enable dark theme, false for light.
 */
void gravitymon_gateway_set_theme(bool darkmode) {
  g_state.flg_darkmode = darkmode;
}

/**
 * Layout 0 loop handler: push cached strings into visible labels.
 *
 * This function is intended to be called from the LVGL main/background
 * thread to update on-screen content from the `g_state` buffers.
 */
static void gravitymon_gateway_loop_layout_0(void) {
  // Update all text labels
  if (g_state.lbl_name) {
    lv_label_set_text(g_state.lbl_name, g_state.data_name);
  }
  if (g_state.lbl_index) {
    lv_label_set_text(g_state.lbl_index, g_state.data_index);
  }
  if (g_state.lbl_temp) {
    lv_label_set_text(g_state.lbl_temp, g_state.data_temp);
  }
  if (g_state.lbl_measurement) {
    lv_label_set_text(g_state.lbl_measurement, g_state.data_measurement);
  }
  if (g_state.lbl_battery) {
    lv_label_set_text(g_state.lbl_battery,
                      strcmp(g_state.data_battery_voltage, "--")
                          ? g_state.data_battery_voltage
                          : g_state.data_battery_percentage);
  }
  if (g_state.lbl_time) {
    lv_label_set_text(g_state.lbl_time, g_state.data_time);
  }
  if (g_state.lbl_status) {
    lv_label_set_text(g_state.lbl_status, g_state.data_status);
  }

  // Update history items
  for (int i = 0; i < 5; i++) {
    if (g_state.lbl_history[i]) {
      lv_label_set_text(g_state.lbl_history[i], g_state.data_history[i]);
    }
  }
}

/**
 * Layout 1 loop handler
 */
static void gravitymon_gateway_loop_layout_1(void) {
  if (g_state.lbl_name) {
    lv_label_set_text(g_state.lbl_name, g_state.data_name);
  }
  if (g_state.lbl_index) {
    lv_label_set_text(g_state.lbl_index, g_state.data_index);
  }
  if (g_state.wid_battery_canvas) {
    lv_obj_del(g_state.wid_battery_canvas);
  }
  // Create battery canbas
  g_state.wid_battery_canvas = create_battery_indicator(
      lv_scr_act(), 260, 5, g_state.battery_percentage);

  if (g_state.lbl_measurement) {
    lv_label_set_text(g_state.lbl_measurement, g_state.data_measurement);
  }
  if (g_state.lbl_temp) {
    lv_label_set_text(g_state.lbl_temp, g_state.data_temp);
  }
  if (g_state.lbl_time) {
    lv_label_set_text(g_state.lbl_time, g_state.data_time_ago);
  }
  if (g_state.lbl_rssi) {
    lv_label_set_text(g_state.lbl_rssi, g_state.data_rssi);
  }
  if (g_state.lbl_type) {
    lv_label_set_text(g_state.lbl_type, g_state.data_type);
  }
  if (g_state.lbl_source) {
    lv_label_set_text(g_state.lbl_source, g_state.data_source);
  }
  if (g_state.lbl_status) {
    lv_label_set_text(g_state.lbl_status, g_state.data_status);
  }
}

/**
 * Gravitymon gateway UI main loop handler.
 *
 * Dispatches to the active layout's update function and applies theme
 * updates when required. This should be called periodically from the
 * LVGL task context.
 */
void gravitymon_gateway_loop(void) {
  if (layout_mgr.current_layout == 0) {
    gravitymon_gateway_loop_layout_0();
  } else if (layout_mgr.current_layout == 1) {
    gravitymon_gateway_loop_layout_1();
  }

  // Apply theme only when it changed since last application
  lv_obj_t* scr = lv_scr_act();
  if (scr && g_state.flg_darkmode != g_state.flg_last_darkmode) {
    ui_theme_colors_t theme_colors = ui_get_theme_colors(
        g_state.flg_darkmode ? UI_THEME_DARK : UI_THEME_LIGHT);
    ui_apply_theme_to_screen(scr, &theme_colors);
    g_state.flg_last_darkmode = g_state.flg_darkmode;
  }
}

/**
 * Cleanup and reset all UI state.
 *
 * Clears `g_state` so subsequent initialization starts from a clean state.
 */
void gravitymon_gateway_cleanup(void) { memset(&g_state, 0, sizeof(g_state)); }

#endif  // ENABLE_LVGL

// EOF
