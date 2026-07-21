import { beforeEach, describe, expect, it, vi } from 'vitest'

describe('pinia module helpers', () => {
  const loadModule = async () => {
    vi.resetModules()
    vi.doUnmock('@/modules/pinia')

    const configStore = {
      alpha: 1,
      beta: 'x',
      sendConfig: vi.fn(),
      $id: 'config',
      $subscribe: vi.fn()
    }
    const globalStore = {
      initialized: true,
      configChanged: false
    }
    const statusStore = {}
    const measurementStore = {}

    vi.doMock('pinia', () => ({
      createPinia: vi.fn(() => ({ id: 'pinia' }))
    }))

    vi.doMock('@/modules/configStore', () => ({
      useConfigStore: vi.fn(() => configStore)
    }))
    vi.doMock('@/modules/globalStore', () => ({
      useGlobalStore: vi.fn(() => globalStore)
    }))
    vi.doMock('@/modules/statusStore', () => ({
      useStatusStore: vi.fn(() => statusStore)
    }))
    vi.doMock('@/modules/measurementStore', () => ({
      useMeasurementStore: vi.fn(() => measurementStore)
    }))
    vi.doMock('@mp-se/espframework-ui-components', () => ({
      logInfo: vi.fn()
    }))

    const module = await vi.importActual('../pinia.js')
    return { module, configStore, globalStore }
  }

  beforeEach(() => {
    vi.clearAllMocks()
  })

  it('saves a config snapshot and resets configChanged', async () => {
    const { module, globalStore } = await loadModule()

    module.saveConfigState()

    expect(globalStore.configChanged).toBe(false)
    expect(module.getConfigChanges()).toEqual({})
  })

  it('returns changed config values compared to the saved snapshot', async () => {
    const { module, configStore, globalStore } = await loadModule()
    module.saveConfigState()
    configStore.alpha = 2

    expect(module.getConfigChanges()).toEqual({ alpha: 2 })

    globalStore.initialized = true
    const subscribeHandler = configStore.$subscribe.mock.calls[0][0]
    subscribeHandler()
    expect(globalStore.configChanged).toBe(true)
  })

  it('keeps configChanged false when there are no saved changes or initialization is incomplete', async () => {
    const { module, configStore, globalStore } = await loadModule()

    expect(module.getConfigChanges()).toEqual({})

    const subscribeHandler = configStore.$subscribe.mock.calls[0][0]
    globalStore.initialized = false
    subscribeHandler()
    expect(globalStore.configChanged).toBe(false)

    globalStore.initialized = true
    module.saveConfigState()
    subscribeHandler()
    expect(globalStore.configChanged).toBe(false)
  })
})
