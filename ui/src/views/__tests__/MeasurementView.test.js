import { beforeEach, describe, expect, it, vi } from 'vitest'
import { flushPromises, mount } from '@vue/test-utils'
import { logDebug } from '@mp-se/espframework-ui-components'
import { global, measurement, status } from '@/modules/pinia'
import MeasurementView from '@/views/MeasurementView.vue'

function makeGravityEntry(id, name, overrides = {}) {
  return {
    id,
    name,
    created: new Date('2026-01-01T12:00:00Z'),
    gravity: 1.04567,
    temp: 20.123,
    getId() {
      return this.id
    },
    getName() {
      return this.name
    },
    ...overrides
  }
}

function makeTiltEntry(id, color, overrides = {}) {
  return {
    id,
    color,
    created: new Date('2026-01-01T13:00:00Z'),
    gravity: 1.01234,
    temp: 19.876,
    getId() {
      return this.id
    },
    getColor() {
      return this.color
    },
    ...overrides
  }
}

function makePressureEntry(id, name, overrides = {}) {
  return {
    id,
    name,
    created: new Date('2026-01-01T14:00:00Z'),
    temp: 18.456,
    pressure: 12.34567,
    getId() {
      return this.id
    },
    getName() {
      return this.name
    },
    ...overrides
  }
}

function makeChamberEntry(id, name, overrides = {}) {
  return {
    id,
    name,
    created: new Date('2026-01-01T15:00:00Z'),
    chamberTemp: 10.987,
    beerTemp: 11.234,
    getId() {
      return this.id
    },
    getName() {
      return this.name
    },
    ...overrides
  }
}

function makeRaptEntry(id, overrides = {}) {
  return {
    id,
    created: new Date('2026-01-01T16:00:00Z'),
    gravity: 1.02345,
    temp: 21.876,
    getId() {
      return this.id
    },
    ...overrides
  }
}

const graphStub = {
  name: 'MeasurementGraphFragment',
  props: ['label1', 'label2', 'dataSet1', 'dataSet2'],
  template:
    '<div class="graph-fragment">{{ label1 }}|{{ label2 }}|{{ dataSet1.length }}|{{ dataSet2.length }}</div>'
}

const tableStub = {
  name: 'MeasurementTableFragment',
  props: ['data', 'columns'],
  template: '<div class="table-fragment">{{ data.length }}|{{ columns.length }}</div>'
}

