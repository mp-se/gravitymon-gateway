import { vi } from 'vitest'
import { config as vtConfig } from '@vue/test-utils'

class LocalStorageMock {
  constructor() {
    this.store = {}
  }

  clear() {
    this.store = {}
  }

  getItem(key) {
    return this.store[key] || null
  }

  setItem(key, value) {
    this.store[key] = String(value)
  }

  removeItem(key) {
    delete this.store[key]
  }

  get length() {
    return Object.keys(this.store).length
  }

  key(index) {
    const keys = Object.keys(this.store)
    return keys[index] || null
  }
}

Object.defineProperty(window, 'localStorage', {
  value: new LocalStorageMock(),
  configurable: true
})

if (typeof HTMLDialogElement !== 'undefined') {
  HTMLDialogElement.prototype.showModal = vi.fn()
  HTMLDialogElement.prototype.close = vi.fn()
}

if (typeof HTMLCanvasElement !== 'undefined') {
  HTMLCanvasElement.prototype.getContext = vi.fn(() => ({}))
}

vi.stubEnv('VITE_APP_VERSION', '1.0.0-test')
vi.stubEnv('VITE_APP_BUILD', '1')

const mockConfigStore = {
  id: '',
  mdns: '',
  temp_unit: 'C',
  gravity_unit: 'G',
  pressure_unit: 'PSI',
  dark_mode: false,
  wifi_pass: '',
  wifi_pass2: '',
  wifi_direct_ssid: '',
  wifi_direct_pass: '',
  wifi_portal_timeout: 60,
  wifi_connect_timeout: 30,
  wifi_scan_ap: false,
  ble_enable: true,
  ble_active_scan: false,
  ble_scan_time: 5,
  timezone: 'CET-1CEST,M3.5.0,M10.5.0/3',
  display_layout_id: 0,
  use_wifi_direct: false,
  wifi_ssid: '',
  wifi_ssid2: '',
  token: '',
  push_timeout: 30,
  push_resend_time: 60,
  sd_log_files: 10,
  sd_log_min_time: 5,
  sleep_interval: 900,
  http_post_target: '',
  http_post_header1: '',
  http_post_header2: '',
  http_post_format_gravity: '',
  http_post_format_pressure: '',
  http_post_gravity: true,
  http_post_pressure: true,
  http_post2_target: '',
  http_post2_header1: '',
  http_post2_header2: '',
  http_post2_format_gravity: '',
  http_post2_format_pressure: '',
  http_post2_gravity: true,
  http_post2_pressure: true,
  http_get_target: '',
  http_get_header1: '',
  http_get_header2: '',
  http_get_format_gravity: '',
  http_get_format_pressure: '',
  http_get_gravity: true,
  http_get_pressure: true,
  influxdb2_target: '',
  influxdb2_org: '',
  influxdb2_bucket: '',
  influxdb2_token: '',
  influxdb2_format_gravity: '',
  influxdb2_format_pressure: '',
  influxdb2_gravity: true,
  influxdb2_pressure: true,
  mqtt_target: '',
  mqtt_port: 1883,
  mqtt_user: '',
  mqtt_pass: '',
  mqtt_format_gravity: '',
  mqtt_format_pressure: '',
  mqtt_gravity: true,
  mqtt_pressure: true,
  load: vi.fn(async () => true),
  loadFormat: vi.fn(async () => true),
  saveAll: vi.fn(async () => true),
  restart: vi.fn(async () => true),
  runPushTest: vi.fn(async () => true),
  runWifiScan: vi.fn(async () => ({ success: true, data: { networks: [] } })),
  runHardwareScan: vi.fn(async () => ({ success: true, data: '' })),
  sendConfig: vi.fn(async () => true),
  sendFormat: vi.fn(async () => true),
  sendPushTest: vi.fn(async () => true),
  isPsi: vi.fn(() => true),
  isBar: vi.fn(() => false),
  isKPa: vi.fn(() => false),
  $state: {},
  $subscribe: vi.fn()
}

const mockGlobalStore = {
  initialized: true,
  disabled: false,
  configChanged: false,
  messageError: '',
  messageWarning: '',
  messageSuccess: '',
  messageInfo: '',
  platform: 'ESP32',
  board: 'DEVKIT',
  app_ver: '1.0.0',
  app_build: '1',
  hardware: 'DEVKIT',
  firmware_file: 'firmware.bin',
  uiVersion: '1.0.0-test',
  uiBuild: '1',
  feature: {
    tft: false,
    sd: false
  },
  ui: {
    enableVoltageFragment: true,
    enableManualWifiEntry: true,
    enableScanForStrongestAp: false
  },
  clearMessages: vi.fn(),
  load: vi.fn(async () => true),
  isError: false,
  isWarning: false,
  isSuccess: false,
  isInfo: false,
  $state: {}
}

const mockStatusStore = {
  id: '',
  rssi: -60,
  mdns: '',
  wifi_ssid: '',
  ip: '',
  total_heap: 0,
  free_heap: 0,
  wifi_setup: false,
  sd_mounted: false,
  gravity_device: [],
  pressure_device: [],
  temperature_device: [],
  uptime_seconds: 0,
  uptime_minutes: 0,
  uptime_hours: 0,
  uptime_days: 0,
  load: vi.fn(async () => true),
  $state: {}
}

