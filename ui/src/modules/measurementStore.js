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
import { global, config } from '@/modules/pinia'
import {
  logDebug,
  logError,
  logInfo,
  sharedHttpClient as http,
  tempToF,
  psiToBar,
  psiToKPa,
  gravityToPlato
} from '@mp-se/espframework-ui-components'

class TiltData {
  constructor({ type, source, created, id, color, temp, gravity, txPower, rssi }) {
    this.type = type
    this.source = source
    this.created = new Date(created)
    this.id = id
    this.color = color
    this.temp = config.temp_unit == 'C' ? temp : tempToF(temp)
    this.gravity = config.gravity_unit == 'G' ? gravity : gravityToPlato(gravity)
    this.txPower = txPower
    this.rssi = rssi
    this.isPro = type === 'Tilt Pro'
  }

  getType() {
    return this.type
  }
  getSource() {
    return this.source
  }
  getCreated() {
    return this.created
  }
  getId() {
    return this.id
  }
  getColor() {
    return this.color
  }
  getTemp() {
    return this.temp
  }
  getGravity() {
    return this.gravity
  }
  getTxPower() {
    return this.txPower
  }
  getRssi() {
    return this.rssi
  }
  getIsPro() {
    return this.isPro
  }

  static isTiltDataCsv(line) {
    const parts = line.split(',')
    return parts.length >= 2 && parts[0] === '1' && (parts[1] === 'Tilt' || parts[1] === 'Tilt Pro')
  }

  // Example: 1,Tilt,BLE Beacon,2025-07-06 12:34:56,Red,Red,20.00,1.0500,-59,-70,,,,
  // Example: 1,Tilt Pro,BLE Beacon,2025-07-06 12:34:56,Red,Red,20.00,1.0500,-59,-70,,,,
  static fromCsvLine(line) {
    const parts = line.split(',')
    if (parts.length < 10) {
      logError('TiltData.fromCsvLine', 'Invalid TiltData CSV line', line)
      return
    }
    if (parts[0] !== '1') {
      logError('TiltData.fromCsvLine', 'Unsupported TiltData CSV version:', parts[0], line)
      return
    }
    if (parts[1] !== 'Tilt' && parts[1] !== 'Tilt Pro') {
      logError('TiltData.fromCsvLine', 'CSV line is not for TiltData:', parts[1], line)
      return
    }
    return new TiltData({
      type: parts[1],
      source: parts[2],
      created: parts[3], // Will be parsed to Date in constructor
      id: parts[4],
      color: parts[5],
      temp: parseFloat(parts[6]),
      gravity: parseFloat(parts[7]),
      txPower: parseInt(parts[8], 10),
      rssi: parseInt(parts[9], 10)
    })
  }
}

class PressureData {
  constructor({
    type,
    source,
    created,
    id,
    name,
    token,
    temp,
    pressure,
    pressure1,
    battery,
    txPower,
    rssi,
    interval
  }) {
    this.type = type
    this.source = source
    this.created = new Date(created)
    this.id = id
    this.name = name
    this.token = token
    this.temp = config.temp_unit == 'C' ? temp : tempToF(temp)
    this.pressure =
      config.pressure_unit == 'PSI'
        ? pressure
        : config.pressure_unit == 'Bar'
          ? psiToBar(pressure)
          : psiToKPa(pressure)
    this.pressure1 =
      config.pressure_unit == 'PSI'
        ? pressure1
        : config.pressure_unit == 'Bar'
          ? psiToBar(pressure1)
          : psiToKPa(pressure1)
    this.battery = battery
    this.txPower = txPower
    this.rssi = rssi
    this.interval = interval
  }

  getType() {
    return this.type
  }
  getSource() {
    return this.source
  }
  getCreated() {
    return this.created
  }
  getId() {
    return this.id
  }
  getName() {
    return this.name
  }
  getToken() {
    return this.token
  }
  getTemp() {
    return this.temp
  }
  getPressure() {
    return this.pressure
  }
  getPressure1() {
    return this.pressure1
  }
  getBattery() {
    return this.battery
  }
  getTxPower() {
    return this.txPower
  }
  getRssi() {
    return this.rssi
  }
  getInterval() {
    return this.interval
  }

  static isPressureDataCsv(line) {
    const parts = line.split(',')
    return parts.length >= 2 && parts[0] === '1' && parts[1] === 'Pressuremon'
  }

