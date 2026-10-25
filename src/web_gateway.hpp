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
#ifndef SRC_WEB_GATEWAY_HPP_
#define SRC_WEB_GATEWAY_HPP_

#if defined(GATEWAY)

#include <queue>
#include <web_brewing.hpp>
#include <web_gateway.hpp>

class GatewayWebServer : public BrewingWebServer {
 private:
  std::queue<String> _postData;
  GravmonGatewayConfig *_gatewayConfig = nullptr;

 public:
  explicit GatewayWebServer(GravmonGatewayConfig *config);
  void webHandleRemotePost(AsyncWebServerRequest *request, JsonVariant &json);
  void webHandleSecureDigital(AsyncWebServerRequest *request,
                              JsonVariant &json);
  void webHandleMdns(AsyncWebServerRequest *request);

  void doWebStatus(JsonObject &obj);
  bool setupWebServer(const char *serviceName);

  void doWebCalibrateStatus(JsonObject &obj) {}
  void doWebConfigWrite() {}
  void doTaskSensorCalibration() {}
  void doTaskPushTestSetup(TemplatingEngine &engine, BrewingPush &push);
  void doTaskHardwareScanning(JsonObject &obj) {}
  void doWebFeature(JsonObject &obj);

  void loop();
};

// Global instance created
extern GatewayWebServer myWebServer;

#endif  // GATEWAY

#endif  // SRC_WEB_GATEWAY_HPP_

// EOF
