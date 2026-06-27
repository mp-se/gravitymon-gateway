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
#if defined(GATEWAY)

#if defined(WAVESHARE_S3_TFT43)

// It wraps the ESP-IDF RGB panel driver which supports the bounce-buffer mode
// that prevents the "screen drift" (left-right shift) caused by PSRAM bandwidth
// contention between the LCD_CAM GDMA and CPU accesses on ESP32-S3.
#include <esp_display_panel.hpp>
#include <lvgl.h>

using namespace esp_panel::drivers;
using namespace esp_panel::board;

static Board* s_board = nullptr;
static TaskHandle_t s_lvgl_task = nullptr;

// ISR: fired at end of every frame scan-out (vsync).  Unblocks the flush
// callback so LVGL can hand the freshly-rendered buffer to the display.
// Guard against the ISR firing before the LVGL task is created (the RGB panel
// starts scanning immediately inside board->begin(), before createUI() runs).
IRAM_ATTR static bool onLcdVsync(void* /*user_data*/) {
  if (!s_lvgl_task) return false;
  BaseType_t yield = pdFALSE;
  vTaskNotifyGiveFromISR(s_lvgl_task, &yield);
  return yield == pdTRUE;
}

// Double-buffer flush: atomically switch which frame buffer the LCD scans out,
// then block until that frame has been fully sent (vsync).  Only after vsync
// does LVGL get back the buffer it just displayed so it can render the next
// frame into it — no tearing, no drift.
static void esp_panel_flush_cb(lv_display_t* disp, const lv_area_t* /*area*/,
                                uint8_t* px_map) {
  s_board->getLCD()->switchFrameBufferTo(px_map);
  ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  lv_display_flush_ready(disp);
}

#else  // !WAVESHARE_S3_TFT43

#include <Touch_CST328.h>

#endif  // WAVESHARE_S3_TFT43

#include <cstdio>
#include <display.hpp>
#include <fonts.hpp>
#include <log.hpp>
#include <looptimer.hpp>
#include <ui_gravitymon_gateway.hpp>
#include <ui_helpers.hpp>

#if defined(ENABLE_TFT)
TaskHandle_t lvglTaskHandler;
static volatile int8_t s_pending_layout = -1;  // -1 = no change pending
#endif

constexpr auto TTF_CALIBRATION_FILENAME = "/tft.dat";

#if defined(WAVESHARE_S3_TFT43)
Display::Display() {}
#else
Display::Display() { _tft = new TFT_eSPI(); }
#endif

#if defined(WAVESHARE_S3_TFT43)
esp_expander::Base* Display::getExpander() {
  if (s_board) {
    auto io_exp = s_board->getIO_Expander();
    return io_exp ? io_exp->getBase() : nullptr;
  }
  return nullptr;
}
#endif

void Display::setup() {
#if defined(WAVESHARE_S3_TFT43)
  s_board = new Board();

  // init() builds the LCD and bus objects — must come first.
  if (!s_board->init()) {
    Log.error(F("DISP: ESP32_Display_Panel init failed." CR));
    return;
  }

  // Configure double-buffering and bounce-buffer AFTER init() (objects exist)
  // but BEFORE begin() (IDF panel not yet started).
  auto lcd = s_board->getLCD();
  // Two hardware frame buffers for tear-free double-buffering.
  lcd->configFrameBufferNumber(2);
  // Bounce buffer: internal-SRAM staging area decouples the LCD_CAM GDMA
  // from PSRAM, preventing the "screen drift" issue on ESP32-S3.
  auto lcd_bus = lcd->getBus();
  if (lcd_bus && lcd_bus->getBasicAttributes().type == ESP_PANEL_BUS_TYPE_RGB) {
    static_cast<BusRGB*>(lcd_bus)->configRGB_BounceBufferSize(TFT_WIDTH * 10);
  }

  if (!s_board->begin()) {
    Log.error(F("DISP: ESP32_Display_Panel begin failed." CR));
    return;
  }

  Log.notice(F("DISP: ESP32_Display_Panel ready, fb0=0x%x fb1=0x%x" CR),
             (uint32_t)(uintptr_t)lcd->getFrameBufferByIndex(0),
             (uint32_t)(uintptr_t)lcd->getFrameBufferByIndex(1));
#else
  if (!_tft) return;

  _tft->init();
  _tft->setSwapBytes(true);
  _tft->setRotation(1);  // 90 degrees
  clear(TFT_BLACK);
  setFont(FontSize::FONT_9);

#if TOUCH_CS == -1  // Using CST328 touch on waveshare
  if (!Touch_Init()) {
    Log.error(F("DISP: Unable to initialize CST328 touch controller." CR));
  }
#endif
#endif  // WAVESHARE_S3_TFT43
}

