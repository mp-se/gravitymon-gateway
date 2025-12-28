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

#if defined(ENABLE_LVGL)

#include <stdbool.h>

#include <ui_helpers.hpp>

void gravitymon_gateway_init(lv_disp_t* disp, bool darkmode, uint8_t layout_id);
void gravitymon_gateway_loop(void);
void gravitymon_gateway_cleanup(void);

void gravitymon_gateway_set_name(const char* name);
void gravitymon_gateway_set_index(uint8_t current, uint8_t total);
void gravitymon_gateway_set_gravity_range(float min, float max, char unit);
void gravitymon_gateway_set_gravity(float gravity, char unit);
void gravitymon_gateway_set_pressure(float pressure, float pressure2,
                                     const char* unit);
void gravitymon_gateway_set_pressure_range(float min, float max,
                                           const char* unit);
void gravitymon_gateway_set_temp(float temp, float temp2, char unit);
void gravitymon_gateway_set_battery_percentage(float percent);
void gravitymon_gateway_set_battery_voltage(float voltage);
void gravitymon_gateway_set_time(const char* time_str);
void gravitymon_gateway_set_history(uint8_t index, const char* value);
void gravitymon_gateway_set_status(const char* status);
void gravitymon_gateway_set_theme(bool darkmode);
void gravitymon_gateway_set_rssi(int8_t rssi_dbm);
void gravitymon_gateway_set_type(const char* type);
void gravitymon_gateway_set_source(const char* source);
void gravitymon_gateway_set_layout(uint8_t layout_id);
uint8_t gravitymon_gateway_get_layout();

#endif  // ENABLE_LVGL

#endif  // SRC_UI_GRAVITYMON_GATEWAY_HPP_

// EOF
