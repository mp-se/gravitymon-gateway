/*
MIT License

Copyright (c) 2021-2024 Magnus

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
#include <Wire.h>
#include <esp_int_wdt.h>
#include <esp_task_wdt.h>

#include <blescanner.hpp>
#include <config.hpp>
#include <helper.hpp>
#include <main.hpp>
#include <pushtarget.hpp>
#include <resources.hpp>
#include <templating.hpp>
#include <uptime.hpp>
#include <webserver.hpp>

GravmonGatewayWebServer::GravmonGatewayWebServer(WebConfig *config)
    : BaseWebServer(config) {}

void GravmonGatewayWebServer::webHandleConfigRead(
    AsyncWebServerRequest *request) {
  if (!isAuthenticated(request)) {
    return;
  }

  Log.notice(F("WEB : webServer callback for /api/config(read)." CR));
  AsyncJsonResponse *response =
      new AsyncJsonResponse(false, JSON_BUFFER_SIZE_L);
  JsonObject obj = response->getRoot().as<JsonObject>();
  myConfig.createJson(obj);
  response->setLength();
  request->send(response);
}

void GravmonGatewayWebServer::webHandleConfigWrite(
    AsyncWebServerRequest *request, JsonVariant &json) {
  if (!isAuthenticated(request)) {
    return;
  }

  Log.notice(F("WEB : webServer callback for /api/config(write)." CR));
  JsonObject obj = json.as<JsonObject>();
  myConfig.parseJson(obj);
  obj.clear();
  myConfig.saveFile();

  AsyncJsonResponse *response =
      new AsyncJsonResponse(false, JSON_BUFFER_SIZE_S);
  obj = response->getRoot().as<JsonObject>();
  obj[PARAM_SUCCESS] = true;
  obj[PARAM_MESSAGE] = "Configuration updated";
  response->setLength();
  request->send(response);
}

void GravmonGatewayWebServer::webHandleFactoryDefaults(
    AsyncWebServerRequest *request) {
  if (!isAuthenticated(request)) {
    return;
  }

  Log.notice(F("WEB : webServer callback for /api/factory." CR));
  myConfig.saveFileWifiOnly();
  LittleFS.remove(ERR_FILENAME);
  LittleFS.remove(TPL_FNAME_POST);
  LittleFS.remove(TPL_FNAME_POST2);
  LittleFS.remove(TPL_FNAME_INFLUXDB);
  LittleFS.remove(TPL_FNAME_MQTT);
  LittleFS.end();
  Log.notice(F("WEB : Deleted files in filesystem, rebooting." CR));

  AsyncJsonResponse *response =
      new AsyncJsonResponse(false, JSON_BUFFER_SIZE_S);
  JsonObject obj = response->getRoot().as<JsonObject>();
  obj[PARAM_SUCCESS] = true;
  obj[PARAM_MESSAGE] = "Factory reset completed, rebooting";
  response->setLength();
  request->send(response);
  _rebootTimer = millis();
  _rebootTask = true;  
}

void GravmonGatewayWebServer::webHandleStatus(AsyncWebServerRequest *request) {
  Log.notice(F("WEB : webServer callback for /api/status(get)." CR));

  // Fallback since sometimes the loop() does not always run after firmware
  // update...
  if (_rebootTask) {
    Log.notice(F("WEB : Rebooting using fallback..." CR));
    delay(500);
    ESP_RESET();
  }

  AsyncJsonResponse *response =
      new AsyncJsonResponse(false, JSON_BUFFER_SIZE_L);
  JsonObject obj = response->getRoot().as<JsonObject>();

  obj[PARAM_ID] = myConfig.getID();
  obj[PARAM_TEMP_FORMAT] = String(myConfig.getTempFormat());
  obj[PARAM_GRAVITY_FORMAT] = String(myConfig.getGravityFormat());
  obj[PARAM_APP_VER] = CFG_APPVER;
  obj[PARAM_APP_BUILD] = CFG_GITREV;
  obj[PARAM_MDNS] = myConfig.getMDNS();
#if defined(ESP32S3)
  obj[PARAM_PLATFORM] = "esp32s3";
#elif defined(ESP32C3)
  obj[PARAM_PLATFORM] = "esp32c3";
#elif defined(ESP32)
  obj[PARAM_PLATFORM] = "esp32";
#endif
  obj[PARAM_RSSI] = WiFi.RSSI();
  obj[PARAM_SSID] = WiFi.SSID();
  obj[PARAM_TOTAL_HEAP] = ESP.getHeapSize();
  obj[PARAM_FREE_HEAP] = ESP.getFreeHeap();
  obj[PARAM_IP] = WiFi.localIP().toString();
  obj[PARAM_WIFI_SETUP] = (runMode == RunMode::wifiSetupMode) ? true : false;

  obj[PARAM_UPTIME_SECONDS] = myUptime.getSeconds();
  obj[PARAM_UPTIME_MINUTES] = myUptime.getMinutes();
  obj[PARAM_UPTIME_HOURS] = myUptime.getHours();
  obj[PARAM_UPTIME_DAYS] = myUptime.getDays();

  JsonArray devices = obj.createNestedArray(PARAM_GRAVITY_DEVICE);

  // Get data from BLE
  for (int i = 0; i < NO_GRAVITYMON; i++) {
    GravitymonData gd = bleScanner.getGravitymonData(i);
    if (gd.id != "") {
      JsonObject n = devices.createNestedObject();
      n[PARAM_DEVICE] = gd.id;
      n[PARAM_GRAVITY] = gd.gravity;
      n[PARAM_TEMP] = gd.tempC;
      n[PARAM_UPDATE_TIME] = gd.getUpdateAge();
      n[PARAM_PUSH_TIME] = gd.getPushAge();
      n[PARAM_ENDPOINT] = "ble";
    }
  }

  // Get data from WIFI
  for (int i = 0; i < NO_GRAVITYMON; i++) {
    GravitymonData gd = getGravitymonData(i);
    if (gd.id != "") {
      JsonObject n = devices.createNestedObject();
      n[PARAM_DEVICE] = gd.id;
      n[PARAM_GRAVITY] = gd.gravity;
      n[PARAM_TEMP] = gd.tempC;
      n[PARAM_UPDATE_TIME] = gd.getUpdateAge();
      n[PARAM_PUSH_TIME] = gd.getPushAge();
      n[PARAM_ENDPOINT] = "wifi";
    }
  }

  response->setLength();
  request->send(response);
}

void GravmonGatewayWebServer::webHandleConfigFormatWrite(
    AsyncWebServerRequest *request, JsonVariant &json) {
  if (!isAuthenticated(request)) {
    return;
  }

  Log.notice(F("WEB : webServer callback for /api/config/format(post)." CR));

  JsonObject obj = json.as<JsonObject>();
  int success = 0;

  if (!obj[PARAM_FORMAT_POST].isNull()) {
    success += writeFile(TPL_FNAME_POST, obj[PARAM_FORMAT_POST]) ? 1 : 0;
  }
  if (!obj[PARAM_FORMAT_POST2].isNull()) {
    success += writeFile(TPL_FNAME_POST2, obj[PARAM_FORMAT_POST2]) ? 1 : 0;
  }
  if (!obj[PARAM_FORMAT_GET].isNull()) {
    success += writeFile(TPL_FNAME_GET, obj[PARAM_FORMAT_GET]) ? 1 : 0;
  }
  if (!obj[PARAM_FORMAT_INFLUXDB].isNull()) {
    success +=
        writeFile(TPL_FNAME_INFLUXDB, obj[PARAM_FORMAT_INFLUXDB]) ? 1 : 0;
  }
  if (!obj[PARAM_FORMAT_MQTT].isNull()) {
    success += writeFile(TPL_FNAME_MQTT, obj[PARAM_FORMAT_MQTT]) ? 1 : 0;
  }

  AsyncJsonResponse *response =
      new AsyncJsonResponse(false, JSON_BUFFER_SIZE_S);
  obj = response->getRoot().as<JsonObject>();
  obj[PARAM_SUCCESS] = success > 0 ? true : false;
  obj[PARAM_MESSAGE] = success > 0 ? "Format template stored"
                                   : "Failed to store format template";
  response->setLength();
  request->send(response);
}

void GravmonGatewayWebServer::webHandleTestPush(AsyncWebServerRequest *request,
                                                JsonVariant &json) {
  if (!isAuthenticated(request)) {
    return;
  }

  Log.notice(F("WEB : webServer callback for /api/test/push." CR));
  JsonObject obj = json.as<JsonObject>();
  _pushTestTarget = obj[PARAM_PUSH_FORMAT].as<String>();
  _pushTestTask = true;
  _pushTestEnabled = false;
  _pushTestLastSuccess = false;
  _pushTestLastCode = 0;
  AsyncJsonResponse *response =
      new AsyncJsonResponse(false, JSON_BUFFER_SIZE_S);
  obj = response->getRoot().as<JsonObject>();
  obj[PARAM_SUCCESS] = true;
  obj[PARAM_MESSAGE] = "Scheduled test for " + _pushTestTarget;
  response->setLength();
  request->send(response);
}

void GravmonGatewayWebServer::webHandleRemotePost(
    AsyncWebServerRequest *request, JsonVariant &json) {
  Log.notice(F("WEB : webServer callback for /post." CR));
  JsonObject obj = json.as<JsonObject>();

  /* Expected format
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

  String id =
      obj.containsKey(PARAM_BLE_ID) ? obj[PARAM_BLE_ID].as<String>() : "";
  String token =
      obj.containsKey(PARAM_BLE_TOKEN) ? obj[PARAM_BLE_TOKEN].as<String>() : "";
  String name =
      obj.containsKey(PARAM_BLE_NAME) ? obj[PARAM_BLE_NAME].as<String>() : "";
  int interval = obj.containsKey(PARAM_BLE_INTERVAL)
                     ? obj[PARAM_BLE_INTERVAL].as<int>()
                     : 0;
  float temp = obj.containsKey(PARAM_BLE_TEMPERATURE)
                   ? obj[PARAM_BLE_TEMPERATURE].as<float>()
                   : 0.0;
  String tempUnits = obj.containsKey(PARAM_BLE_TEMP_UNITS)
                         ? obj[PARAM_BLE_TEMP_UNITS].as<String>()
                         : "";
  float gravity = obj.containsKey(PARAM_BLE_GRAVITY)
                      ? obj[PARAM_BLE_GRAVITY].as<float>()
                      : 0.0;
  float angle =
      obj.containsKey(PARAM_BLE_ANGLE) ? obj[PARAM_BLE_ANGLE].as<float>() : 0.0;
  float battery = obj.containsKey(PARAM_BLE_BATTERY)
                      ? obj[PARAM_BLE_BATTERY].as<float>()
                      : 0.0;
  int rssi =
      obj.containsKey(PARAM_BLE_RSSI) ? obj[PARAM_BLE_RSSI].as<int>() : 0;

  int idx = findGravitymonId(id);
  if (idx >= 0) {
    GravitymonData &data = getGravitymonData(idx);
    Log.info(F("Web : Received post from %s." CR), id.c_str());

    if (tempUnits == "C")
      data.tempC = temp;
    else
      data.tempC = convertFtoC(temp);
    data.gravity = gravity;
    data.angle = angle;
    data.battery = battery;
    data.id = id;
    data.name = name;
    data.interval = interval;
    data.token = token;
    data.rssi = rssi;
    // data.address = "";
    data.type = "Http";
    data.setUpdated();
    request->send(200);
  } else {
    Log.error(F("Web : Max devices reached - no more devices available." CR));
  }

  request->send(422);
}

void GravmonGatewayWebServer::webHandleTestPushStatus(
    AsyncWebServerRequest *request) {
  Log.notice(F("WEB : webServer callback for /api/test/push/status." CR));
  AsyncJsonResponse *response =
      new AsyncJsonResponse(false, JSON_BUFFER_SIZE_S);
  JsonObject obj = response->getRoot().as<JsonObject>();
  String s;

  if (_pushTestTask)
    s = "Running push tests for " + _pushTestTarget;
  else if (!_pushTestTask && _pushTestLastSuccess == 0)
    s = "No push test has been started";
  else
    s = "Push test for " + _pushTestTarget + " is complete";

  obj[PARAM_STATUS] = static_cast<bool>(_pushTestTask);
  obj[PARAM_SUCCESS] = _pushTestLastSuccess;
  obj[PARAM_MESSAGE] = s;
  obj[PARAM_PUSH_ENABLED] = _pushTestEnabled;
  obj[PARAM_PUSH_RETURN_CODE] = _pushTestLastCode;
  response->setLength();
  request->send(response);
}

bool GravmonGatewayWebServer::writeFile(String fname, String data) {
  if (data.length()) {
    data = urldecode(data);
    File file = LittleFS.open(fname, "w");
    if (file) {
      Log.notice(F("WEB : Storing template data in %s." CR), fname.c_str());
      file.write((unsigned char *)data.c_str(), data.length());
      file.close();
      return true;
    }
  } else {
    Log.notice(
        F("WEB : No template data to store in %s, reverting to default." CR),
        fname.c_str());
    LittleFS.remove(fname);
    return true;
  }

  return false;
}

String GravmonGatewayWebServer::readFile(String fname) {
  File file = LittleFS.open(fname, "r");
  if (file) {
    char buf[file.size() + 1];
    memset(&buf[0], 0, file.size() + 1);
    file.readBytes(&buf[0], file.size());
    file.close();
    Log.notice(F("WEB : Read template data from %s." CR), fname.c_str());
    return String(&buf[0]);
  }
  return "";
}

void GravmonGatewayWebServer::webHandleConfigFormatRead(
    AsyncWebServerRequest *request) {
  if (!isAuthenticated(request)) {
    return;
  }

  Log.notice(F("WEB : webServer callback for /api/config/format(read)." CR));

  AsyncJsonResponse *response =
      new AsyncJsonResponse(false, JSON_BUFFER_SIZE_XL);
  JsonObject obj = response->getRoot().as<JsonObject>();
  String s;

  s = readFile(TPL_FNAME_POST);
  obj[PARAM_FORMAT_POST] =
      s.length() ? urlencode(s) : urlencode(String(&iSpindleFormat[0]));
  s = readFile(TPL_FNAME_POST2);
  obj[PARAM_FORMAT_POST2] =
      s.length() ? urlencode(s) : urlencode(String(&iSpindleFormat[0]));
  s = readFile(TPL_FNAME_GET);
  obj[PARAM_FORMAT_GET] =
      s.length() ? urlencode(s) : urlencode(String(&iHttpGetFormat[0]));
  s = readFile(TPL_FNAME_INFLUXDB);
  obj[PARAM_FORMAT_INFLUXDB] =
      s.length() ? urlencode(s) : urlencode(String(&influxDbFormat[0]));
  s = readFile(TPL_FNAME_MQTT);
  obj[PARAM_FORMAT_MQTT] =
      s.length() ? urlencode(s) : urlencode(String(&mqttFormat[0]));

  response->setLength();
  request->send(response);
}

bool GravmonGatewayWebServer::setupWebServer() {
  Log.notice(F("WEB : Configuring web server." CR));

  BaseWebServer::setupWebServer();
  MDNS.addService("gravitymon", "tcp", 80);

  // Static content
  Log.notice(F("WEB : Setting up handlers for gravmon gateway web server." CR));

  AsyncCallbackJsonWebHandler *handler;
  _server->on("/api/format", HTTP_GET,
              std::bind(&GravmonGatewayWebServer::webHandleConfigFormatRead,
                        this, std::placeholders::_1));
  handler = new AsyncCallbackJsonWebHandler(
      "/api/format",
      std::bind(&GravmonGatewayWebServer::webHandleConfigFormatWrite, this,
                std::placeholders::_1, std::placeholders::_2),
      JSON_BUFFER_SIZE_L);
  _server->addHandler(handler);
  handler = new AsyncCallbackJsonWebHandler(
      "/post",
      std::bind(&GravmonGatewayWebServer::webHandleRemotePost, this,
                std::placeholders::_1, std::placeholders::_2),
      JSON_BUFFER_SIZE_L);
  _server->addHandler(handler);
  handler = new AsyncCallbackJsonWebHandler(
      "/api/config",
      std::bind(&GravmonGatewayWebServer::webHandleConfigWrite, this,
                std::placeholders::_1, std::placeholders::_2),
      JSON_BUFFER_SIZE_L);
  _server->addHandler(handler);
  _server->on("/api/config", HTTP_GET,
              std::bind(&GravmonGatewayWebServer::webHandleConfigRead, this,
                        std::placeholders::_1));
  _server->on("/api/factory", HTTP_GET,
              std::bind(&GravmonGatewayWebServer::webHandleFactoryDefaults,
                        this, std::placeholders::_1));
  _server->on("/api/status", HTTP_GET,
              std::bind(&GravmonGatewayWebServer::webHandleStatus, this,
                        std::placeholders::_1));
  _server->on("/api/push/status", HTTP_GET,
              std::bind(&GravmonGatewayWebServer::webHandleTestPushStatus, this,
                        std::placeholders::_1));
  handler = new AsyncCallbackJsonWebHandler(
      "/api/push",
      std::bind(&GravmonGatewayWebServer::webHandleTestPush, this,
                std::placeholders::_1, std::placeholders::_2),
      JSON_BUFFER_SIZE_S);
  _server->addHandler(handler);

  Log.notice(F("WEB : Web server started." CR));
  return true;
}

void GravmonGatewayWebServer::loop() {
  BaseWebServer::loop();

  if (_pushTestTask) {
    Log.notice(F("WEB : Running scheduled push test for %s" CR),
               _pushTestTarget.c_str());

    TemplatingEngine engine;
    GravmonGatewayPush push(&myConfig);
    push.setupTemplateEngine(engine, 45, 1.030, 22.1, 4.12, 900,
                             myConfig.getID(), myConfig.getToken(),
                             myConfig.getMDNS());

    if (!_pushTestTarget.compareTo(PARAM_FORMAT_POST) &&
        myConfig.hasTargetHttpPost()) {
      String tpl = push.getTemplate(GravmonGatewayPush::TEMPLATE_HTTP1);
      String doc = engine.create(tpl.c_str());
      push.sendHttpPost(doc);
      _pushTestEnabled = true;
    } else if (!_pushTestTarget.compareTo(PARAM_FORMAT_POST2) &&
               myConfig.hasTargetHttpPost2()) {
      String tpl = push.getTemplate(GravmonGatewayPush::TEMPLATE_HTTP2);
      String doc = engine.create(tpl.c_str());
      push.sendHttpPost2(doc);
      _pushTestEnabled = true;
    } else if (!_pushTestTarget.compareTo(PARAM_FORMAT_GET) &&
               myConfig.hasTargetHttpGet()) {
      String tpl = push.getTemplate(GravmonGatewayPush::TEMPLATE_HTTP3);
      String doc = engine.create(tpl.c_str());
      push.sendHttpGet(doc);
      _pushTestEnabled = true;
    } else if (!_pushTestTarget.compareTo(PARAM_FORMAT_INFLUXDB) &&
               myConfig.hasTargetInfluxDb2()) {
      String tpl = push.getTemplate(GravmonGatewayPush::TEMPLATE_INFLUX);
      String doc = engine.create(tpl.c_str());
      push.sendInfluxDb2(doc);
      _pushTestEnabled = true;
    } else if (!_pushTestTarget.compareTo(PARAM_FORMAT_MQTT) &&
               myConfig.hasTargetMqtt()) {
      String tpl = push.getTemplate(GravmonGatewayPush::TEMPLATE_MQTT);
      String doc = engine.create(tpl.c_str());
      push.sendMqtt(doc);
      _pushTestEnabled = true;
    }

    engine.freeMemory();
    push.clearTemplate();
    _pushTestLastSuccess = push.getLastSuccess();
    _pushTestLastCode = push.getLastCode();
    if (_pushTestEnabled)
      Log.notice(
          F("WEB : Scheduled push test %s completed, success=%d, code=%d" CR),
          _pushTestTarget.c_str(), _pushTestLastSuccess, _pushTestLastCode);
    else
      Log.notice(F("WEB : Scheduled push test %s failed, not enabled" CR),
                 _pushTestTarget.c_str());
    _pushTestTask = false;
  }
}

// EOF
