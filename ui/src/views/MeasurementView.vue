<!--
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
 -->
<template>
  <div class="container">
    <p></p>
    <p class="h3">Measurements</p>
    <hr />

    <div class="row" v-if="!status.sd_mounted">
      <p>No SD card attached so this feature is not available.</p>
    </div>

    <div class="row" v-if="status.sd_mounted">
      <div class="col-md-12">
        <BsInputRadio
          v-model="deviceType"
          :options="deviceTypeOptions"
          label="Device Type"
          width=""
        ></BsInputRadio>
      </div>

      <div class="col-md-12">&nbsp;</div>

      <template v-if="deviceType === 0">
        <div class="col-md-12">
          <BsSelect
            v-model="gravitymonDevice"
            :options="gravitymonDeviceOptions"
            label="Device"
            width="5"
          ></BsSelect>
        </div>

        <div class="col-md-12">&nbsp;</div>

        <MeasurementGraphFragment
          label1="Gravity"
          label2="Temperature"
          :dataSet1="filteredGravitymonGravityData"
          :dataSet2="filteredGravitymonTemperatureData"
          v-if="gravitymonDevice != ''"
        />

        <div class="col-md-12">&nbsp;</div>

        <MeasurementTableFragment
          :data="filteredGravitymonTableData"
          :columns="gravitymonColumns"
        />
      </template>

      <template v-if="deviceType === 1">
        <div class="col-md-12">
          <BsSelect
            v-model="tiltDevice"
            :options="tiltDeviceOptions"
            label="Device"
            width="5"
          ></BsSelect>
        </div>

        <div class="col-md-12">&nbsp;</div>

        <MeasurementGraphFragment
          label1="Gravity"
          label2="Temperature"
          :dataSet1="filteredTiltGravityData"
          :dataSet2="filteredTiltTemperatureData"
          v-if="tiltDevice != ''"
        />

        <div class="col-md-12">&nbsp;</div>

        <MeasurementTableFragment :data="filteredTiltData" :columns="tiltColumns" />
      </template>

      <template v-if="deviceType === 2">
        <div class="col-md-12">
          <BsSelect
            v-model="pressuremonDevice"
            :options="pressuremonDeviceOptions"
            label="Device"
            width="5"
          ></BsSelect>
        </div>

        <div class="col-md-12">&nbsp;</div>

        <MeasurementGraphFragment
          label1="Pressure"
          label2="Temperature"
          :dataSet1="filteredPressuremonPressureData"
          :dataSet2="filteredPressuremonTemperatureData"
          v-if="pressuremonDevice != ''"
        />

        <div class="col-md-12">&nbsp;</div>

        <MeasurementTableFragment
          :data="filteredPressuremonTableData"
          :columns="pressuremonColumns"
        />
      </template>

      <template v-if="deviceType === 3">
        <div class="col-md-12">
          <BsSelect
            v-model="chamberDevice"
            :options="chamberDeviceOptions"
            label="Device"
            width="5"
          ></BsSelect>
        </div>

        <div class="col-md-12">&nbsp;</div>

        <MeasurementGraphFragment
          label1="Chamber Temperature"
          label2="Beer Temperature"
          :dataSet1="filteredChamberChamberTemperatureData"
          :dataSet2="filteredChamberBeerTemperatureData"
          v-if="chamberDevice != ''"
        />

        <div class="col-md-12">&nbsp;</div>

        <MeasurementTableFragment :data="filteredChamberTableData" :columns="chamberColumns" />
      </template>

      <template v-if="deviceType === 4">
        <div class="col-md-12">
          <BsSelect
            v-model="raptDevice"
            :options="raptDeviceOptions"
            label="Device"
            width="5"
          ></BsSelect>
        </div>

        <div class="col-md-12">&nbsp;</div>

        <MeasurementGraphFragment
          label1="Gravity"
          label2="Temperature"
          :dataSet1="filteredRaptGravityData"
          :dataSet2="filteredRaptTemperatureData"
          v-if="raptDevice != ''"
        />

        <div class="col-md-12">&nbsp;</div>

        <MeasurementTableFragment :data="filteredRaptTableData" :columns="raptColumns" />
      </template>
    </div>
  </div>
