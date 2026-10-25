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
  <div class="table-responsive shadow-sm">
    <table class="table table-sm table-bordered table-hover">
      <thead class="table-primary sticky-top">
        <tr>
          <th scope="col" class="text-center">#</th>
          <th scope="col" class="text-nowrap">Created</th>
          <th v-for="column in columns" :key="column.key" scope="col" class="text-nowrap">
            {{ column.label }}
          </th>
        </tr>
      </thead>
      <tbody class="table-group-divider">
        <tr v-for="(entry, idx) in data" :key="idx" class="align-middle">
          <td class="text-center text-muted fw-light">{{ idx + 1 }}</td>
          <td class="text-nowrap small">
            {{
              entry.getCreated() instanceof Date
                ? `${entry.getCreated().getFullYear()}-${String(entry.getCreated().getMonth() + 1).padStart(2, '0')}-${String(entry.getCreated().getDate()).padStart(2, '0')} ${String(entry.getCreated().getHours()).padStart(2, '0')}:${String(entry.getCreated().getMinutes()).padStart(2, '0')}`
                : entry.getCreated()
            }}
          </td>
          <td v-for="column in columns" :key="column.key" class="text-nowrap">
            {{ formatCellValue(entry, column) }}
          </td>
        </tr>
      </tbody>
    </table>

    <!-- Empty state -->
    <div v-if="data.length === 0" class="text-center py-4 text-muted">
      <p class="mb-0">No measurement data available</p>
    </div>
  </div>
</template>

<script setup>
defineProps({
  data: {
    type: Array,
    required: true,
    default: () => []
  },
  columns: {
    type: Array,
    required: true,
    default: () => []
  }
})

const formatCellValue = (entry, column) => {
  const value = entry[column.method]()

  if (column.format === 'temperature') {
    return `${value}°C`
  } else if (column.format === 'voltage') {
    return `${value}V`
  } else if (column.format === 'seconds') {
    return `${value}s`
  } else if (column.format === 'boolean') {
    return value ? 'Yes' : 'No'
  }

  return value
}
</script>
