/*
 * Pressuremon - Dual License
 *
 * Open Source Use  ->  GNU General Public License v3.0 (GPL v3)
 * Commercial Use   ->  Commercial License Agreement
 *
 * Copyright (c) 2024-2026 Magnus
 *
 * For full license terms and conditions, see LICENSE and LICENSE_COMMERCIAL files.
 * Source: https://github.com/mp-se/pressuremon and https://github.com/mp-se/pressuremon-ui
 */
import { ref } from 'vue'

export const pressureHttpPostFormatOptions = ref([
  {
    label: 'Pressuremon (Single)',
    value:
      '%7B%20%22name%22%20%3A%20%22%24%7Bmdns%7D%22%2C%20%22ID%22%3A%20%22%24%7Bid%7D%22%2C%20%22token%22%20%3A%20%22%24%7Btoken%7D%22%2C%20%22interval%22%3A%20%24%7Bsleep-interval%7D%2C%20%22temperature%22%3A%20%24%7Btemp%7D%2C%20%22temp_units%22%3A%20%22%24%7Btemp-unit%7D%22%2C%20%22pressure%22%3A%20%24%7Bpressure%7D%2C%20%22pressure_unit%22%3A%20%22%24%7Bpressure-unit%7D%22%2C%20%22battery%22%3A%20%24%7Bbattery%7D%2C%20%22RSSI%22%3A%20%24%7Brssi%7D%7D'
  },
  {
    label: 'Pressuremon (Dual)',
    value:
      '%7B%20%22name%22%20%3A%20%22%24%7Bmdns%7D%22%2C%20%22ID%22%3A%20%22%24%7Bid%7D%22%2C%20%22token%22%20%3A%20%22%24%7Btoken%7D%22%2C%20%22interval%22%3A%20%24%7Bsleep-interval%7D%2C%20%22temperature%22%3A%20%24%7Btemp%7D%2C%20%22temp_units%22%3A%20%22%24%7Btemp-unit%7D%22%2C%20%22pressure1%22%3A%20%24%7Bpressure1%7D%2C%20%22pressure2%22%3A%20%24%7Bpressure2%7D%2C%20%22pressure_unit%22%3A%20%22%24%7Bpressure-unit%7D%22%2C%20%22battery%22%3A%20%24%7Bbattery%7D%2C%20%22RSSI%22%3A%20%24%7Brssi%7D%7D'
  },
  {
    label: 'Brewblox (Single)',
    value:
      '%7B%20%20%20%22name%22%3A%20%22%24%7Bmdns%7D%22%2C%20%20%20%22temp%22%3A%20%24%7Btemp%7D%2C%20%20%20%22aux_temp%22%3A%200%2C%20%20%20%22ext_temp%22%3A%200%2C%20%20%20%22temp_unit%22%3A%20%22%24%7Btemp-unit%7D%22%2C%20%20%20%22pressure%22%3A%20%24%7Bpressure%7D%2C%20%20%20%22pressure_unit%22%3A%20%22%24%7Bpressure-unit%7D%22%2C%20%20%20%22battery%22%3A%20%24%7Bbattery%7D%7D'
  },
  {
    label: 'Brewblox (Dual)',
    value:
      '%7B%20%20%20%22name%22%3A%20%22%24%7Bmdns%7D%22%2C%20%20%20%22temp%22%3A%20%24%7Btemp%7D%2C%20%20%20%22aux_temp%22%3A%200%2C%20%20%20%22ext_temp%22%3A%200%2C%20%20%20%22temp_unit%22%3A%20%22%24%7Btemp-unit%7D%22%2C%20%20%20%22pressure%22%3A%20%24%7Bpressure1%7D%2C%20%20%20%22aux_pressure%22%3A%20%24%7Bpressure2%7D%2C%20%20%20%22pressure_unit%22%3A%20%22%24%7Bpressure-unit%7D%22%2C%20%20%20%22battery%22%3A%20%24%7Bbattery%7D%7D'
  }
])

