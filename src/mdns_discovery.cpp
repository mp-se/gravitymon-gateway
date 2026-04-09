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
#include <ESPmDNS.h>
#include <WiFi.h>

#include <log.hpp>
#include <mdns_discovery.hpp>
#include <sdcard_mmc.hpp>
#include <sdcard_sd.hpp>
#include <utility>
#include <vector>

constexpr const char *DEFAULT_FILENAME = "/mdns_devices.json";

#if defined(ENABLE_MMC)
extern SdCardMMC mySdStorage;
#elif defined(ENABLE_SD)
extern SdCardSD mySdStorage;
#endif

MdnsScanner::MdnsScanner(uint32_t scanIntervalMs)
    : _scanIntervalMs(scanIntervalMs), _scanTimer(scanIntervalMs) {}

void MdnsScanner::setup() {
  // Assume filesystem is mounted for persistence
  // Assument networking and MDNS is already initialized
  loadFromFile();
}

void MdnsScanner::loop() {
  if (_scanTimer.hasExpired()) {
    _scanTimer.reset();

    scan();
  }

  if (_saveTimer.hasExpired()) {
    _saveTimer.reset();

    saveToFile();
  }
}

void MdnsScanner::scan() {
  Log.notice(F("MDNS: Scanning for mDNS services on the network." CR));

  // Static list of services to query
  const std::pair<String, String> servicesToQuery[] = {
      {String("http"), String("tcp")},
      {String("chamberctrl"), String("tcp")},
      {String("gravitymon"), String("tcp")},
      {String("pressuremon"), String("tcp")},
      {String("kegmon"), String("tcp")},
  };

  time_t t = time(nullptr);

  for (const auto &sp : servicesToQuery) {
    String svc = sp.first;
    String proto = sp.second.length() ? sp.second : String("tcp");

    // normalize service name (strip leading '_' or trailing parts)
    if (svc.startsWith("_")) {
      svc = svc.substring(1);
    }
    int p = svc.indexOf("._");
    if (p >= 0) svc = svc.substring(0, p);

    int n = MDNS.queryService(svc.c_str(), proto.c_str());
    if (n <= 0) {
      Log.verbose(F("MDNS: No services of type %s/%s found." CR), svc.c_str(),
                  proto.c_str());
      continue;
    }

    for (int i = 0; i < n; ++i) {
      char nameBuf[64];
      strncpy(nameBuf, MDNS.hostname(i).c_str(), sizeof(nameBuf) - 1);
      nameBuf[sizeof(nameBuf) - 1] = '\0';
      if (strlen(nameBuf) == 0) continue;  // Skip invalid hostnames
#if ESP_ARDUINO_VERSION_MAJOR >= 3
      IPAddress ip = MDNS.address(i);  // For Arduino Core 3.x
#else
      IPAddress ip = MDNS.IP(i);  // For Arduino Core 2.x
#endif
      uint16_t port = MDNS.port(i);
      if (port == 0) continue;  // Skip invalid ports

      int idx = findDeviceIndex(String(nameBuf), ip, port);
      if (idx < 0) {
        MdnsDevice d;
        d.name = String(nameBuf);
        d.ip = ip;
        d.port = port;
        d.lastSeen = (t > 0) ? t : 0;
        d.txt.clear();

        // Parse TXT records - iterate through available TXT fields
        for (int t = 0; t < MDNS.numTxt(i); t++) {
          String key = MDNS.txtKey(i, t);
          String value = MDNS.txt(i, t);
          if (key.length() > 0) {
            d.txt.emplace_back(std::make_pair(key, value));
          }
        }

        _devices.emplace_back(std::move(d));
        Log.verbose(F("MDNS: Found new device %s" CR), nameBuf);
      } else {
        MdnsDevice &d = _devices[idx];
        d.lastSeen = (t > 0) ? t : 0;

        // update IP/port in case it changed
        d.ip = ip;
        d.port = port;
        d.txt.clear();

        // Parse TXT records - iterate through available TXT fields
        for (int t = 0; t < MDNS.numTxt(i); t++) {
          String key = MDNS.txtKey(i, t);
          String value = MDNS.txt(i, t);
          if (key.length() > 0) {
            d.txt.emplace_back(std::make_pair(key, value));
          }
        }
      }
    }
  }
}

const std::vector<MdnsDevice> &MdnsScanner::getDevices() const {
  return _devices;
}