global.btoa = global.btoa || ((value) => Buffer.from(String(value), 'binary').toString('base64'))

const mockMeasurementStore = {
  data: [],
  gravitymonData: [],
  tiltData: [],
  pressuremonData: [],
  chamberData: [],
  raptData: [],
  load: vi.fn(async () => true),
  updateMeasurementFiles: vi.fn(async () => true),
  fetchAllMeasurementFiles: vi.fn(async () => true),
  $state: {}
}

vi.mock('@/modules/configStore', () => ({
  useConfigStore: vi.fn(() => mockConfigStore)
}))

vi.mock('@/modules/globalStore', () => ({
  useGlobalStore: vi.fn(() => mockGlobalStore)
}))

vi.mock('@/modules/statusStore', () => ({
  useStatusStore: vi.fn(() => mockStatusStore)
}))

vi.mock('@/modules/measurementStore', () => ({
  useMeasurementStore: vi.fn(() => mockMeasurementStore)
}))

vi.mock('@mp-se/espframework-ui-components', () => ({
  logError: vi.fn(),
  logDebug: vi.fn(),
  logInfo: vi.fn(),
  validateCurrentForm: vi.fn(() => true),
  isValidJson: vi.fn(() => false),
  isValidFormData: vi.fn(() => false),
  isValidMqttData: vi.fn(() => false),
  tempToF: (c) => (c * 9) / 5 + 32,
  tempToC: (f) => ((f - 32) * 5) / 9,
  gravityToPlato: (g) => Number(g).toFixed(1),
  gravityToSG: (g) => g,
  psiToBar: (psi) => psi * 0.0689476,
  psiToKPa: (psi) => psi * 6.89476,
  barToPsi: (bar) => bar * 14.5038,
  kpaToPsi: (kpa) => kpa * 0.145038,
  roundVal: (value, precision) => Number(value.toFixed(precision)),
  sharedHttpClient: {
    auth: vi.fn(async () => true),
    getJson: vi.fn(),
    postJson: vi.fn(),
    putJson: vi.fn(),
    deleteJson: vi.fn(),
    request: vi.fn(async () => ({ ok: true, text: async () => '' })),
    filesystemRequest: vi.fn(async () => ({ success: true })),
    uploadFile: vi.fn(async () => ({ success: true })),
    createWebSocket: vi.fn((path, handlers = {}) => {
      if (handlers.onOpen) handlers.onOpen()
      return {
        close: () => {
          if (handlers.onClose) handlers.onClose()
        },
        socketGetter: () => ({
          close: () => {
            if (handlers.onClose) handlers.onClose()
          },
          send: (data) => {
            if (handlers.onMessage) handlers.onMessage({ data })
          }
        })
      }
    })
  },
  BsMessage: {},
  BsCard: {},
  BsFileUpload: {},
  BsProgress: {},
  BsInputBase: {},
  BsInputText: {},
  BsInputReadonly: {},
  BsSelect: {
    template: '<div><slot /></div>'
  },
  BsInputTextArea: {},
  BsInputNumber: {},
  BsInputSwitch: {},
  BsInputRadio: {},
  BsDropdown: {},
  BsModal: {},
  BsModalConfirm: {},
  BsInputTextAreaFormat: {},
  BsMenuBar: {},
  BsFooter: {},
  BsInput: {},
  BsButton: {},
  IconHome: {},
  IconData: {},
  IconTools: {},
  IconGraphUpArrow: {},
  IconCloudUpArrow: {},
  IconUpArrow: {},
  IconCpu: {},
  IconWifi: {},
  IconEye: {},
  IconEyeSlash: {},
  IconCheckCircle: {},
  IconXCircle: {},
  IconExclamationTriangle: {},
  IconInfoCircle: {},
  version: 'test-1.0.0'
}))

vtConfig.global = vtConfig.global || {}
vtConfig.global.components = vtConfig.global.components || {}

const uiStubs = [
  'BsInputReadonly',
  'BsInputNumber',
  'BsProgress',
  'BsFileUpload',
  'BsModalConfirm',
  'BsInput',
  'BsButton',
  'BsMessage',
  'BsCard',
  'BsInputText',
  'BsInputSwitch',
  'BsInputRadio',
  'BsDropdown',
  'BsModal',
  'BsInputTextAreaFormat',
  'BsSelect',
  'BsMenuBar',
  'BsFooter',
  'router-link',
  'router-view',
  'AdvancedFilesFragment',
  'EnableCorsFragment',
  'ListFilesFragment',
  'MeasurementGraphFragment',
  'MeasurementTableFragment',
  'VoltageFragment'
]

uiStubs.forEach((name) => {
  vtConfig.global.components[name] = {
    template: '<div><slot /></div>'
  }
})

vi.mock('chart.js', () => {
  const chartInstances = []

  class Chart {
    static register = vi.fn()

    constructor(ctx, config) {
      this.ctx = ctx
      this.config = config
      this.data = config.data
      this.options = config.options
      this.update = vi.fn()
      this.destroy = vi.fn()
      chartInstances.push(this)
    }
  }

  return {
    Chart,
    registerables: [],
    __chartInstances: chartInstances
  }
})

vi.mock('chart.js/auto', () => ({}))
