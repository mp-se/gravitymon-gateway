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
    <p class="h3">Backup & Restore</p>
    <hr />

    <div class="row">
      <div class="col-md-12">
        <p>Create a backup of the device configuration and store this in a textfile</p>
      </div>

      <div class="col-md-12">
        <button
          @click="backup"
          type="button"
          class="btn btn-primary w-2"
          data-bs-toggle="tooltip"
          :disabled="global.disabled"
        >
          Create backup
        </button>
      </div>

      <div class="col-md-12">
        <hr />
      </div>

      <div class="col-md-12">
        <p>Restore a previous backup of the device configuration by uploading it.</p>
      </div>
    </div>

    <div class="row">
      <form @submit.prevent="restore">
        <div class="col-md-12">
          <BsFileUpload
            name="upload"
            id="upload"
            label="Select backup file"
            accept=".txt"
            :disabled="global.disabled"
            @change="onFileChange"
          >
          </BsFileUpload>
        </div>

        <div class="col-md-3">
          <p></p>
          <button
            type="submit"
            class="btn btn-primary"
            value="upload"
            data-bs-toggle="tooltip"
            title="Upload the configuration to the device"
            :disabled="global.disabled || !fileSelected"
          >
            <span
              class="spinner-border spinner-border-sm"
              role="status"
              aria-hidden="true"
              v-show="global.disabled"
            ></span>
            &nbsp;Restore
          </button>
        </div>

        <div v-if="progress > 0" class="col-md-12">
          <p></p>
          <BsProgress :progress="progress"></BsProgress>
        </div>
      </form>
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue'
import { global, config, getConfigChanges } from '@/modules/pinia'
import { logDebug, logError } from '@mp-se/espframework-ui-components'

const progress = ref(0)
const fileSelected = ref(false)

function onFileChange(event) {
  const files = event.target.files
  fileSelected.value = files && files.length > 0
}

function backup() {
  let backup = {
    meta: {
      version: '0.8',
      software: 'Gravitymon-Gateway',
      created: new Date().toISOString().slice(0, 10)
    },
    config: JSON.parse(config.toJson())
  }

  logDebug('BackupView.backup()', backup)

  // Gravity formats
  backup.config.http_post_format_gravity = encodeURIComponent(
    backup.config.http_post_format_gravity
  )
  backup.config.http_post2_format_gravity = encodeURIComponent(
    backup.config.http_post2_format_gravity
  )
  backup.config.http_get_format_gravity = encodeURIComponent(backup.config.http_get_format_gravity)
  backup.config.influxdb2_format_gravity = encodeURIComponent(
    backup.config.influxdb2_format_gravity
  )
  backup.config.mqtt_format_gravity = encodeURIComponent(backup.config.mqtt_format_gravity)

  // Pressure formats
  backup.config.http_post_format_pressure = encodeURIComponent(
    backup.config.http_post_format_pressure
  )
  backup.config.http_post2_format_pressure = encodeURIComponent(
    backup.config.http_post2_format_pressure
  )
  backup.config.http_get_format_pressure = encodeURIComponent(
    backup.config.http_get_format_pressure
  )
  backup.config.influxdb2_format_pressure = encodeURIComponent(
    backup.config.influxdb2_format_pressure
  )
  backup.config.mqtt_format_pressure = encodeURIComponent(backup.config.mqtt_format_pressure)

  const s = JSON.stringify(backup, null, 2)
  const name = config.mdns + '.txt'
  download(s, 'text/plain', name)
  global.messageSuccess = 'Backup file created and downloaded as: ' + name
}

async function restore() {
  const fileElement = document.getElementById('upload')

  // Validate file element exists
  if (!fileElement) {
    global.messageError = 'Upload element not found'
    return
  }

  if (fileElement.files.length === 0) {
    global.messageError = 'You need to select one file to restore configuration from'
    return
  }

  global.disabled = true
  logDebug('BackupView.restore()', 'Selected file: ' + fileElement.files[0].name)

  const reader = new FileReader()
  const file = fileElement.files[0]

  reader.addEventListener('load', async function (e) {
    let text = e.target.result
    try {
      const data = JSON.parse(text)
      if (data.meta.software === 'Gravitymon-Gateway' && data.meta.version === '0.8') {
        await doRestore(data.config)
      } else {
        global.messageError = 'Unknown format, unable to process'
      }
    } catch (error) {
      logError('BackupView.restore()', error)
      global.messageError = 'Unable to parse configuration file for Gravitymon-Gateway.'
    } finally {
      global.disabled = false
      // Reset file selection after operation
      fileSelected.value = false
      fileElement.value = ''
    }
  })

  reader.addEventListener('error', () => {
    logError('BackupView.restore()', 'File reading failed')
    global.messageError = 'Failed to read the backup file'
    global.disabled = false
    // Reset file selection after error
    fileSelected.value = false
    fileElement.value = ''
  })

  reader.readAsText(file)
}

function download(content, mimeType, filename) {
  const a = document.createElement('a')
  const blob = new Blob([content], { type: mimeType })
  const url = URL.createObjectURL(blob)
  a.setAttribute('href', url)
  a.setAttribute('download', filename)
  a.click()

  // Clean up the object URL to prevent memory leaks
  setTimeout(() => URL.revokeObjectURL(url), 1000)
}

async function doRestore(json) {
  for (const k in json) {
    if (k.endsWith('_format_gravity') || k.endsWith('_format_pressure')) {
      config[k] = decodeURIComponent(json[k])
    } else {
      config[k] = json[k]
    }
  }

  getConfigChanges()
  await config.saveAll()
}
</script>
