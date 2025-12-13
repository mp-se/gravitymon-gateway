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
#ifndef SRC_UI_GRAVITYMON_GATEWAY_HPP_
#define SRC_UI_GRAVITYMON_GATEWAY_HPP_

#include <stdbool.h>

#include "lvgl.h"

/**
 * Initialize gravitymon gateway UI
 * @param disp LVGL display object
 * @param darkmode true for dark theme, false for light
 */
void gravitymon_gateway_init(lv_disp_t* disp, bool darkmode);

/**
 * Set device name display
 */
void gravitymon_gateway_set_device_name(const char* name);

/**
 * Set device index (current device number / total)
 */
void gravitymon_gateway_set_device_index(uint8_t current, uint8_t total);

/**
 * Set gravity reading
 */
void gravitymon_gateway_set_gravity(float gravity);

/**
 * Set temperature reading
 */
void gravitymon_gateway_set_temperature(float temp);

/**
 * Set battery percentage
 */
void gravitymon_gateway_set_battery(float percent);

/**
 * Set device timestamp (last update)
 */
void gravitymon_gateway_set_timestamp(const char* timestamp);

/**
 * Set history value at index (0-4 for 5 previous readings)
 */
void gravitymon_gateway_set_history(uint8_t index, float value);

/**
 * Set status bar message
 */
void gravitymon_gateway_set_status(const char* status);

/**
 * Toggle dark/light theme
 */
void gravitymon_gateway_set_theme(bool darkmode);

/**
 * Cleanup and release UI resources
 */
void gravitymon_gateway_cleanup(void);

#endif  // SRC_UI_GRAVITYMON_GATEWAY_HPP_
