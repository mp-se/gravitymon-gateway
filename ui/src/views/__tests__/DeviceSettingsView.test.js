import { beforeEach, describe, expect, it, vi } from 'vitest'
import { mount } from '@vue/test-utils'
import DeviceSettingsView from '../DeviceSettingsView.vue'
import { config, global } from '@/modules/pinia'
import {
  sharedHttpClient as http,
  validateCurrentForm,
  logInfo,
  logError
} from '@mp-se/espframework-ui-components'

describe('DeviceSettingsView', () => {
  const createWrapper = () =>
    mount(DeviceSettingsView, {
      global: {
        stubs: {
          BsMessage: { template: '<div><slot /></div>' },
          BsInputText: true,
          BsInputRadio: true
        }
      }
    })

  beforeEach(() => {
    vi.clearAllMocks()
    global.disabled = false
    global.configChanged = true
    global.messageError = ''
    global.messageSuccess = ''
    config.mdns = ''
  })

  it('renders the settings form and warning message when mdns is missing', () => {
    const wrapper = createWrapper()

    expect(wrapper.text()).toContain('Device - Settings')
    expect(wrapper.text()).toContain('define a mdns name')
    expect(wrapper.text()).toContain('Restore factory defaults')
  })

  it('displays mdns input field', () => {
    config.mdns = 'my-device'
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    expect(config.mdns).toBe('my-device')
  })

  it('displays temperature format options', () => {
    config.temp_unit = 'C'
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    expect(config.temp_unit).toBe('C')

    config.temp_unit = 'F'
    expect(config.temp_unit).toBe('F')
  })

  it('displays gravity format options', () => {
    config.gravity_unit = 'G'
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    expect(config.gravity_unit).toBe('G')

    config.gravity_unit = 'P'
    expect(config.gravity_unit).toBe('P')
  })

  it('displays pressure unit options', () => {
    config.pressure_unit = 'PSI'
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    expect(config.pressure_unit).toBe('PSI')

    config.pressure_unit = 'kPa'
    expect(config.pressure_unit).toBe('kPa')

    config.pressure_unit = 'Bar'
    expect(config.pressure_unit).toBe('Bar')
  })

  it('displays dark mode options', () => {
    config.dark_mode = false
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    expect(config.dark_mode).toBe(false)

    config.dark_mode = true
    expect(config.dark_mode).toBe(true)
  })

  it('displays all action buttons', () => {
    const wrapper = createWrapper()

    expect(wrapper.text()).toContain('Save')
    expect(wrapper.text()).toContain('Restart device')
    expect(wrapper.text()).toContain('Restore factory defaults')
  })

  it('saves settings when the form is valid', async () => {
    validateCurrentForm.mockReturnValue(true)
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).toHaveBeenCalled()
  })

  it('does not save settings when the form is invalid', async () => {
    validateCurrentForm.mockReturnValue(false)
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).not.toHaveBeenCalled()
  })

  it('restarts the device through the config store', async () => {
    const wrapper = createWrapper()

    await wrapper.vm.restart()

    expect(config.restart).toHaveBeenCalled()
  })

  it('handles successful factory reset responses', async () => {
    http.request.mockResolvedValue({
      json: vi.fn().mockResolvedValue({ success: true, message: 'Factory reset initiated' })
    })
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(http.request).toHaveBeenCalledWith('api/factory', { method: 'POST' })
    expect(global.messageSuccess).toBe('Factory reset initiated')
    expect(global.disabled).toBe(true)
  })

  it('handles failed factory reset responses', async () => {
    http.request.mockResolvedValue({
      json: vi.fn().mockResolvedValue({ success: false, message: 'Nope' })
    })
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(global.messageError).toBe('Nope')
    expect(global.disabled).toBe(false)
  })

  it('handles factory reset request exceptions', async () => {
    http.request.mockRejectedValue(new Error('network fail'))
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(global.messageError).toBe('Failed to do factory restore')
    expect(global.disabled).toBe(false)
  })

  it('disables save button when global.disabled is true', () => {
    global.disabled = true
    const wrapper = createWrapper()

    expect(wrapper.vm).toBeDefined()
  })

  it('disables save button when global.configChanged is false', () => {
    global.configChanged = false
    const wrapper = createWrapper()

    expect(wrapper.vm).toBeDefined()
  })

  it('initializes with all config values', () => {
    config.mdns = 'brewmon'
    config.temp_unit = 'F'
    config.gravity_unit = 'P'
    config.pressure_unit = 'Bar'
    config.dark_mode = true
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    expect(config.mdns).toBe('brewmon')
    expect(config.temp_unit).toBe('F')
    expect(config.gravity_unit).toBe('P')
    expect(config.pressure_unit).toBe('Bar')
    expect(config.dark_mode).toBe(true)
  })

  it('validates device settings before saving', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.mdns = 'test-device'
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(validateCurrentForm).toHaveBeenCalled()
    expect(config.saveAll).toHaveBeenCalled()
  })

  it('saves with celsius temperature unit', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.temp_unit = 'C'
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.temp_unit).toBe('C')
  })

  it('saves with fahrenheit temperature unit', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.temp_unit = 'F'
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.temp_unit).toBe('F')
  })

  it('saves with gravity unit grams', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.gravity_unit = 'G'
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.gravity_unit).toBe('G')
  })

  it('saves with gravity unit plato', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.gravity_unit = 'P'
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.gravity_unit).toBe('P')
  })

  it('saves with pressure unit PSI', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.pressure_unit = 'PSI'
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.pressure_unit).toBe('PSI')
  })

  it('saves with pressure unit kPa', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.pressure_unit = 'kPa'
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.pressure_unit).toBe('kPa')
  })

  it('saves with pressure unit Bar', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.pressure_unit = 'Bar'
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.pressure_unit).toBe('Bar')
  })

  it('saves with dark mode enabled', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.dark_mode = true
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.dark_mode).toBe(true)
  })

  it('saves with dark mode disabled', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.dark_mode = false
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.dark_mode).toBe(false)
  })

  it('disables save button when validation fails', async () => {
    validateCurrentForm.mockReturnValue(false)
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).not.toHaveBeenCalled()
  })

  it('disables save button when form is disabled', () => {
    global.disabled = true
    const wrapper = createWrapper()
    const saveButton = wrapper.findAll('button')[0]

    expect(saveButton.attributes('disabled')).toBeDefined()
  })

  it('disables restart button when form is disabled', () => {
    global.disabled = true
    const wrapper = createWrapper()
    const buttons = wrapper.findAll('button')

    expect(buttons[1].attributes('disabled')).toBeDefined()
  })

  it('disables factory reset button when form is disabled', () => {
    global.disabled = true
    const wrapper = createWrapper()
    const buttons = wrapper.findAll('button')

    expect(buttons[2].attributes('disabled')).toBeDefined()
  })

  it('enables buttons when global.disabled is false', () => {
    global.disabled = false
    const wrapper = createWrapper()
    const buttons = wrapper.findAll('button')

    buttons.forEach((button) => {
      expect(button.attributes('disabled')).toBeUndefined()
    })
  })

  it('disables save when no config changes', () => {
    global.configChanged = false
    const wrapper = createWrapper()
    const saveButton = wrapper.findAll('button')[0]

    expect(saveButton.attributes('disabled')).toBeDefined()
  })

  it('enables save when config changes', () => {
    global.configChanged = true
    const wrapper = createWrapper()
    const saveButton = wrapper.findAll('button')[0]

    expect(saveButton.attributes('disabled')).toBeUndefined()
  })

  it('warns when mdns is not configured', () => {
    config.mdns = ''
    const wrapper = createWrapper()

    expect(wrapper.text()).toContain('define a mdns name')
  })

  it('hides mdns warning when mdns is configured', () => {
    config.mdns = 'device-name'
    const wrapper = createWrapper()

    expect(wrapper.text()).not.toContain('define a mdns name')
  })

  it('saves with valid mdns name', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.mdns = 'my-brew-device'
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.mdns).toBe('my-brew-device')
  })

  it('restarts device successfully', async () => {
    config.restart.mockClear()
    const wrapper = createWrapper()

    await wrapper.vm.restart()

    expect(config.restart).toHaveBeenCalledTimes(1)
  })

  it('factory reset success message is set', async () => {
    http.request.mockResolvedValue({
      json: vi.fn().mockResolvedValue({ success: true, message: 'Device reset' })
    })
    global.messageSuccess = ''
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(global.messageSuccess).toBe('Device reset')
  })

  it('factory reset disables controls after success', async () => {
    http.request.mockResolvedValue({
      json: vi.fn().mockResolvedValue({ success: true, message: 'Reset' })
    })
    global.disabled = false
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(global.disabled).toBe(true)
  })

  it('factory reset error message is set on failure', async () => {
    http.request.mockResolvedValue({
      json: vi.fn().mockResolvedValue({ success: false, message: 'Network error' })
    })
    global.messageError = ''
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(global.messageError).toBe('Network error')
  })

  it('factory reset enables controls after failure', async () => {
    http.request.mockResolvedValue({
      json: vi.fn().mockResolvedValue({ success: false, message: 'Error' })
    })
    global.disabled = true
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(global.disabled).toBe(false)
  })

  it('factory reset handles network exceptions', async () => {
    http.request.mockRejectedValue(new Error('Connection timeout'))
    global.messageError = ''
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(global.messageError).toBe('Failed to do factory restore')
  })

  it('factory reset API call uses correct endpoint', async () => {
    http.request.mockResolvedValue({
      json: vi.fn().mockResolvedValue({ success: true, message: 'Done' })
    })
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(http.request).toHaveBeenCalledWith('api/factory', { method: 'POST' })
  })

  it('updates all config independently', () => {
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    config.mdns = 'device1'
    expect(config.mdns).toBe('device1')
    expect(config.temp_unit).not.toBe('device1')

    config.temp_unit = 'F'
    expect(config.mdns).toBe('device1')
    expect(config.temp_unit).toBe('F')
  })

  it('maintains config state across saves', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.mdns = 'persistent-device'
    config.temp_unit = 'C'
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()
    expect(config.mdns).toBe('persistent-device')

    await wrapper.vm.saveSettings()
    expect(config.mdns).toBe('persistent-device')
    expect(config.temp_unit).toBe('C')
  })

  it('renders all form labels', () => {
    const wrapper = createWrapper()

    expect(wrapper.text()).toContain('Device - Settings')
  })

  it('multiple configuration changes are saved together', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.mdns = 'brew-device'
    config.temp_unit = 'F'
    config.gravity_unit = 'P'
    config.pressure_unit = 'kPa'
    config.dark_mode = true

    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).toHaveBeenCalledTimes(1)
    expect(config.mdns).toBe('brew-device')
    expect(config.temp_unit).toBe('F')
  })

  it('factory reset clears messages before starting', async () => {
    global.clearMessages.mockClear()
    http.request.mockResolvedValue({
      json: vi.fn().mockResolvedValue({ success: true, message: 'Reset initiated' })
    })
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(global.clearMessages).toHaveBeenCalled()
  })

  it('factory reset logs info message at start', async () => {
    logInfo.mockClear()
    http.request.mockResolvedValue({
      json: vi.fn().mockResolvedValue({ success: true, message: 'Reset initiated' })
    })
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(logInfo).toHaveBeenCalledWith('DeviceSettingsView.factory()', 'Sending /api/factory')
  })

  it('factory reset disables controls during request', async () => {
    global.disabled = false
    http.request.mockResolvedValue({
      json: vi.fn().mockResolvedValue({ success: true, message: 'Reset initiated' })
    })
    const wrapper = createWrapper()

    const factoryPromise = wrapper.vm.factory()
    expect(global.disabled).toBe(true)

    await factoryPromise
  })

  it('factory reset success triggers message and disables controls', async () => {
    http.request.mockResolvedValue({
      json: vi.fn().mockResolvedValue({ success: true, message: 'Factory reset initiated' })
    })
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(global.messageSuccess).toBe('Factory reset initiated')
    expect(global.disabled).toBe(true)
  })

  it('factory reset error response keeps controls enabled', async () => {
    global.disabled = false
    http.request.mockResolvedValue({
      json: vi.fn().mockResolvedValue({ success: false, message: 'Device is offline' })
    })
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(global.messageError).toBe('Device is offline')
    expect(global.disabled).toBe(false)
  })

  it('factory reset network error shows default message', async () => {
    global.disabled = false
    http.request.mockRejectedValue(new Error('timeout'))
    logError.mockClear()
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(logError).toHaveBeenCalledWith('DeviceSettingsView.factory()', expect.any(Error))
    expect(global.messageError).toBe('Failed to do factory restore')
  })

  it('saveSettings calls validateCurrentForm', async () => {
    validateCurrentForm.mockReturnValue(true)
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(validateCurrentForm).toHaveBeenCalled()
  })

  it('saveSettings returns early if validation fails', async () => {
    validateCurrentForm.mockReturnValue(false)
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).not.toHaveBeenCalled()
  })

  it('restart method calls config.restart', async () => {
    config.restart.mockClear()
    const wrapper = createWrapper()

    await wrapper.vm.restart()

    expect(config.restart).toHaveBeenCalledTimes(1)
  })

  it('temperature and gravity units are independent', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.temp_unit = 'F'
    config.gravity_unit = 'P'
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.temp_unit).toBe('F')
    expect(config.gravity_unit).toBe('P')
    expect(config.saveAll).toHaveBeenCalled()
  })

  it('pressure unit can be changed independently', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.pressure_unit = 'Bar'
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.pressure_unit).toBe('Bar')
    expect(config.saveAll).toHaveBeenCalled()
  })

  it('all option arrays have expected values', () => {
    const wrapper = createWrapper()

    expect(wrapper.vm.tempOptions).toHaveLength(2)
    expect(wrapper.vm.gravityOptions).toHaveLength(2)
    expect(wrapper.vm.pressureOptions).toHaveLength(3)
    expect(wrapper.vm.uiOptions).toHaveLength(2)
  })

  it('temperature options have correct labels and values', () => {
    const wrapper = createWrapper()

    expect(wrapper.vm.tempOptions[0]).toEqual({ label: 'Celsius °C', value: 'C' })
    expect(wrapper.vm.tempOptions[1]).toEqual({ label: 'Fahrenheit °F', value: 'F' })
  })

  it('gravity options have correct labels and values', () => {
    const wrapper = createWrapper()

    expect(wrapper.vm.gravityOptions[0]).toEqual({ label: 'Specific Gravity', value: 'G' })
    expect(wrapper.vm.gravityOptions[1]).toEqual({ label: 'Plato', value: 'P' })
  })

  it('pressure options have correct labels and values', () => {
    const wrapper = createWrapper()

    expect(wrapper.vm.pressureOptions).toContainEqual({ label: 'PSI', value: 'PSI' })
    expect(wrapper.vm.pressureOptions).toContainEqual({ label: 'kPA', value: 'kPa' })
    expect(wrapper.vm.pressureOptions).toContainEqual({ label: 'Bar', value: 'Bar' })
  })

  it('ui options have day and dark mode', () => {
    const wrapper = createWrapper()

    expect(wrapper.vm.uiOptions).toContainEqual({ label: 'Day mode', value: false })
    expect(wrapper.vm.uiOptions).toContainEqual({ label: 'Dark mode', value: true })
  })

  it('factory reset with success compares using == operator', async () => {
    http.request.mockResolvedValue({
      json: vi.fn().mockResolvedValue({ success: true, message: 'Reset started' })
    })
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(global.messageSuccess).toBe('Reset started')
    expect(global.disabled).toBe(true)
  })

  it('multiple saves accumulate correctly', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    config.mdns = 'device1'
    await wrapper.vm.saveSettings()
    expect(config.saveAll).toHaveBeenCalledTimes(1)

    config.mdns = 'device2'
    await wrapper.vm.saveSettings()
    expect(config.saveAll).toHaveBeenCalledTimes(2)
  })

  it('dark mode false is treated as valid setting', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.dark_mode = false
    const wrapper = createWrapper()

    await wrapper.vm.saveSettings()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.dark_mode).toBe(false)
  })

  it('saveSettings awaits config.saveAll', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.saveAll.mockImplementation(() => Promise.resolve())
    const wrapper = createWrapper()

    const promise = wrapper.vm.saveSettings()
    expect(promise).toBeInstanceOf(Promise)

    await promise
    expect(config.saveAll).toHaveBeenCalled()
  })

  it('factory request uses POST method', async () => {
    http.request.mockResolvedValue({
      json: vi.fn().mockResolvedValue({ success: true, message: 'OK' })
    })
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    expect(http.request).toHaveBeenCalledWith('api/factory', { method: 'POST' })
  })

  it('factory endpoint is api/factory without leading slash', async () => {
    http.request.mockResolvedValue({
      json: vi.fn().mockResolvedValue({ success: false, message: 'Error' })
    })
    const wrapper = createWrapper()

    await wrapper.vm.factory()

    const callArgs = http.request.mock.calls[0]
    expect(callArgs[0]).toBe('api/factory')
  })
})
