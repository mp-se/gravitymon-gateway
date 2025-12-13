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

#include <ui_gravitymon_gateway.hpp>
#include <stdio.h>
#include <string.h>

/**
 * UI state structure - matching hardware layout
 */
typedef struct {
    // Styles for different text sizes
    lv_style_t style_font12;
    lv_style_t style_font12c;
    lv_style_t style_font16c;
    lv_style_t style_font20c;
    
    // Display labels
    lv_obj_t* txt_device_name;      // Device name (5, 5, 250, 36)
    lv_obj_t* txt_device_index;     // Device index (260, 5, 54, 36)
    lv_obj_t* txt_device_value1;    // Value 1 (30, 45, 75, 36)
    lv_obj_t* txt_device_value2;    // Value 2 (110, 45, 100, 36)
    lv_obj_t* txt_device_value3;    // Value 3 (215, 45, 75, 36)
    lv_obj_t* txt_device_timestamp; // Timestamp (30, 85, 260, 26)
    lv_obj_t* txt_status_bar;       // Status (5, 219, 310, 18)
    
    // History lines (5 items)
    lv_obj_t* txt_history[5];       // History items (5, 114+i*21, 310, 18)
    
    // Buttons
    lv_obj_t* btn_left;             // Previous button (5, 45, 25, 66)
    lv_obj_t* btn_right;            // Next button (290, 45, 26, 66)
    
    // Data buffers
    char data_device_name[64];
    char data_device_index[32];
    char data_device_value1[32];
    char data_device_value2[32];
    char data_device_value3[32];
    char data_device_timestamp[64];
    char data_status_bar[64];
    char data_history[5][64];
} gravitymon_state_t;

static gravitymon_state_t g_state = {0};
static lv_disp_t* g_disp = NULL;
static bool g_darkmode = false;

/**
 * Button callbacks
 */
static void btn_left_callback(lv_event_t* e) {
    (void)e;
    printf("UI: Previous device button pressed\n");
}

static void btn_right_callback(lv_event_t* e) {
    (void)e;
    printf("UI: Next device button pressed\n");
}

/**
 * Create a styled label matching gateway implementation
 */
static lv_obj_t* create_label(lv_obj_t* parent, const char* text,
                              int32_t x, int32_t y, int32_t w, int32_t h,
                              lv_style_t* style) {
    lv_obj_t* lbl = lv_label_create(parent);
    lv_label_set_text(lbl, text);
    lv_obj_set_pos(lbl, x, y);
    lv_obj_set_size(lbl, w, h);
    lv_obj_add_style(lbl, style, 0);
    return lbl;
}

/**
 * Create a button matching gateway implementation
 */
static lv_obj_t* create_button(lv_obj_t* parent, const char* label_text,
                               int32_t x, int32_t y, int32_t w, int32_t h,
                               lv_event_cb_t callback) {
    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_set_size(btn, w, h);
    lv_obj_set_pos(btn, x, y);
    lv_obj_add_event_cb(btn, callback, LV_EVENT_ALL, NULL);
    
    lv_obj_t* lbl = lv_label_create(btn);
    lv_label_set_text(lbl, label_text);
    lv_obj_center(lbl);
    return btn;
}

/**
 * Initialize gravitymon gateway UI - matches hardware layout exactly
 */
