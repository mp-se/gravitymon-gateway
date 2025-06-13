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
#if defined(GATEWAY)

#include <Touch_CST328.h>

#include <cstdio>
#include <display.hpp>
#include <fonts.hpp>
#include <log.hpp>
#include <looptimer.hpp>

#if defined(ENABLE_TFT)
TaskHandle_t lvglTaskHandler;
struct LVGL_Data lvglData;

constexpr auto TTF_CALIBRATION_FILENAME = "/tft.dat";

Display::Display() { _tft = new TFT_eSPI(); }

void Display::setup() {
  if (!_tft) return;

  _tft->init();
  _tft->setSwapBytes(true);
  _tft->setRotation(1);  // 90 degrees
  clear();
  setFont(FontSize::FONT_9);

#if TOUCH_CS == -1  // Using CST328 touch on waveshare
  if (!Touch_Init()) {
    Log.error(F("DISP: Unable to initialize CST328 touch controller." CR));
  }
#endif
}

void Display::setFont(FontSize f) {
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
}

void Display::printLine(int l, const String &text) {
  if (!_tft) return;

  uint16_t h = _tft->fontHeight();
  _tft->fillRect(0, l * h, _tft->width(), h, TFT_BLACK);
  _tft->drawString(text.c_str(), 0, l * h, GFXFF);
}

void Display::printLineCentered(int l, const String &text) {
  if (!_tft) return;

  uint16_t h = _tft->fontHeight();
  uint16_t w = _tft->textWidth(text);
  _tft->fillRect(0, l * h, _tft->width(), h, TFT_BLACK);
  _tft->drawString(text.c_str(), (_tft->width() - w) / 2, l * h, GFXFF);
}

void Display::clear(uint32_t color) {
  if (!_tft) return;

  _backgroundColor = color;
  _tft->fillScreen(_backgroundColor);
  delay(1);
}

void Display::createUI() {
  if (!_tft) return;

  Log.notice(F("DISP: Using LVL v%d.%d.%d." CR), lv_version_major(),
             lv_version_minor(), lv_version_patch());

  lv_init();
  lv_log_register_print_cb(log_print);

#define DRAW_BUF_SIZE (TFT_WIDTH * TFT_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))

  void *draw_buf = ps_malloc(DRAW_BUF_SIZE);

  if (!draw_buf) {
    Log.error(
        F("DISP: Failed to allocate ps ram for display buffer, size=%d" CR),
        DRAW_BUF_SIZE);
  }

  lvglData._display =
      lv_tft_espi_create(TFT_WIDTH, TFT_HEIGHT, draw_buf, DRAW_BUF_SIZE);

  // if (_rotation == Rotation::ROTATION_90) {
  lv_display_set_rotation(lvglData._display, LV_DISPLAY_ROTATION_90);
  // } else {  // Rotation::ROTATION_270
  //   lv_display_set_rotation(lvglData._display, LV_DISPLAY_ROTATION_270);
  // }

  // Initialize an LVGL input device object (Touchscreen)
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touchScreenHandler);

  // Register gesture event handler for the main screen
  lv_obj_t *scr = lv_scr_act();
  lv_obj_add_event_cb(scr, gestureScreenHandler, LV_EVENT_GESTURE, NULL);

  // Create components
  lv_style_init(&lvglData._font12);
  lv_style_init(&lvglData._font12c);
  lv_style_init(&lvglData._font16c);
  lv_style_init(&lvglData._font20c);
  lv_style_set_text_font(&lvglData._font12, &lv_font_montserrat_12);
  lv_style_set_text_font(&lvglData._font12c, &lv_font_montserrat_12);
  lv_style_set_text_font(&lvglData._font16c, &lv_font_montserrat_16);
  lv_style_set_text_font(&lvglData._font20c, &lv_font_montserrat_20);
  lv_style_set_text_align(&lvglData._font12, LV_TEXT_ALIGN_LEFT);
  lv_style_set_text_align(&lvglData._font12c, LV_TEXT_ALIGN_CENTER);
  lv_style_set_text_align(&lvglData._font16c, LV_TEXT_ALIGN_CENTER);
  lv_style_set_text_align(&lvglData._font20c, LV_TEXT_ALIGN_CENTER);

  // For showing the layout
  // lv_style_set_outline_width(&lvglData._font12, 1);
  // lv_style_set_outline_color(&lvglData._font12,
  // lv_palette_main(LV_PALETTE_BLUE));
  // lv_style_set_outline_width(&lvglData._font12c, 1);
  // lv_style_set_outline_color(&lvglData._font12c,
  // lv_palette_main(LV_PALETTE_BLUE));
  // lv_style_set_outline_width(&lvglData._font16c, 1);
  // lv_style_set_outline_color(&lvglData._font16c,
  // lv_palette_main(LV_PALETTE_BLUE));
  // lv_style_set_outline_width(&lvglData._font20c, 1);
  // lv_style_set_outline_color(&lvglData._font20c,
  // lv_palette_main(LV_PALETTE_BLUE));

  Log.notice(F("DISP: Creating UI components." CR));

  lvglData._txtDeviceName = createLabel("", 5, 5, 250, 36, &lvglData._font20c);
  lvglData._txtDeviceIndex =
      createLabel("", 260, 5, 54, 36, &lvglData._font16c);
  lvglData._txtDeviceValue1 =
      createLabel("", 5, 45, 100, 36, &lvglData._font20c);
  lvglData._txtDeviceValue2 =
      createLabel("", 110, 45, 100, 36, &lvglData._font20c);
  lvglData._txtDeviceValue3 =
      createLabel("", 215, 45, 100, 36, &lvglData._font20c);
  lvglData._txtDeviceTimeStamp =
      createLabel("", 5, 85, 310, 26, &lvglData._font16c);
  lvglData._txtStatusbar = createLabel("", 5, 219, 310, 18, &lvglData._font12c);

  for (int i = 0; i < 5; i++)
    lvglData._txtHistory[i] =
        createLabel("", 5, 114 + (i * 21), 310, 18, &lvglData._font12);

  xTaskCreatePinnedToCore(lvgl_loop_handler,  // Function to implement the task
                          "LVGL_Handler",     // Name of the task
                          10000,              // Stack size in words
                          NULL,               // Task input parameter
                          0,                  // Priority of the task
                          &lvglTaskHandler,   // Task handle.
                          0);                 // Core where the task should run
}

