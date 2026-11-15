import { describe, it, expect, beforeEach } from 'vitest'
import { mount } from '@vue/test-utils'
import PushDevicesView from '../PushDevicesView.vue'
import piniaInstance, { global, config, status } from '@/modules/pinia'

const createWrapper = () =>
  mount(PushDevicesView, {
    global: {
      plugins: [piniaInstance],
      stubs: { BsInputText: true, BsInputReadonly: true }
    }
  })

describe('PushDevicesView', () => {
  beforeEach(() => {
    global.disabled = false
    config.ble_devices = []
    status.gravity_device = []
    status.pressure_device = []
  })

  it('shows heading and empty state', () => {
    const wrapper = createWrapper()
    expect(wrapper.text()).toContain('BLE devices')
    expect(wrapper.text()).toContain('No devices configured')
  })

  it('offers detected BLE devices that are not configured', () => {
    status.gravity_device = [
      { device: 'aaa111', name: 'a', source: 'BLE Beacon', type: 'Gravitymon' },
      { device: 'bbb222', name: 'b', source: 'HTTP Post', type: 'Gravitymon' },
      { device: 'ccc333', name: 'c', source: 'BLE Beacon', type: 'Tilt' }
    ]
    const wrapper = createWrapper()
    expect(wrapper.vm.detectedDevices.map((d) => d.device)).toEqual(['aaa111'])
  })

  it('hides detected devices that are already configured', () => {
    status.gravity_device = [
      { device: 'aaa111', name: 'beer', source: 'BLE Beacon', type: 'Gravitymon' }
    ]
    config.ble_devices = [{ id: 'aaa111', name: 'beer', token: '' }]
    const wrapper = createWrapper()
    expect(wrapper.vm.detectedDevices).toEqual([])
  })

  it('adds a detected device only once', () => {
    const wrapper = createWrapper()
    wrapper.vm.addDevice('aaa111', 'beer')
    wrapper.vm.addDevice('aaa111', 'beer')
    expect(config.ble_devices).toEqual([{ id: 'aaa111', name: 'beer', token: '' }])
  })

  it('removes a device', () => {
    config.ble_devices = [
      { id: 'a', name: '', token: '' },
      { id: 'b', name: '', token: '' }
    ]
    const wrapper = createWrapper()
    wrapper.vm.removeDevice(0)
    expect(config.ble_devices.map((d) => d.id)).toEqual(['b'])
  })
})
