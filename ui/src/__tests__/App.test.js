import { beforeEach, describe, expect, it, vi } from 'vitest'
import { flushPromises, mount } from '@vue/test-utils'
import { reactive, toRef } from 'vue'

vi.mock('pinia', async () => {
  const actual = await vi.importActual('pinia')
  return {
    ...actual,
    storeToRefs(store) {
      return {
        disabled: toRef(store, 'disabled')
      }
    }
  }
})

vi.mock('@/modules/router', () => ({
  items: []
}))

import { sharedHttpClient as http } from '@mp-se/espframework-ui-components'
import App from '@/App.vue'
import { config, global, status } from '@/modules/pinia'

describe('App', () => {
  beforeEach(() => {
    vi.clearAllMocks()

    Object.assign(
      global,
      reactive({
        initialized: false,
        disabled: false,
        configChanged: false,
        messageError: '',
        messageWarning: '',
        messageSuccess: '',
        messageInfo: '',
        isError: false,
        isWarning: false,
        isSuccess: false,
        isInfo: false,
        clearMessages: vi.fn(),
        load: vi.fn(async () => true)
      })
    )

    Object.assign(status, {
      wifi_setup: false,
      load: vi.fn(async () => true)
    })

    Object.assign(config, {
      dark_mode: false,
      mdns: 'gateway-test',
      load: vi.fn(async () => true),
      loadFormat: vi.fn(async () => true)
    })

    http.auth.mockResolvedValue(true)
    document.body.style.cursor = 'default'
    document.documentElement.removeAttribute('data-bs-theme')
    document.body.innerHTML = ''
  })

  const mountApp = () => mount(App, { attachTo: document.body })

  it('initializes the app successfully and applies light theme by default', async () => {
    const wrapper = mountApp()
    await flushPromises()

    expect(http.auth).toHaveBeenCalled()
    expect(global.load).toHaveBeenCalled()
    expect(status.load).toHaveBeenCalled()
    expect(config.load).toHaveBeenCalled()
    expect(config.loadFormat).toHaveBeenCalled()
    expect(global.initialized).toBe(true)
    expect(document.documentElement.getAttribute('data-bs-theme')).toBe('light')
    expect(HTMLDialogElement.prototype.showModal).toHaveBeenCalled()
    expect(HTMLDialogElement.prototype.close).toHaveBeenCalled()

    wrapper.unmount()
  })

  it('stops initialization and sets an error when authentication fails', async () => {
    http.auth.mockResolvedValue(false)
    const wrapper = mountApp()
    await flushPromises()

    expect(global.messageError).toBe(
      'Failed to authenticate with device, please try to reload page!'
    )
    expect(global.load).not.toHaveBeenCalled()
    expect(global.initialized).toBe(false)

    wrapper.unmount()
  })

  it('applies dark theme when enabled in config during initialization', async () => {
    config.dark_mode = true
    const wrapper = mountApp()
    await flushPromises()

    expect(document.documentElement.getAttribute('data-bs-theme')).toBe('dark')

    wrapper.unmount()
  })

  it('renders wifi setup messages when the device is in wifi setup mode', async () => {
    status.wifi_setup = true
    const wrapper = mountApp()
    await flushPromises()

    expect(wrapper.text()).toContain('Running in WIFI setup mode.')
    expect(wrapper.text()).toContain('Sensors are not enabled when in wifi setup mode!')

    wrapper.unmount()
  })

  it('stops initialization and sets an error when global.load fails', async () => {
    global.load.mockResolvedValue(false)
    const wrapper = mountApp()
    await flushPromises()

    expect(global.messageError).toBe(
      'Failed to load feature flags from device, please try to reload page!'
    )
    expect(status.load).not.toHaveBeenCalled()
    expect(global.initialized).toBe(false)

    wrapper.unmount()
  })

  it('stops initialization and sets an error when status.load fails', async () => {
    status.load.mockResolvedValue(false)
    const wrapper = mountApp()
    await flushPromises()

    expect(global.messageError).toBe(
      'Failed to load status from device, please try to reload page!'
    )
    expect(config.load).not.toHaveBeenCalled()
    expect(global.initialized).toBe(false)

    wrapper.unmount()
  })

  it('stops initialization and sets an error when config.load fails', async () => {
    config.load.mockResolvedValue(false)
    const wrapper = mountApp()
    await flushPromises()

    expect(global.messageError).toBe(
      'Failed to load configuration data from device, please try to reload page!'
    )
    expect(config.loadFormat).not.toHaveBeenCalled()
    expect(global.initialized).toBe(false)

    wrapper.unmount()
  })

  it('stops initialization and sets an error when config.loadFormat fails', async () => {
    config.loadFormat.mockResolvedValue(false)
    const wrapper = mountApp()
    await flushPromises()

    expect(global.messageError).toBe(
      'Failed to load format templates from device, please try to reload page!'
    )
    expect(global.initialized).toBe(false)

    wrapper.unmount()
  })

  it('handles exceptions during initialization', async () => {
    http.auth.mockRejectedValue(new Error('Network timeout'))
    const wrapper = mountApp()
    await flushPromises()

    expect(global.messageError).toContain('Initialization failed:')
    expect(global.messageError).toContain('Network timeout')
    expect(global.initialized).toBe(false)

    wrapper.unmount()
  })

  it('closes error message when close handler is called with danger alert type', async () => {
    const wrapper = mountApp()
    global.messageError = 'Test error message'
    await flushPromises()

    wrapper.vm.close('danger')
    await wrapper.vm.$nextTick()

    expect(global.messageError).toBe('')

    wrapper.unmount()
  })

  it('closes warning message when close handler is called with warning alert type', async () => {
    const wrapper = mountApp()
    global.messageWarning = 'Test warning message'
    await flushPromises()

    wrapper.vm.close('warning')
    await wrapper.vm.$nextTick()

    expect(global.messageWarning).toBe('')

    wrapper.unmount()
  })

  it('closes success message when close handler is called with success alert type', async () => {
    const wrapper = mountApp()
    global.messageSuccess = 'Test success message'
    await flushPromises()

    wrapper.vm.close('success')
    await wrapper.vm.$nextTick()

    expect(global.messageSuccess).toBe('')

    wrapper.unmount()
  })

  it('closes info message when close handler is called with info alert type', async () => {
    const wrapper = mountApp()
    global.messageInfo = 'Test info message'
    await flushPromises()

    wrapper.vm.close('info')
    await wrapper.vm.$nextTick()

    expect(global.messageInfo).toBe('')

    wrapper.unmount()
  })
})
