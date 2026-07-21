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
    <p class="h3">Device - WIFI AP</p>
    <hr />

    <form @submit.prevent="save" class="needs-validation" novalidate>
      <div class="row">
        <div class="col-md-12">
          <p>
            Wifi Direct allows a device to directly connect with the gateway and bypass the access
            points. This allows for direct link between the two. Can be useful to ensure that the
            closest access point is used. The gateway will broadcast this SSID for use by other
            devices.
          </p>
        </div>

        <div class="col-md-6">
          <BsInputText
            v-model="config.wifi_direct_ssid"
            label="Direct SSID"
            help="Enter the SSID for the wifi direct functionallity"
            :disabled="global.disabled"
          />
        </div>

        <div class="col-md-6">
          <BsInputText
            v-model="config.wifi_direct_pass"
            type="password"
            maxlength="50"
            label="Direct Password"
            help="Enter password for the wifi direct network"
            :disabled="global.disabled"
          ></BsInputText>
        </div>

        <div class="col-md-3">
          <p></p>
          <button
            @click="generate()"
            type="button"
            class="btn btn-secondary w-2"
            :disabled="global.disabled"
          >
            &nbsp;Generate
          </button>
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
import { global, config, status } from '@/modules/pinia'
import { logDebug, validateCurrentForm } from '@mp-se/espframework-ui-components'

function generate() {
  logDebug('DeviceWifiView:generate()')
  config.wifi_direct_ssid = 'gw-' + status.id
  let strings = window.crypto.getRandomValues(new BigUint64Array(2))
  config.wifi_direct_pass = (
    strings[0].toString(36) + strings[1].toString(36).toUpperCase()
  ).substring(0, 10)
}

const save = () => {
  if (!validateCurrentForm()) return

  config.saveAll()
  global.messageInfo = 'If WIFI settings are changed, restart the device so they can take effect!'
}

const restart = async () => {
  await config.restart()
}
</script>
