import { beforeEach, describe, expect, it, vi } from 'vitest'
import { mount, shallowMount } from '@vue/test-utils'
import { validateCurrentForm } from '@mp-se/espframework-ui-components'
import { config, global } from '@/modules/pinia'
import DeviceHardwareView from '@/views/DeviceHardwareView.vue'

describe('DeviceHardwareView', () => {
  beforeEach(() => {
    vi.clearAllMocks()
    Object.assign(config, {
      ble_enable: true,
      ble_active_scan: false,
      ble_scan_time: 5,
      timezone: 'CET-1CEST,M3.5.0,M10.5.0/3',
      display_layout_id: 0,
      saveAll: vi.fn(async () => true),
      restart: vi.fn(async () => true)
    })
    Object.assign(global, {
      disabled: false,
      configChanged: true
    })
  })

  it('mounts without error', () => {
    const wrapper = shallowMount(DeviceHardwareView)

    expect(wrapper.exists()).toBe(true)
  })

  it('renders heading, form, and action buttons', () => {
    const wrapper = mount(DeviceHardwareView)

    expect(wrapper.text()).toContain('Device - Hardware')
    expect(wrapper.find('form').exists()).toBe(true)
    expect(wrapper.text()).toContain('Save')
    expect(wrapper.text()).toContain('Restart device')
  })

  it('save clears messages and persists config when the form is valid', async () => {
    const wrapper = shallowMount(DeviceHardwareView)

    await wrapper.vm.save()

    expect(validateCurrentForm).toHaveBeenCalled()
    expect(global.clearMessages).toHaveBeenCalled()
    expect(config.saveAll).toHaveBeenCalled()
  })

  it('save returns early when the form is invalid', async () => {
    validateCurrentForm.mockReturnValueOnce(false)
    const wrapper = shallowMount(DeviceHardwareView)

    await wrapper.vm.save()

    expect(global.clearMessages).not.toHaveBeenCalled()
    expect(config.saveAll).not.toHaveBeenCalled()
  })

  it('restart delegates to config.restart', async () => {
    const wrapper = shallowMount(DeviceHardwareView)

    await wrapper.vm.restart()

    expect(config.restart).toHaveBeenCalled()
  })

  it('exposes expected scan, timezone, and layout options', () => {
    const wrapper = shallowMount(DeviceHardwareView)

    expect(wrapper.vm.bleScanOptions).toEqual([
      { label: 'Active', value: true },
      { label: 'Passive', value: false }
    ])
    expect(wrapper.vm.displayLayoutOptions).toHaveLength(2)
    expect(wrapper.vm.timezoneOptions.length).toBeGreaterThan(5)
  })

  it('v-model bindings update config fields through emitting stubs', async () => {
    const switchStub = {
      template:
        '<input type="checkbox" :checked="modelValue" @change="$emit(\'update:modelValue\', $event.target.checked)" />',
      props: ['modelValue', 'disabled', 'label', 'width'],
      emits: ['update:modelValue']
    }
    const radioStub = {
      template:
        '<select @change="$emit(\'update:modelValue\', $event.target.value === \'true\')"><option value="true">Active</option><option value="false">Passive</option></select>',
      props: ['modelValue', 'disabled', 'label', 'width', 'options'],
      emits: ['update:modelValue']
    }
    const numberStub = {
      template:
        '<input type="number" :value="modelValue" @input="$emit(\'update:modelValue\', Number($event.target.value))" />',
      props: ['modelValue', 'disabled', 'label', 'width', 'unit'],
      emits: ['update:modelValue']
    }
    const selectStub = {
      template:
        '<select @change="$emit(\'update:modelValue\', $event.target.value)"><option value="opt">opt</option></select>',
      props: ['modelValue', 'disabled', 'label', 'width', 'options'],
      emits: ['update:modelValue']
    }

    const wrapper = mount(DeviceHardwareView, {
      global: {
        stubs: {
          BsInputSwitch: switchStub,
          BsInputRadio: radioStub,
          BsInputNumber: numberStub,
          BsSelect: selectStub
        }
      }
    })

    const checkbox = wrapper.find('input[type="checkbox"]')
    await checkbox.setValue(false)

    const selects = wrapper.findAll('select')
    await selects[0].setValue('true')
    await selects[1].setValue('opt')
    await selects[2].setValue('opt')

    const numberInput = wrapper.find('input[type="number"]')
    await numberInput.setValue('12')

    expect(config.ble_enable).toBe(false)
    expect(config.ble_active_scan).toBe(true)
    expect(config.ble_scan_time).toBe(12)
    expect(config.timezone).toBe('opt')
    expect(config.display_layout_id).toBe('opt')
  })
})
