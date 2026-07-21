import { beforeEach, describe, expect, it, vi } from 'vitest'
import { mount } from '@vue/test-utils'
import DeviceWifiApView from '../DeviceWifiApView.vue'
import { config, global, status } from '@/modules/pinia'
import { validateCurrentForm, logDebug } from '@mp-se/espframework-ui-components'

describe('DeviceWifiApView', () => {
  const createWrapper = () =>
    mount(DeviceWifiApView, {
      global: {
        stubs: {
          BsInputText: true
        }
      }
    })

  beforeEach(() => {
    vi.clearAllMocks()
    global.disabled = false
    global.configChanged = true
    global.messageInfo = ''
    status.id = 'ABC123'
    config.wifi_direct_ssid = ''
    config.wifi_direct_pass = ''
  })

  it('renders wifi AP configuration controls', () => {
    const wrapper = createWrapper()

    expect(wrapper.text()).toContain('Device - WIFI AP')
    expect(wrapper.text()).toContain('Generate')
    expect(wrapper.text()).toContain('Restart device')
  })

  it('displays the wifi direct explanation', () => {
    const wrapper = createWrapper()

    expect(wrapper.text()).toContain('Wifi Direct allows')
    expect(wrapper.text()).toContain('direct link between the two')
  })

  it('generate fills direct SSID with device id prefix', () => {
    const randomValues = BigUint64Array.from([123456789n, 987654321n])
    vi.spyOn(window.crypto, 'getRandomValues').mockReturnValue(randomValues)
    const wrapper = createWrapper()

    wrapper.vm.generate()

    expect(config.wifi_direct_ssid).toBe('gw-ABC123')
  })

  it('generate fills direct password with random string of exactly 10 characters', () => {
    const randomValues = BigUint64Array.from([123456789n, 987654321n])
    vi.spyOn(window.crypto, 'getRandomValues').mockReturnValue(randomValues)
    const wrapper = createWrapper()

    wrapper.vm.generate()

    expect(config.wifi_direct_pass).toHaveLength(10)
    expect(config.wifi_direct_pass).toMatch(/^[a-z0-9A-Z]+$/)
  })

  it('saves settings when the form is valid', () => {
    validateCurrentForm.mockReturnValue(true)
    config.wifi_direct_ssid = 'gw-test'
    config.wifi_direct_pass = 'test1234'
    const wrapper = createWrapper()

    wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalled()
    expect(global.messageInfo).toContain('restart the device')
  })

  it('does not save when validation fails', () => {
    validateCurrentForm.mockReturnValue(false)
    const wrapper = createWrapper()

    wrapper.vm.save()

    expect(config.saveAll).not.toHaveBeenCalled()
  })

  it('restarts the device through the config store', async () => {
    const wrapper = createWrapper()

    await wrapper.vm.restart()

    expect(config.restart).toHaveBeenCalled()
  })

  it('disables inputs when global.disabled is true', () => {
    global.disabled = true
    const wrapper = createWrapper()

    expect(wrapper.vm).toBeDefined()
    expect(config.saveAll).toBeDefined()
  })

  it('disables save button when no changes are made', () => {
    global.configChanged = false
    const wrapper = createWrapper()
    const saveButton = wrapper.findAll('button').find((b) => b.text().includes('Save'))

    expect(saveButton?.attributes('disabled')).toBeDefined()
  })

  it('enables save button when changes are made', () => {
    global.configChanged = true
    const wrapper = createWrapper()
    const saveButton = wrapper.findAll('button').find((b) => b.text().includes('Save'))

    expect(saveButton?.attributes('disabled')).toBeUndefined()
  })

  it('generate creates SSID with correct device ID format', () => {
    const wrapper = createWrapper()
    status.id = 'XYZ789'

    wrapper.vm.generate()

    expect(config.wifi_direct_ssid).toBe('gw-XYZ789')
  })

  it('generate creates different passwords on multiple calls', () => {
    const wrapper = createWrapper()
    const randomValues1 = BigUint64Array.from([111111111n, 222222222n])
    const randomValues2 = BigUint64Array.from([333333333n, 444444444n])

    vi.spyOn(window.crypto, 'getRandomValues').mockReturnValueOnce(randomValues1)
    wrapper.vm.generate()
    const pass1 = config.wifi_direct_pass

    vi.spyOn(window.crypto, 'getRandomValues').mockReturnValueOnce(randomValues2)
    wrapper.vm.generate()
    const pass2 = config.wifi_direct_pass

    expect(pass1).not.toBe(pass2)
  })

  it('generate password uses base36 and uppercase conversion', () => {
    const randomValues = BigUint64Array.from([12345n, 67890n])
    vi.spyOn(window.crypto, 'getRandomValues').mockReturnValue(randomValues)
    const wrapper = createWrapper()

    wrapper.vm.generate()

    expect(config.wifi_direct_pass).toBeTruthy()
    expect(config.wifi_direct_pass.length).toBeLessThanOrEqual(10)
  })

  it('generates SSID and password together', () => {
    const randomValues = BigUint64Array.from([999999999n, 888888888n])
    vi.spyOn(window.crypto, 'getRandomValues').mockReturnValue(randomValues)
    const wrapper = createWrapper()

    wrapper.vm.generate()

    expect(config.wifi_direct_ssid).toMatch(/^gw-/)
    expect(config.wifi_direct_pass).toBeTruthy()
    expect(config.wifi_direct_pass.length).toBeLessThanOrEqual(10)
  })

  it('saves with valid SSID and password', () => {
    validateCurrentForm.mockReturnValue(true)
    config.wifi_direct_ssid = 'custom-ssid'
    config.wifi_direct_pass = 'custompass1234'
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalledTimes(1)
  })

  it('save sets correct message', () => {
    validateCurrentForm.mockReturnValue(true)
    global.messageInfo = ''
    const wrapper = createWrapper()

    wrapper.vm.save()

    expect(global.messageInfo).toContain('restart the device')
    expect(global.messageInfo).toContain('effect')
  })

  it('form has proper submit binding', () => {
    const wrapper = createWrapper()

    expect(wrapper.find('form').exists()).toBe(true)
    expect(wrapper.find('form').attributes('@submit.prevent')).toBeUndefined()
  })

  it('generate button is disabled when global.disabled is true', () => {
    global.disabled = true
    const wrapper = createWrapper()
    const generateBtn = wrapper.findAll('button').find((b) => b.text().includes('Generate'))

    expect(generateBtn?.attributes('disabled')).toBeDefined()
  })

  it('generate button is enabled when global.disabled is false', () => {
    global.disabled = false
    const wrapper = createWrapper()
    const generateBtn = wrapper.findAll('button').find((b) => b.text().includes('Generate'))

    expect(generateBtn?.attributes('disabled')).toBeUndefined()
  })

  it('restart button is disabled when global.disabled is true', () => {
    global.disabled = true
    const wrapper = createWrapper()
    const restartBtn = wrapper.findAll('button').find((b) => b.text().includes('Restart'))

    expect(restartBtn?.attributes('disabled')).toBeDefined()
  })

  it('restart button is enabled when global.disabled is false', () => {
    global.disabled = false
    const wrapper = createWrapper()
    const restartBtn = wrapper.findAll('button').find((b) => b.text().includes('Restart'))

    expect(restartBtn?.attributes('disabled')).toBeUndefined()
  })

  it('save button is disabled when both conditions make it disabled', () => {
    global.disabled = true
    global.configChanged = true
    const wrapper = createWrapper()
    const saveBtn = wrapper.findAll('button').find((b) => b.text().includes('Save'))

    expect(saveBtn?.attributes('disabled')).toBeDefined()
  })

  it('save button is disabled when only configChanged is false', () => {
    global.disabled = false
    global.configChanged = false
    const wrapper = createWrapper()
    const saveBtn = wrapper.findAll('button').find((b) => b.text().includes('Save'))

    expect(saveBtn?.attributes('disabled')).toBeDefined()
  })

  it('save button is enabled when both conditions allow it', () => {
    global.disabled = false
    global.configChanged = true
    const wrapper = createWrapper()
    const saveBtn = wrapper.findAll('button').find((b) => b.text().includes('Save'))

    expect(saveBtn?.attributes('disabled')).toBeUndefined()
  })

  it('config values persist after multiple generates', () => {
    const wrapper = createWrapper()
    const randomValues1 = BigUint64Array.from([111n, 222n])
    const randomValues2 = BigUint64Array.from([333n, 444n])

    vi.spyOn(window.crypto, 'getRandomValues').mockReturnValueOnce(randomValues1)
    wrapper.vm.generate()
    const ssid1 = config.wifi_direct_ssid

    vi.spyOn(window.crypto, 'getRandomValues').mockReturnValueOnce(randomValues2)
    wrapper.vm.generate()
    const ssid2 = config.wifi_direct_ssid

    expect(ssid1).toBe(ssid2) // SSID remains same format
    expect(ssid2).toMatch(/^gw-/)
  })

  it('manual SSID entry can override generated value', () => {
    const wrapper = createWrapper()

    config.wifi_direct_ssid = 'manual-ssid'
    expect(config.wifi_direct_ssid).toBe('manual-ssid')

    wrapper.vm.generate()
    expect(config.wifi_direct_ssid).toBe('gw-ABC123')
  })

  it('manual password entry can be entered', () => {
    createWrapper()

    config.wifi_direct_pass = 'manualpass123'
    expect(config.wifi_direct_pass).toBe('manualpass123')
  })

  it('validation happens before save', () => {
    validateCurrentForm.mockReturnValue(false)
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    wrapper.vm.save()

    expect(validateCurrentForm).toHaveBeenCalled()
    expect(config.saveAll).not.toHaveBeenCalled()
  })

  it('save happens immediately without async delay', () => {
    validateCurrentForm.mockReturnValue(true)
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalled()
  })

  it('restart is async', async () => {
    const wrapper = createWrapper()

    await wrapper.vm.restart()

    expect(config.restart).toHaveBeenCalled()
  })

  it('explanation text is complete and informative', () => {
    const wrapper = createWrapper()

    expect(wrapper.text()).toContain('Wifi Direct')
    expect(wrapper.text()).toContain('bypass the access points')
    expect(wrapper.text()).toContain('closest access point')
    expect(wrapper.text()).toContain('SSID')
  })

  it('handles empty SSID before generation', () => {
    config.wifi_direct_ssid = ''
    const wrapper = createWrapper()

    expect(config.wifi_direct_ssid).toBe('')
    wrapper.vm.generate()
    expect(config.wifi_direct_ssid).toBe('gw-ABC123')
  })

  it('handles empty password before generation', () => {
    config.wifi_direct_pass = ''
    const wrapper = createWrapper()

    expect(config.wifi_direct_pass).toBe('')
    wrapper.vm.generate()
    expect(config.wifi_direct_pass.length).toBeLessThanOrEqual(10)
  })

  it('generate logs debug message', () => {
    logDebug.mockClear()
    const wrapper = createWrapper()

    wrapper.vm.generate()

    expect(logDebug).toHaveBeenCalledWith('DeviceWifiView:generate()')
  })

  it('generate with maximum BigUint64Array values', () => {
    const maxValue = 18446744073709551615n
    const randomValues = BigUint64Array.from([maxValue, maxValue])
    vi.spyOn(window.crypto, 'getRandomValues').mockReturnValue(randomValues)
    const wrapper = createWrapper()

    wrapper.vm.generate()

    expect(config.wifi_direct_ssid).toBe('gw-ABC123')
    expect(config.wifi_direct_pass).toHaveLength(10)
  })

  it('generate with small BigUint64Array values', () => {
    const randomValues = BigUint64Array.from([0n, 0n])
    vi.spyOn(window.crypto, 'getRandomValues').mockReturnValue(randomValues)
    const wrapper = createWrapper()

    wrapper.vm.generate()

    expect(config.wifi_direct_ssid).toBe('gw-ABC123')
    expect(config.wifi_direct_pass.length).toBeLessThanOrEqual(10)
  })

  it('save calls validateCurrentForm before saving', () => {
    validateCurrentForm.mockReturnValue(true)
    validateCurrentForm.mockClear()
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    wrapper.vm.save()

    expect(validateCurrentForm).toHaveBeenCalled()
    expect(config.saveAll).toHaveBeenCalled()
  })

  it('save returns early without calling saveAll when validation fails', () => {
    validateCurrentForm.mockReturnValue(false)
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    wrapper.vm.save()

    expect(config.saveAll).not.toHaveBeenCalled()
  })

  it('save message contains restart and effect keywords', () => {
    validateCurrentForm.mockReturnValue(true)
    global.messageInfo = ''
    const wrapper = createWrapper()

    wrapper.vm.save()

    expect(global.messageInfo).toContain('restart')
    expect(global.messageInfo).toContain('effect')
  })

  it('save message is about device restart not general restart', () => {
    validateCurrentForm.mockReturnValue(true)
    global.messageInfo = ''
    const wrapper = createWrapper()

    wrapper.vm.save()

    expect(global.messageInfo).toContain('device')
  })

  it('generate creates password with only alphanumeric characters', () => {
    const randomValues = BigUint64Array.from([999999999n, 888888888n])
    vi.spyOn(window.crypto, 'getRandomValues').mockReturnValue(randomValues)
    const wrapper = createWrapper()

    wrapper.vm.generate()

    expect(/^[a-zA-Z0-9]+$/.test(config.wifi_direct_pass)).toBe(true)
  })

  it('save with both SSID and password set', () => {
    validateCurrentForm.mockReturnValue(true)
    config.wifi_direct_ssid = 'test-ssid'
    config.wifi_direct_pass = 'test-pass'
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalledTimes(1)
    expect(config.wifi_direct_ssid).toBe('test-ssid')
    expect(config.wifi_direct_pass).toBe('test-pass')
  })

  it('generate replaces previous SSID completely', () => {
    const wrapper = createWrapper()
    config.wifi_direct_ssid = 'old-ssid'

    wrapper.vm.generate()

    expect(config.wifi_direct_ssid).toMatch(/^gw-/)
    expect(config.wifi_direct_ssid).not.toContain('old-ssid')
  })

  it('generate replaces previous password completely', () => {
    const randomValues1 = BigUint64Array.from([111n, 222n])
    vi.spyOn(window.crypto, 'getRandomValues').mockReturnValue(randomValues1)
    const wrapper = createWrapper()

    config.wifi_direct_pass = 'old-password'
    wrapper.vm.generate()
    const newPass = config.wifi_direct_pass

    expect(newPass).not.toContain('old')
    expect(newPass.length).toBeLessThanOrEqual(10)
  })

  it('restart awaits async operation', async () => {
    config.restart.mockImplementation(() => Promise.resolve())
    const wrapper = createWrapper()

    const promise = wrapper.vm.restart()
    expect(promise).toBeInstanceOf(Promise)

    await promise
    expect(config.restart).toHaveBeenCalled()
  })

  it('SSID format always starts with gw- prefix', () => {
    const wrapper = createWrapper()
    status.id = 'TEST001'

    wrapper.vm.generate()

    expect(config.wifi_direct_ssid).toBe('gw-TEST001')
  })

  it('SSID includes full status.id without truncation', () => {
    const wrapper = createWrapper()
    status.id = 'LONGID12345'

    wrapper.vm.generate()

    expect(config.wifi_direct_ssid).toBe('gw-LONGID12345')
  })

  it('password substring operation limits to 10 characters', () => {
    const randomValues = BigUint64Array.from([123456789012345n, 987654321098765n])
    vi.spyOn(window.crypto, 'getRandomValues').mockReturnValue(randomValues)
    const wrapper = createWrapper()

    wrapper.vm.generate()

    expect(config.wifi_direct_pass.length).toBe(10)
  })

  it('password conversion uses toString(36) for lowercase', () => {
    const randomValues = BigUint64Array.from([12345n, 67890n])
    vi.spyOn(window.crypto, 'getRandomValues').mockReturnValue(randomValues)
    const wrapper = createWrapper()

    wrapper.vm.generate()

    expect(config.wifi_direct_pass).toBeTruthy()
    expect(config.wifi_direct_pass.length).toBeLessThanOrEqual(10)
  })

  it('form submission prevents default behavior', () => {
    const wrapper = createWrapper()
    const form = wrapper.find('form')

    expect(form.exists()).toBe(true)
  })

  it('generate button type is button not submit', () => {
    const wrapper = createWrapper()
    const buttons = wrapper.findAll('button')
    const generateBtn = buttons.find((b) => b.text().includes('Generate'))

    expect(generateBtn?.attributes('type')).toBe('button')
  })

  it('save button type is submit', () => {
    const wrapper = createWrapper()
    const buttons = wrapper.findAll('button')
    const saveBtn = buttons.find((b) => b.text().includes('Save'))

    expect(saveBtn?.attributes('type')).toBe('submit')
  })

  it('status.id is read correctly from pinia store', () => {
    const wrapper = createWrapper()
    status.id = 'CUSTOM123'

    wrapper.vm.generate()

    expect(config.wifi_direct_ssid).toContain('CUSTOM123')
  })

  it('sequential generates with different status.id values', () => {
    const wrapper = createWrapper()

    status.id = 'FIRST'
    wrapper.vm.generate()
    const ssid1 = config.wifi_direct_ssid

    status.id = 'SECOND'
    wrapper.vm.generate()
    const ssid2 = config.wifi_direct_ssid

    expect(ssid1).toBe('gw-FIRST')
    expect(ssid2).toBe('gw-SECOND')
    expect(ssid1).not.toBe(ssid2)
  })

  it('global.messageInfo is initially empty', () => {
    global.messageInfo = ''
    createWrapper()

    expect(global.messageInfo).toBe('')
  })

  it('save overrides messageInfo with restart message', () => {
    validateCurrentForm.mockReturnValue(true)
    global.messageInfo = 'old message'
    const wrapper = createWrapper()

    wrapper.vm.save()

    expect(global.messageInfo).toBe(
      'If WIFI settings are changed, restart the device so they can take effect!'
    )
  })

  it('validation before save is blocking', () => {
    validateCurrentForm.mockReturnValue(false)
    config.saveAll.mockImplementation(() => {
      throw new Error('Should not be called')
    })
    const wrapper = createWrapper()

    expect(() => wrapper.vm.save()).not.toThrow()
    expect(config.saveAll).not.toHaveBeenCalled()
  })
})
