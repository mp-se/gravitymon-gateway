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
#ifndef SRC_SDCARD_HPP_
#define SRC_SDCARD_HPP_

#include <FS.h>

#include <log.hpp>

class SdCard {
 public:
  SdCard() {}
  virtual ~SdCard() = default;

  virtual bool hasCard() const = 0;
  virtual void end() = 0;
  virtual File open(const String &path, const char *mode = FILE_READ,
                    bool create = false) = 0;
  virtual bool exists(const String &path) const = 0;
  virtual bool remove(const String &path) = 0;
  virtual bool rename(const String &from, const String &to) = 0;
  virtual uint64_t totalBytes() const = 0;
  virtual uint64_t usedBytes() const = 0;
  virtual FS &getFS() const = 0;
  virtual void listFiles(const char *dir = "/", uint8_t levels = 0) = 0;
};

#endif  // SRC_SDCARD_HPP_

// EOF