</template>

<script setup>
import { measurement, global, status } from '@/modules/pinia'
import { ref, watch, onBeforeMount, onBeforeUnmount, computed } from 'vue'
import { logDebug } from '@mp-se/espframework-ui-components'

const deviceType = ref(-1)
const deviceTypeOptions = ref([
  //  { label: 'Text', value: 0 },
])

const gravitymonDevice = ref('')
const tiltDevice = ref('')
const pressuremonDevice = ref('')
const chamberDevice = ref('')
const raptDevice = ref('')

const gravitymonDeviceOptions = ref([])
const tiltDeviceOptions = ref([])
const pressuremonDeviceOptions = ref([])
const chamberDeviceOptions = ref([])
const raptDeviceOptions = ref([])

// Column definitions for each device type
const gravitymonColumns = ref([
  { key: 'id', label: 'ID', method: 'getId' },
  { key: 'name', label: 'Name', method: 'getName' },
  { key: 'temp', label: 'Temp', method: 'getTemp', format: 'temperature' },
  { key: 'gravity', label: 'Gravity', method: 'getGravity' },
  { key: 'angle', label: 'Angle', method: 'getAngle' },
  { key: 'battery', label: 'Battery', method: 'getBattery', format: 'voltage' },
  { key: 'rssi', label: 'RSSI', method: 'getRssi' },
  { key: 'interval', label: 'Interval', method: 'getInterval', format: 'seconds' }
])

const raptColumns = ref([
  { key: 'id', label: 'ID', method: 'getId' },
  { key: 'temp', label: 'Temp', method: 'getTemp', format: 'temperature' },
  { key: 'gravity', label: 'Gravity', method: 'getGravity' },
  { key: 'angle', label: 'Angle', method: 'getAngle' },
  { key: 'battery', label: 'Battery', method: 'getBattery', format: 'voltage' },
  { key: 'txPower', label: 'Tx Power', method: 'getTxPower' },
  { key: 'rssi', label: 'RSSI', method: 'getRssi' }
])

const tiltColumns = ref([
  { key: 'color', label: 'Color', method: 'getColor' },
  { key: 'temp', label: 'Temp', method: 'getTemp', format: 'temperature' },
  { key: 'gravity', label: 'Gravity', method: 'getGravity' },
  { key: 'txPower', label: 'Tx Power', method: 'getTxPower' },
  { key: 'rssi', label: 'RSSI', method: 'getRssi' },
  { key: 'isPro', label: 'Is Pro', method: 'getIsPro', format: 'boolean' }
])

const pressuremonColumns = ref([
  { key: 'id', label: 'ID', method: 'getId' },
  { key: 'name', label: 'Name', method: 'getName' },
  { key: 'token', label: 'Token', method: 'getToken' },
  { key: 'temp', label: 'Temp', method: 'getTemp', format: 'temperature' },
  { key: 'pressure', label: 'Pressure', method: 'getPressure' },
  { key: 'pressure1', label: 'Pressure1', method: 'getPressure1' },
  { key: 'battery', label: 'Battery', method: 'getBattery', format: 'voltage' },
  { key: 'rssi', label: 'RSSI', method: 'getRssi' },
  { key: 'interval', label: 'Interval', method: 'getInterval', format: 'seconds' }
])

const chamberColumns = ref([
  { key: 'id', label: 'ID', method: 'getId' },
  { key: 'name', label: 'Name', method: 'getName' },
  { key: 'chamberTemp', label: 'Chamber Temp', method: 'getChamberTemp', format: 'temperature' },
  { key: 'beerTemp', label: 'Beer Temp', method: 'getBeerTemp', format: 'temperature' }
])

