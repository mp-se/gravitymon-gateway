import { beforeEach, describe, expect, it, vi } from 'vitest'

describe('statusStore module', () => {
  let http

  const loadStore = async () => {
    vi.resetModules()
    vi.doUnmock('@/modules/statusStore')

    http = {
      getJson: vi.fn()
    }

    vi.doMock('pinia', () => ({
      defineStore: (_id, options) => () => {
        const store = { ...options.state() }
        store.$id = 'status'
        store.$state = store
        for (const [name, fn] of Object.entries(options.actions || {})) {
          store[name] = fn.bind(store)
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

    const { useStatusStore } = await vi.importActual('../statusStore.js')
    return useStatusStore()
  }

  beforeEach(() => {
    vi.clearAllMocks()
  })

  it('loads and normalizes status data from the device', async () => {
    const store = await loadStore()
    http.getJson.mockResolvedValue({
      id: 'gw-1',
      rssi: -55,
      mdns: 'gw-test',
      wifi_ssid: 'brew-net',
      ip: '192.168.1.10',
      total_heap: 20480,
      free_heap: 10240,
      wifi_setup: true,
      gravity_device: [{ id: 1 }],
      pressure_device: [{ id: 2 }],
      temperature_device: [{ id: 3 }],
      uptime_seconds: 4,
      uptime_minutes: 3,
      uptime_hours: 2,
      uptime_days: 1,
      sd_mounted: true
    })

    const result = await store.load()

    expect(result).toBe(true)
    expect(store.id).toBe('gw-1')
    expect(store.wifi_ssid).toBe('brew-net')
    expect(store.gravity_device).toEqual([{ id: 1 }])
    expect(store.total_heap).toBe('20')
    expect(store.free_heap).toBe('10')
    expect(store.sd_mounted).toBe(true)
  })

  it('returns false when status loading fails', async () => {
    const store = await loadStore()
    http.getJson.mockRejectedValue(new Error('network fail'))

    const result = await store.load()

    expect(result).toBe(false)
  })
})