void Display::setFont(FontSize f) {
#if defined(WAVESHARE_S3_TFT43)
  // Do nothing; We only use lvgl, no loading progress
#else 
  if (!_tft) return;

  switch (f) {
    default:
    case FontSize::FONT_9:
      _tft->setFreeFont(FF17);
      break;
    case FontSize::FONT_12:
      _tft->setFreeFont(FF18);
      break;
    case FontSize::FONT_18:
      _tft->setFreeFont(FF19);
      break;
    case FontSize::FONT_24:
      _tft->setFreeFont(FF20);
      break;
  }
#endif
}

void Display::printLine(int l, const String &text) {
#if defined(WAVESHARE_S3_TFT43)
  // Do nothing; We only use lvgl, no loading progress
#else
  if (!_tft) return;

  uint16_t h = _tft->fontHeight();
  _tft->fillRect(0, l * h, _tft->width(), h, TFT_BLACK);
  _tft->drawString(text.c_str(), 0, l * h, GFXFF);
#endif
}

void Display::printLineCentered(int l, const String &text) {
#if defined(WAVESHARE_S3_TFT43)
  // Do nothing; We only use lvgl, no loading progress
#else
  if (!_tft) return;

  uint16_t h = _tft->fontHeight();
  uint16_t w = _tft->textWidth(text);
  _tft->fillRect(0, l * h, _tft->width(), h, TFT_BLACK);
  _tft->drawString(text.c_str(), (_tft->width() - w) / 2, l * h, GFXFF);
#endif
}

void Display::clear(uint32_t color) {
#if defined(WAVESHARE_S3_TFT43)
  // Do nothing; We only use lvgl, no loading progress
#else
  if (!_tft) return;

  _backgroundColor = color;
  _tft->fillScreen(_backgroundColor);
  delay(1);
#endif
}

void Display::createUI(uint8_t layoutId) {
#if defined(WAVESHARE_S3_TFT43)
  if (!s_board) return;
#else
  if (!_tft) return;
#endif

  Log.notice(F("DISP: Using LVL v%d.%d.%d." CR), lv_version_major(),
             lv_version_minor(), lv_version_patch());

  lv_init();
  lv_log_register_print_cb(log_print);

#if defined(WAVESHARE_S3_TFT43)
  // Use the two hardware frame buffers as LVGL's draw buffers.
  // LVGL renders into one while the LCD scans out the other; the flush callback
  // switches them atomically on vsync — no tearing, no copy overhead.
  auto lcd_dev = s_board->getLCD();
  void* buf0 = lcd_dev->getFrameBufferByIndex(0);
  void* buf1 = lcd_dev->getFrameBufferByIndex(1);
  size_t buf_size = (size_t)TFT_WIDTH * TFT_HEIGHT * 2;  // RGB565 = 2 bytes/px
  _display = lv_display_create(TFT_WIDTH, TFT_HEIGHT);
  lv_display_set_flush_cb(_display, esp_panel_flush_cb);
  lv_display_set_buffers(_display, buf0, buf1, buf_size,
                         LV_DISPLAY_RENDER_MODE_FULL);
#else
  const size_t DRAW_BUF_SIZE = TFT_WIDTH * TFT_HEIGHT / 10 * (LV_COLOR_DEPTH / 8);
  void* draw_buf = ps_malloc(DRAW_BUF_SIZE);
  if (!draw_buf) {
    Log.error(F("DISP: Failed to allocate PSRAM draw buffer (%u bytes)" CR),
              (unsigned)DRAW_BUF_SIZE);
    return;
  }
  _display = lv_tft_espi_create(TFT_WIDTH, TFT_HEIGHT, draw_buf, DRAW_BUF_SIZE);
  lv_display_set_rotation(_display, LV_DISPLAY_ROTATION_90);
#endif

  // Initialize an LVGL input device object (Touchscreen)
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touchScreenHandler);

  // Register gesture event handler for the main screen
  lv_obj_t *scr = lv_scr_act();
  lv_obj_add_event_cb(scr, gestureScreenHandler, LV_EVENT_GESTURE, NULL);

  // Initialize the gravitymon gateway UI with selected layout
  gravitymon_gateway_init(_display, false, layoutId);

  xTaskCreatePinnedToCore(lvgl_loop_handler,  // Function to implement the task
                          "LVGL_Handler",     // Name of the task
                          10000,              // Stack size in words
                          NULL,               // Task input parameter
                          0,                  // Priority of the task
                          &lvglTaskHandler,   // Task handle.
                          1);                 // Core 1 — keeps LVGL away from BLE (Core 0)

