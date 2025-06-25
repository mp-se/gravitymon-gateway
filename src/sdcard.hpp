
/*
MIT License

Copyright (c) 2025 Magnus

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
#ifndef SRC_SD_HPP_
#define SRC_SD_HPP_

#if defined(ENABLE_SD)

#include <FS.h>
#include <SD_MMC.h>

#include <log.hpp>

#if defined(WAVESHARE_S3_TFT)
#define SD_CLK_PIN 14
#define SD_CMD_PIN 17
#define SD_D0_PIN 16
// #define SD_D1_PIN 18
// #define SD_D2_PIN 15
// #define SD_D3_PIN 21
#define USE_1BIT_MODE true
#else
#error "Undefined board in sd.hpp"
#endif

class SdCard {
 private:
  uint64_t _cardSize = 0;
  bool _hasCard = false;

  // void disableD3() {
  //   digitalWrite(SD_D3_PIN, LOW);
  //   vTaskDelay(pdMS_TO_TICKS(10));
  // }

  // void enableD3() {
  //   digitalWrite(SD_D3_PIN, HIGH);
  //   vTaskDelay(pdMS_TO_TICKS(10));
  // }

 public:
  SdCard() {}
  ~SdCard() {}

  bool hasCard() const { return _hasCard; }

  bool begin() {
    // pinMode(SD_D3_PIN, OUTPUT);
    if (!SD_MMC.setPins(SD_CLK_PIN, SD_CMD_PIN, SD_D0_PIN)) {
    // if (!SD_MMC.setPins(SD_CLK_PIN, SD_CMD_PIN, SD_D0_PIN, SD_D1_PIN, SD_D2_PIN, SD_D3_PIN)) {
      Log.error(F("SD  : Pin change failed!" CR));
      _hasCard = false;
      return _hasCard;
    }
    // enableD3();

    if (SD_MMC.begin("/sdcard", USE_1BIT_MODE, true)) {
      Log.notice(F("SD  : card initialization successful" CR));
    } else {
      Log.error(F("SD  : card initialization failed!" CR));
    }
    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
      Log.warning(F("SD  : No SD card attached" CR));
      _hasCard = false;
      return _hasCard;
    } else {
      String type;

      if (cardType == CARD_MMC) {
        type = "MMC";
      } else if (cardType == CARD_SD) {
        type = "SDSC";
      } else if (cardType == CARD_SDHC) {
        type = "SDHC";
      } else {
        type = "UNKNOWN";
      }

      Log.notice(F("SD  : SD Card Type %s" CR), type.c_str());

      uint64_t totalBytes = SD_MMC.totalBytes();
      uint64_t usedBytes = SD_MMC.usedBytes();

      _cardSize = totalBytes / (1024 * 1024);
      Log.notice(F("SD  : Total space: %d bytes" CR), totalBytes);
      Log.notice(F("SD  : Used space: %d bytes" CR), usedBytes);
      Log.notice(F("SD  : Free space: %d MB" CR), totalBytes - usedBytes);
    }

    _hasCard = true;
    return _hasCard;
  }

  void end() { SD_MMC.end(); }

  File open(const String& path, const char* mode = FILE_READ,
            bool create = false) {
    if (create) {
      Log.notice(F("SD  : Creating file %s" CR), path.c_str());
      return SD_MMC.open(path.c_str(), FILE_WRITE);
    }

    return SD_MMC.open(path.c_str(), mode);
  }

  bool exists(const String& path) { return SD_MMC.exists(path.c_str()); }
  bool remove(const String& path) { return SD_MMC.remove(path.c_str()); }
  FS& getFS() { return SD_MMC; }

  uint64_t cardSize() { return _cardSize; }
  uint64_t totalBytes() { return SD_MMC.totalBytes(); }
  uint64_t usedBytes() { return SD_MMC.usedBytes(); }
};

#endif  // ENABLE_SD

#endif  // SRC_SD_HPP_
