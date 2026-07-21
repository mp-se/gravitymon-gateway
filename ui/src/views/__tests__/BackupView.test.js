import { afterEach, beforeEach, describe, expect, it, vi } from 'vitest'
import { flushPromises, mount } from '@vue/test-utils'
import { config, global } from '@/modules/pinia'
import BackupView from '@/views/BackupView.vue'

describe('BackupView', () => {
  beforeEach(() => {
    vi.clearAllMocks()

    Object.assign(config, {
      mdns: 'gateway-test',
      http_post_format_gravity: '',
      http_post2_format_gravity: '',
      http_get_format_gravity: '',
      influxdb2_format_gravity: '',
      mqtt_format_gravity: '',
      http_post_format_pressure: '',
      http_post2_format_pressure: '',
      http_get_format_pressure: '',
      influxdb2_format_pressure: '',
      mqtt_format_pressure: '',
      http_post_target: '',
      saveAll: vi.fn(async () => true),
      toJson: vi.fn(() =>
        JSON.stringify({
          mdns: 'gateway-test',
          http_post_format_gravity: '',
          http_post2_format_gravity: '',
          http_get_format_gravity: '',
          influxdb2_format_gravity: '',
          mqtt_format_gravity: '',
          http_post_format_pressure: '',
          http_post2_format_pressure: '',
          http_get_format_pressure: '',
          influxdb2_format_pressure: '',
          mqtt_format_pressure: ''
        })
      )
    })

    Object.assign(global, {
      disabled: false,
      messageError: '',
      messageSuccess: ''
    })

    if (!window.URL) {
      window.URL = {
        createObjectURL: vi.fn(() => 'blob:mock'),
        revokeObjectURL: vi.fn()
      }
    } else {
      window.URL.createObjectURL = vi.fn(() => 'blob:mock')
      window.URL.revokeObjectURL = vi.fn()
    }
  })

  afterEach(() => {
    vi.restoreAllMocks()
  })

  const mountView = () =>
    mount(BackupView, {
      global: {
        stubs: {
          BsFileUpload: {
            template: '<input id="upload" type="file" @change="$emit(\'change\', $event)" />'
          },
          BsProgress: true,
          BsMessage: true
        }
      }
    })

  it('mounts and displays the header', () => {
    const wrapper = mountView()

    expect(wrapper.text()).toContain('Backup & Restore')
  })

  it('tracks file selection changes', async () => {
    const wrapper = mountView()
    const input = wrapper.find('input[type="file"]')

    Object.defineProperty(input.element, 'files', {
      value: [new File(['data'], 'backup.txt')],
      configurable: true
    })

    await input.trigger('change')

    expect(wrapper.vm.fileSelected).toBe(true)
  })

  it('creates and downloads a backup file', async () => {
    const wrapper = mountView()
    const clickSpy = vi.fn()
    const createElementSpy = vi.spyOn(document, 'createElement').mockReturnValue({
      setAttribute: vi.fn(),
      click: clickSpy
    })

    await wrapper.findAll('button')[0].trigger('click')

    expect(config.toJson).toHaveBeenCalled()
    expect(clickSpy).toHaveBeenCalled()
    expect(global.messageSuccess).toContain('gateway-test.txt')

    createElementSpy.mockRestore()
  })

  it('shows an error when the upload element is missing', async () => {
    const wrapper = mountView()
    vi.spyOn(document, 'getElementById').mockReturnValue(null)

    await wrapper.find('form').trigger('submit')

    expect(global.messageError).toBe('Upload element not found')
  })

  it('shows an error when no file is selected', async () => {
    const wrapper = mountView()
    vi.spyOn(document, 'getElementById').mockReturnValue({ files: [], value: '' })

    await wrapper.find('form').trigger('submit')

    expect(global.messageError).toBe('You need to select one file to restore configuration from')
  })

  it('rejects files with an unknown backup format', async () => {
    const wrapper = mountView()
    const mockReader = {
      readAsText: vi.fn(function () {
        this.onload({ target: { result: JSON.stringify({ meta: { software: 'Wrong' } }) } })
      }),
      addEventListener: vi.fn(function (event, cb) {
        if (event === 'load') this.onload = cb
        if (event === 'error') this.onerror = cb
      })
    }

    vi.stubGlobal(
      'FileReader',
      class {
        constructor() {
          Object.assign(this, mockReader)
        }
      }
    )
    vi.spyOn(document, 'getElementById').mockReturnValue({
      files: [new File(['data'], 'backup.txt')],
      value: 'backup.txt'
    })

    await wrapper.find('form').trigger('submit')
    await flushPromises()

    expect(global.messageError).toBe('Unknown format, unable to process')
  })

  it('rejects invalid JSON backups', async () => {
    const wrapper = mountView()
    const mockReader = {
      readAsText: vi.fn(function () {
        this.onload({ target: { result: 'not-json' } })
      }),
      addEventListener: vi.fn(function (event, cb) {
        if (event === 'load') this.onload = cb
        if (event === 'error') this.onerror = cb
      })
    }

    vi.stubGlobal(
      'FileReader',
      class {
        constructor() {
          Object.assign(this, mockReader)
        }
      }
    )
    vi.spyOn(document, 'getElementById').mockReturnValue({
      files: [new File(['data'], 'backup.txt')],
      value: 'backup.txt'
    })

    await wrapper.find('form').trigger('submit')
    await flushPromises()

    expect(global.messageError).toBe('Unable to parse configuration file for Gravitymon-Gateway.')
  })

  it('handles file reader errors', async () => {
    const wrapper = mountView()
    const mockReader = {
      readAsText: vi.fn(function () {
        this.onerror()
      }),
      addEventListener: vi.fn(function (event, cb) {
        if (event === 'load') this.onload = cb
        if (event === 'error') this.onerror = cb
      })
    }

    vi.stubGlobal(
      'FileReader',
      class {
        constructor() {
          Object.assign(this, mockReader)
        }
      }
    )
    vi.spyOn(document, 'getElementById').mockReturnValue({
      files: [new File(['data'], 'backup.txt')],
      value: 'backup.txt'
    })

    await wrapper.find('form').trigger('submit')

    expect(global.messageError).toBe('Failed to read the backup file')
  })

  it('restores a valid gateway backup and saves the config', async () => {
    const wrapper = mountView()
    const backupData = {
      meta: { software: 'Gravitymon-Gateway', version: '0.8' },
      config: {
        mdns: 'restored-device',
        http_post_target: 'https://example.com/push',
        http_post_format_gravity: encodeURIComponent('gravity=${gravity}'),
        http_post_format_pressure: encodeURIComponent('pressure=${pressure}')
      }
    }
    const mockReader = {
      readAsText: vi.fn(function () {
        this.onload({ target: { result: JSON.stringify(backupData) } })
      }),
      addEventListener: vi.fn(function (event, cb) {
        if (event === 'load') this.onload = cb
        if (event === 'error') this.onerror = cb
      })
    }

    vi.stubGlobal(
      'FileReader',
      class {
        constructor() {
          Object.assign(this, mockReader)
        }
      }
    )
    vi.spyOn(document, 'getElementById').mockReturnValue({
      files: [new File(['data'], 'backup.txt')],
      value: 'backup.txt'
    })

    await wrapper.find('form').trigger('submit')
    await flushPromises()

    expect(config.mdns).toBe('restored-device')
    expect(config.http_post_target).toBe('https://example.com/push')
    expect(config.http_post_format_gravity).toBe('gravity=${gravity}')
    expect(config.http_post_format_pressure).toBe('pressure=${pressure}')
    expect(config.saveAll).toHaveBeenCalled()
    expect(global.disabled).toBe(false)
    expect(wrapper.vm.fileSelected).toBe(false)
  })

  it('shows the progress component when progress is set', async () => {
    const wrapper = mountView()

    wrapper.vm.progress = 50
    await wrapper.vm.$nextTick()

    expect(wrapper.vm.progress).toBe(50)
  })
})
