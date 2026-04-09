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
#if defined(GATEWAY)

#include <ArduinoJson.h>

#include <ble_gateway.hpp>
#include <config_gateway.hpp>
#include <main_gateway.hpp>
#include <mdns_discovery.hpp>
#include <measurement.hpp>
#include <memory>
#include <push_gateway.hpp>
#include <sdcard_mmc.hpp>
#include <sdcard_sd.hpp>
#include <uptime.hpp>
#include <web_gateway.hpp>

constexpr auto PARAM_GRAVITY_DEVICE = "gravity_device";
constexpr auto PARAM_PRESSURE_DEVICE = "pressure_device";
constexpr auto PARAM_TEMPERATURE_DEVICE = "temperature_device";
constexpr auto PARAM_RAPT_DEVICE = "rapt_device";
constexpr auto PARAM_DEVICE = "device";
constexpr auto PARAM_NAME = "name";
constexpr auto PARAM_TYPE = "type";
constexpr auto PARAM_SOURCE = "source";
constexpr auto PARAM_GRAVITY = "gravity";
constexpr auto PARAM_VELOCITY = "velocity";
constexpr auto PARAM_PRESSURE = "pressure";
constexpr auto PARAM_PRESSURE1 = "pressure1";
constexpr auto PARAM_TEMP = "temp";
constexpr auto PARAM_CHAMBER_TEMP = "chamber_temp";
constexpr auto PARAM_BEER_TEMP = "beer_temp";
constexpr auto PARAM_UPDATE_TIME = "update_time";
constexpr auto PARAM_PUSH_TIME = "push_time";
constexpr auto PARAM_UPTIME_SECONDS = "uptime_seconds";
constexpr auto PARAM_UPTIME_MINUTES = "uptime_minutes";
constexpr auto PARAM_UPTIME_HOURS = "uptime_hours";
constexpr auto PARAM_UPTIME_DAYS = "uptime_days";
constexpr auto PARAM_SD_MOUNTED = "sd_mounted";
constexpr auto PARAM_SD = "sd";
constexpr auto PARAM_TFT = "tft";

#if defined(ENABLE_MMC)
extern SdCardMMC mySdStorage;
#elif defined(ENABLE_SD)
extern SdCardSD mySdStorage;
#endif

extern MdnsScanner myMdnsScanner;

GatewayWebServer::GatewayWebServer(GravmonGatewayConfig *config)
    : BrewingWebServer(config), _gatewayConfig(config) {}

void GatewayWebServer::doWebFeature(JsonObject &obj) {
  obj[PARAM_FIRMWARE_FILE] = CFG_FILENAMEBIN;
#if defined(ENABLE_MMC) || defined(ENABLE_SD)
  obj[PARAM_SD] = true;
#else
  obj[PARAM_SD] = false;
#endif

#if defined(ENABLE_TFT)
  obj[PARAM_TFT] = true;
#else
  obj[PARAM_TFT] = false;
#endif
}

