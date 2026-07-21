import { beforeEach, describe, expect, it, vi } from 'vitest'

describe('router module', () => {
  let createRouterMock
  let createWebHistoryMock
  let validateCurrentFormMock
  let globalStore
  let guard
  let createRouterArgs

  beforeEach(() => {
    vi.resetModules()
    guard = null
    createRouterArgs = null
    globalStore = {
      disabled: false,
      clearMessages: vi.fn()
    }
    validateCurrentFormMock = vi.fn(() => true)
    createWebHistoryMock = vi.fn((base) => ({ type: 'history', base }))
    createRouterMock = vi.fn((options) => {
      createRouterArgs = options
      return {
        beforeEach: vi.fn((callback) => {
          guard = callback
        })
      }
    })

    vi.doMock('vue-router', () => ({
      createRouter: createRouterMock,
      createWebHistory: createWebHistoryMock
    }))

    vi.doMock('@mp-se/espframework-ui-components', () => ({
      validateCurrentForm: validateCurrentFormMock
    }))

    vi.doMock('@/modules/pinia', () => ({
      global: globalStore
    }))

    vi.doMock('@/modules/badge', () => ({
      deviceBadge: vi.fn(),
      deviceSettingBadge: vi.fn(),
      deviceHardwareBadge: vi.fn(),
      deviceWifiBadge: vi.fn(),
      pushBadge: vi.fn(),
      pushSettingBadge: vi.fn(),
      pushHttpPost1Badge: vi.fn(),
      pushHttpPost2Badge: vi.fn(),
      pushHttpGetBadge: vi.fn(),
      pushHttpInfluxdb2Badge: vi.fn(),
      pushHttpMqttBadge: vi.fn()
    }))
  })

  it('creates the router with the expected history and routes', async () => {
    await import('../router.js')

    expect(createWebHistoryMock).toHaveBeenCalledWith('/')
    expect(createRouterMock).toHaveBeenCalledTimes(1)
    expect(createRouterArgs.routes).toHaveLength(21)
    expect(createRouterArgs.routes[0]).toMatchObject({ path: '/', name: 'home' })
    expect(createRouterArgs.routes.at(-1)).toMatchObject({ name: '404', path: '/:catchAll(.*)' })
  })

  it('exports menu items for the main sections', async () => {
    const { items } = await import('../router.js')

    expect(items.value.map((item) => item.label)).toEqual([
      'Home',
      'Device',
      'Measurements',
      'Push targets',
      'Other'
    ])
    expect(items.value[1].subs.map((item) => item.label)).toContain('Wifi AP')
    expect(items.value[3].subs.map((item) => item.path)).toContain('/push/mqtt')
  })

  it('blocks navigation when the global store is disabled', async () => {
    await import('../router.js')
    globalStore.disabled = true

    expect(guard()).toBe(false)
    expect(validateCurrentFormMock).not.toHaveBeenCalled()
  })

  it('blocks navigation when form validation fails', async () => {
    await import('../router.js')
    validateCurrentFormMock.mockReturnValue(false)

    expect(guard()).toBe(false)
    expect(globalStore.clearMessages).not.toHaveBeenCalled()
  })

  it('clears messages and allows navigation when state is valid', async () => {
    await import('../router.js')

    expect(guard()).toBe(true)
    expect(validateCurrentFormMock).toHaveBeenCalled()
    expect(globalStore.clearMessages).toHaveBeenCalled()
  })
})
