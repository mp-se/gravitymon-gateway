
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

#include <log.hpp>

#if defined(MMC_CLK) && defined(MMC_CMD) && defined(MMC_D0)
#include <SD_MMC.h>
#define SD SD_MMC
#elif defined(SD_CS)
#error "SD card is not yet supported on boards with shared SPI"
// #include <SD.h>
// #include <SPI.h>
// #define SD SD
#endif

class Storage {
 private:
  uint64_t _cardSize = 0;
  bool _hasCard = false;

 public:
  Storage() {}
  ~Storage() { end(); }

  bool hasCard() const { return _hasCard; }

#if defined(MMC_CLK) && defined(MMC_CMD) && defined(MMC_D0)
  bool begin() {
    SD.setPins(MMC_CLK, MMC_CMD, MMC_D0);
    if (SD.begin("/sdcard", true, false, 40000, 5)) {
#else
  // bool begin(SPIClass &spi) {
  //   pinMode(SD_CS, OUTPUT);
  //   digitalWrite(SD_CS, HIGH);  // Deselect the SD card
  //   if (SD.begin(SD_CS, SPI, 4000000)) {
#endif
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

  bool exists(const String &path) {
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

#endif  // SRC_SD_HPP_

// EOF