#if defined(WAVESHARE_S3_TFT43)
  // The flush callback (esp_panel_flush_cb) blocks on ulTaskNotifyTake() and
  // the vsync ISR (onLcdVsync) unblocks it via vTaskNotifyGiveFromISR().
  // s_lvgl_task must point at the LVGL task handle — set it now, after the
  // task has been created and its handle is valid.
  s_lvgl_task = lvglTaskHandler;
#endif
}

void Display::updateEmpty() {
  gravitymon_gateway_set_name("");
  gravitymon_gateway_set_index(0, 0);
  gravitymon_gateway_set_type("");
  gravitymon_gateway_set_source("");
  gravitymon_gateway_set_time("");
  gravitymon_gateway_set_gravity(NAN, ' ');
  gravitymon_gateway_set_temp(NAN, NAN, ' ');
  gravitymon_gateway_set_battery_voltage(NAN);
  gravitymon_gateway_set_battery_percentage(NAN);
  gravitymon_gateway_set_rssi(0);
}

void Display::updateGravity(const char *name, uint8_t index, uint8_t maxIndex,
                            const char *type, const char *source,
                            const char *timestamp, float gravity,
                            char gravityUnit, float temp, char tempUnit,
                            float batteryVoltage, float batteryPercentage,
                            int rrsi) {
  gravitymon_gateway_set_name(name);
  gravitymon_gateway_set_index(index, maxIndex);
  gravitymon_gateway_set_type(type);
  gravitymon_gateway_set_source(source);
  gravitymon_gateway_set_time(timestamp);
  gravitymon_gateway_set_gravity(gravity, gravityUnit);
  gravitymon_gateway_set_temp(temp, NAN, tempUnit);
  gravitymon_gateway_set_battery_voltage(batteryVoltage);
  gravitymon_gateway_set_battery_percentage(batteryPercentage);
  gravitymon_gateway_set_rssi(rrsi);
}

void Display::updatePressure(const char *name, uint8_t index, uint8_t maxIndex,
                             const char *type, const char *source,
                             const char *timestamp, float pressure,
                             float pressure2, const char *pressureUnit,
                             float temp, char tempUnit, float batteryVoltage,
                             float batteryPercentage, int rrsi) {
  gravitymon_gateway_set_name(name);
  gravitymon_gateway_set_index(index, maxIndex);
  gravitymon_gateway_set_type(type);
  gravitymon_gateway_set_source(source);
  gravitymon_gateway_set_time(timestamp);
  gravitymon_gateway_set_pressure(pressure, pressure2, pressureUnit);
  gravitymon_gateway_set_temp(temp, NAN, tempUnit);
  gravitymon_gateway_set_battery_voltage(batteryVoltage);
  gravitymon_gateway_set_battery_percentage(batteryPercentage);
  gravitymon_gateway_set_rssi(rrsi);
}

void Display::updateTemperature(const char *name, uint8_t index,
                                uint8_t maxIndex, const char *type,
                                const char *source, const char *timestamp,
                                float temp, float temp2, char tempUnit,
                                int rrsi) {
  gravitymon_gateway_set_name(name);
  gravitymon_gateway_set_index(index, maxIndex);
  gravitymon_gateway_set_type(type);
  gravitymon_gateway_set_source(source);
  gravitymon_gateway_set_time(timestamp);
  gravitymon_gateway_set_gravity(NAN, ' ');  // clear SG/pressure field
  gravitymon_gateway_set_temp(temp, temp2, tempUnit);
  gravitymon_gateway_set_battery_voltage(NAN);
  gravitymon_gateway_set_battery_percentage(NAN);
  gravitymon_gateway_set_rssi(rrsi);
}

