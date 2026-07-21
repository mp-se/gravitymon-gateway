import { describe, expect, it } from 'vitest'
import { mount } from '@vue/test-utils'
import MeasurementTableFragment from '../MeasurementTableFragment.vue'

const columns = [
  { key: 'temp', label: 'Temp', method: 'getTemp', format: 'temperature' },
  { key: 'voltage', label: 'Voltage', method: 'getVoltage', format: 'voltage' },
  { key: 'seconds', label: 'Age', method: 'getAge', format: 'seconds' },
  { key: 'active', label: 'Active', method: 'isActive', format: 'boolean' }
]

const entry = {
  getCreated: () => new Date('2025-01-09T09:05:00Z'),
  getTemp: () => 20.5,
  getVoltage: () => 3.8,
  getAge: () => 12,
  isActive: () => true
}

describe('MeasurementTableFragment', () => {
  it('renders an empty state when no data is available', () => {
    const wrapper = mount(MeasurementTableFragment, {
      props: { data: [], columns: [] }
    })

    expect(wrapper.text()).toContain('No measurement data available')
  })

  it('renders rows and formats cells by column type', () => {
    const wrapper = mount(MeasurementTableFragment, {
      props: { data: [entry], columns }
    })

    expect(wrapper.findAll('tbody tr')).toHaveLength(1)
    expect(wrapper.text()).toContain('20.5°C')
    expect(wrapper.text()).toContain('3.8V')
    expect(wrapper.text()).toContain('12s')
    expect(wrapper.text()).toContain('Yes')
  })
})
