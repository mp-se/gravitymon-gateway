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
