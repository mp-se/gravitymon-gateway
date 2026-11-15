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

#include <algorithm>

#include <log.hpp>
#include <mdns_discovery.hpp>
#include <sdcard_mmc.hpp>
#include <sdcard_sd.hpp>
#include <utility>
#include <vector>

constexpr const char *DEFAULT_FILENAME = "/mdns_devices.json";
constexpr size_t MAX_DEVICES = 20;  // Cap to limit internal RAM usage
constexpr time_t MAX_AGE_SECONDS = 30 * 24 * 60 * 60;  // Forget after 30 days

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
      if (ip == IPAddress(0, 0, 0, 0)) continue;  // Unresolved address

      // Only keep the TXT fields we use. Devices without an id cannot be
      // matched against BLE/push data, so they are not stored at all.
      std::vector<std::pair<String, String>> txt;
      for (int k = 0; k < MDNS.numTxt(i); k++) {
        String key = MDNS.txtKey(i, k);
        if (key == "id" || key == "app") {
          txt.emplace_back(key, MDNS.txt(i, k));
        }
      }
      String id = txtValue(txt, "id");
      if (!id.length() || !isSupportedApp(txtValue(txt, "app"))) continue;

      // Same id means same device, even if its name or ip has changed
      int idx = findDeviceIndexById(id);
      if (idx < 0) {
        MdnsDevice d;
        d.name = String(nameBuf);
        d.ip = ip;
        d.port = port;
        d.lastSeen = (t > 0) ? t : 0;
        d.txt = std::move(txt);
        _devices.emplace_back(std::move(d));
        Log.verbose(F("MDNS: Found new device %s" CR), nameBuf);
      } else {
        MdnsDevice &d = _devices[idx];
        d.name = String(nameBuf);
        d.ip = ip;
        d.port = port;
        d.lastSeen = (t > 0) ? t : 0;
        d.txt = std::move(txt);
      }
    }
  }

  prune(t);
}

// Only devices the gateway can receive BLE data from are of interest
bool MdnsScanner::isSupportedApp(const String &app) {
  static const char *const apps[] = {"gravitymon", "pressuremon",
                                     "chamber-controller"};
  for (const char *a : apps) {
    if (app.equalsIgnoreCase(a)) return true;
  }
  return false;
}

String MdnsScanner::txtValue(
    const std::vector<std::pair<String, String>> &txt, const char *key) {
  for (const auto &p : txt) {
    if (p.first == key) return p.second;
  }
  return "";
}

int MdnsScanner::findDeviceIndexById(const String &id) const {
  for (size_t i = 0; i < _devices.size(); ++i) {
    if (txtValue(_devices[i].txt, "id") == id) return static_cast<int>(i);
  }
  return -1;
}

void MdnsScanner::prune(time_t now) {
  // Drop devices without id and (when the clock is valid) stale ones
  bool clockOk = now > 1700000000;
  _devices.erase(
      std::remove_if(_devices.begin(), _devices.end(),
                     [&](const MdnsDevice &d) {
                       if (!txtValue(d.txt, "id").length() ||
                           !isSupportedApp(txtValue(d.txt, "app")))
                         return true;
                       return clockOk && d.lastSeen > 0 &&
                              (now - d.lastSeen) > MAX_AGE_SECONDS;
                     }),
      _devices.end());

  // Cap the list, evicting the least recently seen devices first
  while (_devices.size() > MAX_DEVICES) {
    auto oldest = std::min_element(
        _devices.begin(), _devices.end(),
        [](const MdnsDevice &a, const MdnsDevice &b) {
          return a.lastSeen < b.lastSeen;
        });
    _devices.erase(oldest);
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

size_t MdnsScanner::writeJson(Print &out) const {
  size_t n = out.print("{\"mdns\":[");
  bool first = true;
  for (const auto &d : _devices) {
    JsonDocument doc;  // Small, per-device document
    JsonObject o = doc.to<JsonObject>();
    o["name"] = d.name.c_str();
    o["ip"] = d.ip.toString().c_str();
    o["port"] = d.port;
    o["last_seen"] = static_cast<uint32_t>(d.lastSeen);
    JsonObject ta = o["txt"].to<JsonObject>();
    for (const auto &t : d.txt) {
      ta[t.first] = t.second;
    }
    if (!first) n += out.print(',');
    first = false;
    n += serializeJson(doc, out);
  }
  n += out.print("]}");
  return n;
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

  size_t written = writeJson(f);
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
  prune(time(nullptr));  // Also shrinks a list saved by older firmware
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
