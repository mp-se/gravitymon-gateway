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
#ifndef SRC_BLE_GATEWAY_HPP_
#define SRC_BLE_GATEWAY_HPP_

#if defined(GATEWAY)

#include <NimBLEAdvertisedDevice.h>
#include <NimBLEDevice.h>
#include <NimBLEScan.h>
#include <NimBLEUtils.h>

#include <array>
#include <map>
#include <measurement.hpp>
#include <queue>
#include <string>
#include <vector>

// Tilt alternates its iBeacon packets (same approach as TiltBridge):
//  - normal packet:  tx power byte = 197 (0xC5)
//  - battery packet: tx power byte = weeks on battery, only trusted once a 197
//                    has been seen (older Tilts put the real tx power there)
//  - version packet: temp == 999 and gravity < 5000, "gravity" is the firmware
//                    version and the packet must not be used as a reading
struct TiltExtraState {
  static constexpr uint8_t TX_POWER_NO_BATTERY = 197;

  bool sentNoBattery = false;
  bool hasBattery = false;
  int weeksOnBattery = -1;  // -1 = unknown
  int firmwareVersion = 0;  // 0 = unknown

  // Returns true if the packet is a measurement, false if it only carried
  // the firmware version.
  bool update(uint16_t temp, uint16_t gravity, uint8_t txPower) {
    if (temp == 999 && gravity < 5000) {
      firmwareVersion = gravity;
      return false;
    }

    if (txPower == TX_POWER_NO_BATTERY) {
      sentNoBattery = true;
    } else {
      if (sentNoBattery) hasBattery = true;
      if (hasBattery) weeksOnBattery = txPower;
    }
    return true;
  }
};

class BleDeviceCallbacks : public NimBLEScanCallbacks {
  void onResult(const NimBLEAdvertisedDevice *advertisedDevice) override;
};

class BleScanner {
 public:
  BleScanner();
  void init();
  void deInit();

  bool scan();
  void setScanTime(int scanTime) { _scanTime = scanTime; }
  void setAllowActiveScan(bool activeScan) { _activeScan = activeScan; }

  void loop(int sdLogMinTime);

  void proccesTiltBeacon(const std::string &advertStringHex,
                         const int8_t &currentRSSI);
  void proccesGravitymonBeacon(const std::string &advertStringHex,
                               NimBLEAddress address, int8_t rssi,
                               int8_t txPower);
  void processGravitymonEddystoneBeacon(NimBLEAddress address,
                                        const std::vector<uint8_t> &payload,
                                        int8_t rssi, int8_t txPower);
  void proccesRaptBeacon(const std::string &advertStringHex,
                         NimBLEAddress address, int8_t rssi, int8_t txPower);
  void proccesPressuremonBeacon(const std::string &advertStringHex,
                                NimBLEAddress address, int8_t rssi,
                                int8_t txPower);
  void proccesChamberBeacon(const std::string &advertStringHex,
                            NimBLEAddress address, int8_t rssi, int8_t txPower);

 private:
  int _scanTime = 5;
  bool _activeScan = false;
  BLEScan *_bleScan = nullptr;
  BleDeviceCallbacks *_deviceCallbacks = nullptr;
  std::queue<std::unique_ptr<MeasurementBaseData>> _bleData;
  std::map<String, uint32_t> _lastAddTimes;
  // Fixed size, one slot per TiltColor (Red..Pink), cannot grow.
  std::array<TiltExtraState, 8> _tiltExtra;

  TiltColor uuidToTiltColor(std::string uuid);
  void addData(std::unique_ptr<MeasurementBaseData> data);
};

extern BleScanner bleScanner;

#endif  // GATEWAY

#endif  // SRC_BLE_GATEWAY_HPP_
