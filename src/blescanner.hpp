/*
MIT License

Copyright (c) 2023-2024 Magnus

Based on code created by John Beeler on 5/12/18 (Tiltbridge project).

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

#ifndef SRC_BLESCANNER_HPP_
#define SRC_BLESCANNER_HPP_

#include <ArduinoJson.h>
#include <ArduinoLog.h>

#undef LOG_LEVEL_ERROR
#undef LOG_LEVEL_INFO

#include <NimBLEAdvertisedDevice.h>
#include <NimBLEDevice.h>
#include <NimBLEScan.h>
#include <NimBLEUtils.h>

#include <queue>
#include <string>

constexpr auto PARAM_BLE_ID = "ID";
constexpr auto PARAM_BLE_TEMP = "temp";
constexpr auto PARAM_BLE_TEMPERATURE = "temperature";
constexpr auto PARAM_BLE_GRAVITY = "gravity";
constexpr auto PARAM_BLE_ANGLE = "angle";
constexpr auto PARAM_BLE_BATTERY = "battery";
constexpr auto PARAM_BLE_RSSI = "RSSI";
constexpr auto PARAM_BLE_NAME = "name";
constexpr auto PARAM_BLE_TOKEN = "token";
constexpr auto PARAM_BLE_INTERVAL = "interval";
constexpr auto PARAM_BLE_TEMP_UNITS = "temp_units";

class BleDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
  void onResult(NimBLEAdvertisedDevice *advertisedDevice) override;
};

class BleClientCallbacks : public NimBLEClientCallbacks {
  void onConnect(NimBLEClient *pClient) override;
};

enum TiltColor {
  None = -1,
  Red = 0,
  Green = 1,
  Black = 2,
  Purple = 3,
  Orange = 4,
  Blue = 5,
  Yellow = 6,
  Pink = 7
};

class TiltData {
 public:
  // Data points
  float tempF = 0;
  float gravity = 0;
  int txPower = 0;
  int rssi = 0;

  // Internal stuff
  bool updated = false;
  struct tm timeinfoUpdated;
  uint32_t timeUpdated = 0;
  uint32_t timePushed = 0;

  void setUpdated() {
    updated = true;
    timeUpdated = millis();
    getLocalTime(&timeinfoUpdated);
  }

  void setPushed() {
    updated = false;
    timePushed = millis();
  }

  uint32_t getUpdateAge() { return (millis() - timeUpdated) / 1000; }
  uint32_t getPushAge() { return (millis() - timePushed) / 1000; }
};

class GravitymonData {
 public:
  // Data points
  float tempC = 0;
  float gravity = 0;
  float angle = 0;
  float battery = 0;
  int txPower = 0;
  int rssi = 0;
  String id = "";
  String name = "";
  String token = "";
  int interval = 0;

  // Internal stuff
  NimBLEAddress address;
  String type = "";
  String data = "";
  bool updated = false;
  struct tm timeinfoUpdated;
  uint32_t timeUpdated = 0;
  uint32_t timePushed = 0;

  void setUpdated() {
    updated = true;
    timeUpdated = millis();
    getLocalTime(&timeinfoUpdated);
  }

  void setPushed() {
    updated = false;
    timePushed = millis();
  }

  uint32_t getUpdateAge() { return (millis() - timeUpdated) / 1000; }
  uint32_t getPushAge() { return (millis() - timePushed) / 1000; }
};

const auto NO_TILT_COLORS =
    8;  // Number of tilt devices that can be managed (one per color)
const auto NO_GRAVITYMON =
    8;  // Number of gravitymon devices that can be handled

class BleScanner {
 public:
  BleScanner();
  void init();
  void deInit();

  bool scan();
  bool waitForScan();

  void setScanTime(int scanTime) { _scanTime = scanTime; }
  void setAllowActiveScan(bool activeScan) { _activeScan = activeScan; }

  TiltColor proccesTiltBeacon(const std::string &advertStringHex,
                              const int8_t &currentRSSI);
  void proccesGravitymonBeacon(const std::string &advertStringHex,
                               NimBLEAddress address);

  void processGravitymonDevice(NimBLEAddress address);
  void processGravitymonEddystoneBeacon(NimBLEAddress address,
                                        const uint8_t *payload);
  void processGravitymonExtBeacon(NimBLEAddress address,
                                  const std::string &payload);

  TiltData &getTiltData(TiltColor col) { return _tilt[col]; }
  int findGravitymonId(String id) {
    for (int i = 0; i < NO_GRAVITYMON; i++)
      if (_gravitymon[i].id == id || _gravitymon[i].id == "") return i;
    return -1;
  }
  GravitymonData &getGravitymonData(int idx) { return _gravitymon[idx]; }

  const char *getTiltColorAsString(TiltColor col);

 private:
  int _scanTime = 5;
  bool _activeScan = false;

  BLEScan *_bleScan = nullptr;

  BleDeviceCallbacks *_deviceCallbacks = nullptr;
  BleClientCallbacks *_clientCallbacks = nullptr;

  // Tilt related data
  TiltData _tilt[NO_TILT_COLORS];

  // Gravitymon related data
  GravitymonData _gravitymon[NO_GRAVITYMON];
  std::queue<NimBLEAddress> _doConnect;

  TiltColor uuidToTiltColor(std::string uuid);
  bool connectGravitymonDevice(NimBLEAddress address);
};

extern BleScanner bleScanner;

#endif  // SRC_BLESCANNER_HPP_