void GatewayWebServer::doWebStatus(JsonObject &obj) {
  JsonArray gravityDevices = obj[PARAM_GRAVITY_DEVICE].to<JsonArray>();
  JsonArray pressureDevices = obj[PARAM_PRESSURE_DEVICE].to<JsonArray>();
  JsonArray temperatureDevices = obj[PARAM_TEMPERATURE_DEVICE].to<JsonArray>();
  int gravIdx = 0, pressIdx = 0, tempIdx = 0;

  for (int i = 0; i < myMeasurementList.size(); i++) {
    MeasurementEntry *entry = myMeasurementList.getMeasurementEntry(i);

    switch (entry->getType()) {
      case MeasurementType::Gravitymon: {
        Log.notice("WEB: Processing Gravitymon data %d." CR, i);
        const GravityData *gd = entry->getGravityData();

        gravityDevices[gravIdx][PARAM_NAME] = gd->getName();
        gravityDevices[gravIdx][PARAM_DEVICE] = gd->getId();
        if (!isnan(gd->getGravity()))
          gravityDevices[gravIdx][PARAM_GRAVITY] = gd->getGravity();
        if (!isnan(gd->getTempC()))
          gravityDevices[gravIdx][PARAM_TEMP] = gd->getTempC();
        gravityDevices[gravIdx][PARAM_UPDATE_TIME] = entry->getUpdateAge();
        gravityDevices[gravIdx][PARAM_PUSH_TIME] = entry->getPushAge();
        gravityDevices[gravIdx][PARAM_SOURCE] = gd->getSourceAsString();
        gravityDevices[gravIdx][PARAM_TYPE] = gd->getTypeAsString();
        gravIdx++;
      } break;

      case MeasurementType::Pressuremon: {
        Log.notice("WEB: Processing Pressuremon data %d." CR, i);
        const PressureData *pd = entry->getPressureData();

        pressureDevices[pressIdx][PARAM_NAME] = pd->getName();
        pressureDevices[pressIdx][PARAM_DEVICE] = pd->getId();
        if (!isnan(pd->getPressure()))
          pressureDevices[pressIdx][PARAM_PRESSURE] = pd->getPressure();
        if (!isnan(pd->getPressure1()))
          pressureDevices[pressIdx][PARAM_PRESSURE1] = pd->getPressure1();
        if (!isnan(pd->getTempC()))
          pressureDevices[pressIdx][PARAM_TEMP] = pd->getTempC();
        pressureDevices[pressIdx][PARAM_UPDATE_TIME] = entry->getUpdateAge();
        pressureDevices[pressIdx][PARAM_PUSH_TIME] = entry->getPushAge();
        pressureDevices[pressIdx][PARAM_SOURCE] = pd->getSourceAsString();
        pressureDevices[pressIdx][PARAM_TYPE] = pd->getTypeAsString();
        pressIdx++;
      } break;

      case MeasurementType::Tilt:
      case MeasurementType::TiltPro: {
        Log.notice("WEB: Processing Tilt data %d." CR, i);
        const TiltData *td = entry->getTiltData();

        gravityDevices[gravIdx][PARAM_NAME] = td->getTiltColor();
        gravityDevices[gravIdx][PARAM_DEVICE] = td->getId();
        if (!isnan(td->getGravity()))
          gravityDevices[gravIdx][PARAM_GRAVITY] = td->getGravity();
        if (!isnan(td->getTempC()))
          gravityDevices[gravIdx][PARAM_TEMP] = td->getTempC();
        gravityDevices[gravIdx][PARAM_UPDATE_TIME] = entry->getUpdateAge();
        gravityDevices[gravIdx][PARAM_PUSH_TIME] = entry->getPushAge();
        gravityDevices[gravIdx][PARAM_SOURCE] = td->getSourceAsString();
        gravityDevices[gravIdx][PARAM_TYPE] = td->getTypeAsString();
        gravIdx++;
      } break;

      case MeasurementType::Chamber: {
        Log.notice("WEB: Processing Chamber data %d." CR, i);
        const ChamberData *cd = entry->getChamberData();

        temperatureDevices[tempIdx][PARAM_NAME] = cd->getName();
        temperatureDevices[tempIdx][PARAM_DEVICE] = cd->getId();
        if (!isnan(cd->getChamberTempC()))
          temperatureDevices[tempIdx][PARAM_CHAMBER_TEMP] =
              cd->getChamberTempC();
        if (!isnan(cd->getBeerTempC()))
          temperatureDevices[tempIdx][PARAM_BEER_TEMP] = cd->getBeerTempC();
        temperatureDevices[tempIdx][PARAM_UPDATE_TIME] = entry->getUpdateAge();
        temperatureDevices[tempIdx][PARAM_PUSH_TIME] = entry->getPushAge();
        temperatureDevices[tempIdx][PARAM_SOURCE] = cd->getSourceAsString();
        temperatureDevices[tempIdx][PARAM_TYPE] = cd->getTypeAsString();
        tempIdx++;
      } break;

      case MeasurementType::Rapt: {
        Log.notice("WEB: Processing Rapt data %d." CR, i);
        const RaptData *rd = entry->getRaptData();

        gravityDevices[gravIdx][PARAM_DEVICE] = rd->getId();
        if (!isnan(rd->getGravity()))
          gravityDevices[gravIdx][PARAM_GRAVITY] = rd->getGravity();
        if (!isnan(rd->getVelocity()))
          gravityDevices[gravIdx][PARAM_VELOCITY] = rd->getVelocity();
        if (!isnan(rd->getTempC()))
          gravityDevices[gravIdx][PARAM_TEMP] = rd->getTempC();
        gravityDevices[gravIdx][PARAM_UPDATE_TIME] = entry->getUpdateAge();
        gravityDevices[gravIdx][PARAM_PUSH_TIME] = entry->getPushAge();
        gravityDevices[gravIdx][PARAM_SOURCE] = rd->getSourceAsString();
        gravityDevices[gravIdx][PARAM_TYPE] = rd->getTypeAsString();
        gravIdx++;
      } break;
    }
  }

  // Add other params
  obj[CONFIG_GRAVITY_UNIT] = String(myConfig.getGravityUnit());
  obj[CONFIG_PRESSURE_UNIT] = myConfig.getPressureUnit();
  obj[PARAM_UPTIME_SECONDS] = myUptime.getSeconds();
  obj[PARAM_UPTIME_MINUTES] = myUptime.getMinutes();
  obj[PARAM_UPTIME_HOURS] = myUptime.getHours();
  obj[PARAM_UPTIME_DAYS] = myUptime.getDays();
#if defined(ENABLE_MMC) || defined(ENABLE_SD)
  obj[PARAM_SD_MOUNTED] = mySdStorage.hasCard();
#else
  obj[PARAM_SD_MOUNTED] = false;
#endif
}

