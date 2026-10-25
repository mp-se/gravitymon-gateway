import { describe, expect, it } from 'vitest'
import { shallowMount } from '@vue/test-utils'
import AboutView from '@/views/AboutView.vue'

describe('AboutView (smoke)', () => {
  it('mounts without error', () => {
    const wrapper = shallowMount(AboutView)

    expect(wrapper.exists()).toBe(true)
  })
})
