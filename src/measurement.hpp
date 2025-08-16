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
#ifndef SRC_MEASUREMENT_HPP_
#define SRC_MEASUREMENT_HPP_

#if defined(GATEWAY)

#include <Arduino.h>
#include <FS.h>

#include <cstdio>
#include <deque>
#include <memory>
#include <sdcard_mmc.hpp>
#include <sdcard_sd.hpp>
#include <utility>
#include <utils.hpp>

#if defined(ENABLE_MMC) || defined(ENABLE_SD)
extern Storage mySdStorage;
#endif

enum MeasurementType {
  NoType = 0,
  Tilt = 1,
  TiltPro = 2,
  Gravitymon = 3,
  Pressuremon = 4,
  Chamber = 5,
  Rapt = 6,
};

enum MeasurementSource {
  NoSource = 0,
  BleBeacon = 1,
  BleEddyStone = 2,
  HttpPost = 3,
};

// Container for the measurement data
class MeasurementBaseData {
 private:
  MeasurementType _type = MeasurementType::NoType;
  MeasurementSource _source = MeasurementSource::NoSource;
  String _id;
  String _created;

 public:
  MeasurementBaseData(String id, MeasurementType type, MeasurementSource src) {
    _id = id;
    _type = type;
    _source = src;

    struct tm time;
    getLocalTime(&time);

    char buf[40];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
             time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour,
             time.tm_min, time.tm_sec);
    _created = String(buf);
  }
  virtual ~MeasurementBaseData() {}

  virtual void writeToFile(File& file) const {}

  const char* getCreatedAsString() const { return _created.c_str(); }

  MeasurementType getType() const { return _type; }
  const char* getTypeAsString() const {
    switch (_type) {
      case MeasurementType::Tilt:
        return "Tilt";
      case MeasurementType::TiltPro:
        return "Tilt Pro";
      case MeasurementType::Gravitymon:
        return "Gravitymon";
      case MeasurementType::Pressuremon:
        return "Pressuremon";
      case MeasurementType::Chamber:
        return "Chamber Controller";
      case MeasurementType::Rapt:
        return "RAPT";
      default:
        return "";
    }
  }

  MeasurementSource getSource() const { return _source; }
  const char* getSourceAsString() const {
    switch (_source) {
      case MeasurementSource::BleBeacon:
        return "BLE Beacon";
      case MeasurementSource::BleEddyStone:
        return "BLE Eddystone";
      case MeasurementSource::HttpPost:
        return "HTTP Post";
      default:
        return "";
    }
  }

  const char* getId() const { return _id.c_str(); }
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

class TiltData : public MeasurementBaseData {
 private:
  float _tempF = 0;
  float _gravity = 0;
  int _txPower = 0;
  int _rssi = 0;
  TiltColor _tiltColor;

  const char* colorToString(TiltColor color) const {
    switch (color) {
      case TiltColor::Red:
        return "Red";
      case TiltColor::Green:
        return "Green";
      case TiltColor::Black:
        return "Black";
      case TiltColor::Purple:
        return "Purple";
      case TiltColor::Orange:
        return "Orange";
      case TiltColor::Blue:
        return "Blue";
      case TiltColor::Yellow:
        return "Yellow";
      case TiltColor::Pink:
        return "Pink";
      default:
        return "";
    }
  }

 public:
  TiltData(MeasurementSource source, TiltColor color, float tempF,
           float gravity, int txPower, int rssi, bool pro)
      : MeasurementBaseData(
            colorToString(color),
            pro ? MeasurementType::TiltPro : MeasurementType::Tilt, source) {
    _tiltColor = color;
    _tempF = tempF;
    _gravity = gravity;
    _txPower = txPower;
    _rssi = rssi;
  }
  virtual ~TiltData() {}

  float getTempF() const { return _tempF; }
  float getTempC() const { return convertFtoC(_tempF); }

  float getGravity() const { return _gravity; }
  int getTxPower() const { return _txPower; }
  int getRssi() const { return _rssi; }
  TiltColor getTiltColor() const { return _tiltColor; }