void MdnsScanner::populateJson(JsonObject &doc) const {
  JsonArray arr = doc["mdns"].to<JsonArray>();
  for (const auto &d : _devices) {
    JsonObject o = arr.add<JsonObject>();
    o["name"] = d.name.c_str();
    o["ip"] = d.ip.toString().c_str();
    o["port"] = d.port;
    o["last_seen"] = static_cast<uint32_t>(d.lastSeen);
    JsonObject ta = o["txt"].to<JsonObject>();
    for (const auto &t : d.txt) {
      ta[t.first] = t.second;
    }
  }
}

bool MdnsScanner::saveToFile() {
#if defined(ENABLE_MMC) || defined(ENABLE_SD)
  const char *fn = DEFAULT_FILENAME;
  if (!mySdStorage.hasCard()) {
    Log.warning(F("MDNS: SD card not available, cannot save." CR));
    return false;
  }

  File f = mySdStorage.open(String(fn), "w", true);
  if (!f) {
    Log.error(F("MDNS: Failed to open %s for writing." CR), fn);
    return false;
  }

  JsonDocument doc;
  JsonObject obj = doc.to<JsonObject>();
  populateJson(obj);

  size_t written = serializeJson(doc, f);
  if (written == 0) {
    Log.error(F("MDNS: Failed to write JSON to %s." CR), fn);
    f.close();
    return false;
  }

  f.close();
  Log.info(F("MDNS: Saved %d devices to %s." CR), _devices.size(), fn);
#endif  // ENABLE_MMC || ENABLE_SD
  return true;
}

bool MdnsScanner::loadFromFile() {
#if defined(ENABLE_MMC) || defined(ENABLE_SD)
  const char *fn = DEFAULT_FILENAME;
  if (!mySdStorage.hasCard()) {
    Log.warning(F("MDNS: SD card not available, cannot load." CR));
    return false;
  }

  if (!mySdStorage.exists(String(fn))) {
    Log.info(F("MDNS: %s does not exist, nothing to load." CR), fn);
    return true;
  }

  File f = mySdStorage.open(String(fn), "r");
  if (!f) {
    Log.error(F("MDNS: Failed to open %s for reading." CR), fn);
    return false;
  }

  size_t sz = f.size();
  if (sz == 0) {
    f.close();
    return true;
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err) {
    Log.error(F("MDNS: Failed to parse JSON %s: %s" CR), fn, err.c_str());
    return false;
  }

  parseJson(doc);
  Log.info(F("MDNS: Loaded %d devices from %s." CR), _devices.size(), fn);
#endif  // ENABLE_MMC || ENABLE_SD
  return true;
}

void MdnsScanner::parseJson(const JsonDocument &doc) {
  _devices.clear();
  JsonArrayConst arr = doc["mdns"].as<JsonArrayConst>();
  for (JsonVariantConst v : arr) {
    JsonObjectConst o = v.as<JsonObjectConst>();
    MdnsDevice d;
    d.name = o["name"].as<const char *>();
    d.ip.fromString(o["ip"].as<const char *>());
    d.port = static_cast<uint16_t>(o["port"].as<uint32_t>());
    d.lastSeen = (time_t)(o["last_seen"].as<uint32_t>());
    d.txt.clear();
    if (o["txt"].is<JsonObjectConst>()) {
      for (JsonPairConst kv : o["txt"].as<JsonObjectConst>()) {
        String key(kv.key().c_str());
        String value = kv.value().as<const char *>();
        d.txt.emplace_back(std::make_pair(key, value));
      }
    }
    _devices.emplace_back(std::move(d));
  }
}

void MdnsScanner::clear() { _devices.clear(); }

String MdnsScanner::findDeviceByTxt(const String &key, const String &value,
                                    bool valueOnNotFound) const {
  for (const auto &d : _devices) {
    for (const auto &t : d.txt) {
      if (t.first == key && t.second == value) {
        return d.name;
      }
    }
  }

  return valueOnNotFound ? value : "";
}

int MdnsScanner::findDeviceIndex(const String &name, const IPAddress &ip,
                                 uint16_t port) const {
  for (size_t i = 0; i < _devices.size(); ++i) {
    const MdnsDevice &d = _devices[i];
    if (d.name == name && d.ip == ip && d.port == port)
      return static_cast<int>(i);
  }
  return -1;
}

// EOF