onBeforeMount(async () => {
  if (!status.sd_mounted) {
    logDebug('MeasurementView.onBeforeMount()', 'No SD card present, skipping data load')
    return
  }

  const success = await measurement.updateMeasurementFiles()
  if (success) {
    logDebug('MeasurementView.onBeforeMount()')

    await measurement.fetchAllMeasurementFiles()
    logDebug('MeasurementView.onBeforeMount()', 'fetched files')

    if (measurement.gravitymonData.length > 0) {
      deviceTypeOptions.value.push({ label: 'Gravitymon', value: 0 })

      const seenIds = new Set()
      gravitymonDeviceOptions.value = measurement.gravitymonData
        .filter((entry) => {
          if (seenIds.has(entry.getId())) return false
          seenIds.add(entry.getId())
          return true
        })
        .map((entry) => ({
          label: entry.getId() + ' - ' + entry.getName(),
          value: entry.getId()
        }))
      gravitymonDeviceOptions.value.push({ label: 'All devices', value: '' })
    }

    if (measurement.tiltData.length > 0) {
      deviceTypeOptions.value.push({ label: 'Tilt', value: 1 })

      const seenTiltIds = new Set()
      tiltDeviceOptions.value = measurement.tiltData
        .filter((entry) => {
          if (seenTiltIds.has(entry.getId())) return false
          seenTiltIds.add(entry.getId())
          return true
        })
        .map((entry) => ({
          label: entry.getId() + ' - ' + entry.getColor(),
          value: entry.getId()
        }))
      tiltDeviceOptions.value.push({ label: 'All devices', value: '' })
    }

    if (measurement.pressuremonData.length > 0) {
      deviceTypeOptions.value.push({ label: 'Pressuremon', value: 2 })

      const seenPressureIds = new Set()
      pressuremonDeviceOptions.value = measurement.pressuremonData
        .filter((entry) => {
          if (seenPressureIds.has(entry.getId())) return false
          seenPressureIds.add(entry.getId())
          return true
        })
        .map((entry) => ({
          label: entry.getId() + ' - ' + entry.getName(),
          value: entry.getId()
        }))
      pressuremonDeviceOptions.value.push({ label: 'All devices', value: '' })
    }

    if (measurement.chamberData.length > 0) {
      deviceTypeOptions.value.push({ label: 'Chamber Controller', value: 3 })

      const seenChamberIds = new Set()
      chamberDeviceOptions.value = measurement.chamberData
        .filter((entry) => {
          if (seenChamberIds.has(entry.getId())) return false
          seenChamberIds.add(entry.getId())
          return true
        })
        .map((entry) => ({
          label: entry.getId() + ' - ' + getNameForId(measurement.chamberData, entry.getId()),
          value: entry.getId()
        }))
      chamberDeviceOptions.value.push({ label: 'All devices', value: '' })
    }

    if (measurement.raptData.length > 0) {
      deviceTypeOptions.value.push({ label: 'Rapt', value: 4 })

      const seenIds = new Set()
      raptDeviceOptions.value = measurement.raptData
        .filter((entry) => {
          if (seenIds.has(entry.getId())) return false
          seenIds.add(entry.getId())
          return true
        })
        .map((entry) => ({
          label: entry.getId(),
          value: entry.getId()
        }))
      raptDeviceOptions.value.push({ label: 'All devices', value: '' })
    }
  } else {
    global.messageError = 'Failed to fetch list of measurement files'
  }
})

const getNameForId = (entries, id) => {
  const entry = entries.find(
    (e) => e.getId() === id && e.getName() !== '' && e.getName() !== e.getId()
  )
  return entry ? entry.getName() : ''
}

onBeforeUnmount(() => {})

// Watch for changes to deviceType
watch(deviceType, (newValue) => {
  logDebug('MeasurementView.watch()', 'Selected deviceType:', newValue)
})