  void writeToFile(File& file) const {
    char buffer[300];

    // Data parameters
    // ----------------------------------------
    // 0, Format Version (1)
    // 1, Type
    // 2, Source
    // 3, Created (timestamp)
    // 4, ID
    // 5, Color
    // 6, Temperature (C)
    // 7, Gravity (SG)
    // 8, Tx Power
    // 9, Rssi
    // 10,
    // 11,
    // 12,
    // 13,

    snprintf(buffer, sizeof(buffer),
             "1,%s,%s,%s,%s,%s,"
             "%.2f,%.4f,%d,%d,,,,",
             getTypeAsString(), getSourceAsString(), getCreatedAsString(),
             getId(), colorToString(_tiltColor), getTempC(), getGravity(),
             getTxPower(), getRssi());
    file.println(buffer);
  }
};

class GravityData : public MeasurementBaseData {
 private:
  String _name = "";
  String _token = "";
  float _tempC = 0;
  float _gravity = 0;
  float _angle = 0;
  float _battery = 0;
  int _rssi = 0;
  int _txPower = 0;
  int _interval = 0;

 public:
  GravityData(MeasurementSource source, String id, String name, String token,
              float tempC, float gravity, float angle, float battery,
              int txPower, int rssi, int interval)
      : MeasurementBaseData(id, MeasurementType::Gravitymon, source) {
    _name = name;
    _token = token;
    _tempC = tempC;
    _gravity = gravity;
    _angle = angle;
    _battery = battery;
    _txPower = txPower;
    _rssi = rssi;
    _interval = interval;
  }
  virtual ~GravityData() {}

  const char* getName() const { return _name.c_str(); }
  const char* getToken() const { return _token.c_str(); }
  float getTempC() const { return _tempC; }
  float getGravity() const { return _gravity; }
  float getAngle() const { return _angle; }
  float getBattery() const { return _battery; }
  int getTxPower() const { return _txPower; }
  int getRssi() const { return _rssi; }
  int getInterval() const { return _interval; }

  void writeToFile(File& file) const {
    char buffer[300];

    // Data parameters
    // ----------------------------------------
    // 0, Format Version (1)
    // 1, Type
    // 2, Source
    // 3, Created (timestamp)
    // 4, ID
    // 5, Name
    // 6, Token
    // 7, Temperature (C)
    // 8, Gravity (SG)
    // 9, Angle
    // 10, Battery
    // 11, Tx Power
    // 12, Rssi
    // 13, Interval

    snprintf(buffer, sizeof(buffer),
             "1,%s,%s,%s,%s,%s,%s,"
             "%.2f,%.4f,%.4f,%.2f,%d,%d,%d",
             getTypeAsString(), getSourceAsString(), getCreatedAsString(),
             getId(), getName(), getToken(), getTempC(), getGravity(),
             getAngle(), getBattery(), getTxPower(), getRssi(), getInterval());
    file.println(buffer);
  }
};

class PressureData : public MeasurementBaseData {
 private:
  String _name = "";
  String _token = "";
  float _tempC = 0;
  float _pressure = 0;
  float _pressure1 = 0;
  float _battery = 0;
  int _rssi = 0;
  int _txPower = 0;
  int _interval = 0;

 public:
  PressureData(MeasurementSource source, String id, String name, String token,
               float tempC, float pressure, float pressure1, float battery,
               int txPower, int rssi, int interval)
      : MeasurementBaseData(id, MeasurementType::Pressuremon, source) {
    _name = name;
    _token = token;
    _tempC = tempC;
    _pressure = pressure;
    _pressure1 = pressure1;
    _battery = battery;
    _txPower = txPower;
    _rssi = rssi;
    _interval = interval;
  }
  virtual ~PressureData() {}

  const char* getName() const { return _name.c_str(); }
  const char* getToken() const { return _token.c_str(); }
  float getTempC() const { return _tempC; }
  float getPressure() const { return _pressure; }
  float getPressure1() const { return _pressure1; }
  float getBattery() const { return _battery; }
  int getTxPower() const { return _txPower; }
  int getRssi() const { return _rssi; }
  int getInterval() const { return _interval; }