describe('MeasurementView', () => {
  beforeEach(() => {
    vi.clearAllMocks()
    Object.assign(global, {
      messageError: ''
    })
    Object.assign(status, {
      sd_mounted: false
    })
    Object.assign(measurement, {
      gravitymonData: [],
      tiltData: [],
      pressuremonData: [],
      chamberData: [],
      raptData: [],
      updateMeasurementFiles: vi.fn(async () => true),
      fetchAllMeasurementFiles: vi.fn(async () => true)
    })
  })

  it('shows the no-SD-card message and skips loading', async () => {
    const wrapper = mount(MeasurementView)
    await flushPromises()

    expect(wrapper.text()).toContain('No SD card attached so this feature is not available.')
    expect(measurement.updateMeasurementFiles).not.toHaveBeenCalled()
  })

  it('shows an error when measurement file discovery fails', async () => {
    status.sd_mounted = true
    measurement.updateMeasurementFiles.mockResolvedValue(false)

    mount(MeasurementView)
    await flushPromises()

    expect(global.messageError).toBe('Failed to fetch list of measurement files')
  })

  it('builds gravitymon device options from loaded data', async () => {
    status.sd_mounted = true
    measurement.gravitymonData = [
      makeGravityEntry('G-1', 'Alpha'),
      makeGravityEntry('G-1', 'Alpha Duplicate'),
      makeGravityEntry('G-2', 'Beta')
    ]

    const wrapper = mount(MeasurementView)
    await flushPromises()

    expect(measurement.updateMeasurementFiles).toHaveBeenCalled()
    expect(measurement.fetchAllMeasurementFiles).toHaveBeenCalled()
    expect(wrapper.vm.deviceTypeOptions).toContainEqual({ label: 'Gravitymon', value: 0 })
    expect(wrapper.vm.gravitymonDeviceOptions).toEqual([
      { label: 'G-1 - Alpha', value: 'G-1' },
      { label: 'G-2 - Beta', value: 'G-2' },
      { label: 'All devices', value: '' }
    ])
  })

  it('filters gravitymon table data by selected device id', async () => {
    status.sd_mounted = true
    measurement.gravitymonData = [makeGravityEntry('G-1', 'Alpha'), makeGravityEntry('G-2', 'Beta')]

    const wrapper = mount(MeasurementView)
    await flushPromises()

    wrapper.vm.gravitymonDevice = 'G-2'
    await wrapper.vm.$nextTick()

    expect(wrapper.vm.filteredGravitymonTableData).toHaveLength(1)
    expect(wrapper.vm.filteredGravitymonTableData[0].getId()).toBe('G-2')
  })

  it('shapes gravity and temperature graph data for the selected device', async () => {
    status.sd_mounted = true
    measurement.gravitymonData = [makeGravityEntry('G-1', 'Alpha')]

    const wrapper = mount(MeasurementView)
    await flushPromises()

    wrapper.vm.gravitymonDevice = 'G-1'
    await wrapper.vm.$nextTick()

    expect(wrapper.vm.filteredGravitymonGravityData).toEqual([
      {
        x: measurement.gravitymonData[0].created,
        y: 1.0457
      }
    ])
    expect(wrapper.vm.filteredGravitymonTemperatureData).toEqual([
      {
        x: measurement.gravitymonData[0].created,
        y: 20.12
      }
    ])
  })

  it('builds options for all device families and resolves chamber names from matching entries', async () => {
    status.sd_mounted = true
    measurement.gravitymonData = [makeGravityEntry('G-1', 'Alpha')]
    measurement.tiltData = [makeTiltEntry('T-1', 'Red'), makeTiltEntry('T-1', 'Duplicate')]
    measurement.pressuremonData = [
      makePressureEntry('P-1', 'Pressure Alpha'),
      makePressureEntry('P-1', 'Ignored Duplicate')
    ]
    measurement.chamberData = [
      makeChamberEntry('C-1', ''),
      makeChamberEntry('C-1', 'Fermenter 1'),
      makeChamberEntry('C-2', 'C-2')
    ]
    measurement.raptData = [makeRaptEntry('R-1'), makeRaptEntry('R-1')]

    const wrapper = mount(MeasurementView)
    await flushPromises()

    expect(wrapper.vm.deviceTypeOptions).toEqual([
      { label: 'Gravitymon', value: 0 },
      { label: 'Tilt', value: 1 },
      { label: 'Pressuremon', value: 2 },
      { label: 'Chamber Controller', value: 3 },
      { label: 'Rapt', value: 4 }
    ])
    expect(wrapper.vm.tiltDeviceOptions).toEqual([
      { label: 'T-1 - Red', value: 'T-1' },
      { label: 'All devices', value: '' }
    ])
    expect(wrapper.vm.pressuremonDeviceOptions).toEqual([
      { label: 'P-1 - Pressure Alpha', value: 'P-1' },
      { label: 'All devices', value: '' }
    ])
    expect(wrapper.vm.chamberDeviceOptions).toEqual([
      { label: 'C-1 - Fermenter 1', value: 'C-1' },
      { label: 'C-2 - ', value: 'C-2' },
      { label: 'All devices', value: '' }
    ])
    expect(wrapper.vm.raptDeviceOptions).toEqual([
      { label: 'R-1', value: 'R-1' },
      { label: 'All devices', value: '' }
    ])
  })

  it('renders tilt, pressure, chamber and rapt fragments with filtered graph data', async () => {
    status.sd_mounted = true
    measurement.tiltData = [makeTiltEntry('T-1', 'Red'), makeTiltEntry('T-2', 'Blue')]
    measurement.pressuremonData = [
      makePressureEntry('P-1', 'Pressure Alpha'),
      makePressureEntry('P-2', 'Pressure Beta')
    ]
    measurement.chamberData = [
      makeChamberEntry('C-1', 'Fermenter 1'),
      makeChamberEntry('C-2', 'Fermenter 2')
    ]
    measurement.raptData = [makeRaptEntry('R-1'), makeRaptEntry('R-2')]

    const wrapper = mount(MeasurementView, {
      global: {
        stubs: {
          MeasurementGraphFragment: graphStub,
          MeasurementTableFragment: tableStub
        }
      }
    })
    await flushPromises()

    wrapper.vm.deviceType = 1
    wrapper.vm.tiltDevice = 'T-2'
    await wrapper.vm.$nextTick()
    expect(wrapper.find('.graph-fragment').text()).toBe('Gravity|Temperature|1|1')
    expect(wrapper.find('.table-fragment').text()).toBe('1|6')
    expect(wrapper.vm.filteredTiltGravityData).toEqual([
      { x: measurement.tiltData[1].created, y: 1.0123 }
    ])
    expect(wrapper.vm.filteredTiltTemperatureData).toEqual([
      { x: measurement.tiltData[1].created, y: 19.88 }
    ])

    wrapper.vm.deviceType = 2
    wrapper.vm.pressuremonDevice = 'P-1'
    await wrapper.vm.$nextTick()
    expect(wrapper.find('.graph-fragment').text()).toBe('Pressure|Temperature|1|1')
    expect(wrapper.find('.table-fragment').text()).toBe('1|9')
    expect(wrapper.vm.filteredPressuremonPressureData).toEqual([
      { x: measurement.pressuremonData[0].created, y: 12.3457 }
    ])
    expect(wrapper.vm.filteredPressuremonTemperatureData).toEqual([
      { x: measurement.pressuremonData[0].created, y: 18.46 }
    ])

    wrapper.vm.deviceType = 3
    wrapper.vm.chamberDevice = 'C-2'
    await wrapper.vm.$nextTick()
    expect(wrapper.find('.graph-fragment').text()).toBe('Chamber Temperature|Beer Temperature|1|1')
    expect(wrapper.find('.table-fragment').text()).toBe('1|4')
    expect(wrapper.vm.filteredChamberChamberTemperatureData).toEqual([
      { x: measurement.chamberData[1].created, y: 10.99 }
    ])
    expect(wrapper.vm.filteredChamberBeerTemperatureData).toEqual([
      { x: measurement.chamberData[1].created, y: 11.23 }
    ])

    wrapper.vm.deviceType = 4
    wrapper.vm.raptDevice = 'R-1'
    await wrapper.vm.$nextTick()
    expect(wrapper.find('.graph-fragment').text()).toBe('Gravity|Temperature|1|1')
    expect(wrapper.find('.table-fragment').text()).toBe('1|7')
    expect(wrapper.vm.filteredRaptGravityData).toEqual([
      { x: measurement.raptData[0].created, y: 1.0234 }
    ])
    expect(wrapper.vm.filteredRaptTemperatureData).toEqual([
      { x: measurement.raptData[0].created, y: 21.88 }
    ])
  })

  it('logs device type changes and keeps graph datasets empty until a device is selected', async () => {
    status.sd_mounted = true
    measurement.tiltData = [makeTiltEntry('T-1', 'Red')]

    const wrapper = mount(MeasurementView)
    await flushPromises()

    wrapper.vm.deviceType = 1
    await wrapper.vm.$nextTick()

    expect(logDebug).toHaveBeenCalledWith('MeasurementView.watch()', 'Selected deviceType:', 1)
    expect(wrapper.vm.filteredTiltData).toEqual(measurement.tiltData)
    expect(wrapper.vm.filteredTiltGravityData).toEqual([])
    expect(wrapper.vm.filteredTiltTemperatureData).toEqual([])
  })
})
