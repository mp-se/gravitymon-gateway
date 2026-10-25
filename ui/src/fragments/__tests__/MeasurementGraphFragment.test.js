import { beforeEach, describe, expect, it, vi } from 'vitest'
import { mount } from '@vue/test-utils'
import MeasurementGraphFragment from '../MeasurementGraphFragment.vue'
import { __chartInstances, Chart } from 'chart.js'
import { logDebug, logError } from '@mp-se/espframework-ui-components'

describe('MeasurementGraphFragment', () => {
  beforeEach(() => {
    vi.clearAllMocks()
    __chartInstances.length = 0
  })

  it('registers chart components and creates a chart on mount', () => {
    const wrapper = mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: new Date('2025-01-09T09:00:00Z'), y: 1.012 }],
        dataSet2: [{ x: new Date('2025-01-09T09:00:00Z'), y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(Chart.register).toHaveBeenCalled()
    expect(wrapper.find('canvas').exists()).toBe(true)
    expect(__chartInstances).toHaveLength(1)
    expect(__chartInstances[0].data.datasets[0].label).toBe('Gravity')
  })

  it('updates the existing chart when model props change', async () => {
    const wrapper = mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    await wrapper.setProps({
      dataSet1: [{ x: 2, y: 1.01 }],
      dataSet2: [{ x: 2, y: 19.8 }],
      label1: 'SG',
      label2: 'Temp'
    })

    expect(__chartInstances[0].data.datasets[0].data).toEqual([{ x: 2, y: 1.01 }])
    expect(__chartInstances[0].data.datasets[1].label).toBe('Temp')
    expect(__chartInstances[0].update).toHaveBeenCalled()
  })

  it('logs debug message on mount with initial props', () => {
    logDebug.mockClear()
    mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(logDebug).toHaveBeenCalledWith(
      'MeasurementGraphFragment.onMounted()',
      expect.anything(),
      expect.anything(),
      expect.anything(),
      expect.anything()
    )
  })

  it('logs debug message when creating chart', () => {
    logDebug.mockClear()
    mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(logDebug).toHaveBeenCalledWith('MeasurementGraphFragment.onMounted()', 'Creating chart')
  })

  it('handles missing canvas element gracefully', () => {
    logError.mockClear()
    const wrapper = mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: undefined
    })

    expect(wrapper.vm).toBeDefined()
  })

  it('logs debug message when creating chart', () => {
    logDebug.mockClear()
    mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(logDebug).toHaveBeenCalledWith('MeasurementGraphFragment.onMounted()', 'Creating chart')
  })

  it('chart configuration has correct axes setup', () => {
    mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(__chartInstances[0].options.scales.x.type).toBe('time')
    expect(__chartInstances[0].options.scales.x.time.unit).toBe('hour')
  })

  it('chart has animation disabled', () => {
    mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(__chartInstances[0].options.animation).toBe(false)
  })

  it('first dataset uses green color', () => {
    mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(__chartInstances[0].data.datasets[0].borderColor).toBe('green')
    expect(__chartInstances[0].data.datasets[0].backgroundColor).toBe('green')
  })

  it('second dataset uses blue color', () => {
    mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(__chartInstances[0].data.datasets[1].borderColor).toBe('blue')
    expect(__chartInstances[0].data.datasets[1].backgroundColor).toBe('blue')
  })

  it('datasets use separate y axes', () => {
    mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(__chartInstances[0].data.datasets[0].yAxisID).toBe('y1')
    expect(__chartInstances[0].data.datasets[1].yAxisID).toBe('y2')
  })

  it('chart uses cubic interpolation for smooth curves', () => {
    mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(__chartInstances[0].data.datasets[0].cubicInterpolationMode).toBe('monotone')
    expect(__chartInstances[0].data.datasets[1].cubicInterpolationMode).toBe('monotone')
  })

  it('chart has tension set for smooth animation', () => {
    mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(__chartInstances[0].data.datasets[0].tension).toBe(0.4)
    expect(__chartInstances[0].data.datasets[1].tension).toBe(0.4)
  })

  it('chart has point radius set to zero', () => {
    mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(__chartInstances[0].data.datasets[0].pointRadius).toBe(0)
    expect(__chartInstances[0].data.datasets[1].pointRadius).toBe(0)
  })

  it('updates only first dataset label when dataSet1 label changes', async () => {
    const wrapper = mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'OriginalLabel1',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    await wrapper.setProps({
      label1: 'NewLabel1'
    })

    expect(__chartInstances[0].data.datasets[0].label).toBe('NewLabel1')
  })

  it('updates only second dataset label when dataSet2 label changes', async () => {
    const wrapper = mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'OriginalLabel2'
      },
      attachTo: document.body
    })

    await wrapper.setProps({
      label2: 'NewLabel2'
    })

    expect(__chartInstances[0].data.datasets[1].label).toBe('NewLabel2')
  })

  it('updates only first dataset data when dataSet1 changes', async () => {
    const wrapper = mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.0 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    const newData1 = [{ x: 2, y: 1.05 }]
    await wrapper.setProps({
      dataSet1: newData1
    })

    expect(__chartInstances[0].data.datasets[0].data).toEqual(newData1)
  })

  it('updates only second dataset data when dataSet2 changes', async () => {
    const wrapper = mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.0 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    const newData2 = [{ x: 2, y: 21.5 }]
    await wrapper.setProps({
      dataSet2: newData2
    })

    expect(__chartInstances[0].data.datasets[1].data).toEqual(newData2)
  })

  it('chart updates when all props change simultaneously', async () => {
    const wrapper = mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Old1',
        label2: 'Old2'
      },
      attachTo: document.body
    })

    const newData1 = [{ x: 3, y: 1.08 }]
    const newData2 = [{ x: 3, y: 22.5 }]

    await wrapper.setProps({
      dataSet1: newData1,
      dataSet2: newData2,
      label1: 'New1',
      label2: 'New2'
    })

    expect(__chartInstances[0].data.datasets[0].data).toEqual(newData1)
    expect(__chartInstances[0].data.datasets[0].label).toBe('New1')
    expect(__chartInstances[0].data.datasets[1].data).toEqual(newData2)
    expect(__chartInstances[0].data.datasets[1].label).toBe('New2')
    expect(__chartInstances[0].update).toHaveBeenCalled()
  })

  it('watch callback responds to dataSet1 changes', async () => {
    const wrapper = mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    const updateSpy = vi.spyOn(__chartInstances[0], 'update')
    updateSpy.mockClear()

    await wrapper.setProps({
      dataSet1: [{ x: 5, y: 1.02 }]
    })

    expect(updateSpy).toHaveBeenCalled()
  })

  it('watch callback responds to dataSet2 changes', async () => {
    const wrapper = mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    const updateSpy = vi.spyOn(__chartInstances[0], 'update')
    updateSpy.mockClear()

    await wrapper.setProps({
      dataSet2: [{ x: 5, y: 25.5 }]
    })

    expect(updateSpy).toHaveBeenCalled()
  })

  it('watch callback responds to label1 changes', async () => {
    const wrapper = mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'OriginalLabel',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    const updateSpy = vi.spyOn(__chartInstances[0], 'update')
    updateSpy.mockClear()

    await wrapper.setProps({
      label1: 'ChangedLabel'
    })

    expect(updateSpy).toHaveBeenCalled()
  })

  it('watch callback responds to label2 changes', async () => {
    const wrapper = mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'OriginalLabel'
      },
      attachTo: document.body
    })

    const updateSpy = vi.spyOn(__chartInstances[0], 'update')
    updateSpy.mockClear()

    await wrapper.setProps({
      label2: 'ChangedLabel'
    })

    expect(updateSpy).toHaveBeenCalled()
  })

  it('canvas element has correct id', () => {
    const wrapper = mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(wrapper.find('#dataChart').exists()).toBe(true)
  })

  it('component renders within container row and column divs', () => {
    const wrapper = mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(wrapper.find('.row').exists()).toBe(true)
    expect(wrapper.find('.col-md-12').exists()).toBe(true)
  })

  it('chart registers all required plugins', () => {
    mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(Chart.register).toHaveBeenCalled()
  })

  it('time format configuration includes all required units', () => {
    mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [{ x: 1, y: 1.012 }],
        dataSet2: [{ x: 1, y: 20.1 }],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    const timeConfig = __chartInstances[0].options.scales.x.time
    expect(timeConfig.displayFormats).toHaveProperty('hour')
    expect(timeConfig.displayFormats).toHaveProperty('day')
    expect(timeConfig.displayFormats).toHaveProperty('week')
    expect(timeConfig.displayFormats).toHaveProperty('month')
  })

  it('handles empty dataset arrays', () => {
    mount(MeasurementGraphFragment, {
      props: {
        dataSet1: [],
        dataSet2: [],
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(__chartInstances).toHaveLength(1)
    expect(__chartInstances[0].data.datasets[0].data).toEqual([])
    expect(__chartInstances[0].data.datasets[1].data).toEqual([])
  })

  it('handles large datasets', () => {
    const largeData1 = Array.from({ length: 1000 }, (_, i) => ({ x: i, y: 1.01 + i * 0.001 }))
    const largeData2 = Array.from({ length: 1000 }, (_, i) => ({ x: i, y: 20 + i * 0.01 }))

    mount(MeasurementGraphFragment, {
      props: {
        dataSet1: largeData1,
        dataSet2: largeData2,
        label1: 'Gravity',
        label2: 'Temperature'
      },
      attachTo: document.body
    })

    expect(__chartInstances[0].data.datasets[0].data).toHaveLength(1000)
    expect(__chartInstances[0].data.datasets[1].data).toHaveLength(1000)
  })
})