export const pressureHttpGetFormatOptions = ref([
  {
    label: 'Pressuremon (Single)',
    value:
      '%3Fname%3D%24%7Bmdns%7D%26id%3D%24%7Bid%7D%26token%3D%24%7Btoken2%7D%26interval%3D%24%7Bsleep-interval%7D%26temperature%3D%24%7Btemp%7D%26temp-units%3D%24%7Btemp-unit%7D%26pressure%3D%24%7Bpressure%7D%26pressure-unit%3D%24%7Bpressure-unit%7D%26battery%3D%24%7Bbattery%7D%26rssi%3D%24%7Brssi%7D'
  },
  {
    label: 'Pressuremon (Dual)',
    value:
      '%3Fname%3D%24%7Bmdns%7D%26id%3D%24%7Bid%7D%26token%3D%24%7Btoken2%7D%26interval%3D%24%7Bsleep-interval%7D%26temperature%3D%24%7Btemp%7D%26temp-units%3D%24%7Btemp-unit%7D%26pressure1%3D%24%7Bpressure1%7D%26pressure2%3D%24%7Bpressure2%7D%26pressure-unit%3D%24%7Bpressure-unit%7D%26battery%3D%24%7Bbattery%7D%26rssi%3D%24%7Brssi%7D'
  }
])

export const pressureInfluxdb2FormatOptions = ref([
  {
    label: 'Pressuremon (Single)',
    value:
      'measurement%2Chost%3D%24%7Bmdns%7D%2Cdevice%3D%24%7Bid%7D%2Ctemp%2Dformat%3D%24%7Btemp%2Dunit%7D%2Cpressure%2Dformat%3D%24%7Bpressure%2Dunit%7D%20pressure%3D%24%7Bpressure%7D%2Ctemp%3D%24%7Btemp%7D%2Cbattery%3D%24%7Bbattery%7D%2Crssi%3D%24%7Brssi%7D%0A'
  },
  {
    label: 'Pressuremon (Dual)',
    value:
      'measurement%2Chost%3D%24%7Bmdns%7D%2Cdevice%3D%24%7Bid%7D%2Ctemp%2Dformat%3D%24%7Btemp%2Dunit%7D%2Cpressure%2Dformat%3D%24%7Bpressure%2Dunit%7D%20pressure1%3D%24%7Bpressure1%7D%2Cpressure2%3D%24%7Bpressure2%7D%2Ctemp%3D%24%7Btemp%7D%2Cbattery%3D%24%7Bbattery%7D%2Crssi%3D%24%7Brssi%7D%0A'
  }
])

export const pressureMqttFormatOptions = ref([
  {
    label: 'Pressuremon / Brewblox',
    value:
      'pressuremon%2F%24%7Bmdns%7D%2Fpressure%3A%24%7Bpressure%7D%7Cpressuremon%2F%24%7Bmdns%7D%2Ftemperature%3A%24%7Btemp%7D%7Cpressuremon%2F%24%7Bmdns%7D%2Ftemp_units%3A%24%7Btemp-unit%7D%7Cpressuremon%2F%24%7Bmdns%7D%2Fpress_units%3A%24%7Bpressure-unit%7D%7Cpressuremon%2F%24%7Bmdns%7D%2Fbattery%3A%24%7Bbattery%7D%7Cpressuremon%2F%24%7Bmdns%7D%2FRSSI%3A%24%7Brssi%7D%7C'
  },
  {
    label: 'HomeAssistant',
    value:
      'pressuremon%2F%24%7Bmdns%7D%2Fpressure%3A%24%7Bpressure%7D%7Cpressuremon%2F%24%7Bmdns%7D%2Ftemperature%3A%24%7Btemp%7D%7Cpressuremon%2F%24%7Bmdns%7D%2Frssi%3A%24%7Brssi%7D%7Cpressuremon%2F%24%7Bmdns%7D%2Fbattery%3A%24%7Bbattery%7D%7C'
  }
])
