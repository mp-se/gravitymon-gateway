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

#include <config_gateway.hpp>
#include <main.hpp>

GravmonGatewayConfig::GravmonGatewayConfig(String baseMDNS, String fileName)
    : BrewingConfig(baseMDNS, fileName) {}

void GravmonGatewayConfig::createJson(JsonObject& doc) const {
  BrewingConfig::createJson(doc);

  // Call base class functions
  createJsonBase(doc);
  createJsonWifi(doc);
  createJsonPush(doc);

  doc[CONFIG_GRAVITY_UNIT] = String(getGravityUnit());
  doc[CONFIG_TIMEZONE] = getTimezone();
  doc[CONFIG_BLE_ACTIVE_SCAN] = getBleActiveScan();
  doc[CONFIG_BLE_ENABLE] = isBleEnable();
  doc[CONFIG_BLE_SCAN_TIME] = getBleScanTime();
  doc[CONFIG_PUSH_RESEND_TIME] = getPushResendTime();
  doc[CONFIG_PRESSURE_UNIT] = getPressureUnit();
  doc[CONFIG_SD_LOG_FILES] = getSdLogFiles();
  doc[CONFIG_SD_LOG_MIN_TIME] = getSdLogMinTime();
  doc[CONFIG_DISPLAY_LAYOUT_ID] = getDisplayLayoutId();

  doc[CONFIG_HTTP_POST_GRAVITY_ENABLE] = isHttpPostGravityEnable();
  doc[CONFIG_HTTP_POST_PRESSURE_ENABLE] = isHttpPostPressureEnable();
  doc[CONFIG_HTTP_POST2_GRAVITY_ENABLE] = isHttpPost2GravityEnable();
  doc[CONFIG_HTTP_POST2_PRESSURE_ENABLE] = isHttpPost2PressureEnable();
  doc[CONFIG_HTTP_GET_GRAVITY_ENABLE] = isHttpGetGravityEnable();
  doc[CONFIG_HTTP_GET_PRESSURE_ENABLE] = isHttpGetPressureEnable();
  doc[CONFIG_INFLUXDB2_GRAVITY_ENABLE] = isInfluxdb2GravityEnable();
  doc[CONFIG_INFLUXDB2_PRESSURE_ENABLE] = isInfluxdb2PressureEnable();
  doc[CONFIG_MQTT_GRAVITY_ENABLE] = isMqttGravityEnable();
  doc[CONFIG_MQTT_PRESSURE_ENABLE] = isMqttPressureEnable();
}

void GravmonGatewayConfig::parseJson(JsonObject& doc) {
  BrewingConfig::parseJson(doc);

  // Call base class functions
  parseJsonBase(doc);
  parseJsonWifi(doc);
  parseJsonPush(doc);

  if (!doc[CONFIG_GRAVITY_UNIT].isNull()) {
    String s = doc[CONFIG_GRAVITY_UNIT];
    setGravityUnit(s.charAt(0));
  }
  if (!doc["gravity_format"].isNull()) {  // Legacy support for gravity_format
    String s = doc["gravity_format"];
    setGravityUnit(s.charAt(0));
  }

  if (!doc[CONFIG_TIMEZONE].isNull()) setTimezone(doc[CONFIG_TIMEZONE]);
  if (!doc[CONFIG_BLE_ENABLE].isNull())
    setBleEnable(doc[CONFIG_BLE_ENABLE].as<bool>());
  if (!doc[CONFIG_BLE_ACTIVE_SCAN].isNull())
    setBleActiveScan(doc[CONFIG_BLE_ACTIVE_SCAN].as<bool>());
  if (!doc[CONFIG_BLE_SCAN_TIME].isNull())
    setBleScanTime(doc[CONFIG_BLE_SCAN_TIME].as<int>());
  if (!doc[CONFIG_PUSH_RESEND_TIME].isNull())
    setPushResendTime(doc[CONFIG_PUSH_RESEND_TIME].as<int>());
  if (!doc[CONFIG_PRESSURE_UNIT].isNull())
    setPressureUnit(doc[CONFIG_PRESSURE_UNIT].as<String>());
  if (!doc[CONFIG_SD_LOG_FILES].isNull())
    setSdLogFiles(doc[CONFIG_SD_LOG_FILES].as<int>());
  if (!doc[CONFIG_SD_LOG_MIN_TIME].isNull())
    setSdLogMinTime(doc[CONFIG_SD_LOG_MIN_TIME].as<int>());
  if (!doc[CONFIG_DISPLAY_LAYOUT_ID].isNull())
    setDisplayLayoutId(doc[CONFIG_DISPLAY_LAYOUT_ID].as<int>());

  if (!doc[CONFIG_HTTP_POST_GRAVITY_ENABLE].isNull())
    setHttpPostGravityEnable(doc[CONFIG_HTTP_POST_GRAVITY_ENABLE].as<bool>());
  if (!doc[CONFIG_HTTP_POST_PRESSURE_ENABLE].isNull())
    setHttpPostPressureEnable(doc[CONFIG_HTTP_POST_PRESSURE_ENABLE].as<bool>());

  if (!doc[CONFIG_HTTP_POST2_GRAVITY_ENABLE].isNull())
    setHttpPost2GravityEnable(doc[CONFIG_HTTP_POST2_GRAVITY_ENABLE].as<bool>());
  if (!doc[CONFIG_HTTP_POST2_PRESSURE_ENABLE].isNull())
    setHttpPost2PressureEnable(
        doc[CONFIG_HTTP_POST2_PRESSURE_ENABLE].as<bool>());

  if (!doc[CONFIG_HTTP_GET_GRAVITY_ENABLE].isNull())
    setHttpGetGravityEnable(doc[CONFIG_HTTP_GET_GRAVITY_ENABLE].as<bool>());
  if (!doc[CONFIG_HTTP_GET_PRESSURE_ENABLE].isNull())
    setHttpGetPressureEnable(doc[CONFIG_HTTP_GET_PRESSURE_ENABLE].as<bool>());
  if (!doc[CONFIG_INFLUXDB2_GRAVITY_ENABLE].isNull())
    setInfluxdb2GravityEnable(doc[CONFIG_INFLUXDB2_GRAVITY_ENABLE].as<bool>());
  if (!doc[CONFIG_INFLUXDB2_PRESSURE_ENABLE].isNull())
    setInfluxdb2PressureEnable(
        doc[CONFIG_INFLUXDB2_PRESSURE_ENABLE].as<bool>());
  if (!doc[CONFIG_MQTT_GRAVITY_ENABLE].isNull())
    setMqttGravityEnable(doc[CONFIG_MQTT_GRAVITY_ENABLE].as<bool>());
  if (!doc[CONFIG_MQTT_PRESSURE_ENABLE].isNull())
    setMqttPressureEnable(doc[CONFIG_MQTT_PRESSURE_ENABLE].as<bool>());
}

#endif  // GATEWAY

// EOF