  void writeToFile(File& file) const {
    char buffer[300];

    // Data parameters
    // ----------------------------------------
    // 0, Format Version (1)
    // 1, Type
    // 2, Source
    // 3, Created (timestamp)
    // 4, ID
    // 5, Name
    // 6, Token
    // 7, Temperature (C)
    // 8, Pressure (PSI)
    // 9, Pressure1 (PSI)
    // 10, Battery
    // 11, Tx Power
    // 12, Rssi
    // 13, Interval

    snprintf(buffer, sizeof(buffer),
             "1,%s,%s,%s,%s,%s,%s,"
             "%.2f,%.4f,%.4f,%.2f,%d,%d,%d",
             getTypeAsString(), getSourceAsString(), getCreatedAsString(),
             getId(), getName(), getToken(), getTempC(), getPressure(),
             getPressure1(), getBattery(), getTxPower(), getRssi(),
             getInterval());
    file.println(buffer);
  }
};

class ChamberData : public MeasurementBaseData {
 private:
  float _chamberTempC = 0;
  float _beerTempC = 0;
  int _rssi = 0;

 public:
  ChamberData(MeasurementSource source, String id, float chamberTempC,
              float beerTempC, int rssi)
      : MeasurementBaseData(id, MeasurementType::Chamber, source) {
    _chamberTempC = chamberTempC;
    _beerTempC = beerTempC;
    _rssi = rssi;
  }
  virtual ~ChamberData() {}

  float getChamberTempC() const { return _chamberTempC; }
  float getBeerTempC() const { return _beerTempC; }
  int getRssi() const { return _rssi; }

  void writeToFile(File& file) const {
    char buffer[300];

    // Data parameters
    // ----------------------------------------
    // 0, Format Version (1)
    // 1, Type
    // 2, Source
    // 3, Created (timestamp)
    // 4, ID
    // 5, ChamberTemp (C)
    // 6, BeerTemp (C)
    // 7, Rssi
    // 8,
    // 9,
    // 10,
    // 11,
    // 12,
    // 13,

    snprintf(buffer, sizeof(buffer),
             "1,%s,%s,%s,%s,"
             "%.2f,%.2f,%d,,,,,,",
             getTypeAsString(), getSourceAsString(), getCreatedAsString(),
             getId(), getChamberTempC(), getBeerTempC(), getRssi());
    file.println(buffer);
  }
};

class RaptData : public MeasurementBaseData {
 private:
  float _tempC = 0;
  float _gravity = 0;
  float _angle = 0;
  float _velocity = 0;
  float _battery = 0;
  int _rssi = 0;
  int _txPower = 0;

 public:
  // Note! For RAPT the last part of the MAC adress is used as ID since the
  // payload does not contain that.
  RaptData(MeasurementSource source, String id, float tempC, float gravity,
           float velocity, float angle, float battery, int txPower, int rssi)
      : MeasurementBaseData(id, MeasurementType::Rapt, source) {
    _tempC = tempC;
    _velocity = velocity;
    _gravity = gravity;
    _angle = angle;
    _battery = battery;
    _txPower = txPower;
    _rssi = rssi;
  }
  virtual ~RaptData() {}

  float getTempC() const { return _tempC; }
  float getGravity() const { return _gravity; }
  float getVelocity() const { return _velocity; }
  float getAngle() const { return _angle; }
  float getBattery() const { return _battery; }
  int getTxPower() const { return _txPower; }
  int getRssi() const { return _rssi; }

  void writeToFile(File& file) const {
    char buffer[300];

    // Data parameters
    // ----------------------------------------
    // 0, Format Version (1)
    // 1, Type
    // 2, Source
    // 3, Created (timestamp)
    // 4, ID
    // 5, Temperature (C)
    // 6, Gravity (SG)
    // 7, Angle
    // 8, Battery
    // 9, Tx Power
    // 10, Rssi

    snprintf(buffer, sizeof(buffer),
             "1,%s,%s,%s,%s,"
             "%.2f,%.4f,%.4f,%.2f,%d,%d,,,",
             getTypeAsString(), getSourceAsString(), getCreatedAsString(),
             getId(), getTempC(), getGravity(),
             getAngle(), getBattery(), getTxPower(), getRssi());
    file.println(buffer);
  }
};