bool GatewayWebServer::setupWebServer(const char *serviceName) {
  bool b = BrewingWebServer::setupWebServer(serviceName);

  AsyncCallbackJsonWebHandler *handler;
  handler = new AsyncCallbackJsonWebHandler(
      "/post", std::bind(&GatewayWebServer::webHandleRemotePost, this,
                         std::placeholders::_1, std::placeholders::_2));
  _server->addHandler(handler);
  handler = new AsyncCallbackJsonWebHandler(
      "/api/sd", std::bind(&GatewayWebServer::webHandleSecureDigital, this,
                           std::placeholders::_1, std::placeholders::_2));
  _server->addHandler(handler);
  _server->on(
      "/api/mdns", (WebRequestMethodComposite)HTTP_GET,
      [this](AsyncWebServerRequest *request) { webHandleMdns(request); });
#if defined(ENABLE_MMC) || defined(ENABLE_SD)
  _server->serveStatic("/sd", mySdStorage.getFS(), "/");
#endif
  return b;
}

constexpr auto GRAVMON_NAME = "name";
constexpr auto GRAVMON_ID = "ID";
constexpr auto GRAVMON_TOKEN = "token";
constexpr auto GRAVMON_INTERVAL = "interval";
constexpr auto GRAVMON_TEMPERATURE = "temperature";
constexpr auto GRAVMON_TEMPERATURE_UNIT = "temp_units";
constexpr auto GRAVMON_GRAVITY = "gravity";
constexpr auto GRAVMON_ANGLE = "angle";
constexpr auto GRAVMON_BATTERY = "battery";
constexpr auto GRAVMON_RSSI = "RSSI";

constexpr auto PRESSMON_NAME = "name";
constexpr auto PRESSMON_ID = "id";
constexpr auto PRESSMON_TOKEN = "token";
constexpr auto PRESSMON_INTERVAL = "interval";
constexpr auto PRESSMON_TEMPERATURE = "temperature";
constexpr auto PRESSMON_TEMPERATURE_UNIT = "temperature-unit";
constexpr auto PRESSMON_PRESSURE = "pressure";
constexpr auto PRESSMON_PRESSURE1 = "pressure1";
constexpr auto PRESSMON_PRESSURE_UNIT = "pressure-unit";
constexpr auto PRESSMON_BATTERY = "battery";
constexpr auto PRESSMON_RSSI = "rssi";

void GatewayWebServer::webHandleRemotePost(AsyncWebServerRequest *request,
                                           JsonVariant &json) {
  Log.notice(F("WEB : webServer callback for /post." CR));
  JsonObject obj = json.as<JsonObject>();

  /* Expected gravitymon format.
  {
    "name": "gravitymon-gwfa413c",
    "ID": "fa413c",
    "token": "",
    "interval": 900,
    "temperature": 20.1,
    "temp_units": "C",
    "gravity": 1.015,
    "angle": 35,
    "battery": 4,
    "RSSI": -79
  }*/

  /* Expected pressuremon format.
  {
    "name": "test",
    "id": "e17d44",
    "token": "",
    "interval": 300,
    "temperature": 22.3,
    "temperature-unit": "C",
    "pressure": 1.001,
    "pressure-unit": "PSI",
    "battery": 3.85,
    "rssi": -76.2
  }*/

  if (!obj[GRAVMON_GRAVITY].isNull() || !obj[PRESSMON_PRESSURE].isNull()) {
    String json;
    json.reserve(200);
    serializeJson(obj, json);
    _postData.push(json);
    request->send(200);
    return;
  }

  request->send(422);
}

