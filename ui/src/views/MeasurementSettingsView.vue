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
    <p class="h2">Measurement - Settings</p>
    <hr />

    <form @submit.prevent="saveSettings" class="needs-validation" novalidate>
      <div class="row">
        <div class="col-md-4">
          <BsInputNumber
            v-model="config.sd_log_files"
            label="Max number of log files on SD card"
            min="2"
            max="100"
            step="1"
            width="5"
            help="Max number of log files to keep on the SD card"
            :disabled="global.disabled || !global.feature.sd"
          ></BsInputNumber>
        </div>
        <div class="col-md-4">
          <BsInputNumber
            v-model="fixedSize"
            unit="kb"
            label="Size of each log file on SD card"
            step="1"
            width="5"
            help="Size of each logfile on the SD card"
            :disabled="true"
          ></BsInputNumber>
        </div>
        <div class="col-md-4">
          <BsSelect
            v-model="config.sd_log_min_time"
            :options="timeOptions"
            label="Minimum time between logs to SD card (BLE)"
            width="5"
            help="Minimum time to log BLE data to sd card per device."
            :disabled="global.disabled || !global.feature.sd"
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
        </div>
      </div>
    </form>
  </div>
</template>

<script setup>
import { ref } from 'vue'
import { BsSelect, validateCurrentForm } from '@mp-se/espframework-ui-components'
import { global, config } from '@/modules/pinia'

const fixedSize = ref(16)

const timeOptions = ref([
  { label: '1 min', value: 1 },
  { label: '3 min', value: 3 },
  { label: '5 min', value: 5 },
  { label: '10 min', value: 10 },
  { label: '15 min', value: 15 },
  { label: '30 min', value: 30 },
  { label: '45 min', value: 45 },
  { label: '60 min', value: 60 }
])

const saveSettings = async () => {
  if (!validateCurrentForm()) return

  await config.saveAll()
}
</script>
