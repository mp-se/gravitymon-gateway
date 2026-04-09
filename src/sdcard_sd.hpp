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
#ifndef SRC_SDCARD_SD_HPP_
#define SRC_SDCARD_SD_HPP_

/**
 * This implementation of SD card support does not work in combination
 * with the TFT due to conflicts with the SPI bus and multithreading (especially
 * lvgl library).
 */

#if defined(ENABLE_SD)

#include <SD.h>

#include <log.hpp>
#include <sdcard.hpp>

class SdCardSD : public SdCard {
 public:
  uint64_t _cardSize = 0;
  bool _hasCard = false;

 public:
  SdCardSD() {}
  ~SdCardSD() { end(); }

  bool hasCard() const { return _hasCard; }

  bool begin(uint8_t cs, SPIClass &spi) {
    if (SD.begin(cs, spi)) {
      _hasCard = true;
      _cardSize = SD.cardSize();

      const char *type = "Unknown";
      switch (SD.cardType()) {
        case CARD_NONE:
          type = "No Card";
          break;
        case CARD_MMC:
          type = "MMC";
          break;
        case CARD_SD:
          type = "SDSC";
          break;
        case CARD_SDHC:
          type = "SDHC/SDXC";
          break;
      }
      Log.notice(F("SD  : Card initialized. Size: %d Mb, Type: %s." CR),
                 _cardSize / 1024 / 1024, type);
    } else {
      Log.error(F("SD  :Failed to initialize SD card." CR));
      _hasCard = false;
    }

    listFiles();

    return _hasCard;
  }

  void end() { SD.end(); }

  File open(const String &path, const char *mode = FILE_READ,
            bool create = false) {
    if (!_hasCard) {
      Log.error(F("SD  : Card not initialized." CR));
      return File();
    }

    if (create && !SD.exists(path)) {  // Create file if it does not exist
      File file = SD.open(path, FILE_WRITE);
      if (!file) {
        Log.error(F("SD  : Failed to create file." CR));
        return File();
      }
    }

    return SD.open(path, mode);
  }

  bool exists(const String &path) const {
    Serial.println("3");

    if (!_hasCard) {
      Log.error(F("SD  : Card not initialized." CR));
      return false;
    }
    return SD.exists(path);
  }

  bool remove(const String &path) {
    if (!_hasCard) {
      Log.error(F("SD  : Card not initialized." CR));
      return false;
    }
    return SD.remove(path);
  }

  bool rename(const String &from, const String &to) {
    if (!_hasCard) {
      Log.error(F("SD  : Card not initialized." CR));
      return false;
    }
    return SD.rename(from, to);
  }

  uint64_t totalBytes() const {
    if (!_hasCard) {
      Log.error(F("SD  : Card not initialized." CR));
      return 0;
    }
    return SD.totalBytes();
  }

  uint64_t usedBytes() const {
    if (!_hasCard) {
      Log.error(F("SD  : Card not initialized." CR));
      return 0;
    }
    return SD.usedBytes();
  }

  FS &getFS() const { return SD; }

  void listFiles(const char *dir = "/", uint8_t levels = 0) {
    if (!_hasCard) {
      Log.error(F("SD  : Card not initialized." CR));
      return;
    }
    File root = SD.open(dir);
    if (!root) {
      Log.error(F("SD  : Failed to open directory %s." CR), dir);
      return;
    }
    if (!root.isDirectory()) {
      Log.error(F("SD  : Not a directory: %s." CR), dir);
      root.close();
      return;
    }
    File file = root.openNextFile();
    while (file) {
      if (file.isDirectory()) {
        Log.notice(F("SD  : Dir : %s" CR), file.name());
        if (levels) {
          listFiles(file.name(), levels - 1);
        }
      } else {
        Log.notice(F("SD  : File : %s  Size : %d" CR), file.name(),
                   file.size());
      }
      file = root.openNextFile();
    }
    root.close();
  }
};

#endif  // ENABLE_SD

#endif  // SRC_SDCARD_SD_HPP_

// EOF
