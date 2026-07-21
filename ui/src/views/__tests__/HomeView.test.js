import { beforeEach, describe, expect, it, vi } from 'vitest'
import { mount } from '@vue/test-utils'
import { config, global, status } from '@/modules/pinia'
import HomeView from '@/views/HomeView.vue'

describe('HomeView', () => {
  beforeEach(() => {
    vi.clearAllMocks()

    Object.assign(config, {
      gravity_unit: 'G',
      temp_unit: 'C',
      pressure_unit: 'PSI'
    })

    Object.assign(global, {
      app_ver: '1.2.3',
      app_build: '45',
      uiVersion: '2.5.0',
      uiBuild: '99',
      platform: 'ESP32',
      board: 'S3'
    })

    Object.assign(status, {
      gravity_device: [
        {
          device: 'gravity-1',
          name: 'Fermenter',
          update_time: 120,
          push_time: 3600,
          gravity: 1.05,
          temp: 20.5,
          source: 'BLE',
          type: 'Tilt'
        }
      ],
      pressure_device: [
        {
          device: 'pressure-1',
          name: 'Pressure Tank',
          update_time: 59,
          push_time: 86400,
          pressure: 12.345,
          pressure1: 11.2,
          temp: 19.5,
          source: 'BLE',
          type: 'Pressuremon'
        }
      ],
      temperature_device: [
        {
          device: 'temp-1',
          name: 'Chamber',
          update_time: 61,
          chamber_temp: 18.3,
          beer_temp: 20.1,
          source: 'BLE',
          type: 'Probe'
        }
      ],
      rssi: -55,
      wifi_ssid: 'brew-net',
      ip: '192.168.1.2',
      free_heap: 123,
      total_heap: 456,
      id: 'GW-1',
      uptime_days: 1,
      uptime_hours: 2,
      uptime_minutes: 3,
      uptime_seconds: 4,
      load: vi.fn(async () => true)
    })
  })

  it('renders device summaries and refreshes status on mount', async () => {
    const wrapper = mount(HomeView)
    await Promise.resolve()

    expect(status.load).toHaveBeenCalledTimes(1)
    expect(wrapper.text()).toContain('Gravity: 1.050')
    expect(wrapper.text()).toContain('Temperature: 20.50 C')
    expect(wrapper.text()).toContain('Pressure: 12.345')
    expect(wrapper.text()).toContain('brew-net')
    expect(wrapper.text()).toContain('Firmware: 1.2.3 (45) UI: 2.5.0 (99)')

    wrapper.unmount()
  })
})