// Base class for measurement data keeping track of last updated and pushed
class MeasurementEntry {
 private:
  std::unique_ptr<MeasurementBaseData> _measurement;
  bool _updated = false;
  struct tm _timeinfoUpdated;
  uint32_t _timeUpdated = 0;
  uint32_t _timePushed = 0;
  String _id = "";

 public:
  explicit MeasurementEntry(String id) { _id = id; }
  ~MeasurementEntry() {}

  const MeasurementBaseData* getData() const { return _measurement.get(); }
  const TiltData* getTiltData() {
    return static_cast<TiltData*>(_measurement.get());
  }
  const GravityData* getGravityData() const {
    return static_cast<GravityData*>(_measurement.get());
  }
  const RaptData* getRaptData() const {
    return static_cast<RaptData*>(_measurement.get());
  }
  const PressureData* getPressureData() const {
    return static_cast<PressureData*>(_measurement.get());
  }
  const ChamberData* getChamberData() const {
    return static_cast<ChamberData*>(_measurement.get());
  }

  void setMeasurement(std::unique_ptr<MeasurementBaseData> measurement) {
    _measurement = std::move(measurement);
    setUpdated();
  }

  MeasurementType getType() const {
    if (_measurement != nullptr) return _measurement->getType();
    return MeasurementType::NoType;
  }

  bool isUpdated() const { return _updated; }
  const String& getId() const { return _id; }

  void setUpdated() {
    _updated = true;
    _timeUpdated = millis();
    getLocalTime(&_timeinfoUpdated);
  }

  void setPushed() {
    _updated = false;
    _timePushed = millis();
  }

  uint32_t getUpdateAge() const { return (millis() - _timeUpdated) / 1000; }
  uint32_t getPushAge() const { return (millis() - _timePushed) / 1000; }
  const struct tm* getTimeinfoUpdated() const { return &_timeinfoUpdated; }
};

// List of data measurements
class MeasurementList {
 private:
  std::deque<std::unique_ptr<MeasurementEntry>> _list;
  const int MAX_ENTRIES = 20;

 public:
  MeasurementList() {}
  ~MeasurementList() { clear(); }

  void updateData(std::unique_ptr<MeasurementBaseData>& data) {
    if (data.get() == nullptr) {
      return;
    }

    if (size() > MAX_ENTRIES)  // If list if full, remove the oldest entry
      _list.pop_front();

    int i = findMeasurementById(data->getId());

#if defined(ENABLE_MMC) || defined(ENABLE_SD)
    if (mySdStorage.hasCard()) {
      File file = mySdStorage.open("/data.csv", FILE_APPEND, true);
      if (file) {
        data->writeToFile(file);
        file.close();
      } else {
        Log.error(F("SD  : Failed to open data.csv for writing." CR));
      }
    }
#endif

    if (i == -1) {
      std::unique_ptr<MeasurementEntry> entry;

      entry.reset(new MeasurementEntry(data->getId()));
      entry->setMeasurement(std::move(data));
      _list.push_back(std::move(entry));
    } else {
      MeasurementEntry* entry = getMeasurementEntry(i);

      if (entry != nullptr) entry->setMeasurement(std::move(data));
    }
  }

  MeasurementType getMeasurementType(int index) {
    if (index < 0 || index >= size()) return MeasurementType::NoType;
    return getMeasurementEntry(index)->getType();
  }

  int findMeasurementById(const String id) const {
    int i = 0;
    for (const std::unique_ptr<MeasurementEntry>& item : _list) {
      if (id == item->getId()) {
        return i;
      }
      i++;
    }

    return -1;
  }

  MeasurementEntry* getMeasurementEntry(int index) {
    return _list[index].get();
  }

  void clear() { _list.clear(); }
  int size() const { return _list.size(); }
};

extern MeasurementList myMeasurementList;

#endif  // GATEWAY

#endif  // SRC_MEASUREMENT_HPP_