  // Example: 1,Pressuremon,BLE Beacon,2025-07-06 12:34:56,ID,Name,Token,20.00,1.0000,1.0000,3.70,-59,-70,60
  static fromCsvLine(line) {
    const parts = line.split(',')
    if (parts.length < 14) {
      logError('PressureData.fromCsvLine', 'Invalid PressureData CSV line', line)
      return
    }
    if (parts[0] !== '1') {
      logError('PressureData.fromCsvLine', 'Unsupported PressureData CSV version:', parts[0], line)
      return
    }
    if (parts[1] !== 'Pressuremon') {
      logError('PressureData.fromCsvLine', 'CSV line is not for PressureData:', parts[1], line)
      return
    }
    return new PressureData({
      type: parts[1],
      source: parts[2],
      created: parts[3],
      id: parts[4],
      name: parts[5],
      token: parts[6],
      temp: parseFloat(parts[7]),
      pressure: parseFloat(parts[8]),
      pressure1: parseFloat(parts[9]),
      battery: parseFloat(parts[10]),
      txPower: parseInt(parts[11], 10),
      rssi: parseInt(parts[12], 10),
      interval: parseInt(parts[13], 10)
    })
  }
}

class GravityData {
  constructor({
    type,
    source,
    created,
    id,
    name,
    token,
    temp,
    gravity,
    angle,
    battery,
    txPower,
    rssi,
    interval
  }) {
    this.type = type
    this.source = source
    this.created = new Date(created)
    this.id = id
    this.name = name
    this.token = token
    this.temp = config.temp_unit == 'C' ? temp : tempToF(temp)
    this.gravity = config.gravity_unit == 'G' ? gravity : gravityToPlato(gravity)
    this.angle = angle
    this.battery = battery
    this.txPower = txPower
    this.rssi = rssi
    this.interval = interval
  }

  getType() {
    return this.type
  }
  getSource() {
    return this.source
  }
  getCreated() {
    return this.created
  }
  getId() {
    return this.id
  }
  getName() {
    return this.name
  }
  getToken() {
    return this.token
  }
  getTemp() {
    return this.temp
  }
  getGravity() {
    return this.gravity
  }
  getAngle() {
    return this.angle
  }
  getBattery() {
    return this.battery
  }
  getTxPower() {
    return this.txPower
  }
  getRssi() {
    return this.rssi
  }
  getInterval() {
    return this.interval
  }

  static isGravityDataCsv(line) {
    const parts = line.split(',')
    return parts.length >= 2 && parts[0] === '1' && parts[1] === 'Gravitymon'
  }

  // Example: 1,Gravitymon,BLE Beacon,2025-07-06 12:34:56,ID,Name,Token,20.00,1.0500,45.00,3.70,-59,-70,60
  static fromCsvLine(line) {
    const parts = line.split(',')
    if (parts.length < 14) {
      logError('GravityData.fromCsvLine', 'Invalid GravityData CSV line', line)
      return
    }
    if (parts[0] !== '1') {
      logError('GravityData.fromCsvLine', 'Unsupported GravityData CSV version:', parts[0], line)
      return
    }
    if (parts[1] !== 'Gravitymon') {
      logError('GravityData.fromCsvLine', 'CSV line is not for GravityData:', parts[1], line)
      return
    }
    return new GravityData({
      type: parts[1],
      source: parts[2],
      created: parts[3],
      id: parts[4],
      name: parts[5],
      token: parts[6],
      temp: parseFloat(parts[7]),
      gravity: parseFloat(parts[8]),
      angle: parseFloat(parts[9]),
      battery: parseFloat(parts[10]),
      txPower: parseInt(parts[11], 10),
      rssi: parseInt(parts[12], 10),
      interval: parseInt(parts[13], 10)
    })
  }
}

class RaptData {
  constructor({ type, source, created, id, temp, gravity, angle, battery, txPower, rssi }) {
    this.type = type
    this.source = source
    this.created = new Date(created)
    this.id = id
    this.temp = config.temp_unit == 'C' ? temp : tempToF(temp)
    this.gravity = config.gravity_unit == 'G' ? gravity : gravityToPlato(gravity)
    this.angle = angle
    this.battery = battery
    this.txPower = txPower
    this.rssi = rssi
  }

  getType() {
    return this.type
  }
  getSource() {
    return this.source
  }
  getCreated() {
    return this.created
  }
  getId() {
    return this.id
  }
  getTemp() {
    return this.temp
  }
  getGravity() {
    return this.gravity
  }
  getAngle() {
    return this.angle
  }
  getBattery() {
    return this.battery
  }
  getTxPower() {
    return this.txPower
  }
  getRssi() {
    return this.rssi
  }

