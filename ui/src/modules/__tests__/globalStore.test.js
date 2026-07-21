import { beforeEach, describe, expect, it, vi } from 'vitest'

describe('globalStore module', () => {
  let http

  const loadStore = async () => {
    vi.resetModules()
    vi.doUnmock('@/modules/globalStore')

    http = {
      getJson: vi.fn()
    }

    vi.doMock('pinia', () => ({
      defineStore: (_id, options) => () => {
        const store = { ...options.state() }
        store.$id = 'global'
        store.$state = store
        for (const [name, fn] of Object.entries(options.actions || {})) {
          store[name] = fn.bind(store)
        }
        const getters = options.getters || {}
        for (const [name, fn] of Object.entries(getters)) {
          Object.defineProperty(store, name, {
            get: () => fn.call(store),
            enumerable: true
          })
        }
        return store
      }
    }))

    vi.doMock('@mp-se/espframework-ui-components', () => ({
      logInfo: vi.fn(),
      logDebug: vi.fn(),
      logError: vi.fn(),
      sharedHttpClient: http
    }))

    const { useGlobalStore } = await vi.importActual('../globalStore.js')
    return useGlobalStore()
  }

  beforeEach(() => {
    vi.clearAllMocks()
  })

  it('clears all message fields', async () => {
    const store = await loadStore()
    store.messageError = 'e'
    store.messageWarning = 'w'
    store.messageSuccess = 's'
    store.messageInfo = 'i'

    store.clearMessages()

    expect(store.messageError).toBe('')
    expect(store.messageWarning).toBe('')
    expect(store.messageSuccess).toBe('')
    expect(store.messageInfo).toBe('')
  })

  it('derives message-state getters', async () => {
    const store = await loadStore()
    store.messageError = 'e'
    store.messageWarning = 'w'
    store.messageSuccess = 's'
    store.messageInfo = 'i'

    expect(store.isError).toBe(true)
    expect(store.isWarning).toBe(true)
    expect(store.isSuccess).toBe(true)
    expect(store.isInfo).toBe(true)
  })

  it('loads feature metadata from the device', async () => {
    const store = await loadStore()
    http.getJson.mockResolvedValue({
      board: 'devkit',
      app_ver: '1.2.3',
      app_build: '45',
      platform: 'esp32',
      firmware_file: 'FW.BIN',
      tft: true,
      sd: true
    })

    const result = await store.load()

    expect(result).toBe(true)
    expect(store.board).toBe('DEVKIT')
    expect(store.platform).toBe('ESP32')
    expect(store.firmware_file).toBe('fw.bin')
    expect(store.feature).toEqual({ tft: true, sd: true })
  })

  it('returns false when feature loading fails', async () => {
    const store = await loadStore()
    http.getJson.mockRejectedValue(new Error('network fail'))

    const result = await store.load()

    expect(result).toBe(false)
  })
})
