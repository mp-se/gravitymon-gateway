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
#ifndef SRC_MDNS_DISCOVERY_HPP_
#define SRC_MDNS_DISCOVERY_HPP_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <IPAddress.h>

#include <looptimer.hpp>
#include <map>
#include <utility>
#include <vector>

struct MdnsDevice {
  String name;
  IPAddress ip;
  uint16_t port = 0;
  std::vector<std::pair<String, String>> txt;
  time_t lastSeen = 0;  // 0 means unknown/not available
};

class MdnsScanner {
 public:
  explicit MdnsScanner(uint32_t scanIntervalMs = 30000);
  void setup();
  void loop();

  const std::vector<MdnsDevice> &getDevices() const;

  bool saveToFile();
  bool loadFromFile();
  void populateJson(JsonObject &doc) const;
  void parseJson(const JsonDocument &doc);
  void clear();

  String findDeviceByTxt(const String &key, const String &value,
                         bool valueOnNotFound = false) const;

 private:
  std::vector<MdnsDevice> _devices;
  uint32_t _scanIntervalMs;
  LoopTimer _scanTimer{0};
  LoopTimer _saveTimer{1000 * 60 * 30};  // Save mdns data every 30 minutes

  void scan();
  int findDeviceIndex(const String &name, const IPAddress &ip,
                      uint16_t port) const;
};

#endif  // SRC_MDNS_DISCOVERY_HPP_

// EOF
