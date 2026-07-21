import { beforeEach, describe, expect, it, vi } from 'vitest'

describe('measurementStore module', () => {
  let http
  let globalStore
  let configStore

  const loadStore = async () => {
    vi.resetModules()
    vi.doUnmock('@/modules/measurementStore')

    http = {
      postJson: vi.fn(),
      request: vi.fn()
    }

    globalStore = {
      disabled: false
    }

    configStore = {
      temp_unit: 'C',
      gravity_unit: 'G',
      pressure_unit: 'PSI',
      sd_log_files: 3
    }

    vi.doMock('pinia', () => ({
      defineStore: (_id, options) => () => {
        const state = options.state()
        const store = { ...state }
        store.$id = 'measurement'
        store.$state = store
        for (const [name, fn] of Object.entries(options.actions)) {
          store[name] = fn.bind(store)
        }
        return store
      }
    }))

    vi.doMock('@/modules/pinia', () => ({
      global: globalStore,
      config: configStore
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

    const { useMeasurementStore } = await vi.importActual('../measurementStore.js')
    return useMeasurementStore()
  }

  beforeEach(() => {
    vi.clearAllMocks()
  })

  it('filters SD measurement files by prefix, extension, and configured count', async () => {
    const store = await loadStore()
    store.sendSecureDiskRequest = vi.fn(async () => ({
      success: true,
      text: JSON.stringify({
        files: [
          { file: '/data.csv' },
          { file: '/data1.csv' },
          { file: '/data2.csv' },
          { file: '/data3.csv' },
          { file: '/notes.txt' },
          { file: '/other.csv' }
        ]
      })
    }))

    const result = await store.updateMeasurementFiles()

    expect(result).toBe(true)
    expect(store.files).toEqual(['/data.csv', '/data1.csv', '/data2.csv'])
  })

  it('returns false when measurement file listing cannot be parsed', async () => {
    const store = await loadStore()
    store.sendSecureDiskRequest = vi.fn(async () => ({ success: true, text: 'not-json' }))

    const result = await store.updateMeasurementFiles()

    expect(result).toBe(false)
  })

  it('validates dates from 2025 onwards only', async () => {
    const store = await loadStore()

    expect(store.isValidDate({ getCreated: () => new Date('2025-01-01T00:00:00Z') })).toBe(true)
    expect(store.isValidDate({ getCreated: () => new Date('2024-06-01T12:00:00Z') })).toBe(false)
    expect(store.isValidDate(null)).toBe(false)
  })

  it('fetches and parses all supported measurement CSV types', async () => {
    const store = await loadStore()
    store.files = ['/data.csv', '/data1.csv']
    store.fetchSecureDiskFile = vi
      .fn()
      .mockResolvedValueOnce({
        success: true,
        text: [
          '1,Tilt,BLE Beacon,2025-07-06 12:34:56,Red,Red,20.00,1.0500,-59,-70,,,,',
          '1,Gravitymon,BLE Beacon,2025-07-06 12:34:57,G-1,Fermenter,token,20.00,1.0400,45.00,3.70,-59,-70,60',
          '1,Pressuremon,BLE Beacon,2025-07-06 12:34:58,P-1,Pressure,token,20.00,12.1000,11.1000,3.70,-59,-70,60'
        ].join('\n')
      })
      .mockResolvedValueOnce({
        success: true,
        text: [
          '1,Chamber Controller,BLE Beacon,2025-07-06 12:34:59,C-1,20.00,19.00,-70,Chamber,5,,,,',
          '1,RAPT,BLE Beacon,2025-08-13 08:04:40,RAPT-1,15.20,1.0300,35.3300,3.84,10,-72,,,',
          '1,Tilt,BLE Beacon,2024-07-06 12:34:56,Old,Old,20.00,1.0100,-59,-70,,,,'
        ].join('\n')
      })

    await store.fetchAllMeasurementFiles()

    expect(store.tiltData).toHaveLength(1)
    expect(store.gravitymonData).toHaveLength(1)
    expect(store.pressuremonData).toHaveLength(1)
    expect(store.chamberData).toHaveLength(1)
    expect(store.raptData).toHaveLength(1)
    expect(store.gravitymonData[0].getId()).toBe('G-1')
    expect(store.chamberData[0].getName()).toBe('Chamber')
  })

  it('sorts parsed measurements by creation time', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: [
        '1,Gravitymon,BLE Beacon,2025-07-06 12:35:57,G-1,Fermenter,token,20.00,1.0400,45.00,3.70,-59,-70,60',
        '1,Gravitymon,BLE Beacon,2025-07-06 12:34:57,G-1,Fermenter,token,20.00,1.0410,46.00,3.71,-58,-69,60'
      ].join('\n')
    }))

    await store.fetchAllMeasurementFiles()

    expect(store.gravitymonData[0].getCreated().getTime()).toBeLessThan(
      store.gravitymonData[1].getCreated().getTime()
    )
  })

  it('wraps secure disk postJson requests', async () => {
    const store = await loadStore()
    http.postJson.mockResolvedValue({ text: vi.fn().mockResolvedValue('{"ok":true}') })

    const result = await store.sendSecureDiskRequest({ command: 'dir' })

    expect(http.postJson).toHaveBeenCalledWith('api/sd', { command: 'dir' })
    expect(result).toEqual({ success: true, text: '{"ok":true}' })
    expect(globalStore.disabled).toBe(false)
  })

  it('wraps secure disk GET file fetches', async () => {
    const store = await loadStore()
    http.request.mockResolvedValue({ text: vi.fn().mockResolvedValue('csv-data') })

    const result = await store.fetchSecureDiskFile('/data.csv')

    expect(http.request).toHaveBeenCalledWith('sd/data.csv', { method: 'GET' })
    expect(result).toEqual({ success: true, text: 'csv-data' })
    expect(globalStore.disabled).toBe(false)
  })

  it('handles errors in sendSecureDiskRequest and resets disabled flag', async () => {
    const store = await loadStore()
    http.postJson.mockRejectedValue(new Error('Network error'))

    const result = await store.sendSecureDiskRequest({ command: 'dir' })

    expect(result).toEqual({ success: false, text: '' })
    expect(globalStore.disabled).toBe(false)
  })

  it('handles errors in fetchSecureDiskFile and resets disabled flag', async () => {
    const store = await loadStore()
    http.request.mockRejectedValue(new Error('Network error'))

    const result = await store.fetchSecureDiskFile('/data.csv')

    expect(result).toEqual({ success: false, text: '' })
    expect(globalStore.disabled).toBe(false)
  })

  it('rejects CSV lines with wrong version number', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: [
        '2,Gravitymon,BLE Beacon,2025-07-06 12:34:57,G-1,Fermenter,token,20.00,1.0400,45.00,3.70,-59,-70,60',
        '0,Tilt,BLE Beacon,2025-07-06 12:34:56,Red,Red,20.00,1.0500,-59,-70,,,,',
        '3,Pressuremon,BLE Beacon,2025-07-06 12:34:58,P-1,Pressure,token,20.00,12.1000,11.1000,3.70,-59,-70,60'
      ].join('\n')
    }))

    await store.fetchAllMeasurementFiles()

    expect(store.gravitymonData).toHaveLength(0)
    expect(store.tiltData).toHaveLength(0)
    expect(store.pressuremonData).toHaveLength(0)
  })

  it('rejects CSV lines with insufficient fields', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: [
        '1,Gravitymon,BLE,2025-07-06,G-1,Fermenter',
        '1,Tilt,BLE,2025-07-06,Red',
        '1,Pressuremon,BLE,2025-07-06,P-1',
        '1,Chamber Controller,BLE,2025-07-06,C-1',
        '1,RAPT,BLE,2025-07-06,RAPT-1,15.20'
      ].join('\n')
    }))

    await store.fetchAllMeasurementFiles()

    expect(store.gravitymonData).toHaveLength(0)
    expect(store.tiltData).toHaveLength(0)
    expect(store.pressuremonData).toHaveLength(0)
    expect(store.chamberData).toHaveLength(0)
    expect(store.raptData).toHaveLength(0)
  })

  it('rejects CSV lines with mismatched type field', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: [
        '1,WrongType,BLE Beacon,2025-07-06 12:34:57,G-1,Fermenter,token,20.00,1.0400,45.00,3.70,-59,-70,60',
        '1,Tilt Lite,BLE Beacon,2025-07-06 12:34:56,Red,Red,20.00,1.0500,-59,-70,,,,',
        '1,Pressuremon Plus,BLE Beacon,2025-07-06 12:34:58,P-1,Pressure,token,20.00,12.1000,11.1000,3.70,-59,-70,60'
      ].join('\n')
    }))

    await store.fetchAllMeasurementFiles()

    expect(store.gravitymonData).toHaveLength(0)
    expect(store.tiltData).toHaveLength(0)
    expect(store.pressuremonData).toHaveLength(0)
  })

  it('handles Tilt Pro variant alongside regular Tilt', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: [
        '1,Tilt,BLE Beacon,2025-07-06 12:34:56,Red,Red,20.00,1.0500,-59,-70,,,,',
        '1,Tilt Pro,BLE Beacon,2025-07-06 12:34:57,Blue,Blue,20.50,1.0600,-60,-71,,,'
      ].join('\n')
    }))

    await store.fetchAllMeasurementFiles()

    expect(store.tiltData).toHaveLength(2)
    expect(store.tiltData[0].getIsPro()).toBe(false)
    expect(store.tiltData[0].getType()).toBe('Tilt')
    expect(store.tiltData[1].getIsPro()).toBe(true)
    expect(store.tiltData[1].getType()).toBe('Tilt Pro')
  })

  it('ignores unknown CSV line types and logs error', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: [
        '1,UnknownDevice,BLE Beacon,2025-07-06 12:34:56,ID,Name,Value',
        '1,Gravitymon,BLE Beacon,2025-07-06 12:34:57,G-1,Fermenter,token,20.00,1.0400,45.00,3.70,-59,-70,60'
      ].join('\n')
    }))

    await store.fetchAllMeasurementFiles()

    expect(store.gravitymonData).toHaveLength(1)
  })

  it('filters out empty lines in CSV files', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: [
        '',
        '1,Gravitymon,BLE Beacon,2025-07-06 12:34:57,G-1,Fermenter,token,20.00,1.0400,45.00,3.70,-59,-70,60',
        '  ',
        '\t',
        '1,Tilt,BLE Beacon,2025-07-06 12:34:56,Red,Red,20.00,1.0500,-59,-70,,,,'
      ].join('\n')
    }))

    await store.fetchAllMeasurementFiles()

    expect(store.gravitymonData).toHaveLength(1)
    expect(store.tiltData).toHaveLength(1)
  })

  it('handles updateMeasurementFiles failure from HTTP error', async () => {
    const store = await loadStore()
    http.postJson.mockRejectedValue(new Error('Network error'))

    const result = await store.updateMeasurementFiles()

    expect(result).toBe(false)
    expect(store.files).toHaveLength(0)
  })

  it('handles updateMeasurementFiles with non-CSV files in response', async () => {
    const store = await loadStore()
    store.sendSecureDiskRequest = vi.fn(async () => ({
      success: true,
      text: JSON.stringify({
        files: [
          { file: '/data.csv' },
          { file: '/notes.txt' },
          { file: '/README.md' },
          { file: '/backup.zip' }
        ]
      })
    }))

    const result = await store.updateMeasurementFiles()

    expect(result).toBe(true)
    expect(store.files).toEqual(['/data.csv'])
  })

  it('respects sd_log_files limit when listing measurement files', async () => {
    const store = await loadStore()
    const { config: moduleConfig } = await import('@/modules/pinia')
    moduleConfig.sd_log_files = 2

    store.sendSecureDiskRequest = vi.fn(async () => ({
      success: true,
      text: JSON.stringify({
        files: [
          { file: '/data.csv' },
          { file: '/data1.csv' },
          { file: '/data2.csv' },
          { file: '/data3.csv' },
          { file: '/data10.csv' }
        ]
      })
    }))

    const result = await store.updateMeasurementFiles()

    expect(result).toBe(true)
    expect(store.files).toEqual(['/data.csv', '/data1.csv'])
  })

  // TiltData getter tests
  it('TiltData getters return correct values', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: '1,Tilt,BLE Beacon,2025-07-06 12:34:56,Red,Red,20.00,1.0500,-59,-70,,,,'
    }))

    await store.fetchAllMeasurementFiles()

    const tilt = store.tiltData[0]
    expect(tilt.getType()).toBe('Tilt')
    expect(tilt.getSource()).toBe('BLE Beacon')
    expect(tilt.getId()).toBe('Red')
    expect(tilt.getColor()).toBe('Red')
    expect(tilt.getTemp()).toBe(20)
    expect(tilt.getGravity()).toBe(1.05)
    expect(tilt.getTxPower()).toBe(-59)
    expect(tilt.getRssi()).toBe(-70)
    expect(tilt.getIsPro()).toBe(false)
    expect(tilt.getCreated()).toBeInstanceOf(Date)
  })

  it('TiltData converts temperature from Celsius to Fahrenheit', async () => {
    const store = await loadStore()
    configStore.temp_unit = 'F'
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: '1,Tilt,BLE Beacon,2025-07-06 12:34:56,Red,Red,0.00,1.0500,-59,-70,,,,'
    }))

    await store.fetchAllMeasurementFiles()

    const tilt = store.tiltData[0]
    expect(tilt.getTemp()).toBe(32) // 0°C = 32°F
  })

  it('TiltData converts gravity to Plato', async () => {
    const store = await loadStore()
    configStore.gravity_unit = 'P'
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: '1,Tilt,BLE Beacon,2025-07-06 12:34:56,Red,Red,20.00,1.0500,-59,-70,,,,'
    }))

    await store.fetchAllMeasurementFiles()

    const tilt = store.tiltData[0]
    expect(tilt.getGravity()).toBe('1.1') // Gravity converted to Plato
  })

  it('Tilt Pro has IsPro flag set correctly', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: '1,Tilt Pro,BLE Beacon,2025-07-06 12:34:56,Blue,Blue,20.50,1.0600,-60,-71,,,,'
    }))

    await store.fetchAllMeasurementFiles()

    const tilt = store.tiltData[0]
    expect(tilt.getIsPro()).toBe(true)
    expect(tilt.getType()).toBe('Tilt Pro')
  })

  // GravityData getter tests
  it('GravityData getters return correct values', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: '1,Gravitymon,BLE Beacon,2025-07-06 12:34:57,G-1,Fermenter,token,20.00,1.0400,45.00,3.70,-59,-70,60'
    }))

    await store.fetchAllMeasurementFiles()

    const grav = store.gravitymonData[0]
    expect(grav.getType()).toBe('Gravitymon')
    expect(grav.getSource()).toBe('BLE Beacon')
    expect(grav.getId()).toBe('G-1')
    expect(grav.getName()).toBe('Fermenter')
    expect(grav.getToken()).toBe('token')
    expect(grav.getTemp()).toBe(20)
    expect(grav.getGravity()).toBe(1.04)
    expect(grav.getAngle()).toBe(45)
    expect(grav.getBattery()).toBe(3.7)
    expect(grav.getTxPower()).toBe(-59)
    expect(grav.getRssi()).toBe(-70)
    expect(grav.getInterval()).toBe(60)
  })

  // PressureData getter tests
  it('PressureData getters return correct values', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: '1,Pressuremon,BLE Beacon,2025-07-06 12:34:58,P-1,Pressure,token,20.00,12.1000,11.1000,3.70,-59,-70,60'
    }))

    await store.fetchAllMeasurementFiles()

    const press = store.pressuremonData[0]
    expect(press.getType()).toBe('Pressuremon')
    expect(press.getId()).toBe('P-1')
    expect(press.getName()).toBe('Pressure')
    expect(press.getToken()).toBe('token')
    expect(press.getTemp()).toBe(20)
    expect(press.getPressure()).toBe(12.1)
    expect(press.getPressure1()).toBe(11.1)
    expect(press.getBattery()).toBe(3.7)
    expect(press.getTxPower()).toBe(-59)
    expect(press.getRssi()).toBe(-70)
    expect(press.getInterval()).toBe(60)
  })

  it('PressureData converts pressure from PSI to Bar', async () => {
    const store = await loadStore()
    configStore.pressure_unit = 'Bar'
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: '1,Pressuremon,BLE Beacon,2025-07-06 12:34:58,P-1,Pressure,token,20.00,10.0000,5.0000,3.70,-59,-70,60'
    }))

    await store.fetchAllMeasurementFiles()

    const press = store.pressuremonData[0]
    expect(Math.round(press.getPressure() * 100) / 100).toBe(0.69) // 10 PSI ≈ 0.69 Bar
  })

  it('PressureData converts pressure from PSI to KPa', async () => {
    const store = await loadStore()
    configStore.pressure_unit = 'KPa'
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: '1,Pressuremon,BLE Beacon,2025-07-06 12:34:58,P-1,Pressure,token,20.00,10.0000,5.0000,3.70,-59,-70,60'
    }))

    await store.fetchAllMeasurementFiles()

    const press = store.pressuremonData[0]
    expect(Math.round(press.getPressure() * 10) / 10).toBe(68.9) // 10 PSI ≈ 68.9 KPa
  })

  // RaptData getter tests
  it('RaptData getters return correct values', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: '1,RAPT,BLE Beacon,2025-08-13 08:04:40,EEE222,15.20,1.0300,35.3300,3.84,10,-72,,,,'
    }))

    await store.fetchAllMeasurementFiles()

    const rapt = store.raptData[0]
    expect(rapt.getType()).toBe('RAPT')
    expect(rapt.getSource()).toBe('BLE Beacon')
    expect(rapt.getId()).toBe('EEE222')
    expect(rapt.getTemp()).toBe(15.2)
    expect(rapt.getGravity()).toBe(1.03)
    expect(rapt.getAngle()).toBe(35.33)
    expect(rapt.getBattery()).toBe(3.84)
    expect(rapt.getTxPower()).toBe(10)
    expect(rapt.getRssi()).toBe(-72)
  })

  // ChamberData getter tests
  it('ChamberData getters return correct values', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: '1,Chamber Controller,BLE Beacon,2025-07-06 12:34:59,C-1,20.00,19.00,-70,MyChamber,5,,,,,'
    }))

    await store.fetchAllMeasurementFiles()

    const chamber = store.chamberData[0]
    expect(chamber.getType()).toBe('Chamber Controller')
    expect(chamber.getSource()).toBe('BLE Beacon')
    expect(chamber.getId()).toBe('C-1')
    expect(chamber.getChamberTemp()).toBe(20)
    expect(chamber.getBeerTemp()).toBe(19)
    expect(chamber.getRssi()).toBe(-70)
    expect(chamber.getName()).toBe('MyChamber')
    expect(chamber.getTxPower()).toBe(5)
  })

  it('ChamberData converts temperature from Celsius to Fahrenheit', async () => {
    const store = await loadStore()
    configStore.temp_unit = 'F'
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: '1,Chamber Controller,BLE Beacon,2025-07-06 12:34:59,C-1,0.00,0.00,-70,Chamber,5,,,,,'
    }))

    await store.fetchAllMeasurementFiles()

    const chamber = store.chamberData[0]
    expect(chamber.getChamberTemp()).toBe(32) // 0°C = 32°F
    expect(chamber.getBeerTemp()).toBe(32)
  })

  it('fetchAllMeasurementFiles returns early when no files are set', async () => {
    const store = await loadStore()
    store.files = []

    const result = await store.fetchAllMeasurementFiles()

    expect(result).toBeUndefined()
    expect(store.tiltData).toHaveLength(0)
    expect(store.gravitymonData).toHaveLength(0)
  })

  it('parseData handles mixed line endings (CRLF and LF)', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: '1,Gravitymon,BLE Beacon,2025-07-06 12:34:57,G-1,Fermenter,token,20.00,1.0400,45.00,3.70,-59,-70,60\r\n1,Tilt,BLE Beacon,2025-07-06 12:34:56,Red,Red,20.00,1.0500,-59,-70,,,,'
    }))

    await store.fetchAllMeasurementFiles()

    expect(store.gravitymonData).toHaveLength(1)
    expect(store.tiltData).toHaveLength(1)
  })

  it('sendSecureDiskRequest sets disabled flag during request', async () => {
    const store = await loadStore()
    let disabledDuringRequest = null
    http.postJson.mockImplementation(async () => {
      disabledDuringRequest = globalStore.disabled
      return { text: vi.fn().mockResolvedValue('{}') }
    })

    await store.sendSecureDiskRequest({ command: 'dir' })

    expect(disabledDuringRequest).toBe(true)
    expect(globalStore.disabled).toBe(false) // Reset after request
  })

  it('fetchSecureDiskFile sets disabled flag during request', async () => {
    const store = await loadStore()
    let disabledDuringRequest = null
    http.request.mockImplementation(async () => {
      disabledDuringRequest = globalStore.disabled
      return { text: vi.fn().mockResolvedValue('data') }
    })

    await store.fetchSecureDiskFile('/data.csv')

    expect(disabledDuringRequest).toBe(true)
    expect(globalStore.disabled).toBe(false)
  })

  it('isValidDate rejects invalid date objects', async () => {
    const store = await loadStore()

    // Objects with getCreated function that return invalid values
    expect(store.isValidDate({ getCreated: () => null })).toBe(false)
    expect(store.isValidDate({ getCreated: () => new Date('2024-12-31') })).toBe(false)
    expect(store.isValidDate(null)).toBe(false)
  })

  it('isValidDate accepts string dates from 2025 onwards', async () => {
    const store = await loadStore()

    // String dates should be converted to Date objects
    const objectWithStringDate = {
      getCreated: () => '2025-01-01T00:00:00Z'
    }
    expect(store.isValidDate(objectWithStringDate)).toBe(true)
  })

  it('handles updateMeasurementFiles when response has empty files array', async () => {
    const store = await loadStore()
    store.sendSecureDiskRequest = vi.fn(async () => ({
      success: true,
      text: JSON.stringify({ files: [] })
    }))

    const result = await store.updateMeasurementFiles()

    expect(result).toBe(true)
    expect(store.files).toHaveLength(0)
  })

  it('parses CSV lines with maximum field values', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: '1,Gravitymon,BLE Beacon,2025-07-06 12:34:57,DEVICE-ID-LONG,Very Long Fermenter Name,SecureToken123,99.99,1.9999,359.99,4.99,-1,-127,3600'
    }))

    await store.fetchAllMeasurementFiles()

    expect(store.gravitymonData).toHaveLength(1)
    const grav = store.gravitymonData[0]
    expect(grav.getName()).toBe('Very Long Fermenter Name')
    expect(grav.getTemp()).toBe(99.99)
    expect(grav.getInterval()).toBe(3600)
  })

  it('parses CSV lines with minimum field values', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: '1,Pressuremon,BLE Beacon,2025-01-01 00:00:00,A,B,C,0.00,0.0000,0.0000,0.00,-100,-127,0'
    }))

    await store.fetchAllMeasurementFiles()

    expect(store.pressuremonData).toHaveLength(1)
    const press = store.pressuremonData[0]
    expect(press.getTemp()).toBe(0)
    expect(press.getPressure()).toBe(0)
    expect(press.getInterval()).toBe(0)
  })

  it('handles fetchAllMeasurementFiles with zero valid files', async () => {
    const store = await loadStore()
    store.files = ['/data.csv']
    store.fetchSecureDiskFile = vi.fn(async () => ({
      success: true,
      text: '1,Gravitymon,BLE Beacon,2024-12-31 23:59:59,G-1,Fermenter,token,20.00,1.0400,45.00,3.70,-59,-70,60'
    }))

    await store.fetchAllMeasurementFiles()

    expect(store.gravitymonData).toHaveLength(0)
  })

  it('handles multiple measurement files from different sources', async () => {
    const store = await loadStore()
    store.files = ['/data.csv', '/data1.csv']
    store.fetchSecureDiskFile = vi
      .fn()
      .mockResolvedValueOnce({
        success: true,
        text: '1,Tilt,BLE Beacon,2025-07-06 12:34:56,Red,Red,20.00,1.0500,-59,-70,,,,'
      })
      .mockResolvedValueOnce({
        success: true,
        text: '1,Gravitymon,BLE Beacon,2025-07-06 12:34:57,G-1,Fermenter,token,20.00,1.0400,45.00,3.70,-59,-70,60'
      })

    await store.fetchAllMeasurementFiles()

    expect(store.tiltData).toHaveLength(1)
    expect(store.gravitymonData).toHaveLength(1)
  })
})