void GatewayWebServer::loop() {
  BrewingWebServer::loop();

  // Process data that was received in posts
  while (!_postData.empty()) {
    String data = _postData.front();

    Serial.printf("Processing data: %s\n", data.c_str());

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, data);
    JsonObject obj = doc.as<JsonObject>();

    // Process gravitymon data
    if (!obj[GRAVMON_GRAVITY].isNull()) {
      String id = !obj[GRAVMON_ID].isNull() ? obj[GRAVMON_ID].as<String>() : "";
      String token =
          !obj[GRAVMON_TOKEN].isNull() ? obj[GRAVMON_TOKEN].as<String>() : "";
      String name =
          !obj[GRAVMON_NAME].isNull() ? obj[GRAVMON_NAME].as<String>() : "";
      int interval =
          !obj[GRAVMON_INTERVAL].isNull() ? obj[GRAVMON_INTERVAL].as<int>() : 0;
      float temp = !obj[GRAVMON_TEMPERATURE].isNull()
                       ? obj[GRAVMON_TEMPERATURE].as<float>()
                       : 0.0;
      String tempUnits = !obj[GRAVMON_TEMPERATURE_UNIT].isNull()
                             ? obj[GRAVMON_TEMPERATURE_UNIT].as<String>()
                             : "";
      float gravity = !obj[GRAVMON_GRAVITY].isNull()
                          ? obj[GRAVMON_GRAVITY].as<float>()
                          : 0.0;
      float angle =
          !obj[GRAVMON_ANGLE].isNull() ? obj[GRAVMON_ANGLE].as<float>() : 0.0;
      float battery = !obj[GRAVMON_BATTERY].isNull()
                          ? obj[GRAVMON_BATTERY].as<float>()
                          : 0.0;
      int rssi = !obj[GRAVMON_RSSI].isNull() ? obj[GRAVMON_RSSI].as<int>() : 0;

      std::unique_ptr<MeasurementBaseData> gravityData;
      gravityData.reset(new GravityData(MeasurementSource::HttpPost, id, name,
                                        token, temp, gravity, angle, battery, 0,
                                        rssi, interval));

      Log.info(F("WEB : Update data for gravitymon %s." CR),
               gravityData->getId());
      myMeasurementList.updateData(gravityData);
    }

    // Process pressuremon data
    if (!obj[PRESSMON_PRESSURE].isNull()) {
      String id =
          !obj[PRESSMON_ID].isNull() ? obj[PRESSMON_ID].as<String>() : "";
      String token =
          !obj[PRESSMON_TOKEN].isNull() ? obj[PRESSMON_TOKEN].as<String>() : "";
      String name =
          !obj[PRESSMON_NAME].isNull() ? obj[PRESSMON_NAME].as<String>() : "";
      int interval = !obj[PRESSMON_INTERVAL].isNull()
                         ? obj[PRESSMON_INTERVAL].as<int>()
                         : 0;
      float temp = !obj[PRESSMON_TEMPERATURE].isNull()
                       ? obj[PRESSMON_TEMPERATURE].as<float>()
                       : 0.0;
      String tempUnits = !obj[PRESSMON_PRESSURE_UNIT].isNull()
                             ? obj[PRESSMON_PRESSURE_UNIT].as<String>()
                             : "";
      float pressure = !obj[PRESSMON_PRESSURE].isNull()
                           ? obj[PRESSMON_PRESSURE].as<float>()
                           : 0.0;
      float pressure1 = !obj[PRESSMON_PRESSURE1].isNull()
                            ? obj[PRESSMON_PRESSURE1].as<float>()
                            : 0.0;
      float battery = !obj[PRESSMON_BATTERY].isNull()
                          ? obj[PRESSMON_BATTERY].as<float>()
                          : 0.0;
      int rssi =
          !obj[PRESSMON_RSSI].isNull() ? obj[PRESSMON_RSSI].as<int>() : 0;

      std::unique_ptr<MeasurementBaseData> pressureData;
      pressureData.reset(new PressureData(MeasurementSource::HttpPost, id, name,
                                          token, temp, pressure, pressure1,
                                          battery, 0, rssi, interval));

      Log.info(F("WEB : Update data for pressuremon %s." CR),
               pressureData->getId());
      myMeasurementList.updateData(pressureData);
    }

    _postData.pop();
  }
}

