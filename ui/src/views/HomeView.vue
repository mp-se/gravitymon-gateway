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

    <div v-if="status" class="container overflow-hidden text-center">
      <div class="row gy-4">
        <template v-for="g in status.gravity_device" :key="g.device">
          <div class="col-md-4">
            <BsCard
              :header="'Gravity Device: ' + g.device"
              color="info"
              :title="
                g.name + ' (' + formatTime(g.update_time) + ' / ' + formatTime(g.push_time) + ')'
              "
            >
              <p class="text-center">
                Gravity: {{ formatGravity(g.gravity) }}
                {{ config.gravity_unit === 'G' ? ' SG' : ' P' }} Temperature:
                {{ formatTemp(g.temp) }} {{ config.temp_unit }}
              </p>

              <span class="badge bg-primary">{{ g.source }}</span
              >&nbsp;
              <span class="badge bg-primary">{{ g.type }}</span>
            </BsCard>
          </div>
        </template>

        <template v-for="p in status.pressure_device" :key="p.device">
          <div class="col-md-4">
            <BsCard
              :header="'Pressure Device: ' + p.device"
              color="info"
              :title="
                p.name + ' (' + formatTime(p.update_time) + ' / ' + formatTime(p.push_time) + ')'
              "
            >
              <p class="text-center">
                Pressure: {{ formatPressure(p.pressure) }}
                {{
                  config.pressure_unit === 'PSI'
                    ? ' psi'
                    : config.pressure_unit === 'kPa'
                      ? ' kPa'
                      : ' Bar'
                }}
                <template v-if="p.pressure1 !== undefined">
                  , Pressure1: {{ formatPressure(p.pressure1) }}
                  {{
                    config.pressure_unit === 'PSI'
                      ? ' psi'
                      : config.pressure_unit === 'kPa'
                        ? ' kPa'
                        : ' Bar'
                  }}
                </template>
                <template v-if="p.temp !== undefined">
                  , Temperature: {{ formatTemp(p.temp) }} {{ config.temp_unit }}
                </template>
              </p>

              <span class="badge bg-primary">{{ p.source }}</span
              >&nbsp;
              <span class="badge bg-primary">{{ p.type }}</span>
            </BsCard>
          </div>
        </template>

        <template v-for="t in status.temperature_device" :key="t.device">
          <div class="col-md-4">
            <BsCard
              header="Temperature Device"
              color="secondary"
              :title="t.name === '' ? t.device : t.name + ' (' + formatTime(t.update_time) + ')'"
            >
              <p class="text-center">
                <template v-if="t.chamber_temp !== undefined">
                  Chamber: {{ formatTemp(t.chamber_temp) }} {{ config.temp_unit }}
                </template>
                <template v-if="t.beer_temp !== undefined">
                  , Beer: {{ formatTemp(t.beer_temp) }} {{ config.temp_unit }}
                </template>
              </p>

              <span class="badge bg-primary">{{ t.source }}</span
              >&nbsp;
              <span class="badge bg-primary">{{ t.type }}</span>
            </BsCard>
          </div>
        </template>

        <div class="col-md-4">
          <BsCard header="Device" title="WIFI">
            <p class="text-center">{{ status.rssi }} dBm - {{ status.wifi_ssid }}</p>
          </BsCard>
        </div>

        <div class="col-md-4">
          <BsCard header="Device" title="IP Address">
            <p class="text-center">
              {{ status.ip }}
            </p>
          </BsCard>
        </div>

        <div class="col-md-4">
          <BsCard header="Device" title="Memory">
            <p class="text-center">
              Free: {{ status.free_heap }} kb, Total: {{ status.total_heap }} kb
            </p>
          </BsCard>
        </div>

        <div class="col-md-4">
          <BsCard header="Device" title="Software version">
            <p class="text-center">
              Firmware: {{ global.app_ver }} ({{ global.app_build }}) UI: {{ global.uiVersion }} ({{
                global.uiBuild
              }})
            </p>
          </BsCard>
        </div>

        <div class="col-md-4">
          <BsCard header="Device" title="Platform">
            Platform: <span class="badge bg-secondary">{{ global.platform }}</span> Board:
            <span class="badge bg-secondary">{{ global.board }}</span>
          </BsCard>
        </div>

        <div class="col-md-4">
          <BsCard header="Device" title="Device ID">
            <p class="text-center">{{ status.id }}</p>
          </BsCard>
        </div>

        <div class="col-md-4">
          <BsCard header="Device" title="Uptime">
            <p class="text-center">
              {{ status.uptime_days }} days {{ status.uptime_hours }} hours
              {{ status.uptime_minutes }} minutes {{ status.uptime_seconds }} seconds
            </p>
          </BsCard>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { global, status, config } from '@/modules/pinia'
import { ref, onBeforeMount, onBeforeUnmount } from 'vue'
import { psiToBar, psiToKPa, tempToF, gravityToPlato } from '@mp-se/espframework-ui-components'

const polling = ref(null)

function formatTime(t) {
  if (t < 60)
    // less than 1 min
    return new Number(t).toFixed(0) + 's'

  if (t < 60 * 60)
    // less than 1 hour
    return new Number(t / 60).toFixed(0) + 'm'

  if (t < 60 * 60 * 24)
    // less than 1 day
    return new Number(t / (60 * 60)).toFixed(0) + 'h'

  return new Number(t / (60 * 60 * 24)).toFixed(0) + 'd'
}

function formatGravity(g) {
  return config.gravity_unit === 'G'
    ? new Number(g).toFixed(3)
    : gravityToPlato(new Number(g).toFixed(1))
}

function formatPressure(p) {
  return config.pressure_unit === 'PSI'
    ? new Number(p).toFixed(3)
    : config.pressure_unit === 'kPa'
      ? new Number(psiToKPa(p)).toFixed(2)
      : new Number(psiToBar(p)).toFixed(2)
}

function formatTemp(t) {
  return config.temp_unit === 'C' ? new Number(t).toFixed(2) : tempToF(new Number(t).toFixed(1))
}

async function refresh() {
  await status.load()
}

onBeforeMount(() => {
  refresh()
  polling.value = setInterval(refresh, 10000)
})

onBeforeUnmount(() => {
  clearInterval(polling.value)
})
</script>

<style></style>