void gravitymon_gateway_init(lv_disp_t* disp, bool darkmode) {
    if (!disp) {
        fprintf(stderr, "gravitymon_gateway_init: NULL display\n");
        return;
    }
    
    g_disp = disp;
    g_darkmode = darkmode;
    memset(&g_state, 0, sizeof(g_state));
    
    // Get active screen
    lv_obj_t* scr = lv_scr_act();
    if (!scr) {
        fprintf(stderr, "ERROR: No active screen\n");
        return;
    }
    
    // Get theme colors
    ui_theme_colors_t theme_colors = ui_get_theme_colors(darkmode ? UI_THEME_DARK : UI_THEME_LIGHT);
    
    // Apply background
    lv_obj_set_style_bg_color(scr, theme_colors.bg, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);
    
    // Initialize styles - matching gateway implementation
    lv_style_init(&g_state.style_font12);
    lv_style_init(&g_state.style_font12c);
    lv_style_init(&g_state.style_font16c);
    lv_style_init(&g_state.style_font20c);
    
    lv_style_set_text_font(&g_state.style_font12, &lv_font_montserrat_12);
    lv_style_set_text_font(&g_state.style_font12c, &lv_font_montserrat_12);
    lv_style_set_text_font(&g_state.style_font16c, &lv_font_montserrat_16);
    lv_style_set_text_font(&g_state.style_font20c, &lv_font_montserrat_20);
    
    lv_style_set_text_align(&g_state.style_font12, LV_TEXT_ALIGN_LEFT);
    lv_style_set_text_align(&g_state.style_font12c, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_align(&g_state.style_font16c, LV_TEXT_ALIGN_CENTER);
    lv_style_set_text_align(&g_state.style_font20c, LV_TEXT_ALIGN_CENTER);
    
    lv_style_set_text_color(&g_state.style_font12, theme_colors.text);
    lv_style_set_text_color(&g_state.style_font12c, theme_colors.text);
    lv_style_set_text_color(&g_state.style_font16c, theme_colors.text);
    lv_style_set_text_color(&g_state.style_font20c, theme_colors.text);
    
    // Device name (5, 5, 250, 36)
    g_state.txt_device_name = create_label(scr, "", 5, 5, 250, 36, &g_state.style_font20c);
    
    // Device index (260, 5, 54, 36)
    g_state.txt_device_index = create_label(scr, "", 260, 5, 54, 36, &g_state.style_font16c);
    
    // Device value 1 (30, 45, 75, 36)
    g_state.txt_device_value1 = create_label(scr, "", 30, 45, 75, 36, &g_state.style_font16c);
    
    // Device value 2 (110, 45, 100, 36)
    g_state.txt_device_value2 = create_label(scr, "", 110, 45, 100, 36, &g_state.style_font16c);
    
    // Device value 3 (215, 45, 75, 36)
    g_state.txt_device_value3 = create_label(scr, "", 215, 45, 75, 36, &g_state.style_font16c);
    
    // Timestamp (30, 85, 260, 26)
    g_state.txt_device_timestamp = create_label(scr, "", 30, 85, 260, 26, &g_state.style_font16c);
    
    // Status bar (5, 219, 310, 18)
    g_state.txt_status_bar = create_label(scr, "", 5, 219, 310, 18, &g_state.style_font12c);
    
    // History items (5, 114+i*21, 310, 18)
    for (int i = 0; i < 5; i++) {
        g_state.txt_history[i] = create_label(scr, "", 5, 114 + (i * 21), 310, 18, &g_state.style_font12);
    }
    
    // Left button (5, 45, 25, 66)
    g_state.btn_left = create_button(scr, "<", 5, 45, 25, 66, btn_left_callback);
    lv_obj_set_style_bg_color(g_state.btn_left, theme_colors.button_bg, LV_PART_MAIN);
    
    // Right button (290, 45, 26, 66)
    g_state.btn_right = create_button(scr, ">", 290, 45, 26, 66, btn_right_callback);
    lv_obj_set_style_bg_color(g_state.btn_right, theme_colors.button_bg, LV_PART_MAIN);
    
    printf("Gravitymon Gateway UI initialized (320x240 landscape, %s mode) - layout matches hardware\n", 
           darkmode ? "dark" : "light");
}

/**
 * Update label text
 */
static void update_label(lv_obj_t* obj, const char* text) {
    if (obj && text) {
        lv_label_set_text(obj, text);
    }
}

/**
 * Set device name display
 */
void gravitymon_gateway_set_device_name(const char* name) {
    if (g_state.txt_device_name && name) {
        strncpy(g_state.data_device_name, name, sizeof(g_state.data_device_name) - 1);
        update_label(g_state.txt_device_name, g_state.data_device_name);
    }
}

/**
 * Set device index (current device number / total)
 */
void gravitymon_gateway_set_device_index(uint8_t current, uint8_t total) {
    if (!g_state.txt_device_index) return;
    
    snprintf(g_state.data_device_index, sizeof(g_state.data_device_index), 
            "%d/%d", current, total);
    update_label(g_state.txt_device_index, g_state.data_device_index);
}

/**
 * Set gravity reading
 */
void gravitymon_gateway_set_gravity(float gravity) {
    if (!g_state.txt_device_value1) return;
    
    snprintf(g_state.data_device_value1, sizeof(g_state.data_device_value1), 
            "%.3f", gravity);
    update_label(g_state.txt_device_value1, g_state.data_device_value1);
}

/**
 * Set temperature reading
 */
void gravitymon_gateway_set_temperature(float temp) {
    if (!g_state.txt_device_value2) return;
    
    snprintf(g_state.data_device_value2, sizeof(g_state.data_device_value2), 
            "%.1fC", temp);
    update_label(g_state.txt_device_value2, g_state.data_device_value2);
}

/**
 * Set battery percentage
 */
void gravitymon_gateway_set_battery(float percent) {
    if (!g_state.txt_device_value3) return;
    
    snprintf(g_state.data_device_value3, sizeof(g_state.data_device_value3), 
            "%.0f%%", percent);
    update_label(g_state.txt_device_value3, g_state.data_device_value3);
}

/**
 * Set device timestamp
 */
void gravitymon_gateway_set_timestamp(const char* timestamp) {
    if (g_state.txt_device_timestamp && timestamp) {
        strncpy(g_state.data_device_timestamp, timestamp, sizeof(g_state.data_device_timestamp) - 1);
        update_label(g_state.txt_device_timestamp, g_state.data_device_timestamp);
    }
}

/**
 * Set history value at index
 */
void gravitymon_gateway_set_history(uint8_t index, float value) {
    if (index >= 5) return;
    
    snprintf(g_state.data_history[index], sizeof(g_state.data_history[index]), 
            "%.3f", value);
    update_label(g_state.txt_history[index], g_state.data_history[index]);
}

/**
 * Set status bar message
 */
void gravitymon_gateway_set_status(const char* status) {
    if (g_state.txt_status_bar && status) {
        strncpy(g_state.data_status_bar, status, sizeof(g_state.data_status_bar) - 1);
        update_label(g_state.txt_status_bar, g_state.data_status_bar);
    }
}

/**
 * Toggle dark/light theme
 */
void gravitymon_gateway_set_theme(bool darkmode) {
    lv_obj_t* scr = lv_scr_act();
    if (!scr) return;
    
    g_darkmode = darkmode;
    ui_theme_colors_t theme_colors = ui_get_theme_colors(darkmode ? UI_THEME_DARK : UI_THEME_LIGHT);
    
    // Update background
    lv_obj_set_style_bg_color(scr, theme_colors.bg, LV_PART_MAIN);
    
    // Update all text colors
    lv_obj_set_style_text_color(g_state.txt_device_name, theme_colors.text, 0);
    lv_obj_set_style_text_color(g_state.txt_device_index, theme_colors.text, 0);
    lv_obj_set_style_text_color(g_state.txt_device_value1, theme_colors.text, 0);
    lv_obj_set_style_text_color(g_state.txt_device_value2, theme_colors.text, 0);
    lv_obj_set_style_text_color(g_state.txt_device_value3, theme_colors.text, 0);
    lv_obj_set_style_text_color(g_state.txt_device_timestamp, theme_colors.text, 0);
    lv_obj_set_style_text_color(g_state.txt_status_bar, theme_colors.text, 0);
    
    for (int i = 0; i < 5; i++) {
        lv_obj_set_style_text_color(g_state.txt_history[i], theme_colors.text, 0);
    }
    
    // Update button colors
    lv_obj_set_style_bg_color(g_state.btn_left, theme_colors.button_bg, LV_PART_MAIN);
    lv_obj_set_style_bg_color(g_state.btn_right, theme_colors.button_bg, LV_PART_MAIN);
}

/**
 * Cleanup UI resources
 */
void gravitymon_gateway_cleanup(void) {
    memset(&g_state, 0, sizeof(g_state));
}
#endif  // ENABLE_LVGL

// EOF