// Filter per device
const filteredGravitymonTableData = computed(() => {
  if (!gravitymonDevice.value) return measurement.gravitymonData
  return measurement.gravitymonData.filter((entry) => entry.getId() === gravitymonDevice.value)
})
const filteredTiltData = computed(() => {
  if (!tiltDevice.value) return measurement.tiltData
  return measurement.tiltData.filter((entry) => entry.getId() === tiltDevice.value)
})
const filteredPressuremonTableData = computed(() => {
  if (!pressuremonDevice.value) return measurement.pressuremonData
  return measurement.pressuremonData.filter((entry) => entry.getId() === pressuremonDevice.value)
})
const filteredChamberTableData = computed(() => {
  if (!chamberDevice.value) return measurement.chamberData
  return measurement.chamberData.filter((entry) => entry.getId() === chamberDevice.value)
})
const filteredRaptTableData = computed(() => {
  if (!raptDevice.value) return measurement.raptData
  return measurement.raptData.filter((entry) => entry.getId() === raptDevice.value)
})

// Filter and extract data for graphs
const filteredGravitymonGravityData = computed(() => {
  if (!gravitymonDevice.value) return []

  var result = []

  filteredGravitymonTableData.value.forEach((entry) => {
    result.push({
      x: entry.created,
      y: parseFloat(new Number(entry.gravity).toFixed(4))
    })
  })

  return result
})

const filteredGravitymonTemperatureData = computed(() => {
  if (!gravitymonDevice.value) return []

  var result = []

  filteredGravitymonTableData.value.forEach((entry) => {
    result.push({
      x: entry.created,
      y: parseFloat(new Number(entry.temp).toFixed(2))
    })
  })

  return result
})

const filteredTiltGravityData = computed(() => {
  if (!tiltDevice.value) return []

  var result = []

  filteredTiltData.value.forEach((entry) => {
    result.push({
      x: entry.created,
      y: parseFloat(new Number(entry.gravity).toFixed(4))
    })
  })

  return result
})

const filteredTiltTemperatureData = computed(() => {
  if (!tiltDevice.value) return []

  var result = []

  filteredTiltData.value.forEach((entry) => {
    result.push({
      x: entry.created,
      y: parseFloat(new Number(entry.temp).toFixed(2))
    })
  })

  return result
})

const filteredPressuremonPressureData = computed(() => {
  if (!pressuremonDevice.value) return []

  var result = []

  filteredPressuremonTableData.value.forEach((entry) => {
    result.push({
      x: entry.created,
      y: parseFloat(new Number(entry.pressure).toFixed(4))
    })
  })

  return result
})

const filteredPressuremonTemperatureData = computed(() => {
  if (!pressuremonDevice.value) return []

  var result = []

  filteredPressuremonTableData.value.forEach((entry) => {
    result.push({
      x: entry.created,
      y: parseFloat(new Number(entry.temp).toFixed(2))
    })
  })

  return result
})

const filteredChamberChamberTemperatureData = computed(() => {
  if (!chamberDevice.value) return []

  var result = []

  filteredChamberTableData.value.forEach((entry) => {
    result.push({
      x: entry.created,
      y: parseFloat(new Number(entry.chamberTemp).toFixed(2))
    })
  })

  return result
})

const filteredChamberBeerTemperatureData = computed(() => {
  if (!chamberDevice.value) return []

  var result = []

  filteredChamberTableData.value.forEach((entry) => {
    result.push({
      x: entry.created,
      y: parseFloat(new Number(entry.beerTemp).toFixed(2))
    })
  })

  return result
})

const filteredRaptGravityData = computed(() => {
  if (!raptDevice.value) return []

  var result = []

  filteredRaptTableData.value.forEach((entry) => {
    result.push({
      x: entry.created,
      y: parseFloat(new Number(entry.gravity).toFixed(4))
    })
  })

  return result
})

const filteredRaptTemperatureData = computed(() => {
  if (!raptDevice.value) return []

  var result = []

  filteredRaptTableData.value.forEach((entry) => {
    result.push({
      x: entry.created,
      y: parseFloat(new Number(entry.temp).toFixed(2))
    })
  })

  return result
})
</script>
