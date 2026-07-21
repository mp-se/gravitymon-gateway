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
    <p class="h3">Device - Hardware</p>
    <hr />

    <form @submit.prevent="save" class="needs-validation" novalidate>
      <div class="row">
        <div class="col-md-4">
          <BsInputSwitch
            v-model="config.ble_enable"
            label="Enable Bluetooth"
            width=""
            :disabled="global.disabled"
          ></BsInputSwitch>
        </div>
        <div class="col-md-4">
          <BsInputRadio
            v-model="config.ble_active_scan"
            :options="bleScanOptions"
            label="Scan Mode"
            width=""
            :disabled="global.disabled || !config.ble_enable"
          ></BsInputRadio>
        </div>
        <div class="col-md-4">
          <BsInputNumber
            v-model="config.ble_scan_time"
            unit="s"
            label="Scanning Time"
            width="5"
            :disabled="global.disabled || !config.ble_enable"
          ></BsInputNumber>
        </div>
        <div class="col-md-12">
          <p>&nbsp;</p>
          <p class="fw-normal">Enable/Disable bluetooth requires a restart of the device.</p>
          <p class="fw-normal">Supporting the following GravityMon BLE transmission options:</p>
          <ul>
            <li>Gravitymon iBeacon (<b>Passive scanning</b>)</li>
            <li>Gravitymon EddyStone (<b>Active scanning, Require BLE 5.0, i.e. ESP32 S3</b>)</li>
            <li>Pressuremon iBeacon (<b>Passive scanning</b>)</li>
          </ul>
          <p class="fw-normal">If you want to use the Tilt options I recommend TiltBridge!</p>
        </div>

        <div class="col-md-12">
          <hr />
        </div>

        <div class="col-md-4">
          <BsSelect
            v-model="config.timezone"
            :options="timezoneOptions"
            label="Timezone"
            width=""
            :disabled="global.disabled"
          ></BsSelect>
        </div>

        <div class="col-md-4">
          <BsSelect
            v-model="config.display_layout_id"
            :options="displayLayoutOptions"
            label="Display Layout"
            width=""
            :disabled="global.disabled"
          ></BsSelect>
        </div>
      </div>
      <div class="row gy-2">
        <div class="col-md-12">
          <hr />
        </div>
        <div class="col-md-12">
          <button
            type="submit"
            class="btn btn-primary w-2"
            :disabled="global.disabled || !global.configChanged"
          >
            <span
              class="spinner-border spinner-border-sm"
              role="status"
              aria-hidden="true"
              :hidden="!global.disabled"
            ></span>
            &nbsp;Save</button
          >&nbsp;

          <button
            @click="restart()"
            type="button"
            class="btn btn-secondary"
            :disabled="global.disabled"
          >
            <span
              class="spinner-border spinner-border-sm"
              role="status"
              aria-hidden="true"
              :hidden="!global.disabled"
            ></span>
            &nbsp;Restart device
          </button>
        </div>
      </div>
    </form>
  </div>
</template>

<script setup>
import { ref } from 'vue'
import { validateCurrentForm } from '@mp-se/espframework-ui-components'
import { global, config } from '@/modules/pinia'

const bleScanOptions = ref([
  { label: 'Active', value: true },
  { label: 'Passive', value: false }
])

const displayLayoutOptions = ref([
  { label: 'Overview', value: 0 },
  { label: 'Device focused', value: 1 }
])

// List of timezones, https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

const timezoneOptions = ref([
  { label: 'America/Los Angeles', value: 'PST8PDT,M3.2.0,M11.1.0' },
  { label: 'America/Phoenix', value: 'MST7' },
  { label: 'America/Denver', value: 'MST7MDT,M3.2.0,M11.1.0' },
  { label: 'America/Chicago', value: 'CST6CDT,M3.2.0,M11.1.0' },
  { label: 'America/New York', value: 'EST5EDT,M3.2.0,M11.1.0' },
  { label: 'Europe/London', value: 'GMT0BST,M3.5.0/1,M10.5.0' },
  { label: 'Europe/Stockholm', value: 'CET-1CEST,M3.5.0,M10.5.0/3' },
  { label: 'Europe/Helsinki', value: 'EET-2EEST,M3.5.0/3,M10.5.0/4' },
  { label: 'Africa/Johannesburg', value: 'SAST-2' },
  { label: 'Asia/Singapore', value: '<+08>-8' },
  { label: 'Asia/Tokyo', value: 'JST-9' },
  { label: 'Australia/Perth', value: 'AWST-8' },
  { label: 'Australia/Sydney', value: 'AEST-10AEDT,M10.1.0,M4.1.0/3' },
  { label: 'Pacific/Auckland', value: 'NZST-12NZDT,M9.5.0,M4.1.0/3' },
  { label: 'Pacific/Honolulu', value: 'HST10' }
  //{ label: '', value: "" },
  //{ label: '', value: "" },
])

const restart = async () => {
  await config.restart()
}

const save = async () => {
  if (!validateCurrentForm()) return

  global.clearMessages()
  await config.saveAll()
}
</script>