void GatewayWebServer::doTaskPushTestSetup(TemplatingEngine &engine,
                                           BrewingPush &push) {
  Log.notice(F("WEB : Running scheduled push test for %s" CR),
             _pushTestTarget.c_str());

  if (!_pushTestTarget.compareTo(PARAM_FORMAT_POST_GRAVITY) &&
      _gatewayConfig->hasTargetHttpPost()) {
    setupTemplateEngineGravityGateway(_gatewayConfig, engine, 32.12, 1.015,
                                      22.15, 3.78, 300, "012345", "token",
                                      "grav-test");
    String tpl = push.getTemplate(BrewingPush::GRAVITY_TEMPLATE_HTTP1);
    String doc = engine.create(tpl.c_str());
    push.sendHttpPost(doc);
    _pushTestEnabled = true;
  } else if (!_pushTestTarget.compareTo(PARAM_FORMAT_POST2_GRAVITY) &&
             _gatewayConfig->hasTargetHttpPost2()) {
    setupTemplateEngineGravityGateway(_gatewayConfig, engine, 32.12, 1.015,
                                      22.15, 3.78, 300, "012345", "token",
                                      "grav-test");
    String tpl = push.getTemplate(BrewingPush::GRAVITY_TEMPLATE_HTTP2);
    String doc = engine.create(tpl.c_str());
    push.sendHttpPost2(doc);
    _pushTestEnabled = true;
  } else if (!_pushTestTarget.compareTo(PARAM_FORMAT_GET_GRAVITY) &&
             _gatewayConfig->hasTargetHttpGet()) {
    setupTemplateEngineGravityGateway(_gatewayConfig, engine, 32.12, 1.015,
                                      22.15, 3.78, 300, "012345", "token",
                                      "grav-test");
    String tpl = push.getTemplate(BrewingPush::GRAVITY_TEMPLATE_HTTP3);
    String doc = engine.create(tpl.c_str());
    push.sendHttpGet(doc);
    _pushTestEnabled = true;
  } else if (!_pushTestTarget.compareTo(PARAM_FORMAT_INFLUXDB_GRAVITY) &&
             _gatewayConfig->hasTargetInfluxDb2()) {
    setupTemplateEngineGravityGateway(_gatewayConfig, engine, 32.12, 1.015,
                                      22.15, 3.78, 300, "012345", "token",
                                      "grav-test");
    String tpl = push.getTemplate(BrewingPush::GRAVITY_TEMPLATE_INFLUX);
    String doc = engine.create(tpl.c_str());
    push.sendInfluxDb2(doc);
    _pushTestEnabled = true;
  } else if (!_pushTestTarget.compareTo(PARAM_FORMAT_MQTT_GRAVITY) &&
             _gatewayConfig->hasTargetMqtt()) {
    setupTemplateEngineGravityGateway(_gatewayConfig, engine, 32.12, 1.015,
                                      22.15, 3.78, 300, "012345", "token",
                                      "grav-test");
    String tpl = push.getTemplate(BrewingPush::GRAVITY_TEMPLATE_MQTT);
    String doc = engine.create(tpl.c_str());
    push.sendMqtt(doc);

    _pushTestEnabled = true;
  } else if (!_pushTestTarget.compareTo(PARAM_FORMAT_POST_PRESSURE) &&
             _gatewayConfig->hasTargetHttpPost()) {
    setupTemplateEnginePressureGateway(_gatewayConfig, engine, 32.12, 1.015,
                                       22.15, 3.78, 300, "012345", "token",
                                       "press-test");
    String tpl = push.getTemplate(BrewingPush::PRESSURE_TEMPLATE_HTTP1);
    String doc = engine.create(tpl.c_str());
    push.sendHttpPost(doc);
    _pushTestEnabled = true;
  } else if (!_pushTestTarget.compareTo(PARAM_FORMAT_POST2_PRESSURE) &&
             _gatewayConfig->hasTargetHttpPost2()) {
    setupTemplateEnginePressureGateway(_gatewayConfig, engine, 1.234, 2.345,
                                       22.15, 3.78, 300, "012345", "token",
                                       "press-test");
    String tpl = push.getTemplate(BrewingPush::PRESSURE_TEMPLATE_HTTP2);
    String doc = engine.create(tpl.c_str());
    push.sendHttpPost2(doc);
    _pushTestEnabled = true;
  } else if (!_pushTestTarget.compareTo(PARAM_FORMAT_GET_PRESSURE) &&
             _gatewayConfig->hasTargetHttpGet()) {
    setupTemplateEnginePressureGateway(_gatewayConfig, engine, 1.234, 2.345,
                                       22.15, 3.78, 300, "012345", "token",
                                       "press-test");
    String tpl = push.getTemplate(BrewingPush::PRESSURE_TEMPLATE_HTTP3);
    String doc = engine.create(tpl.c_str());
    push.sendHttpGet(doc);
    _pushTestEnabled = true;
  } else if (!_pushTestTarget.compareTo(PARAM_FORMAT_INFLUXDB_PRESSURE) &&
             _gatewayConfig->hasTargetInfluxDb2()) {
    setupTemplateEnginePressureGateway(_gatewayConfig, engine, 1.234, 2.345,
                                       22.15, 3.78, 300, "012345", "token",
                                       "press-test");
    String tpl = push.getTemplate(BrewingPush::PRESSURE_TEMPLATE_INFLUX);
    String doc = engine.create(tpl.c_str());
    push.sendInfluxDb2(doc);
    _pushTestEnabled = true;
  } else if (!_pushTestTarget.compareTo(PARAM_FORMAT_MQTT_PRESSURE) &&
             _gatewayConfig->hasTargetMqtt()) {
    setupTemplateEnginePressureGateway(_gatewayConfig, engine, 1.234, 2.345,
                                       22.15, 3.78, 300, "012345", "token",
                                       "press-test");
    String tpl = push.getTemplate(BrewingPush::PRESSURE_TEMPLATE_MQTT);
    String doc = engine.create(tpl.c_str());
    push.sendMqtt(doc);
    _pushTestEnabled = true;
  }

  engine.freeMemory();
  push.clearTemplate();
}

