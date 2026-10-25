import { beforeEach, describe, expect, it, vi } from 'vitest'

describe('configStore module', () => {
  let http
  let globalStore
  let saveConfigStateMock
  let getConfigChangesMock

  const loadStore = async () => {
    vi.resetModules()
    vi.doUnmock('@/modules/configStore')
    http = {
      getJson: vi.fn(),
      postJson: vi.fn(),
      request: vi.fn(),
      restart: vi.fn(),
      getErrorString: vi.fn((code) => `error-${code}`)
    }
    globalStore = {
      disabled: false,
      messageError: '',
      messageSuccess: '',
      messageWarning: '',
      clearMessages: vi.fn()
    }
    saveConfigStateMock = vi.fn()
    getConfigChangesMock = vi.fn(() => ({}))

    vi.doMock('pinia', () => ({
      defineStore: (_id, options) => () => {
        const state = options.state()
        const store = { ...state }
        store.$id = 'config'
        store.$state = store
        for (const [name, fn] of Object.entries(options.actions)) {
          store[name] = fn.bind(store)
        }
        return store
      }
    }))

    vi.doMock('@/modules/pinia', () => ({
      global: globalStore,
      saveConfigState: saveConfigStateMock,
      getConfigChanges: getConfigChangesMock
    }))

    vi.doMock('@mp-se/espframework-ui-components', () => ({
      logDebug: vi.fn(),
      logError: vi.fn(),
      logInfo: vi.fn(),
      tempToF: (c) => (c * 9) / 5 + 32,
      psiToBar: (psi) => psi * 0.0689476,
      psiToKPa: (psi) => psi * 6.89476,
      gravityToPlato: (gravity) => Number(gravity).toFixed(1),
      sharedHttpClient: http
    }))

    const { useConfigStore } = await vi.importActual('../configStore.js')
    return useConfigStore()
  }

  beforeEach(() => {
    vi.clearAllMocks()
  })

  it('reports the selected pressure unit helpers', async () => {
    const store = await loadStore()
    store.pressure_unit = 'PSI'
    expect(store.isPsi()).toBe(true)
    store.pressure_unit = 'Bar'
    expect(store.isBar()).toBe(true)
    store.pressure_unit = 'kPa'
    expect(store.isKPa()).toBe(true)
  })

  it('serializes current store state to JSON', async () => {
    const store = await loadStore()
    store.mdns = 'gateway-test'
    store.token = 'secret'

    const json = JSON.parse(store.toJson())

    expect(json.mdns).toBe('gateway-test')
    expect(json.token).toBe('secret')
  })

  it('loads configuration values from the device', async () => {
    const store = await loadStore()
    http.getJson.mockResolvedValue({
      id: 'gw-1',
      mdns: 'gw-test',
      temp_unit: 'C',
      gravity_unit: 'G',
      pressure_unit: 'PSI',
      dark_mode: true,
      ble_enable: true,
      ble_active_scan: false,
      ble_scan_time: 7,
      timezone: 'UTC',
      sd_log_files: 12,
      sd_log_min_time: 10,
      display_layout_id: 1,
      wifi_portal_timeout: 30,
      wifi_connect_timeout: 12,
      use_wifi_direct: true,
      wifi_ssid: 'Home',
      wifi_ssid2: 'Guest',
      wifi_pass: 'pw1',
      wifi_pass2: 'pw2',
      wifi_direct_ssid: 'direct',
      wifi_direct_pass: 'directpw',
      token: 'token',
      push_timeout: 45,
      push_resend_time: 90,
      http_post_target: 'https://post1',
      http_post_header1: 'h1',
      http_post_header2: 'h2',
      http_post2_target: 'https://post2',
      http_post2_header1: 'h3',
      http_post2_header2: 'h4',
      http_get_target: 'https://get',
      http_get_header1: 'h5',
      http_get_header2: 'h6',
      influxdb2_target: 'https://influx',
      influxdb2_org: 'org',
      influxdb2_bucket: 'bucket',
      influxdb2_token: 'token2',
      mqtt_target: 'mqtt://broker',
      mqtt_port: 1883,
      mqtt_user: 'user',
      mqtt_pass: 'pass',
      http_post_gravity: true,
      http_post_pressure: false,
      http_post2_gravity: false,
      http_post2_pressure: true,
      http_get_gravity: true,
      http_get_pressure: true,
      influxdb2_gravity: false,
      influxdb2_pressure: true,
      mqtt_gravity: true,
      mqtt_pressure: false
    })

    const result = await store.load()

    expect(result).toBe(true)
    expect(store.mdns).toBe('gw-test')
    expect(store.wifi_direct_ssid).toBe('direct')
    expect(store.influxdb2_bucket).toBe('bucket')
    expect(store.mqtt_port).toBe(1883)
    expect(globalStore.disabled).toBe(false)
  })

  it('loads and decodes format payloads', async () => {
    const store = await loadStore()
    http.getJson.mockResolvedValue({
      http_post_format_gravity: encodeURIComponent('gravity=${gravity}'),
      http_post2_format_gravity: encodeURIComponent('gravity2=${gravity}'),
      http_get_format_gravity: encodeURIComponent('get=${gravity}'),
      influxdb2_format_gravity: encodeURIComponent('influx=${gravity}'),
      mqtt_format_gravity: encodeURIComponent('topic:gravity|topic:temp'),
      http_post_format_pressure: encodeURIComponent('pressure=${pressure}'),
      http_post2_format_pressure: encodeURIComponent('pressure2=${pressure}'),
      http_get_format_pressure: encodeURIComponent('getp=${pressure}'),
      influxdb2_format_pressure: encodeURIComponent('influxp=${pressure}'),
      mqtt_format_pressure: encodeURIComponent('ptopic:pressure|ptopic:temp')
    })

    const result = await store.loadFormat()

    expect(result).toBe(true)
    expect(store.http_post_format_gravity).toBe('gravity=${gravity}')
    expect(store.mqtt_format_gravity).toContain('topic:gravity|\n')
    expect(store.http_get_format_pressure).toBe('getp=${pressure}')
  })

  it('sends config changes without format fields', async () => {
    const store = await loadStore()
    getConfigChangesMock.mockReturnValue({
      mdns: 'gw-new',
      http_post_format_gravity: 'ignore-me',
      mqtt_format_pressure: 'ignore-too',
      push_timeout: 42
    })

    const result = await store.sendConfig()

    expect(result).toBe(true)
    expect(http.postJson).toHaveBeenCalledWith('api/config', {
      mdns: 'gw-new',
      push_timeout: 42
    })
  })

  it('skips sending config when no changes are present', async () => {
    const store = await loadStore()
    getConfigChangesMock.mockReturnValue({})

    const result = await store.sendConfig()

    expect(result).toBe(true)
    expect(http.postJson).not.toHaveBeenCalled()
  })

  it('encodes and submits one format payload', async () => {
    const store = await loadStore()
    http.postJson.mockResolvedValue({})

    const result = await store.sendOneFormat({ http_post_format_gravity: 'gravity%3Dvalue' })

    expect(result).toBe(true)
    expect(http.postJson).toHaveBeenCalledWith('api/format', {
      http_post_format_gravity: 'gravity%3Dvalue'
    })
  })

  it('orchestrates saveAll through config and format sends', async () => {
    const store = await loadStore()
    store.sendConfig = vi.fn(async () => true)
    store.sendFormat = vi.fn(async () => true)

    await store.saveAll()

    expect(globalStore.clearMessages).toHaveBeenCalled()
    expect(store.sendConfig).toHaveBeenCalled()
    expect(store.sendFormat).toHaveBeenCalled()
    expect(globalStore.messageSuccess).toContain('saved to device')
    expect(saveConfigStateMock).toHaveBeenCalled()
    expect(globalStore.disabled).toBe(false)
  })

  it('handles restart success responses', async () => {
    const store = await loadStore()
    store.mdns = 'gw-test'
    http.restart.mockResolvedValue({
      success: true,
      json: {
        status: true,
        message: 'Restarting.'
      }
    })

    await store.restart()

    expect(globalStore.clearMessages).toHaveBeenCalled()
    expect(globalStore.messageSuccess).toContain('Redirecting to http://gw-test.local')
    expect(globalStore.disabled).toBe(false)
  })

  it('runs wifi scans until a completed status is returned', async () => {
    const store = await loadStore()
    vi.useFakeTimers()
    store.sendWifiScan = vi.fn(async () => true)
    store.getWifiScanStatus = vi
      .fn()
      .mockResolvedValueOnce({ success: true, data: { status: true } })
      .mockResolvedValueOnce({
        success: true,
        data: { status: false, success: true, networks: [] }
      })

    const resultPromise = store.runWifiScan()
    await vi.runAllTimersAsync()
    const result = await resultPromise

    expect(result).toEqual({ success: true, data: { status: false, success: true, networks: [] } })
    expect(store.getWifiScanStatus).toHaveBeenCalledTimes(2)
    vi.useRealTimers()
  })

  it('sends and counts format payloads, stripping mqtt newlines', async () => {
    const store = await loadStore()
    store.sendOneFormat = vi.fn(async (data) => Object.keys(data).length <= 1)
    getConfigChangesMock.mockReturnValue({
      http_post_format_gravity: 'gravity=${gravity}',
      mqtt_format_gravity: 'topic:one|\nvalue:two',
      http_post_format_pressure: 'pressure=${pressure}'
    })

    const result = await store.sendFormat()

    expect(result).toBe(true)
    expect(store.sendOneFormat).toHaveBeenCalledWith({
      http_post_format_gravity: encodeURIComponent('gravity=${gravity}')
    })
    expect(store.sendOneFormat).toHaveBeenCalledWith({
      mqtt_format_gravity: encodeURIComponent('topic:one|value:two')
    })
    expect(store.sendOneFormat).toHaveBeenCalledWith({
      http_post_format_pressure: encodeURIComponent('pressure=${pressure}')
    })
  })

  it('sends push test payloads and handles request failures', async () => {
    const store = await loadStore()
    http.postJson.mockResolvedValue({})
    expect(await store.sendPushTest({ push_format: 'mqtt_format_gravity' })).toBe(true)

    http.postJson.mockRejectedValueOnce(new Error('push failed'))
    expect(await store.sendPushTest({ push_format: 'mqtt_format_gravity' })).toBe(false)
  })

  it('handles sleep mode request success, http failure, and exception', async () => {
    const store = await loadStore()
    http.request.mockResolvedValueOnce({ ok: true, json: vi.fn().mockResolvedValue({}) })
    expect(await store.setSleepMode(true)).toBe(true)

    http.request.mockResolvedValueOnce({ ok: false, status: 500, statusText: 'Boom' })
    expect(await store.setSleepMode(false)).toBe(false)

    http.request.mockRejectedValueOnce(new Error('network fail'))
    expect(await store.setSleepMode(true)).toBe(false)
  })

  it('covers restart error branches', async () => {
    const store = await loadStore()
    http.restart.mockResolvedValueOnce({ success: true, json: { status: false, message: 'Nope' } })
    await store.restart()
    expect(globalStore.messageError).toBe('Nope')

    globalStore.messageError = ''
    http.restart.mockResolvedValueOnce({ success: false })
    await store.restart()
    expect(globalStore.messageError).toBe('Failed to request restart')

    globalStore.messageError = ''
    http.restart.mockRejectedValueOnce(new Error('network fail'))
    await store.restart()
    expect(globalStore.messageError).toBe('Failed to do restart')
  })

  it('gets push test status and handles failures', async () => {
    const store = await loadStore()
    http.getJson.mockResolvedValueOnce({ status: false, success: true })
    expect(await store.getPushTestStatus()).toEqual({
      success: true,
      data: { status: false, success: true }
    })

    http.getJson.mockRejectedValueOnce(new Error('status fail'))
    expect(await store.getPushTestStatus()).toEqual({ success: false, data: null })
  })

  it('covers wifi and hardware status request helpers', async () => {
    const store = await loadStore()
    http.request.mockResolvedValueOnce({})
    expect(await store.sendWifiScan()).toBe(true)
    http.request.mockRejectedValueOnce(new Error('wifi fail'))
    expect(await store.sendWifiScan()).toBe(false)

    http.getJson.mockResolvedValueOnce({ status: false, success: true })
    expect(await store.getWifiScanStatus()).toEqual({
      success: true,
      data: { status: false, success: true }
    })
    http.getJson.mockRejectedValueOnce(new Error('wifi status fail'))
    expect(await store.getWifiScanStatus()).toEqual({ success: false, data: null })

    http.request.mockResolvedValueOnce({})
    expect(await store.sendHardwareScan()).toBe(true)
    http.request.mockRejectedValueOnce(new Error('hw fail'))
    expect(await store.sendHardwareScan()).toBe(false)

    http.getJson.mockResolvedValueOnce({ status: false, success: true })
    expect(await store.getHardwareScanStatus()).toEqual({
      success: true,
      data: { status: false, success: true }
    })
    http.getJson.mockRejectedValueOnce(new Error('hw status fail'))
    expect(await store.getHardwareScanStatus()).toEqual({ success: false, data: null })
  })

  it('covers saveAll and runPushTest failure branches', async () => {
    const store = await loadStore()

    store.sendConfig = vi.fn(async () => false)
    await store.saveAll()
    expect(globalStore.messageError).toBe('Failed to store configuration to device')

    globalStore.messageError = ''
    store.sendConfig = vi.fn(async () => true)
    store.sendFormat = vi.fn(async () => false)
    await store.saveAll()
    expect(globalStore.messageError).toBe('Failed to store format to device')

    globalStore.messageError = ''
    store.sendConfig = vi.fn(async () => {
      throw new Error('save fail')
    })
    await store.saveAll()
    expect(globalStore.messageError).toBe('Failed to save configuration')

    store.sendPushTest = vi.fn(async () => false)
    expect(await store.runPushTest({ push_format: 'x' })).toBe(false)
    expect(globalStore.messageError).toBe('Failed to start push test')

    globalStore.messageError = ''
    store.sendPushTest = vi.fn(async () => true)
    store.getPushTestStatus = vi.fn(async () => ({ success: false, data: null }))
    expect(await store.runPushTest({ push_format: 'x' })).toBe(false)
    expect(globalStore.messageError).toBe('Failed to get push test status')
  })

  it('covers push test and hardware scan polling results', async () => {
    const store = await loadStore()
    vi.useFakeTimers()

    store.sendPushTest = vi.fn(async () => true)
    store.getPushTestStatus = vi
      .fn()
      .mockResolvedValueOnce({ success: true, data: { status: true } })
      .mockResolvedValueOnce({
        success: true,
        data: { status: false, success: false, push_return_code: 12 }
      })
    expect(
      await (async () => {
        const resultPromise = store.runPushTest({ push_format: 'x' })
        await vi.runAllTimersAsync()
        return resultPromise
      })()
    ).toBe(true)
    expect(globalStore.messageError).toContain('12')

    globalStore.messageError = ''
    globalStore.messageWarning = ''
    globalStore.messageSuccess = ''
    store.getPushTestStatus = vi.fn(async () => ({
      success: true,
      data: { status: false, success: true, push_enabled: false, push_return_code: 0 }
    }))
    expect(await store.runPushTest({ push_format: 'x' })).toBe(true)
    expect(globalStore.messageWarning).toContain('No endpoint')

    globalStore.messageWarning = ''
    store.getPushTestStatus = vi.fn(async () => ({
      success: true,
      data: { status: false, success: true, push_enabled: true, push_return_code: 0 }
    }))
    expect(await store.runPushTest({ push_format: 'x' })).toBe(true)
    expect(globalStore.messageSuccess).toBe('Test was successful')

    store.sendHardwareScan = vi.fn(async () => true)
    store.getHardwareScanStatus = vi
      .fn()
      .mockResolvedValueOnce({ success: true, data: { status: true } })
      .mockResolvedValueOnce({ success: true, data: { status: false, success: true, found: [] } })
    const hwPromise = store.runHardwareScan()
    await vi.runAllTimersAsync()
    expect(await hwPromise).toEqual({
      success: true,
      data: { status: false, success: true, found: [] }
    })

    store.sendHardwareScan = vi.fn(async () => false)
    expect(await store.runHardwareScan()).toEqual({ success: false })
    expect(globalStore.messageError).toBe('Failed to start hardware scan')

    vi.useRealTimers()
  })
})
