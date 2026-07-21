import { beforeEach, describe, expect, it, vi } from 'vitest'
import { mount } from '@vue/test-utils'
import { sharedHttpClient as http } from '@mp-se/espframework-ui-components'
import { global } from '@/modules/pinia'
import SupportView from '@/views/SupportView.vue'

describe('SupportView (interaction tests)', () => {
  beforeEach(() => {
    vi.clearAllMocks()
    Object.assign(global, {
      disabled: false,
      platform: 'ESP32',
      app_ver: '1.2.3',
      app_build: '45',
      uiVersion: '2.5.0',
      uiBuild: '99',
      messageSuccess: ''
    })
  })

  it('renders support links and platform details', () => {
    const wrapper = mount(SupportView)
    const links = wrapper.findAll('a')

    expect(wrapper.text()).toContain('Links and device logs')
    expect(wrapper.text()).toContain('Platform')
    expect(wrapper.text()).toContain('User interface')
    expect(links.find((link) => link.attributes('href')?.includes('github.com'))).toBeDefined()
    expect(links.find((link) => link.attributes('href')?.includes('homebrewtalk'))).toBeDefined()
    expect(links.find((link) => link.attributes('href')?.includes('gravitymon.com'))).toBeDefined()
  })
})

describe('SupportView (action tests)', () => {
  beforeEach(() => {
    vi.clearAllMocks()
    Object.assign(global, {
      disabled: false,
      messageSuccess: ''
    })
  })

  it('fetchLog concatenates log lines in reverse order', async () => {
    http.filesystemRequest.mockResolvedValueOnce({
      success: true,
      text: 'Line 1\nLine 2\nLine 3'
    })
    const wrapper = mount(SupportView)

    const result = await wrapper.vm.fetchLog('/error.log')

    expect(result).toBe(true)
    expect(wrapper.vm.logData).toContain('Line 3')
    expect(wrapper.vm.logData.indexOf('Line 3')).toBeLessThan(wrapper.vm.logData.indexOf('Line 1'))
  })

  it('viewLogs fetches both log files and resets disabled state', async () => {
    http.filesystemRequest
      .mockResolvedValueOnce({ success: true, text: 'secondary log' })
      .mockResolvedValueOnce({ success: true, text: 'primary log' })
    const wrapper = mount(SupportView)

    wrapper.vm.viewLogs()
    await new Promise((resolve) => setTimeout(resolve, 0))

    expect(global.clearMessages).toHaveBeenCalled()
    expect(http.filesystemRequest).toHaveBeenCalledWith({ command: 'get', file: '/error2.log' })
    expect(http.filesystemRequest).toHaveBeenCalledWith({ command: 'get', file: '/error.log' })
    expect(global.disabled).toBe(false)
  })

  it('removeLogs deletes both log files and sets success message', async () => {
    http.filesystemRequest
      .mockResolvedValueOnce({ success: true })
      .mockResolvedValueOnce({ success: true })
    const wrapper = mount(SupportView)

    wrapper.vm.removeLogs()
    await new Promise((resolve) => setTimeout(resolve, 0))

    expect(http.filesystemRequest).toHaveBeenCalledWith({ command: 'del', file: '/error2.log' })
    expect(http.filesystemRequest).toHaveBeenCalledWith({ command: 'del', file: '/error.log' })
    expect(global.messageSuccess).toBe('Requested logs to be deleted')
  })

  it('fetchMdns formats timestamps and stores the JSON output', async () => {
    http.getJson.mockResolvedValueOnce({
      mdns: [
        {
          host: 'gw-1.local',
          last_seen: 1713012345
        }
      ]
    })
    const wrapper = mount(SupportView)

    const result = await wrapper.vm.fetchMdns()

    expect(result).toBe(true)
    expect(wrapper.vm.logData).toContain('gw-1.local')
    expect(wrapper.vm.logData).toContain('last_seen')
    expect(wrapper.vm.logData).not.toContain('1713012345')
  })
})