void Display::updateHistory(const char *history, int idx) {
  gravitymon_gateway_set_history(idx, history);
}

void Display::updateStatus(const char *status) {
  gravitymon_gateway_set_status(status);
}

void Display::updateDarkmode(bool darkmode) {
  gravitymon_gateway_set_theme(darkmode);
}

void Display::setLayout(uint8_t layoutId) {
#if defined(ENABLE_TFT)
  // Signal the LVGL task to switch layout on its next iteration.
  // gravitymon_gateway_set_layout() calls LVGL APIs that are not thread-safe,
  // so it must run from the LVGL task — never from the main task.
  s_pending_layout = static_cast<int8_t>(layoutId);
#endif
}

void Display::calibrateTouch() {
#if defined(ENABLE_LVGL) && \
    TOUCH_CS != -1  // Only needed when using TFT_eSPI touch handler
  if (!_tft) return;

  uint16_t x, y, pressed, i = 0;

  myDisplay.printLineCentered(4, "Press screen to calibrate");

  do {
    delay(300);
    pressed = _tft->getTouch(&x, &y, 600);
    Log.info(F("DISP: Screen touched %d." CR), pressed);
  } while (!pressed && ++i < 10);

  if (pressed) {
    clear(TFT_GREEN);
    myDisplay.printLineCentered(4, "Touch detected");
    Log.info(F("DISP: Touch screen pressed, force calibration." CR));
    delay(3000);
  }

  File file = LittleFS.open(TTF_CALIBRATION_FILENAME, "r");

  if (file) {
    Log.info(F("DISP: Loading touch calibration data from file." CR));
    file.read(reinterpret_cast<uint8_t *>(&this->_touchCalibrationlData),
              sizeof(_touchCalibrationlData));
    file.close();
  } else {
    _touchCalibrationlData[0] = 0;
  }

  if (pressed || (_touchCalibrationlData[0] == 0)) {
    Log.info(F("DISP: Running calibration sequence." CR));

    clear(TFT_BLACK);
    myDisplay.printLineCentered(4, "Calibration started");
    _tft->calibrateTouch(_touchCalibrationlData, TFT_GREEN, TFT_BLACK, 15);

    file = LittleFS.open(TTF_CALIBRATION_FILENAME, "w");

    if (file) {
      file.write(reinterpret_cast<uint8_t *>(&this->_touchCalibrationlData),
                 sizeof(_touchCalibrationlData));
      file.close();
    } else {
      Log.warning(F("DISP: Failed to write calibration data to file." CR));
    }

    myDisplay.printLineCentered(4, "Touch calibration completed");
    delay(3000);
  }

  myDisplay.printLineCentered(4, "");
#endif
}

bool Display::getTouch(uint16_t *x, uint16_t *y) {
#if defined(ENABLE_TFT)

#if defined(WAVESHARE_S3_TFT43)
  if (!s_board) return false;
  auto touch = s_board->getTouch();
  if (touch) {
    TouchPoint tp;
    if (touch->readPoints(&tp, 1, 0) > 0) {
      *x = tp.x;
      *y = tp.y;
      return true;
    }
  }
#elif TOUCH_CS == -1  // Using CST328 touch on waveshare
  uint16_t xt[CST328_LCD_TOUCH_MAX_POINTS] = {0};
  uint16_t yt[CST328_LCD_TOUCH_MAX_POINTS] = {0};
  uint16_t strength[CST328_LCD_TOUCH_MAX_POINTS] = {0};
  uint8_t cnt = 0;

  Touch_Read_Data();
  uint8_t b =
      Touch_Get_XY(xt, yt, strength, &cnt, CST328_LCD_TOUCH_MAX_POINTS);

  if (b && cnt > 0) {
    *x = TFT_WIDTH - xt[0];
    *y = TFT_HEIGHT - yt[0];
    return true;
  }
#else
  uint16_t xt, yt;
  uint8_t b = _tft->getTouch(&xt, &yt);

  if (b) {
    if (xt < 0) xt = 0;
    if (yt < 0) yt = 0;

    *x = yt;
    *y = TFT_HEIGHT - xt;
    return true;
  }
#endif
  return false;
#else
  return false;
#endif
}

