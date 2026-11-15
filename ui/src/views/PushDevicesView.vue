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
    <p class="h3">Push - BLE devices</p>
    <hr />

    <p class="text-muted">
      Map each BLE device ID to a name and a device token (for example a BrewGraph device token).
      The token is used instead of the global token when data from that device is forwarded.
    </p>

    <form @submit.prevent="save" class="needs-validation" novalidate>
      <div class="row gy-2">
        <div class="col-md-12">
          <h5>Configured devices</h5>
        </div>
        <div class="col-md-12" v-if="config.ble_devices.length === 0">
          <p class="text-muted">No devices configured.</p>
        </div>
        <template v-for="(d, index) in config.ble_devices" :key="d.id">
          <div class="col-md-2">
            <BsInputReadonly v-model="d.id" label="Device ID" :disabled="global.disabled" />
          </div>
          <div class="col-md-4">
            <BsInputText
              v-model="d.name"
              maxlength="32"
              label="Name"
              help="Name sent to the target for this device"
              :disabled="global.disabled"
            />
          </div>
          <div class="col-md-5">
            <BsInputText
              v-model="d.token"
              type="password"
              maxlength="128"
              label="Token"
              help="Device token used for this device"
              :disabled="global.disabled"
            />
          </div>
          <div class="col-md-1 d-flex align-items-center">
            <button
              type="button"
              class="btn btn-outline-danger"
              title="Remove device"
              :disabled="global.disabled"
              @click="removeDevice(index)"
            >
              Remove
            </button>
          </div>
        </template>
      </div>

      <div class="row gy-2 mt-2">
        <div class="col-md-12">
          <hr />
          <h5>Detected BLE devices</h5>
        </div>
        <div class="col-md-12" v-if="detectedDevices.length === 0">
          <p class="text-muted">No new BLE devices detected.</p>
        </div>
        <div class="col-md-12" v-else>
          <template v-for="d in detectedDevices" :key="d.device">
            <button
              type="button"
              class="btn btn-outline-primary me-2 mb-2"
              :disabled="global.disabled"
              @click="addDevice(d.device, d.name)"
            >
              Add {{ d.device }} ({{ d.type }})
            </button>
          </template>
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
              v-show="global.disabled"
            ></span>
            &nbsp;Save
          </button>
        </div>
      </div>
    </form>
  </div>
</template>

<script setup>
import { computed, onBeforeMount } from 'vue'
import { validateCurrentForm } from '@mp-se/espframework-ui-components'
import { global, config, status } from '@/modules/pinia'

const isBle = (d) => d.source === 'BLE Beacon' || d.source === 'BLE Eddystone'
const isPushable = (d) => d.type === 'Gravitymon' || d.type === 'Pressuremon' || d.type === 'RAPT'

const detectedDevices = computed(() => {
  const known = new Set(config.ble_devices.map((d) => d.id))
  return [...(status.gravity_device || []), ...(status.pressure_device || [])].filter(
    (d) => isBle(d) && isPushable(d) && d.device && !known.has(d.device)
  )
})

const addDevice = (id, name) => {
  id = id.trim()
  if (!id || config.ble_devices.some((d) => d.id === id)) return
  config.ble_devices.push({ id, name: name || '', token: '' })
}

const removeDevice = (index) => {
  config.ble_devices.splice(index, 1)
}

const save = async () => {
  if (!validateCurrentForm()) return

  await config.saveAll()
}

onBeforeMount(() => status.load())
</script>
