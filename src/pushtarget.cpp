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
#include <MQTT.h>

#include <config.hpp>
#include <helper.hpp>
#include <main.hpp>
#include <pushtarget.hpp>
#include <templating.hpp>

// Use iSpindle format for compatibility, HTTP POST
const char iSpindleFormat[] PROGMEM =
    "{"
    "\"name\": \"${mdns}\", "
    "\"ID\": \"${id}\", "
    "\"token\": \"${token}\", "
    "\"interval\": ${sleep-interval}, "
    "\"temperature\": ${temp}, "
    "\"temp_units\": \"${temp-unit}\", "
    "\"gravity\": ${gravity}, "
    "\"angle\": ${angle}, "
    "\"battery\": ${battery}, "
    "\"RSSI\": ${rssi}, "
    "}";

GravmonGatewayPush::GravmonGatewayPush(
    GravmonGatewayConfig* gravmonGatewayConfig)
    : BasePush(gravmonGatewayConfig) {
  _gravmonGatewayConfig = gravmonGatewayConfig;
}

void GravmonGatewayPush::sendAll(float angle, float gravitySG, float tempC,
                                 float battery, int interval, const char* id,
                                 const char* token, const char* mdns) {
  printHeap("PUSH");
  _http.setReuse(true);
  _httpSecure.setReuse(true);

  TemplatingEngine engine;
  setupTemplateEngine(engine, angle, gravitySG, tempC, battery, interval, id,
                      token, mdns);

  if (myConfig.hasTargetHttpPost()) {
    String tpl = getTemplate(GravmonGatewayPush::TEMPLATE_HTTP1);
    String doc = engine.create(tpl.c_str());
    Log.notice(F("PUSH: Data to send=%s." CR), doc.c_str());
    sendHttpPost(doc);
  }

  engine.freeMemory();
}

const char* GravmonGatewayPush::getTemplate(Templates t,
                                            bool useDefaultTemplate) {
  String fname;
  _baseTemplate.reserve(600);

  // Load templates from memory
  switch (t) {
    case TEMPLATE_HTTP1:
      _baseTemplate = String(iSpindleFormat);
      fname = TPL_FNAME_POST;
      break;
  }

  if (!useDefaultTemplate) {
    File file = LittleFS.open(fname, "r");
    if (file) {
      char buf[file.size() + 1];
      memset(&buf[0], 0, file.size() + 1);
      file.readBytes(&buf[0], file.size());
      _baseTemplate = String(&buf[0]);
      file.close();
      Log.notice(F("PUSH: Template loaded from disk %s." CR), fname.c_str());
    }
  }

  return _baseTemplate.c_str();
}

void GravmonGatewayPush::setupTemplateEngine(TemplatingEngine& engine,
                                             float angle, float gravitySG,
                                             float tempC, float voltage,
                                             int interval, const char* id,
                                             const char* token,
                                             const char* name) {
  // Names
  engine.setVal(TPL_MDNS, strlen(name) ? name : myConfig.getMDNS());
  engine.setVal(TPL_ID, id);
  engine.setVal(TPL_TOKEN, strlen(token) ? token : myConfig.getToken());

  // Temperature
  if (myConfig.isTempFormatC()) {
    engine.setVal(TPL_TEMP, tempC, DECIMALS_TEMP);
  } else {
    engine.setVal(TPL_TEMP, convertCtoF(tempC), DECIMALS_TEMP);
  }

  engine.setVal(TPL_TEMP_C, tempC, DECIMALS_TEMP);
  engine.setVal(TPL_TEMP_F, convertCtoF(tempC), DECIMALS_TEMP);
  engine.setVal(TPL_TEMP_UNITS, myConfig.getTempFormat());

  // Battery & Timer
  engine.setVal(TPL_BATTERY, voltage, DECIMALS_BATTERY);
  engine.setVal(TPL_SLEEP_INTERVAL, interval);

  // Performance metrics
  engine.setVal(TPL_RSSI, WiFi.RSSI());

  // Angle/Tilt
  engine.setVal(TPL_TILT, angle, DECIMALS_TILT);
  engine.setVal(TPL_ANGLE, angle, DECIMALS_TILT);

  // Gravity options
  if (myConfig.isGravitySG()) {
    engine.setVal(TPL_GRAVITY, gravitySG, DECIMALS_SG);
  } else {
    engine.setVal(TPL_GRAVITY, convertToPlato(gravitySG), DECIMALS_PLATO);
  }

  engine.setVal(TPL_GRAVITY_G, gravitySG, DECIMALS_SG);
  engine.setVal(TPL_GRAVITY_P, convertToPlato(gravitySG), DECIMALS_PLATO);
  engine.setVal(TPL_GRAVITY_UNIT, myConfig.getGravityFormat());

  engine.setVal(TPL_APP_VER, CFG_APPVER);
  engine.setVal(TPL_APP_BUILD, CFG_GITREV);
}

// EOF