// LVGL Wrappers and Handlers
// **************************************************************************************************

#if defined(ENABLE_TFT)
void touchScreenHandler(lv_indev_t *indev, lv_indev_data_t *data) {
  uint16_t x = 0, y = 0;

  if (myDisplay.getTouch(&x, &y)) {
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = x;
    data->point.y = y;

    Log.verbose(F("LVGL: %d:%d." CR), x, y);
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void btnLeftEventHandler(lv_event_t *e) {
  if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
    gestureLeft();
  }
}

void btnRightEventHandler(lv_event_t *e) {
  if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
    gestureRight();
  }
}

void gestureScreenHandler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_GESTURE) {
    lv_dir_t gesture = lv_indev_get_gesture_dir(lv_indev_get_act());
    switch (gesture) {
      case LV_DIR_LEFT:
        Log.info(F("DISP: Gesture LEFT." CR));
        gestureLeft();
        break;
      case LV_DIR_RIGHT:
        Log.info(F("DISP: Gesture RIGHT." CR));
        gestureRight();
        break;
      case LV_DIR_TOP:
        Log.info(F("DISP: Gesture UP." CR));
        break;
      case LV_DIR_BOTTOM:
        Log.info(F("DISP: Gesture DOWN." CR));
        break;
      default:
        break;
    }
  }
}

void log_print(lv_log_level_t level, const char *buf) {
  LV_UNUSED(level);
  Log.notice(F("LVGL: %s." CR), buf);
}

void updateLabel(lv_obj_t *obj, const char *label) {
  lv_label_set_text(obj, label);
}

void lvgl_loop_handler(void *parameter) {
  // Capture this task's handle so the vsync ISR can unblock the flush callback.
#if defined(WAVESHARE_S3_TFT43)
  s_lvgl_task = xTaskGetCurrentTaskHandle();
  if (s_board) {
    s_board->getLCD()->attachRefreshFinishCallback(onLcdVsync, nullptr);
  }
#endif

  LoopTimer taskLoop(500);

  for (;;) {
    // Apply any pending layout switch requested from the main task.
    // gravitymon_gateway_set_layout() touches LVGL objects so it must run here.
    int8_t pending = s_pending_layout;
    if (pending >= 0) {
      s_pending_layout = -1;
      gravitymon_gateway_set_layout(static_cast<uint8_t>(pending));
    }

    if (taskLoop.hasExpired()) {
      taskLoop.reset();
      gravitymon_gateway_loop();
    }

    lv_task_handler();
    lv_tick_inc(10);
    delay(10);
  }
}

#else

Display::Display() {}

void Display::setup() {}

void Display::createUI(uint8_t layoutId) {}

void Display::calibrateTouch() {}

void Display::setFont(FontSize f) {}

void Display::printLine(int l, const String &text) {}

void Display::printLineCentered(int l, const String &text) {}

void Display::clear(uint32_t color) {}

void Display::updateEmpty() {}
void Display::updateGravity(const char *name, uint8_t index, uint8_t maxIndex,
                            const char *timestamp, float gravity, char unit,
                            float temp, char tempUnit, float batteryVoltage,
                            float batteryPercentage, int rssi) {}

void Display::updatePressure(const char *name, uint8_t index, uint8_t maxIndex,
                             const char *timestamp, float pressure, char unit,
                             float temp, char tempUnit, float batteryVoltage,
                             float batteryPercentage, int rssi) {}
void Display::updateTemperature(const char *name, uint8_t index,
                                uint8_t maxIndex, const char *timestamp,
                                const char *type, const char *source,
                                float temp, float temp2, char tempUnit,
                                int rssi) {}
void Display::updateHistory(const char *history, int idx) {}

void Display::updateStatus(const char *status) {}

void Display::updateDarkmode(bool darkmode) {}

void Display::setLayout(uint8_t layoutId) {}

#endif  // ENABLE_TFT

#endif  // GATEWAY

// EOF
