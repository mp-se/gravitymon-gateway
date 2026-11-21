<!--
  GravityMon
  Copyright (c) 2021-2026 Magnus

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Alternatively, this software may be used under the terms of a
  commercial license. See LICENSE_COMMERCIAL for details.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
-->
<template>
  <div class="container">
    <p></p>
    <p class="h3">Firmware Upload</p>
    <hr />

    <div class="row">
      <form @submit.prevent="upload">
        <div style="col-md-12">
          <p>
            Selet the firmware file that matches your device. Platform:
            <span class="badge bg-secondary">{{ global.platform }}</span>
            <template v-if="global.app_ver && global.app_build"
              >, Version: <span class="badge bg-secondary">{{ global.app_ver }}</span> ({{
                global.app_build
              }})
            </template>
            <template v-if="global.hardware"
              >, Hardware: <span class="badge bg-secondary">{{ global.hardware }}</span></template
            ><template v-if="global.firmware_file"
              >, Filename:
              <span class="badge bg-secondary">{{ global.firmware_file }}</span></template
            >
          </p>
        </div>

        <div class="col-md-12">
          <BsFileUpload
            name="upload"
            id="upload"
            label="Select firmware file"
            accept=".bin"
            help="Choose the firmware file (.bin) that will be used to update the device. The upload button will be enabled once a file is selected."
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
            id="upload-btn"
            value="upload"
            data-bs-toggle="tooltip"
            :title="
              !hasFileSelected
                ? 'Please select a firmware file first'
                : 'Update the device with the selected firmware'
            "
            :disabled="global.disabled || !hasFileSelected"
          >
            <span
              class="spinner-border spinner-border-sm"
              role="status"
              aria-hidden="true"
              v-show="global.disabled"
            ></span>
            &nbsp;Flash firmware
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
import { global } from '@/modules/pinia'
import { checkFirmwareFile } from '@/modules/firmwareCheck'
import { sharedHttpClient as http } from '@mp-se/espframework-ui-components'
import { logDebug, logError } from '@mp-se/espframework-ui-components'

// Error messages for the checks in checkFirmwareFile()
const firmwareErrors = {
  not_firmware: () => 'The selected file is not a valid firmware image',
  wrong_chip: () =>
    `The selected firmware is built for another chip, this device is an ${global.platform}`,
  wrong_name: () =>
    `The selected file does not have the expected name ${global.firmware_file}. Use the firmware file for this device and make sure it has not been renamed, for example by the browser when downloaded twice`
}

const progress = ref(0)
const hasFileSelected = ref(false)

const onFileChange = (event) => {
  const fileElement = event.target
  hasFileSelected.value = fileElement.files && fileElement.files.length > 0
}

async function upload() {
  const fileElement = document.getElementById('upload')

  // Start from a clean slate, a message from a previous attempt is outdated
  global.messageError = ''
  global.messageSuccess = ''

  if (fileElement.files.length === 0) {
    global.messageError = 'You need to select one file with firmware to upload'
  } else {
    // Reject a wrong file now instead of after the whole file has been sent
    const check = await checkFirmwareFile(
      fileElement.files[0],
      global.platform,
      global.firmware_file
    )
    if (!check.ok) {
      global.messageError = firmwareErrors[check.error]()
      return
    }

    global.disabled = true
    logDebug('FirmwareView.upload()', 'Selected file: ' + fileElement.files[0].name)

    progress.value = 0

    try {
      const res = await http.uploadFile('api/firmware', fileElement.files[0], {
        timeoutMs: 180000,
        onProgress: (percent) => {
          logDebug('FirmwareView.upload()', 'Upload progress: ' + Math.round(percent) + '%')
          progress.value = Math.round(percent)
        }
      })
      progress.value = 100
      if (res.success) {
        global.messageSuccess =
          'File upload completed, waiting for device to restart before doing refresh!'
        global.messageError = ''

        // Use a more reliable redirect with timeout cleanup
        const redirectTimeout = setTimeout(() => {
          try {
            location.href = location.href.replace('/other/firmware', '')
          } catch (error) {
            logError('FirmwareView.redirect()', error)
            // Fallback redirect
            window.location.reload()
          }
        }, 10000)

        // Clean up timeout on page unload
        window.addEventListener(
          'beforeunload',
          () => {
            clearTimeout(redirectTimeout)
          },
          { once: true }
        )
      } else {
        global.messageError = `Upload failed: ${res.status}`
      }
    } catch (err) {
      global.messageError = `Upload error: ${err.message || err}`
    } finally {
      global.disabled = false
    }
  }
}
</script>
