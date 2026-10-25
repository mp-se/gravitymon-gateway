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
    <p class="h3">Links and device logs</p>
    <hr />
    <div class="row">
      <p>
        If you need support, want to discuss the software or request any new features you can do
        that on github.com or homebrewtalk.com.
      </p>
    </div>
    <div class="row">
      <div class="col-md-4">
        <a
          class="link-primary link-offset-2 link-underline-opacity-25 link-underline-opacity-100-hover"
          href="https://github.com/mp-se/gravitymon"
          target="_blank"
          >Report issues on github.com</a
        >
      </div>
      <div class="col-md-4">
        <a
          class="link-primary link-offset-2 link-underline-opacity-25 link-underline-opacity-100-hover"
          href="https://www.homebrewtalk.com/"
          target="_blank"
          >Discuss on homebrewtalk.com</a
        >
      </div>
      <div class="col-md-4">
        <a
          class="link-primary link-offset-2 link-underline-opacity-25 link-underline-opacity-100-hover"
          href="https://www.gravitymon.com/"
          target="_blank"
          >Read docs on gravitymon.com</a
        >
      </div>
    </div>

    <hr />
    <div class="row">
      <div class="col">
        <p>
          Platform: <span class="badge bg-secondary">{{ global.platform }}</span> Firmware:
          <span class="badge bg-secondary">{{ global.app_ver }} ({{ global.app_build }})</span> User
          interface:
          <span class="badge bg-secondary">{{ global.uiVersion }} ({{ global.uiBuild }})</span>
        </p>
      </div>
    </div>
    <hr />

    <div class="row">
      <div class="col-md-12">
        <button @click="viewLogs" type="button" class="btn btn-primary" :disabled="global.disabled">
          <span
            class="spinner-border spinner-border-sm"
            role="status"
            aria-hidden="true"
            :hidden="!global.disabled"
          ></span>
          &nbsp;View device logs</button
        >&nbsp;

        <button
          @click="removeLogs"
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
          &nbsp;Erase device logs</button
        >&nbsp;

        <button
          @click="fetchMdns"
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
          &nbsp;Fetch mDNS data
        </button>
      </div>
    </div>

    <div class="row">
      <div class="col">
        <p></p>
      </div>
    </div>
    <div class="row">
      <div class="col">
        <pre>{{ logData }}</pre>
      </div>
      <div class="form-text">Starts with the latest log entry first.</div>
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue'
import { global } from '@/modules/pinia'
import { logDebug } from '@mp-se/espframework-ui-components'
import { sharedHttpClient as http } from '@mp-se/espframework-ui-components'

const logData = ref('')

async function fetchLog(file) {
  const data = { command: 'get', file: file }
  try {
    const res = await http.filesystemRequest(data)
    if (res && res.success) {
      logDebug('SupportView.fetchLog()', 'Fetching ' + file + ' completed')
      const list = res.text.split('\n')
      list.forEach(function (item) {
        if (item.length) logData.value = item + '\n' + logData.value
      })
      return true
    }
  } catch (err) {
    logDebug('SupportView.fetchLog()', 'Error fetching ' + file, err)
  }
  return false
}

async function fetchMdns() {
  try {
    const res = await http.getJson('api/mdns')
    logDebug('SupportView.fetchMdns()', res)
    if (res) {
      logDebug('SupportView.fetchMdns()', 'Fetching mdns data completed')

      // Convert last_seen timestamp to formatted date string
      if (res.mdns && Array.isArray(res.mdns)) {
        res.mdns.forEach((item) => {
          if (item.last_seen) {
            const date = new Date(item.last_seen * 1000)
            const year = date.getFullYear()
            const month = String(date.getMonth() + 1).padStart(2, '0')
            const day = String(date.getDate()).padStart(2, '0')
            const hours = String(date.getHours()).padStart(2, '0')
            const minutes = String(date.getMinutes()).padStart(2, '0')
            const seconds = String(date.getSeconds()).padStart(2, '0')
            item.last_seen = `${year}-${month}-${day} ${hours}:${minutes}:${seconds}`
          }
        })
      }

      logData.value = JSON.stringify(res.mdns, null, 2)
      return true
    }
  } catch (err) {
    logDebug('SupportView.fetchMdns()', 'Error fetching mdns data', err)
  }
  return false
}

async function removeLog(file) {
  const data = { command: 'del', file: file }
  try {
    const res = await http.filesystemRequest(data)
    return res && res.success
  } catch (err) {
    logDebug('SupportView.removeLog()', 'Error deleting ' + file, err)
    return false
  }
}

function viewLogs() {
  global.clearMessages()
  global.disabled = true
  logData.value = ''
  ;(async () => {
    await fetchLog('/error2.log')
    await fetchLog('/error.log')
    global.disabled = false
  })()
}

function removeLogs() {
  global.clearMessages()
  global.disabled = true
  logData.value = ''
  ;(async () => {
    await removeLog('/error2.log')
    await removeLog('/error.log')
    global.messageSuccess = 'Requested logs to be deleted'
    global.disabled = false
  })()
}
</script>
