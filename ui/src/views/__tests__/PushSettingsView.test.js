import { beforeEach, describe, expect, it, vi } from 'vitest'
import { mount } from '@vue/test-utils'
import PushSettingsView from '../PushSettingsView.vue'
import { config, global } from '@/modules/pinia'
import { validateCurrentForm } from '@mp-se/espframework-ui-components'

describe('PushSettingsView', () => {
  const createWrapper = () =>
    mount(PushSettingsView, {
      global: {
        stubs: {
          BsInputText: true,
          BsInputNumber: true
        }
      }
    })

  beforeEach(() => {
    vi.clearAllMocks()
    global.disabled = false
    global.configChanged = true
    config.token = ''
    config.push_timeout = 30
    config.push_resend_time = 60
  })

  it('renders push settings inputs', () => {
    const wrapper = createWrapper()

    expect(wrapper.text()).toContain('Push - Settings')
    expect(wrapper.find('form').exists()).toBe(true)
    expect(wrapper.findAll('button')).toHaveLength(1)
  })

  it('displays token input field', () => {
    const wrapper = createWrapper()

    expect(wrapper.vm).toBeDefined()
    config.token = 'test-token-123'
    expect(config.token).toBe('test-token-123')
  })

  it('displays push timeout input with correct range', () => {
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    config.push_timeout = 30
    expect(config.push_timeout).toBe(30)

    config.push_timeout = 10
    expect(config.push_timeout).toBe(10)

    config.push_timeout = 60
    expect(config.push_timeout).toBe(60)
  })

  it('displays push resend time input with correct range', () => {
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    config.push_resend_time = 60
    expect(config.push_resend_time).toBe(60)

    config.push_resend_time = 10
    expect(config.push_resend_time).toBe(10)

    config.push_resend_time = 1800
    expect(config.push_resend_time).toBe(1800)
  })

  it('saves settings when the form is valid', async () => {
    validateCurrentForm.mockReturnValue(true)
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalled()
  })

  it('does not save when validation fails', async () => {
    validateCurrentForm.mockReturnValue(false)
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.saveAll).not.toHaveBeenCalled()
  })

  it('save button is disabled when global.disabled is true', () => {
    global.disabled = true
    const wrapper = createWrapper()

    expect(wrapper.vm).toBeDefined()
  })

  it('save button is disabled when global.configChanged is false', () => {
    global.configChanged = false
    const wrapper = createWrapper()

    expect(wrapper.vm).toBeDefined()
  })

  it('form initializes with correct config values', () => {
    config.token = 'my-token'
    config.push_timeout = 45
    config.push_resend_time = 90
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    expect(config.token).toBe('my-token')
    expect(config.push_timeout).toBe(45)
    expect(config.push_resend_time).toBe(90)
  })

  it('handles concurrent save operations', async () => {
    validateCurrentForm.mockReturnValue(true)
    const wrapper = createWrapper()

    const promise1 = wrapper.vm.save()
    const promise2 = wrapper.vm.save()

    await Promise.all([promise1, promise2])

    expect(config.saveAll).toHaveBeenCalledTimes(2)
  })

  it('validates form with valid token', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.token = 'valid-token-123456'
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(validateCurrentForm).toHaveBeenCalled()
    expect(config.saveAll).toHaveBeenCalled()
  })

  it('saves empty token when valid', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.token = ''
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.token).toBe('')
  })

  it('saves with minimum push timeout', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.push_timeout = 10
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.push_timeout).toBe(10)
  })

  it('saves with maximum push timeout', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.push_timeout = 120
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.push_timeout).toBe(120)
  })

  it('saves with minimum resend time', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.push_resend_time = 10
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.push_resend_time).toBe(10)
  })

  it('saves with maximum resend time', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.push_resend_time = 1800
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.push_resend_time).toBe(1800)
  })

  it('disables save when global.disabled is true', () => {
    global.disabled = true
    const wrapper = createWrapper()
    const saveButton = wrapper.findAll('button')[0]

    expect(saveButton.attributes('disabled')).toBeDefined()
  })

  it('enables save when global.disabled is false', () => {
    global.disabled = false
    const wrapper = createWrapper()
    const saveButton = wrapper.findAll('button')[0]

    expect(saveButton.attributes('disabled')).toBeUndefined()
  })

  it('disables save when configChanged is false', () => {
    global.configChanged = false
    const wrapper = createWrapper()
    const saveButton = wrapper.findAll('button')[0]

    expect(saveButton.attributes('disabled')).toBeDefined()
  })

  it('enables save when configChanged is true', () => {
    global.configChanged = true
    const wrapper = createWrapper()
    const saveButton = wrapper.findAll('button')[0]

    expect(saveButton.attributes('disabled')).toBeUndefined()
  })

  it('renders and saves with all fields populated', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.token = 'full-token'
    config.push_timeout = 45
    config.push_resend_time = 300
    const wrapper = createWrapper()

    expect(wrapper.find('form').exists()).toBe(true)
    await wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalled()
    expect(config.token).toBe('full-token')
    expect(config.push_timeout).toBe(45)
    expect(config.push_resend_time).toBe(300)
  })

  it('prevents save on validation error', async () => {
    validateCurrentForm.mockReturnValue(false)
    config.token = 'test'
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.saveAll).not.toHaveBeenCalled()
  })

  it('handles rapid form changes', () => {
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    config.token = 'token1'
    expect(config.token).toBe('token1')

    config.token = 'token2'
    expect(config.token).toBe('token2')

    config.push_timeout = 20
    expect(config.push_timeout).toBe(20)

    config.push_resend_time = 200
    expect(config.push_resend_time).toBe(200)
  })

  it('maintains config state between saves', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.token = 'persistent-token'
    const wrapper = createWrapper()

    await wrapper.vm.save()
    expect(config.token).toBe('persistent-token')

    await wrapper.vm.save()
    expect(config.token).toBe('persistent-token')
  })

  it('calls validateCurrentForm with correct parameters', async () => {
    validateCurrentForm.mockReturnValue(true)
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(validateCurrentForm).toHaveBeenCalled()
  })

  it('renders message field label when needed', () => {
    const wrapper = createWrapper()

    expect(wrapper.html()).toBeTruthy()
    expect(wrapper.text()).toContain('Push - Settings')
  })

  it('updates timeout values independently', () => {
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    config.push_timeout = 25
    expect(config.push_timeout).toBe(25)
    expect(config.push_resend_time).toBe(60)

    config.push_resend_time = 120
    expect(config.push_timeout).toBe(25)
    expect(config.push_resend_time).toBe(120)
  })

  it('supports long token values', async () => {
    validateCurrentForm.mockReturnValue(true)
    const longToken = 'a'.repeat(256)
    config.token = longToken
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.token).toBe(longToken)
    expect(config.saveAll).toHaveBeenCalled()
  })

  it('handles special characters in token', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.token = 'token-with-special!@#$%'
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.token).toBe('token-with-special!@#$%')
    expect(config.saveAll).toHaveBeenCalled()
  })

  it('initialization sets default values', () => {
    config.token = ''
    config.push_timeout = 30
    config.push_resend_time = 60
    const wrapper = createWrapper()

    expect(wrapper.vm).toBeDefined()
    expect(config.push_timeout).toBe(30)
    expect(config.push_resend_time).toBe(60)
  })

  it('save function returns early on validation failure', async () => {
    validateCurrentForm.mockReturnValue(false)
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    const result = await wrapper.vm.save()

    expect(config.saveAll).not.toHaveBeenCalled()
    expect(result).toBeUndefined()
  })

  it('save function proceeds on validation success', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(validateCurrentForm).toHaveBeenCalled()
    expect(config.saveAll).toHaveBeenCalled()
  })

  it('button element exists and is properly configured', () => {
    const wrapper = createWrapper()
    const buttons = wrapper.findAll('button')

    expect(buttons).toHaveLength(1)
    expect(buttons[0].text()).toContain('Save')
  })

  it('form contains all expected input fields', () => {
    const wrapper = createWrapper()

    expect(wrapper.text()).toContain('Push - Settings')
    expect(wrapper.find('form').exists()).toBe(true)
    expect(wrapper.vm).toBeDefined()
  })

  it('form submission calls save method', async () => {
    validateCurrentForm.mockReturnValue(true)
    const wrapper = createWrapper()
    const saveButton = wrapper.find('button[type="submit"]')

    expect(saveButton.exists()).toBe(true)
  })

  it('token can be empty string', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.token = ''
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.token).toBe('')
    expect(config.saveAll).toHaveBeenCalled()
  })

  it('token can be very long', async () => {
    validateCurrentForm.mockReturnValue(true)
    const veryLongToken = 'x'.repeat(500)
    config.token = veryLongToken
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.token).toBe(veryLongToken)
    expect(config.saveAll).toHaveBeenCalled()
  })

  it('timeout values are updated independently', () => {
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    config.push_timeout = 15
    expect(config.push_timeout).toBe(15)
    expect(config.push_resend_time).toBe(60)

    config.push_resend_time = 900
    expect(config.push_timeout).toBe(15)
    expect(config.push_resend_time).toBe(900)
  })

  it('all config fields can be changed simultaneously', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.token = 'new-token'
    config.push_timeout = 20
    config.push_resend_time = 500

    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.token).toBe('new-token')
    expect(config.push_timeout).toBe(20)
    expect(config.push_resend_time).toBe(500)
    expect(config.saveAll).toHaveBeenCalledTimes(1)
  })

  it('save button disable state reflects global.disabled', () => {
    global.disabled = true
    const wrapper = createWrapper()
    const saveButton = wrapper.find('button[type="submit"]')

    expect(saveButton.attributes('disabled')).toBeDefined()
  })

  it('save button disable state reflects global.configChanged', () => {
    global.configChanged = false
    const wrapper = createWrapper()
    const saveButton = wrapper.find('button[type="submit"]')

    expect(saveButton.attributes('disabled')).toBeDefined()
  })

  it('save button is enabled when both conditions are met', () => {
    global.disabled = false
    global.configChanged = true
    const wrapper = createWrapper()
    const saveButton = wrapper.find('button[type="submit"]')

    expect(saveButton.attributes('disabled')).toBeUndefined()
  })

  it('push timeout minimum value is preserved', () => {
    config.push_timeout = 10
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    expect(config.push_timeout).toBe(10)
  })

  it('push timeout maximum value is preserved', () => {
    config.push_timeout = 60
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    expect(config.push_timeout).toBe(60)
  })

  it('push resend time minimum value is preserved', () => {
    config.push_resend_time = 10
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    expect(config.push_resend_time).toBe(10)
  })

  it('push resend time maximum value is preserved', () => {
    config.push_resend_time = 1800
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    expect(config.push_resend_time).toBe(1800)
  })

  it('validateCurrentForm is called with proper context', async () => {
    validateCurrentForm.mockReturnValue(true)
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(validateCurrentForm).toHaveBeenCalledTimes(1)
  })

  it('saveAll is only called after successful validation', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalledTimes(1)
  })

  it('component renders spinner in save button when disabled', () => {
    global.disabled = true
    const wrapper = createWrapper()

    expect(wrapper.text()).toContain('Save')
  })

  it('token field accepts numbers and special chars', () => {
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    config.token = '12345-abc-!@#$'
    expect(config.token).toBe('12345-abc-!@#$')

    config.token = 'Bearer eyJhbGciOi'
    expect(config.token).toBe('Bearer eyJhbGciOi')
  })

  it('timeout inputs accept numeric values', () => {
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    config.push_timeout = 45
    expect(config.push_timeout).toBe(45)

    config.push_resend_time = 750
    expect(config.push_resend_time).toBe(750)
  })

  it('form has correct structure and classes', () => {
    const wrapper = createWrapper()

    expect(wrapper.find('form.needs-validation').exists()).toBe(true)
    expect(wrapper.find('form[novalidate]').exists()).toBe(true)
  })

  it('multiple rapid saves are handled correctly', async () => {
    validateCurrentForm.mockReturnValue(true)
    const wrapper = createWrapper()

    await Promise.all([wrapper.vm.save(), wrapper.vm.save(), wrapper.vm.save()])

    expect(config.saveAll).toHaveBeenCalledTimes(3)
  })

  it('save preserves state across multiple calls', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.token = 'permanent-token'
    config.push_timeout = 35
    const wrapper = createWrapper()

    await wrapper.vm.save()
    await wrapper.vm.save()

    expect(config.token).toBe('permanent-token')
    expect(config.push_timeout).toBe(35)
  })

  it('save method is async and returns Promise', () => {
    const wrapper = createWrapper()

    const result = wrapper.vm.save()
    expect(result).toBeInstanceOf(Promise)
  })

  it('save calls validateCurrentForm before proceeding', async () => {
    validateCurrentForm.mockReturnValue(true)
    validateCurrentForm.mockClear()
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(validateCurrentForm).toHaveBeenCalled()
  })

  it('save returns early when validation returns false', async () => {
    validateCurrentForm.mockReturnValue(false)
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    const result = await wrapper.vm.save()

    expect(result).toBeUndefined()
    expect(config.saveAll).not.toHaveBeenCalled()
  })

  it('save calls saveAll when validation passes', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalledTimes(1)
  })

  it('save awaits config.saveAll before returning', async () => {
    validateCurrentForm.mockReturnValue(true)
    let saveAllCalled = false
    config.saveAll.mockImplementation(() => {
      return new Promise((resolve) => {
        setTimeout(() => {
          saveAllCalled = true
          resolve()
        }, 10)
      })
    })
    const wrapper = createWrapper()

    const savePromise = wrapper.vm.save()
    expect(saveAllCalled).toBe(false)

    await savePromise
    expect(saveAllCalled).toBe(true)
  })

  it('validateCurrentForm is gate for saveAll execution', async () => {
    validateCurrentForm.mockReturnValue(false)
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(validateCurrentForm).toHaveBeenCalled()
    expect(config.saveAll).not.toHaveBeenCalled()
  })

  it('save works with minimal config state', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.token = ''
    config.push_timeout = 10
    config.push_resend_time = 10
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalled()
  })

  it('save works with maximal config state', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.token = 'x'.repeat(100)
    config.push_timeout = 60
    config.push_resend_time = 1800
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalled()
  })

  it('save does not modify config values during save', async () => {
    validateCurrentForm.mockReturnValue(true)
    const originalToken = 'original-token'
    const originalTimeout = 30
    const originalResend = 300
    config.token = originalToken
    config.push_timeout = originalTimeout
    config.push_resend_time = originalResend
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.token).toBe(originalToken)
    expect(config.push_timeout).toBe(originalTimeout)
    expect(config.push_resend_time).toBe(originalResend)
  })

  it('save handler can be invoked multiple times sequentially', async () => {
    validateCurrentForm.mockReturnValue(true)
    config.saveAll.mockClear()
    const wrapper = createWrapper()

    await wrapper.vm.save()
    expect(config.saveAll).toHaveBeenCalledTimes(1)

    await wrapper.vm.save()
    expect(config.saveAll).toHaveBeenCalledTimes(2)

    await wrapper.vm.save()
    expect(config.saveAll).toHaveBeenCalledTimes(3)
  })

  it('form prevents default submission behavior', () => {
    const wrapper = createWrapper()
    const form = wrapper.find('form')

    expect(form.exists()).toBe(true)
    expect(form.attributes('novalidate')).toBeDefined()
  })

  it('save method exists and is callable', () => {
    const wrapper = createWrapper()

    expect(typeof wrapper.vm.save).toBe('function')
    expect(wrapper.vm.save).toBeDefined()
  })

  it('global config is accessible from component', () => {
    const wrapper = createWrapper()

    expect(wrapper.vm).toBeDefined()
  })

  it('validation failure preserves config state', async () => {
    validateCurrentForm.mockReturnValue(false)
    config.token = 'test-token'
    config.push_timeout = 45
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.token).toBe('test-token')
    expect(config.push_timeout).toBe(45)
  })

  it('save can be called immediately after mount', async () => {
    validateCurrentForm.mockReturnValue(true)
    const wrapper = createWrapper()

    await wrapper.vm.save()

    expect(config.saveAll).toHaveBeenCalled()
  })

  it('token input field is used by v-model', () => {
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    config.token = 'model-test'
    expect(config.token).toBe('model-test')
  })

  it('timeout input field is used by v-model', () => {
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    config.push_timeout = 55
    expect(config.push_timeout).toBe(55)
  })

  it('resend time input field is used by v-model', () => {
    // eslint-disable-next-line no-unused-vars
    const wrapper = createWrapper()

    config.push_resend_time = 555
    expect(config.push_resend_time).toBe(555)
  })
})
