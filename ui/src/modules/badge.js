/*
 * GravityMon Gateway UI
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

import { config } from '@/modules/pinia'

/**
 * Used in menybar to show the total amount of items that require user action.
 *
 * @returns number of items that needs attention
 */
export function deviceBadge() {
  return deviceSettingBadge() + deviceHardwareBadge() + deviceWifiBadge()
}

export function deviceSettingBadge() {
  return deviceMdnsBadge()
}

export function deviceHardwareBadge() {
  return false
}

export function deviceMdnsBadge() {
  return config.mdns === '' ? 1 : 0
}

export function deviceWifiBadge() {
  return deviceWifi1Badge() | deviceWifi2Badge() ? 1 : 0
}

export function deviceWifi1Badge() {
  if (config.wifi_ssid === '') return 1
  return 0
}

export function deviceWifi2Badge() {
  if (config.wifi_ssid2 === '' && config.wifi_ssid === '') return 1
  return 0
}

/**
 * Used in menybar to show the total amount of items that require user action.
 *
 * @returns number of items that needs attention
 */
export function pushBadge() {
  return (
    pushSettingBadge() +
    pushHttpPost1Badge() +
    pushHttpPost2Badge() +
    pushHttpGetBadge() +
    pushHttpInfluxdb2Badge() +
    pushHttpMqttBadge()
  )
}

function pushTargetCount() {
  var cnt = 0
  cnt += config.http_post_target === '' ? 0 : 1
  cnt += config.http_post2_target === '' ? 0 : 1
  cnt += config.http_get_target === '' ? 0 : 1
  cnt += config.influxdb2_target === '' ? 0 : 1
  cnt += config.mqtt_target === '' ? 0 : 1
  return cnt
}

export function pushSettingBadge() {
  return 0
}

export function pushHttpPost1Badge() {
  return pushTargetCount() === 0 ? 1 : 0
}

export function pushHttpPost2Badge() {
  return pushTargetCount() === 0 ? 1 : 0
}

export function pushHttpGetBadge() {
  return pushTargetCount() === 0 ? 1 : 0
}

export function pushHttpInfluxdb2Badge() {
  return pushTargetCount() === 0 ? 1 : 0
}

export function pushHttpMqttBadge() {
  return pushTargetCount() === 0 ? 1 : 0
}

export function pushHttpBluetoothBadge() {
  return pushTargetCount() === 0 ? 1 : 0
}
