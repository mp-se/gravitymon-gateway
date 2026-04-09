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
#ifndef SRC_DISPLAY_HPP_
#define SRC_DISPLAY_HPP_

#if defined(GATEWAY)

#include <SPI.h>

#include <config_gateway.hpp>
#include <main.hpp>

#if defined(ENABLE_TFT)
#include <TFT_eSPI.h>
#include <freertos/semphr.h>
#include <lvgl.h>

#include <ui_helpers.hpp>
#endif

enum FontSize { FONT_9 = 9, FONT_12 = 12, FONT_18 = 18, FONT_24 = 24 };

class Display {
 private:
#if defined(ENABLE_TFT)
  TFT_eSPI* _tft = nullptr;
  lv_display_t* _display = nullptr;
  uint32_t _backgroundColor = TFT_BLACK;
  uint16_t _touchCalibrationlData[5] = {0, 0, 0, 0, 0};
  SemaphoreHandle_t _uiSemaphore = nullptr;
#endif
  FontSize _fontSize = FontSize::FONT_9;
  // Rotation _rotation = ROTATION_90;

 public:
  Display();
  void setup();
  void createUI();
  void calibrateTouch();

  SPIClass& getSPI() {
#if defined(ENABLE_TFT)
    return _tft->getSPIinstance();
#else
    return SPI;
#endif
  }

  void clear(uint32_t color);
  void setFont(FontSize f);
  void printLine(int l, const String& text);
  void printLineCentered(int l, const String& text);
  // Rotation getRotation() { return _rotation; }
  // void setRotation(Rotation rotation);

  void createUI(uint8_t layoutId = 0);

  // Composite setters for device data
  void updateEmpty();
  void updateGravity(const char* name, uint8_t index, uint8_t maxIndex,
                     const char* type, const char* source,
                     const char* timestamp, float gravity, char gravityUnit,
                     float temp, char tempUnit, float batteryVoltage,
                     float batteryPercentage, int rssi);
  void updatePressure(const char* name, uint8_t index, uint8_t maxIndex,
                      const char* type, const char* source,
                      const char* timestamp, float pressure, float pressure2,
                      const char* pressureUnit, float temp, char tempUnit,
                      float batteryVoltage, float batteryPercentage, int rssi);
  void updateTemperature(const char* name, uint8_t index, uint8_t maxIndex,
                         const char* timestamp, const char* type,
                         const char* source, float temp, float temp2,
                         char tempUnit, int rssi);
  void updateHistory(const char* history, int idx);
  void updateStatus(const char* status);
  void updateDarkmode(bool darkmode);
  void setLayout(uint8_t layoutId);

  // LVGL methods
  bool getTouch(uint16_t* x, uint16_t* y);  // Check for touch callback
  void handleGestureEventEvent(char gesture);
  SemaphoreHandle_t getUISemaphore() { return _uiSemaphore; }
};

// LVGL handlers and utilities
#if defined(ENABLE_TFT)
void updateLabel(lv_obj_t* obj, const char* label);
void touchScreenHandler(lv_indev_t* indev, lv_indev_data_t* data);
void gestureScreenHandler(lv_event_t* e);
void log_print(lv_log_level_t level, const char* buf);
void lvgl_loop_handler(void* parameter);
void btnLeftEventHandler(lv_event_t* e);
void btnRightEventHandler(lv_event_t* e);
void gestureLeft();
void gestureRight();
#endif
extern Display myDisplay;

#endif  // GATEWAY

#endif  // SRC_DISPLAY_HPP_

// EOF
