
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
#ifndef SRC_SDCARD_SDFAT_HPP_
#define SRC_SDCARD_SDFAT_HPP_

/**
 * Note! This is experimental and the SDFat implementation is not working at the moment. 
 * 
 * Leaving the code here for now.
*/

#if defined(ENABLE_SD_SDFAT)

#define DISABLE_FS_H_WARNING
// #define ENABLE_DEDICATED_SPI 1
// #define SDFAT_FILE_TYPE 3
// #define SPI_DRIVER_SELECT 2

#include <FS.h>
#include <SdFat.h>
#include "driver/sdmmc_host.h"
#include <log.hpp>
#include <SPI.h>
#include <FSImpl.h>

class StorageFileWrapper : public fs::FileImpl {
 public:
  FsFile* _fsfile = nullptr;
  mutable char _nameBuf[32] = {0};

  StorageFileWrapper() = default;
  StorageFileWrapper(FsFile& ff) : _fsfile(&ff) {}

  static oflag_t arduinoModeToOfLag(const char* mode) {
    // Arduino: "r", "w", "a", "r+", "w+", "a+", "rb", "wb", "ab", "r+b", "w+b", "a+b"
    // SdFat: O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, O_TRUNC, O_APPEND
    if (!mode || !*mode) return O_RDONLY;
    if (strcmp(mode, "r") == 0 || strcmp(mode, "rb") == 0) return O_RDONLY;
    if (strcmp(mode, "w") == 0 || strcmp(mode, "wb") == 0) return O_WRONLY | O_CREAT | O_TRUNC;
    if (strcmp(mode, "a") == 0 || strcmp(mode, "ab") == 0) return O_WRONLY | O_CREAT | O_APPEND;
    if (strcmp(mode, "r+") == 0 || strcmp(mode, "r+b") == 0) return O_RDWR;
    if (strcmp(mode, "w+") == 0 || strcmp(mode, "w+b") == 0) return O_RDWR | O_CREAT | O_TRUNC;
    if (strcmp(mode, "a+") == 0 || strcmp(mode, "a+b") == 0) return O_RDWR | O_CREAT | O_APPEND;
    // Default fallback
    return O_RDONLY;
  }

  size_t write(const uint8_t* buf, size_t size) {
    return _fsfile ? _fsfile->write(buf, size) : 0;
  }

  size_t write(uint8_t b) { return _fsfile ? _fsfile->write(&b, 1) : 0; }

  size_t read(uint8_t* buf, size_t size) {
    return _fsfile ? _fsfile->read(buf, size) : 0;
  }
  void flush() {
    if (_fsfile) _fsfile->sync();
  }
  bool seek(uint32_t pos, SeekMode mode) {
    // Only SEEK_SET supported
    if (!_fsfile) return false;
    if (mode == fs::SeekSet) return _fsfile->seekSet(pos);
    // Not implemented: SEEK_CUR, SEEK_END
    return false;
  }
  size_t position() const { return _fsfile ? _fsfile->curPosition() : 0; }
  size_t size() const { return _fsfile ? _fsfile->fileSize() : 0; }
  bool setBufferSize(size_t size) { return false; }  // Not supported
  void close() {
    if (_fsfile) _fsfile->close();
  }
  time_t getLastWrite() { return 0; }           // Not supported
  const char* path() const { return nullptr; }  // Not supported
  const char* name() const {
    if (_fsfile) _fsfile->getName(_nameBuf, sizeof(_nameBuf));
    return _nameBuf;
  }
  boolean isDirectory(void) { return _fsfile && _fsfile->isDir(); }
  fs::FileImplPtr openNextFile(const char* mode = FILE_READ) override {
    // Convert Arduino mode string to oflag_t for SdFat
    if (!_fsfile) return nullptr;
    oflag_t flags = arduinoModeToOfLag(mode);
    FsFile next = _fsfile->openNextFile(flags);
    if (next.isOpen()) return std::make_shared<StorageFileWrapper>(next);
    return nullptr;
  }
  boolean seekDir(long position) { return false; }          // Not supported
  String getNextFileName(void) { return String(); }         // Not supported
  String getNextFileName(bool* isDir) { return String(); }  // Not supported
  void rewindDirectory(void) { /* Not supported */ }
  operator bool() { return _fsfile && _fsfile->isOpen(); }

  // // --- Convenience Arduino-style methods ---
  // int read() { return _fsfile ? _fsfile->read() : -1; }
  // int peek() { return _fsfile ? _fsfile->peek() : -1; }
  // int available() { return _fsfile ? _fsfile->available() : 0; }
  // // ...existing code...
};

// FSImpl-based wrapper for SdFat
class StorageFSWrapper : public fs::FSImpl {
 public:
  enum Mode { SPI, SDIO };

  SdFat& _sd;
  Mode _mode = Mode::SPI;

  StorageFSWrapper(SdFat& sd) : _sd(sd) {}

