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
    <p class="h3">Push - Settings</p>
    <hr />

    <form @submit.prevent="save" class="needs-validation" novalidate>
      <div class="row">
        <div class="col-md-6">
          <BsInputText
            v-model="config.token"
            maxlength="50"
            label="Token"
            help="Token can be used in the format template as a variable, some services use this for authentication"
            :disabled="global.disabled"
          />
        </div>
        <div class="col-md-6">
          <BsInputNumber
            v-model="config.push_timeout"
            label="Push timeout"
            unit="s"
            min="10"
            max="60"
            step="1"
            width="5"
            help="The number of seconds that the device will wait until a remote service accepts the connection"
            :disabled="global.disabled"
          />
        </div>
        <div class="col-md-6">
          <BsInputNumber
            v-model="config.push_resend_time"
            label="Push minimum resend time"
            unit="s"
            min="10"
            max="1800"
            step="1"
            width="5"
            help="The number of seconds before a value can be resent to a push target"
            :disabled="global.disabled"
          />
        </div>
      </div>

      <div class="row gy-2">
        <div class="col-md-12">
          <hr />
        </div>
        <div class="col-md-3">
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
            &nbsp;Save
          </button>
        </div>
      </div>
    </form>
  </div>
</template>

<script setup>
import { validateCurrentForm } from '@mp-se/espframework-ui-components'
import { global, config } from '@/modules/pinia'

const save = async () => {
  if (!validateCurrentForm()) return

  await config.saveAll()
}
</script>