  static isRaptDataCsv(line) {
    const parts = line.split(',')
    return parts.length >= 2 && parts[0] === '1' && parts[1] === 'RAPT'
  }

  // Example: 1,RAPT,BLE Beacon,2025-08-13 08:04:40,EEE222,15.20,1.0300,35.3300,3.84,10,-72,,,
  static fromCsvLine(line) {
    const parts = line.split(',')
    if (parts.length < 14) {
      logError('RaptData.fromCsvLine', 'Invalid RaptData CSV line', line)
      return
    }
    if (parts[0] !== '1') {
      logError('RaptData.fromCsvLine', 'Unsupported RaptData CSV version:', parts[0], line)
      return
    }
    if (parts[1] !== 'RAPT') {
      logError('RaptData.fromCsvLine', 'CSV line is not for RaptData:', parts[1], line)
      return
    }
    return new RaptData({
      type: parts[1],
      source: parts[2],
      created: parts[3],
      id: parts[4],
      temp: parseFloat(parts[5]),
      gravity: parseFloat(parts[6]),
      angle: parseFloat(parts[7]),
      battery: parseFloat(parts[8]),
      txPower: parseInt(parts[9], 10),
      rssi: parseInt(parts[10], 10)
    })
  }
}

class ChamberData {
  constructor({ type, source, created, id, chamberTemp, beerTemp, rssi, name, txPower }) {
    this.type = type
    this.source = source
    this.created = new Date(created)
    this.id = id
    this.chamberTemp = config.temp_unit == 'C' ? chamberTemp : tempToF(chamberTemp)
    this.beerTemp = config.temp_unit == 'C' ? beerTemp : tempToF(beerTemp)
    this.rssi = rssi
    this.name = name
    this.txPower = txPower
  }

  getType() {
    return this.type
  }
  getSource() {
    return this.source
  }
  getCreated() {
    return this.created
  }
  getId() {
    return this.id
  }
  getChamberTemp() {
    return this.chamberTemp
  }
  getBeerTemp() {
    return this.beerTemp
  }
  getRssi() {
    return this.rssi
  }
  getName() {
    return this.name
  }
  getTxPower() {
    return this.txPower
  }

  static isChamberDataCsv(line) {
    const parts = line.split(',')
    return parts.length >= 2 && parts[0] === '1' && parts[1] === 'Chamber Controller'
  }

  // Example: 1,Chamber Controller,BLE Beacon,2025-07-06 12:34:56,ID,20.00,19.00,-70,Name,,,,,
  static fromCsvLine(line) {
    const parts = line.split(',')
    if (parts.length < 8) {
      logError('ChamberData.fromCsvLine', 'Invalid ChamberData CSV line', line)
      return
    }
    if (parts[0] !== '1') {
      logError('ChamberData.fromCsvLine', 'Unsupported ChamberData CSV version:', parts[0], line)
      return
    }
    if (parts[1] !== 'Chamber Controller') {
      logError('ChamberData.fromCsvLine', 'CSV line is not for ChamberData:', parts[1], line)
      return
    }
    return new ChamberData({
      type: parts[1],
      source: parts[2],
      created: parts[3],
      id: parts[4],
      chamberTemp: parseFloat(parts[5]),
      beerTemp: parseFloat(parts[6]),
      rssi: parseInt(parts[7], 10),
      name: parts[8],
      txPower: parseInt(parts[9], 10)
    })
  }
}