  fs::FileImplPtr open(const char* path, const char* mode, bool create) override {
    oflag_t flags = StorageFileWrapper::arduinoModeToOfLag(mode);
    if (create) flags |= O_CREAT;
    FsFile f = _sd.open(path, flags);
    if (f.isOpen()) return std::make_shared<StorageFileWrapper>(f);
    return nullptr;
  }
  bool exists(const char* path) override { return _sd.exists(path); }
  bool remove(const char* path) override { return _sd.remove(path); }
  bool rename(const char* pathFrom, const char* pathTo) override { return _sd.rename(pathFrom, pathTo); }
  bool mkdir(const char* path) override { return _sd.mkdir(path); }
  bool rmdir(const char* path) override { return _sd.rmdir(path); }
  void end() { _sd.end(); }
  // Optionally add cardSize, totalBytes, usedBytes as non-virtual helpers
  uint64_t cardSize() const { return _sd.card() ? _sd.card()->sectorCount() * 512ULL : 0; }
  uint64_t totalBytes() const { return _sd.vol() ? _sd.vol()->clusterCount() * _sd.vol()->bytesPerCluster() : 0; }
  uint64_t usedBytes() const { return totalBytes() - (_sd.vol() ? _sd.vol()->freeClusterCount() * _sd.vol()->bytesPerCluster() : 0); }
};

class Storage {
 private:
  uint64_t _cardSize = 0;
  bool _hasCard = false;
  SdFs _sd;
  StorageFSWrapper _fs;
  fs::FS _fsObj;

 public:
  Storage() : _sd(), _fs(_sd), _fsObj(std::shared_ptr<StorageFSWrapper>(&_fs, [](StorageFSWrapper*){})) {}
  ~Storage() { end(); }

  bool hasCard() const { return _hasCard; }

  // Initialize SD card using a shared SPI bus (SPIClass reference)
  bool begin(uint8_t csPin, SPIClass* spi = nullptr, uint32_t speed = SD_SCK_MHZ(4)) {
    // pinMode(csPin, OUTPUT);
    // digitalWrite(csPin, HIGH);
    // SdSpiConfig spiConfig(csPin, SHARED_SPI, speed, &spi);
    // SdSpiConfig spiConfig(csPin);
    // if (_sd.begin(spiConfig)) {
    // if (_sd.begin(csPin)) {
    if (!_sd.begin(SdSpiConfig(SD_CS, DEDICATED_SPI, SD_SCK_MHZ(25)))) {
      _hasCard = true;
      _cardSize = _fs.cardSize();
      Log.notice(F("SD  : Card initialized (shared SPI). Size: %d Mb." CR),
                 _cardSize / 1024 / 1024);
    } else {
      Log.error(F("SD  :Failed to initialize SD card (shared SPI)." CR));
      _hasCard = false;
    }
    listFiles();
    return _hasCard;
  }

  void end() { _fs.end(); }

  fs::File open(const String& path, const char* mode = "r", bool create = false) {
    if (!_hasCard) {
      Log.error(F("SD  : Card not initialized." CR));
      return fs::File();
    }
    if (create && !_fs.exists(path.c_str())) {
      auto fileImplPtr = _fs.open(path.c_str(), "w+", true);
      fs::File file(fileImplPtr);
      if (!file) {
        Log.error(F("SD  : Failed to create file." CR));
        return fs::File();
      }
    }
    auto fileImplPtr = _fs.open(path.c_str(), mode, false);
    return fs::File(fileImplPtr);
  }

  bool exists(const String& path) {
    if (!_hasCard) {
      Log.error(F("SD  : Card not initialized." CR));
      return false;
    }
    return _fs.exists(path.c_str());
  }

  bool remove(const String& path) {
    if (!_hasCard) {
      Log.error(F("SD  : Card not initialized." CR));
      return false;
    }
    return _fs.remove(path.c_str());
  }

  bool rename(const String& from, const String& to) {
    if (!_hasCard) {
      Log.error(F("SD  : Card not initialized." CR));
      return false;
    }
    return _fs.rename(from.c_str(), to.c_str());
  }

  uint64_t totalBytes() const {
    if (!_hasCard) {
      Log.error(F("SD  : Card not initialized." CR));
      return 0;
    }
    return _fs.totalBytes();
  }

  uint64_t usedBytes() const {
    if (!_hasCard) {
      Log.error(F("SD  : Card not initialized." CR));
      return 0;
    }
    return _fs.usedBytes();
  }

  // Return a reference to a persistent fs::FS object using the persistent FSImpl wrapper
  fs::FS& getFS() { return _fsObj; }

  void listFiles(const char* dir = "/", uint8_t levels = 0) {
    if (!_hasCard) {
      Log.error(F("SD  : Card not initialized." CR));
      return;
    }
    fs::File root(_fs.open(dir, "r", false));
    if (!root) {
      Log.error(F("SD  : Failed to open directory %s." CR), dir);
      return;
    }
    if (!root.isDirectory()) {
      Log.error(F("SD  : Not a directory: %s." CR), dir);
      root.close();
      return;
    }
    fs::File file = root.openNextFile();
    while (file) {
      if (file.isDirectory()) {
        Log.notice(F("SD  : Dir : %s" CR), file.name());
        if (levels) {
          listFiles(file.name(), levels - 1);
        }
      } else {
        Log.notice(F("SD  : File : %s  Size : %d" CR), file.name(), file.size());
      }
      file = root.openNextFile();
    }
    root.close();
  }
};

#endif  // ENABLE_SD_SDFAT

#endif  // SRC_SDCARD_SDFAT_HPP_

// EOF
