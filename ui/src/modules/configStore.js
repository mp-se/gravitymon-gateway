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

import { defineStore } from 'pinia'
import { global, saveConfigState, getConfigChanges } from '@/modules/pinia'
import { logDebug, logError, logInfo } from '@mp-se/espframework-ui-components'
import { sharedHttpClient as http } from '@mp-se/espframework-ui-components'

export const useConfigStore = defineStore('config', {
  state: () => {
    return {
      // Device
      id: '',
      mdns: '',
      temp_unit: '',
      gravity_unit: '',
      pressure_unit: '',
      dark_mode: false,
      // Hardware
      ble_enable: true,
      ble_active_scan: false,
      ble_scan_time: 5,
      timezone: '',
      // Logging
      sd_log_files: 4,
      sd_log_min_time: 5,
      display_layout_id: 0,
      // Wifi
      wifi_portal_timeout: 0,
      wifi_connect_timeout: 0,
      use_wifi_direct: false,
      wifi_ssid: '',
      wifi_ssid2: '',
      wifi_pass: '',
      wifi_pass2: '',
      wifi_direct_ssid: '',
      wifi_direct_pass: '',
      // Push - Generic
      token: '',
      push_timeout: 0,
      push_resend_time: 300,
      // Push - Http Post 1
      http_post_target: '',
      http_post_header1: '',
      http_post_header2: '',
      http_post_format_gravity: '',
      http_post_format_pressure: '',
      // Push - Http Post 2
      http_post2_target: '',
      http_post2_header1: '',
      http_post2_header2: '',
      http_post2_format_gravity: '',
      http_post2_format_pressure: '',
      // Push - Http Get
      http_get_target: '',
      http_get_header1: '',
      http_get_header2: '',
      http_get_format_gravity: '',
      http_get_format_pressure: '',
      // Push - Influx
      influxdb2_target: '',
      influxdb2_org: '',
      influxdb2_bucket: '',
      influxdb2_token: '',
      influxdb2_format_gravity: '',
      influxdb2_format_pressure: '',
      // Push - MQTT
      mqtt_target: '',
      mqtt_port: '',
      mqtt_user: '',
      mqtt_pass: '',
      mqtt_format_gravity: '',
      mqtt_format_pressure: '',
      // Push
      http_post_gravity: true,
      http_post_pressure: true,
      http_post2_gravity: true,
      http_post2_pressure: true,
      http_get_gravity: true,
      http_get_pressure: true,
      influxdb2_gravity: true,
      influxdb2_pressure: true,
      mqtt_gravity: true,
      mqtt_pressure: true,

      // Values that are not updated but needed for format template viewer
      sleep_interval: 900
    }
  },
  actions: {
    isPsi() {
      return this.pressure_unit === 'PSI'
    },
    isBar() {
      return this.pressure_unit === 'Bar'
    },
    isKPa() {
      return this.pressure_unit === 'kPa'
    },
    toJson() {
      logInfo('configStore.toJSON()')
      const dest = {}

      for (const key in this.$state) {
        if (!key.startsWith('$')) {
          if (key === 'gyro_calibration_data') {
            dest[key] = []
            for (var i in this.$state[key]) {
              dest[key][i] = this.$state[key][i]
            }
          } else if (key === 'formula_calculation_data') {
            dest[key] = []
            for (i in this.$state[key]) {
              dest[key][i] = {}
              dest[key][i].a = this.$state[key][i].a
              dest[key][i].g = this.$state[key][i].g
            }
          } else {
            dest[key] = this[key]
          }
        }
      }

      logInfo('configStore.toJSON()', dest)
      return JSON.stringify(dest, null, 2)
    },
    async load() {
      global.disabled = true
      logInfo('configStore.load()', 'Fetching /api/config')
      try {
        const json = await http.getJson('api/config')
        logDebug('configStore.load()', json)
        global.disabled = false
        this.id = json.id
        // Device
        this.mdns = json.mdns
        this.temp_unit = json.temp_unit
        this.gravity_unit = json.gravity_unit
        this.pressure_unit = json.pressure_unit
        this.dark_mode = json.dark_mode
        // Hardware
        this.ble_enable = json.ble_enable
        this.ble_active_scan = json.ble_active_scan
        this.ble_scan_time = json.ble_scan_time
        this.timezone = json.timezone
        // Logging
        this.sd_log_files = json.sd_log_files
        this.sd_log_min_time = json.sd_log_min_time
        this.display_layout_id = json.display_layout_id
        // Wifi
        this.wifi_portal_timeout = json.wifi_portal_timeout
        this.wifi_connect_timeout = json.wifi_connect_timeout
        this.use_wifi_direct = json.use_wifi_direct
        this.wifi_ssid = json.wifi_ssid
        this.wifi_ssid2 = json.wifi_ssid2
        this.wifi_pass = json.wifi_pass
        this.wifi_pass2 = json.wifi_pass2
        this.wifi_direct_ssid = json.wifi_direct_ssid
        this.wifi_direct_pass = json.wifi_direct_pass
        // Push - Generic
        this.token = json.token
        this.push_timeout = json.push_timeout
        this.push_resend_time = json.push_resend_time
        // Push - Http Post 1
        this.http_post_target = json.http_post_target
        this.http_post_header1 = json.http_post_header1
        this.http_post_header2 = json.http_post_header2
        // this.http_post_format_gravity = json.http_post_format_gravity
        // this.http_post_format_pressure = json.http_post_format_pressure
        // Push - Http Post 2
        this.http_post2_target = json.http_post2_target
        this.http_post2_header1 = json.http_post2_header1
        this.http_post2_header2 = json.http_post2_header2
        // this.http_post2_format_gravity = json.http_post2_format_gravity
        // this.http_post2_format_pressure = json.http_post2_format_pressure
        // Push - Http Get
        this.http_get_target = json.http_get_target
        this.http_get_header1 = json.http_get_header1
        this.http_get_header2 = json.http_get_header2
        // this.http_get_format_gravity = json.http_get_format_gravity
        // this.http_get_format_pressure = json.http_get_format_pressure
        // Push - Influx
        this.influxdb2_target = json.influxdb2_target
        this.influxdb2_org = json.influxdb2_org
        this.influxdb2_bucket = json.influxdb2_bucket
        this.influxdb2_token = json.influxdb2_token
        // this.influxdb2_format_gravity = json.influxdb2_format_gravity
        // this.influxdb2_format_pressure = json.influxdb2_format_pressure
        // Push - MQTT
        this.mqtt_target = json.mqtt_target
        this.mqtt_port = json.mqtt_port
        this.mqtt_user = json.mqtt_user
        this.mqtt_pass = json.mqtt_pass
        // this.mqtt_format_gravity = json.mqtt_format_gravity
        // this.mqtt_format_pressure = json.mqtt_format_pressure
        // Push - flags
        this.http_post_gravity = json.http_post_gravity
        this.http_post_pressure = json.http_post_pressure
        this.http_post2_gravity = json.http_post2_gravity
        this.http_post2_pressure = json.http_post2_pressure
        this.http_get_gravity = json.http_get_gravity
        this.http_get_pressure = json.http_get_pressure
        this.influxdb2_gravity = json.influxdb2_gravity
        this.influxdb2_pressure = json.influxdb2_pressure
        this.mqtt_gravity = json.mqtt_gravity
        this.mqtt_pressure = json.mqtt_pressure

        return true
      } catch (err) {
        global.disabled = false
        logError('configStore.load()', err)
        return false
      }
    },
    async loadFormat() {
      global.disabled = true
      logInfo('configStore.loadFormat()', 'Fetching /api/format')
      try {
        const json = await http.getJson('api/format')
        logDebug('configStore.loadFormat()', json)
        global.disabled = false
        this.http_post_format_gravity = decodeURIComponent(json.http_post_format_gravity)
        this.http_post2_format_gravity = decodeURIComponent(json.http_post2_format_gravity)
        this.http_get_format_gravity = decodeURIComponent(json.http_get_format_gravity)
        this.influxdb2_format_gravity = decodeURIComponent(json.influxdb2_format_gravity)
        this.mqtt_format_gravity = decodeURIComponent(json.mqtt_format_gravity)

        // Pressure format fields
        this.http_post_format_pressure = decodeURIComponent(json.http_post_format_pressure)
        this.http_post2_format_pressure = decodeURIComponent(json.http_post2_format_pressure)
        this.http_get_format_pressure = decodeURIComponent(json.http_get_format_pressure)
        this.influxdb2_format_pressure = decodeURIComponent(json.influxdb2_format_pressure)
        this.mqtt_format_pressure = decodeURIComponent(json.mqtt_format_pressure)

        // Add linebreaks so the editor shows the data correctly
        this.mqtt_format_gravity = this.mqtt_format_gravity.replaceAll('|', '|\n')
        return true
      } catch (err) {
        global.disabled = false
        logError('configStore.loadFormat()', err)
        return false
      }
    },
    async sendConfig() {
      global.disabled = true
      logInfo('configStore.sendConfig()', 'Sending /api/config')

      const data = getConfigChanges()
      delete data.http_post_format_gravity
      delete data.http_post2_format_gravity
      delete data.http_get_format_gravity
      delete data.influxdb2_format_gravity
      delete data.mqtt_format_gravity
      // Pressure format fields should not be sent in this payload
      delete data.http_post_format_pressure
      delete data.http_post2_format_pressure
      delete data.http_get_format_pressure
      delete data.influxdb2_format_pressure
      delete data.mqtt_format_pressure
      logDebug('configStore.sendConfig()', data)

      if (JSON.stringify(data).length == 2) {
        logInfo('configStore.sendConfig()', 'No config data to store, skipping step')
        global.disabled = false
        return true
      }

      try {
        await http.postJson('api/config', data)
        global.disabled = false
        logInfo('configStore.sendConfig()', 'Sending /api/config completed')
        return true
      } catch (err) {
        logError('configStore.sendConfig()', err)
        global.disabled = false
        return false
      }
    },
    async sendFormat() {
      global.disabled = true
      logInfo('configStore.sendFormat()', 'Sending /api/format')

      const data2 = getConfigChanges()
      let data = {}
      let cnt = 0

      logDebug('configStore.sendFormat()', data)
      try {
        data =
          data2.http_post_format_gravity !== undefined
            ? { http_post_format_gravity: encodeURIComponent(data2.http_post_format_gravity) }
            : {}
        if (await this.sendOneFormat(data)) cnt += 1

        data =
          data2.http_post2_format_gravity !== undefined
            ? { http_post2_format_gravity: encodeURIComponent(data2.http_post2_format_gravity) }
            : {}
        if (await this.sendOneFormat(data)) cnt += 1

        data =
          data2.http_get_format_gravity !== undefined
            ? { http_get_format_gravity: encodeURIComponent(data2.http_get_format_gravity) }
            : {}
        if (await this.sendOneFormat(data)) cnt += 1

        data =
          data2.influxdb2_format_gravity !== undefined
            ? { influxdb2_format_gravity: encodeURIComponent(data2.influxdb2_format_gravity) }
            : {}
        if (await this.sendOneFormat(data)) cnt += 1

        if (data2.mqtt_format_gravity !== undefined) {
          data2.mqtt_format_gravity = data2.mqtt_format_gravity.replaceAll('\n', '')
          data2.mqtt_format_gravity = data2.mqtt_format_gravity.replaceAll('\r', '')
        }

        data =
          data2.mqtt_format_gravity !== undefined
            ? { mqtt_format_gravity: encodeURIComponent(data2.mqtt_format_gravity) }
            : {}
        if (await this.sendOneFormat(data)) cnt += 1

        data =
          data2.http_post_format_pressure !== undefined
            ? { http_post_format_pressure: encodeURIComponent(data2.http_post_format_pressure) }
            : {}
        if (await this.sendOneFormat(data)) cnt += 1

        data =
          data2.http_post2_format_pressure !== undefined
            ? { http_post2_format_pressure: encodeURIComponent(data2.http_post2_format_pressure) }
            : {}
        if (await this.sendOneFormat(data)) cnt += 1

        data =
          data2.http_get_format_pressure !== undefined
            ? { http_get_format_pressure: encodeURIComponent(data2.http_get_format_pressure) }
            : {}
        if (await this.sendOneFormat(data)) cnt += 1

        data =
          data2.influxdb2_format_pressure !== undefined
            ? { influxdb2_format_pressure: encodeURIComponent(data2.influxdb2_format_pressure) }
            : {}
        if (await this.sendOneFormat(data)) cnt += 1

        if (data2.mqtt_format_pressure !== undefined) {
          data2.mqtt_format_pressure = data2.mqtt_format_pressure.replaceAll('\n', '')
          data2.mqtt_format_pressure = data2.mqtt_format_pressure.replaceAll('\r', '')
        }

        data =
          data2.mqtt_format_pressure !== undefined
            ? { mqtt_format_pressure: encodeURIComponent(data2.mqtt_format_pressure) }
            : {}
        if (await this.sendOneFormat(data)) cnt += 1

        return cnt == 10
      } finally {
        global.disabled = false
      }
    },
    async sendOneFormat(data) {
      logInfo('configStore.sendOneFormat()', 'Sending /api/format')

      if (JSON.stringify(data).length == 2) {
        logInfo('configStore.sendOneFormat()', 'No format data to store, skipping step')
        return true
      }

      try {
        await http.postJson('api/format', data)
        global.disabled = false
        logInfo('configStore.sendOneFormat()', 'Sending /api/format completed')
        return true
      } catch (err) {
        logError('configStore.sendOneFormat()', err)
        return false
      }
    },
    async sendPushTest(data) {
      global.disabled = true
      logInfo('configStore.sendPushTest()', 'Sending /api/push')
      try {
        await http.postJson('api/push', data)
        return true
      } catch (err) {
        logError('configStore.sendPushTest()', err)
        return false
      }
    },
    async setSleepMode(flag) {
      try {
        logInfo('configStore.setSleepMode()', 'Sending /api/sleepmode')
        const response = await http.request('api/sleepmode', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ sleep_mode: flag })
        })
        if (!response.ok) {
          logError('configStore.setSleepMode()', `HTTP ${response.status}: ${response.statusText}`)
          return false
        }

        await response.json()
        logInfo('configStore.setSleepMode()', 'Sending /api/sleepmode completed')
        return true
      } catch (err) {
        logError('configStore.setSleepMode()', err)
        return false
      }
    },
    async restart() {
      global.clearMessages()
      global.disabled = true
      try {
        const res = await http.restart(this.mdns, { redirectDelayMs: 8000 })
        if (res.success && res.json && res.json.status === true) {
          global.messageSuccess =
            (res.json.message || '') +
            ' Redirecting to http://' +
            this.mdns +
            '.local in 8 seconds.'
          logInfo('configStore.restart()', 'Restart requested, redirect scheduled')
        } else if (res.success && res.json) {
          global.messageError = res.json.message || 'Failed to restart device'
        } else {
          global.messageError = 'Failed to request restart'
        }
      } catch (err) {
        logError('configStore.restart()', err)
        global.messageError = 'Failed to do restart'
      } finally {
        global.disabled = false
      }
    },
    async getPushTestStatus() {
      logInfo('configStore.getPushTest()', 'Fetching /api/push/status')
      try {
        const json = await http.getJson('api/push/status')
        logDebug('configStore.getPushTest()', json)
        logInfo('configStore.getPushTest()', 'Fetching /api/push/status completed')
        return { success: true, data: json }
      } catch (err) {
        logError('configStore.getPushTest()', err)
        return { success: false, data: null }
      }
    },
    async sendWifiScan() {
      global.disabled = true
      logInfo('configStore.sendWifiScan()', 'Sending /api/wifi')
      try {
        await http.request('api/wifi')
        logInfo('configStore.sendWifiScan()', 'Sending /api/wifi completed')
        return true
      } catch (err) {
        logError('configStore.sendWifiScan()', err)
        return false
      }
    },
    async getWifiScanStatus() {
      logInfo('configStore.getWifiScanStatus()', 'Fetching /api/wifi/status')
      try {
        const json = await http.getJson('api/wifi/status')
        logDebug('configStore.getWifiScanStatus()', json)
        logInfo('configStore.getWifiScanStatus()', 'Fetching /api/wifi/status completed')
        return { success: true, data: json }
      } catch (err) {
        logError('configStore.getWifiScanStatus()', err)
        return { success: false, data: null }
      }
    },
    async sendHardwareScan() {
      global.disabled = true
      logInfo('configStore.sendHardwareScan()', 'Sending /api/hardware')
      try {
        await http.request('api/hardware')
        logInfo('configStore.sendHardwareScan()', 'Sending /api/hardware completed')
        return true
      } catch (err) {
        logError('configStore.sendHardwareScan()', err)
        return false
      }
    },
    async getHardwareScanStatus() {
      logInfo('configStore.getHardwareScanStatus()', 'Fetching /api/hardware/status')
      try {
        const json = await http.getJson('api/hardware/status')
        logDebug('configStore.getHardwareScanStatus()', json)
        logInfo('configStore.getHardwareScanStatus()', 'Fetching /api/hardware/status completed')
        return { success: true, data: json }
      } catch (err) {
        logError('configStore.getHardwareScanStatus()', err)
        return { success: false, data: null }
      }
    },
    async saveAll() {
      global.clearMessages()
      global.disabled = true

      try {
        const configSuccess = await this.sendConfig()
        if (!configSuccess) {
          global.messageError = 'Failed to store configuration to device'
          return
        }

        const formatSuccess = await this.sendFormat()
        if (!formatSuccess) {
          global.messageError = 'Failed to store format to device'
          return
        }

        global.messageSuccess = 'Configuration has been saved to device'
        saveConfigState()
      } catch (error) {
        logError('configStore.saveAll()', error)
        global.messageError = 'Failed to save configuration'
      } finally {
        global.disabled = false
      }
    },
    async runPushTest(data) {
      global.disabled = true
      logInfo('configStore.runPushTest()', 'Starting push test')

      try {
        const pushStarted = await this.sendPushTest(data)
        if (!pushStarted) {
          global.messageError = 'Failed to start push test'
          return false
        }

        // Poll for test completion
        const result = await (async () => {
          while (true) {
            const statusRes = await this.getPushTestStatus()
            if (!statusRes.success) {
              global.messageError = 'Failed to get push test status'
              return false
            }

            const d = statusRes.data
            if (d.status) {
              // still running
              await new Promise((r) => setTimeout(r, 2000))
              continue
            }

            if (!d.success) {
              global.messageError = 'Test failed with error code (' + d.push_return_code + ')'
              return true
            } else {
              if (!d.push_enabled) {
                global.messageWarning = 'No endpoint is defined for this target. Cannot run test.'
              } else if (!d.success && d.push_return_code > 0) {
                global.messageError =
                  'Test failed with error code (' + http.getErrorString(d.push_return_code) + ')'
              } else if (!d.success && d.push_return_code == 0) {
                global.messageError =
                  'Test not started. Might be blocked due to skip SSL flag enabled on esp8266'
              } else {
                global.messageSuccess = 'Test was successful'
              }
              return true
            }
          }
        })()

        return result
      } catch (error) {
        logError('configStore.runPushTest()', error)
        global.messageError = 'Push test failed unexpectedly'
        return false
      } finally {
        global.disabled = false
      }
    },

    async runWifiScan() {
      global.disabled = true
      logInfo('configStore.runWifiScan()', 'Starting wifi scan')

      try {
        const started = await this.sendWifiScan()
        if (!started) {
          global.messageError = 'Failed to start wifi scan'
          return { success: false }
        }

        while (true) {
          const statusRes = await this.getWifiScanStatus()
          if (!statusRes.success) {
            global.messageError = 'Failed to get wifi scan status'
            return { success: false }
          }

          if (statusRes.data.status) {
            await new Promise((r) => setTimeout(r, 2000))
            continue
          }

          global.disabled = false
          return { success: statusRes.data.success, data: statusRes.data }
        }
      } finally {
        global.disabled = false
      }
    },

    async runHardwareScan() {
      global.disabled = true
      logInfo('configStore.runHardwareScan()', 'Starting hardware scan')

      try {
        const started = await this.sendHardwareScan()
        if (!started) {
          global.messageError = 'Failed to start hardware scan'
          return { success: false }
        }

        while (true) {
          const statusRes = await this.getHardwareScanStatus()
          if (!statusRes.success) {
            global.messageError = 'Failed to get hardware scan status'
            return { success: false }
          }

          if (statusRes.data.status) {
            await new Promise((r) => setTimeout(r, 2000))
            continue
          }

          global.disabled = false
          return { success: statusRes.data.success, data: statusRes.data }
        }
      } finally {
        global.disabled = false
      }
    }
  }
})