export const useMeasurementStore = defineStore('measurement', {
  state: () => {
    return {
      files: [],
      gravitymonData: [],
      pressuremonData: [],
      tiltData: [],
      chamberData: [],
      raptData: []
    }
  },
  getters: {},
  actions: {
    async fetchAllMeasurementFiles() {
      if (!this.files || this.files.length === 0) return

      logInfo('measurementStore.fetchAllMeasurementFiles()', 'Fetching from', this.files)

      const fetchFile = async (file) => {
        const res = await this.fetchSecureDiskFile(file)
        return res.success && res.text ? res.text : ''
      }
      const fileContents = await Promise.all(this.files.map(fetchFile))
      const allLines = fileContents
        .map((content) => content.split(/\r?\n/).filter((line) => line.trim().length > 0))
        .flat()

      // logInfo('measurementStore.fetchAllMeasurementFiles()', allLines)

      this.tiltData = []
      this.gravitymonData = []
      this.pressuremonData = []
      this.chamberData = []
      this.raptData = []

      for (const line of allLines) {
        if (!line.trim()) continue
        if (TiltData.isTiltDataCsv(line)) {
          const obj = TiltData.fromCsvLine(line)
          if (obj && this.isValidDate(obj)) this.tiltData.push(obj)
        } else if (GravityData.isGravityDataCsv(line)) {
          const obj = GravityData.fromCsvLine(line)
          if (obj && this.isValidDate(obj)) this.gravitymonData.push(obj)
        } else if (PressureData.isPressureDataCsv(line)) {
          const obj = PressureData.fromCsvLine(line)
          if (obj && this.isValidDate(obj)) this.pressuremonData.push(obj)
        } else if (ChamberData.isChamberDataCsv(line)) {
          const obj = ChamberData.fromCsvLine(line)
          if (obj && this.isValidDate(obj)) this.chamberData.push(obj)
        } else if (RaptData.isRaptDataCsv(line)) {
          const obj = RaptData.fromCsvLine(line)
          if (obj && this.isValidDate(obj)) this.raptData.push(obj)
        } else {
          logError('parseMeasurementFileLines', 'Unknown CSV line type', line)
        }
      }

      // Sort each data set by created (ascending)
      const getTime = (entry) =>
        entry.getCreated() instanceof Date
          ? entry.getCreated().getTime()
          : new Date(entry.getCreated()).getTime()
      this.gravitymonData.sort((a, b) => getTime(a) - getTime(b))
      this.pressuremonData.sort((a, b) => getTime(a) - getTime(b))
      this.tiltData.sort((a, b) => getTime(a) - getTime(b))
      this.chamberData.sort((a, b) => getTime(a) - getTime(b))
      this.raptData.sort((a, b) => getTime(a) - getTime(b))

      return
    },

    // Helper function to validate date - only accept data from 2025 onwards
    isValidDate(obj) {
      if (!obj || !obj.getCreated()) return false
      const date = obj.getCreated() instanceof Date ? obj.getCreated() : new Date(obj.getCreated())
      return date.getFullYear() >= 2025
    },

    async updateMeasurementFiles() {
      logInfo(
        'measurementStore.updateMeasurementFiles()',
        'Updating measurement files, limit',
        config.sd_log_files
      )

      const data = { command: 'dir' }

      this.files = []

      const res = await this.sendSecureDiskRequest(data)
      if (res.success) {
        try {
          const json = JSON.parse(res.text)
          logInfo('measurementStore.updateMeasurementFiles()', json)
          for (const f of json.files) {
            if (f.file.endsWith('.csv') && f.file.startsWith('/data')) {
              // Extract the number after '/data' (e.g., '/data.csv' -> 0, '/data1.csv' -> 1)
              const fileName = f.file.replace('/data', '').replace('.csv', '')
              const fileNumber = fileName === '' ? 0 : parseInt(fileName, 10)

              // Only include files where the number is < config.sd_log_files
              if (fileNumber < config.sd_log_files) {
                this.files.push(f.file)
              }
            }
          }
          logInfo('measurementStore.updateMeasurementFiles()', this.files)
          return true
        } catch (err) {
          logError('measurementStore.updateMeasurementFiles()', 'Failed to parse response', err)
          return false
        }
      } else {
        logError('measurementStore.updateMeasurementFiles()', 'Failed to fetch measurement files')
        return false
      }
    },

    async sendSecureDiskRequest(data) {
      global.disabled = true
      logInfo('dataStore.sendSecureDiskRequest()', 'Sending /api/sd')
      try {
        // Use postJson to send JSON payload and get the raw response
        const resp = await http.postJson('api/sd', data)
        // postJson returns a Response-like object; read text
        const text = await resp.text()
        logDebug('dataStore.sendSecureDiskRequest()', text)
        global.disabled = false
        return { success: true, text }
      } catch (err) {
        logError('dataStore.sendSecureDiskRequest()', err)
        global.disabled = false
        return { success: false, text: '' }
      }
    },

    async fetchSecureDiskFile(fileName) {
      global.disabled = true
      logInfo('dataStore.fetchSecureDiskFile()', 'Fetching file from /sd')
      try {
        const resp = await http.request('sd' + fileName, { method: 'GET' })
        const text = await resp.text()
        // logDebug('dataStore.fetchSecureDiskFile()', text)
        global.disabled = false
        return { success: true, text }
      } catch (err) {
        logError('dataStore.fetchSecureDiskFile()', err)
        global.disabled = false
        return { success: false, text: '' }
      }
    }
  }
})