void GatewayWebServer::webHandleSecureDigital(AsyncWebServerRequest *request,
                                              JsonVariant &json) {
  if (!isAuthenticated(request)) {
    return;
  }

  Log.notice(F("WEB : webServer callback for /api/sd." CR));
#if defined(ENABLE_MMC) || defined(ENABLE_SD)
  JsonObject obj = json.as<JsonObject>();

  if (!obj[PARAM_COMMAND].isNull()) {
    if (obj[PARAM_COMMAND] == String("dir")) {
      Log.notice(F("WEB : File system listing requested." CR));
      AsyncJsonResponse *response = new AsyncJsonResponse(false);
      JsonObject obj = response->getRoot().as<JsonObject>();

      obj[PARAM_TOTAL] = mySdStorage.totalBytes();
      obj[PARAM_USED] = mySdStorage.usedBytes();
      obj[PARAM_FREE] = mySdStorage.totalBytes() - mySdStorage.usedBytes();

      File root = mySdStorage.open("/", FILE_READ);
      File f = root.openNextFile();
      int i = 0;

      JsonArray arr = obj[PARAM_FILES].to<JsonArray>();
      while (f) {
        Log.notice(F("WEB : %s." CR), f.name());
        if (!String(f.name()).startsWith(
                ".")) {  // Ignore files with . (hidden files)
          arr[i][PARAM_FILE] = "/" + String(f.name());
          arr[i][PARAM_SIZE] = static_cast<int>(f.size());
          i++;
        }
        f = root.openNextFile();
      }
      f.close();
      root.close();

      response->setLength();
      request->send(response);
    } else if (obj[PARAM_COMMAND] == String("del")) {
      Log.notice(F("WEB : File system delete requested." CR));

      if (!obj[PARAM_FILE].isNull()) {
        String f = obj[PARAM_FILE];
        mySdStorage.remove(f);
        request->send(200);
      } else {
        request->send(400);
      }
    } else {
      Log.warning(F("WEB : Unknown file system command." CR));
      request->send(400);
    }
  } else {
    Log.warning(F("WEB : Unknown file system command." CR));
    request->send(400);
  }
#endif
}

void GatewayWebServer::webHandleMdns(AsyncWebServerRequest *request) {
  if (!isAuthenticated(request)) {
    return;
  }

  Log.notice(F("WEB : webServer callback for /api/hardware." CR));
  AsyncJsonResponse *response = new AsyncJsonResponse(false);
  JsonObject obj = response->getRoot().as<JsonObject>();
  myMdnsScanner.populateJson(obj);
  response->setLength();
  request->send(response);
}

// void GatewayWebServer::doTaskHardwareScanning(JsonObject &obj) {
// }

#endif  // GATEWAY

// EOF
