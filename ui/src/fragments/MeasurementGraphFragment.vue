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
  <div class="row">
    <div class="col-md-12">
      <canvas id="dataChart"></canvas>
    </div>
  </div>
</template>

<script setup>
import { onMounted, watch } from 'vue'

import { Chart, registerables } from 'chart.js'
import 'date-fns'
import 'chartjs-adapter-date-fns'
import { logDebug, logError } from '@mp-se/espframework-ui-components'

var chart = null // Do not use ref for this, will cause stack overflow...

Chart.register(...registerables)

const dataSet1 = defineModel('dataSet1')
const dataSet2 = defineModel('dataSet2')

const label1 = defineModel('label1')
const label2 = defineModel('label2')

watch([dataSet1, dataSet2, label1, label2], () => {
  if (chart) {
    chart.data.datasets[0].data = dataSet1.value
    chart.data.datasets[0].label = label1.value
    chart.data.datasets[1].data = dataSet2.value
    chart.data.datasets[1].label = label2.value
    chart.update()
  }
})

onMounted(() => {
  logDebug(
    'MeasurementGraphFragment.onMounted()',
    dataSet1.value,
    dataSet2.value,
    label1.value,
    label2.value
  )

  try {
    const chartOptions = {
      type: 'line',
      data: {
        datasets: [
          {
            label: label1.value,
            data: dataSet1.value,
            borderColor: 'green',
            backgroundColor: 'green',
            yAxisID: 'y1',
            pointRadius: 0,
            cubicInterpolationMode: 'monotone',
            tension: 0.4
          },
          {
            label: label2.value,
            data: dataSet2.value,
            borderColor: 'blue',
            backgroundColor: 'blue',
            yAxisID: 'y2',
            pointRadius: 0,
            cubicInterpolationMode: 'monotone',
            tension: 0.4
          }
        ]
      },
      options: {
        scales: {
          x: {
            type: 'time',
            time: {
              unit: 'hour',
              displayFormats: {
                hour: 'E HH:mm',
                day: 'HH:mm',
                week: 'E HH:mm',
                month: 'd HH:mm'
              }
            }
          }
        },
        animation: false,
        plugins: {}
      }
    }

    //   if (config.gravity) {
    //   }

    logDebug('MeasurementGraphFragment.onMounted()', 'Creating chart')

    if (document.getElementById('dataChart') == null) {
      logError('MeasurementGraphFragment.onMounted()', 'Unable to find the chart canvas')
    } else {
      chart = new Chart(document.getElementById('dataChart').getContext('2d'), chartOptions)
      chart.update()
    }
  } catch (err) {
    logDebug('MeasurementGraphFragment.onMounted()', err)
  }
})

// function updateDataSet() {
//   logDebug('BatchGravityGraphView.updateDataSet()')

//   const filteredGravityList = gravityList.value.filter(
//     (g) => g.active && g.gravity >= infoFG.value && g.gravity <= infoOG.value
//   )

//   gravityStats.value = getGravityDataAnalytics(filteredGravityList)

//   gravityData.value = mapGravityData(filteredGravityList)
//   batteryData.value = mapBatteryData(filteredGravityList)
//   temperatureData.value = mapTemperatureData(filteredGravityList)
//   alcoholData.value = mapAlcoholData(filteredGravityList)
//   chamberData.value = mapChamberData(filteredGravityList)

//   gravityVelocityData.value = mapGravityVelocityData(
//     filteredGravityList
//   )

//   currentDataCount.value = gravityData.value.length
//   configureChart(graphOptions.value)
// }
</script>