void Display::updateDevice(const char *name, const char *value1,
                           const char *value2, const char *value3,
                           const char *timestamp, int index, int maxIndex) {
  lvglData._dataDeviceName = name;
  lvglData._dataDeviceValue1 = value1;
  lvglData._dataDeviceValue2 = value2;
  lvglData._dataDeviceValue3 = value3;
  lvglData._dataDeviceTimeStamp = timestamp;

  char buf[10] = "";

  if (maxIndex) snprintf(buf, sizeof(buf), "%d/%d", index + 1, maxIndex);

  lvglData._dataDeviceIndex = buf;
}

void Display::updateHistory(const char *history, int idx) {
  lvglData._dataHistory[idx] = history;
}

void Display::updateStatus(const char *status, bool darkmode) {
  lvglData._dataStatusbar = status;
  lvglData._darkmode = darkmode;
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
    // Log.info(F("DISP: Screen touched %d." CR), pressed);
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

    clear();
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

#if TOUCH_CS == -1  // Using CST328 touch on waveshare
  uint16_t xt[CST328_LCD_TOUCH_MAX_POINTS] = {0};
  uint16_t yt[CST328_LCD_TOUCH_MAX_POINTS] = {0};
  uint16_t strength[CST328_LCD_TOUCH_MAX_POINTS] = {0};
  uint8_t cnt = 0;

  Touch_Read_Data();
  uint8_t b =
      Touch_Get_XY(xt, yt, strength, &cnt, uint8_t CST328_LCD_TOUCH_MAX_POINTS);

  if (b && cnt > 0) {
    // if (_rotation == Rotation::ROTATION_90) {
    //   *x = yt[0];
    //   *y = TFT_HEIGHT - xt[0];
    // } else {  // Rotation::ROTATION_270
    //   *x = yt[0];
    //   *y = TFT_HEIGHT - xt[0];
    // }

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

    // if (_rotation == Rotation::ROTATION_90) {
    *x = yt;
    *y = TFT_HEIGHT - xt;
    // } else {  // Rotation::ROTATION_270
    //   *x = yt;
    //   *y = TFT_HEIGHT - xt;
    // }
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

void touchScreenHandler(lv_indev_t *indev, lv_indev_data_t *data) {
  uint16_t x = 0, y = 0;

  if (myDisplay.getTouch(&x, &y)) {
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = x;
    data->point.y = y;

    // Log.notice(F("LVGL : %d:%d." CR), x, y);
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void gestureLeft();
void gestureRight();

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
        // Log.info(F("DISP: Gesture UP." CR));
        break;
      case LV_DIR_BOTTOM:
        // Log.info(F("DISP: Gesture DOWN." CR));
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

lv_obj_t *createLabel(const char *label, int32_t x, int32_t y, int32_t w,
                      int32_t h, lv_style_t *style) {
  lv_obj_t *lbl = lv_label_create(lv_screen_active());
  lv_label_set_text(lbl, label);
  lv_obj_set_size(lbl, w, h);
  lv_obj_set_pos(lbl, x, y);
  lv_obj_add_style(lbl, style, 0);
  return lbl;
}

void updateLabel(lv_obj_t *obj, const char *label) {
  lv_label_set_text(obj, label);
}

void lvgl_loop_handler(void *parameter) {
  LoopTimer taskLoop(500);

  for (;;) {
    if (taskLoop.hasExpired()) {
      taskLoop.reset();

      lv_obj_t *scr = lv_scr_act();
      lv_color_t color;

      if (lvglData._darkmode) {
        lv_obj_set_style_bg_color(scr, lv_color_hex(0x1F1F1F), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);
        color = lv_color_white();
      } else {
        lv_obj_set_style_bg_color(scr, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);
        color = lv_color_black();
      }

      lv_obj_set_style_text_color(lvglData._txtDeviceName, color, 0);
      lv_obj_set_style_text_color(lvglData._txtDeviceIndex, color, 0);
      lv_obj_set_style_text_color(lvglData._txtDeviceValue1, color, 0);
      lv_obj_set_style_text_color(lvglData._txtDeviceValue2, color, 0);
      lv_obj_set_style_text_color(lvglData._txtDeviceValue3, color, 0);
      lv_obj_set_style_text_color(lvglData._txtDeviceTimeStamp, color, 0);
      lv_obj_set_style_text_color(lvglData._txtStatusbar, color, 0);

      for (int i = 0; i < 5; i++)
        lv_obj_set_style_text_color(lvglData._txtHistory[i], color, 0);

      updateLabel(lvglData._txtDeviceName, lvglData._dataDeviceName.c_str());
      updateLabel(lvglData._txtDeviceIndex, lvglData._dataDeviceIndex.c_str());
      updateLabel(lvglData._txtDeviceValue1,
                  lvglData._dataDeviceValue1.c_str());
      updateLabel(lvglData._txtDeviceValue2,
                  lvglData._dataDeviceValue2.c_str());
      updateLabel(lvglData._txtDeviceValue3,
                  lvglData._dataDeviceValue3.c_str());
      updateLabel(lvglData._txtDeviceTimeStamp,
                  lvglData._dataDeviceTimeStamp.c_str());
      updateLabel(lvglData._txtStatusbar, lvglData._dataStatusbar.c_str());

      for (int i = 0; i < 5; i++)
        updateLabel(lvglData._txtHistory[i], lvglData._dataHistory[i].c_str());
    }

    lv_task_handler();
    lv_tick_inc(5);
    delay(5);
  }
}

#else

Display::Display() {}

void Display::setup() {}

void Display::createUI() {}

void Display::setFont(FontSize f) {}

void Display::printLine(int l, const String& text) {}

void Display::printLineCentered(int l, const String& text) {}

void Display::clear(uint32_t color) {}

void Display::updateDevice(const char* name, const char* value1,
                           const char* value2, const char* value3,
                           const char* timestamp, int index, int maxIndex) {}
void Display::updateHistory(const char* history, int idx) {}
void Display::updateStatus(const char* status) {}

#endif

#endif  // GATEWAY

// EOF
