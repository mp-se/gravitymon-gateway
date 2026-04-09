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
