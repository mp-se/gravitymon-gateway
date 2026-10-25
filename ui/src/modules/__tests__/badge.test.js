import { beforeEach, describe, expect, it } from 'vitest'
import { config } from '@/modules/pinia'
import {
  deviceBadge,
  deviceWifiBadge,
  pushBadge,
  deviceSettingBadge,
  deviceHardwareBadge,
  deviceMdnsBadge,
  deviceWifi1Badge,
  deviceWifi2Badge,
  pushSettingBadge,
  pushHttpPost1Badge,
  pushHttpPost2Badge,
  pushHttpGetBadge,
  pushHttpInfluxdb2Badge,
  pushHttpMqttBadge,
  pushHttpBluetoothBadge
} from '@/modules/badge'

describe('badge module', () => {
  beforeEach(() => {
    Object.assign(config, {
      mdns: '',
      wifi_ssid: '',
      wifi_ssid2: '',
      http_post_target: '',
      http_post2_target: '',
      http_get_target: '',
      influxdb2_target: '',
      mqtt_target: ''
    })
  })

  it('counts missing device configuration badges', () => {
    expect(deviceBadge()).toBe(2)
  })

  it('clears the wifi badge when the primary network is configured', () => {
    config.wifi_ssid = 'brew-net'

    expect(deviceWifiBadge()).toBe(0)
  })

  it('flags every push target when none are configured', () => {
    expect(pushBadge()).toBe(5)
  })

  it('clears push badges when at least one target exists', () => {
    config.mqtt_target = 'mqtt://broker'

    expect(pushBadge()).toBe(0)
  })

  it('deviceSettingBadge returns 1 when mdns is not configured', () => {
    config.mdns = ''
    expect(deviceSettingBadge()).toBe(1)
  })

  it('deviceSettingBadge returns 0 when mdns is configured', () => {
    config.mdns = 'gateway-test'
    expect(deviceSettingBadge()).toBe(0)
  })

  it('deviceHardwareBadge always returns false', () => {
    expect(deviceHardwareBadge()).toBe(false)
  })

  it('deviceMdnsBadge returns 1 when mdns is empty', () => {
    config.mdns = ''
    expect(deviceMdnsBadge()).toBe(1)
  })

  it('deviceMdnsBadge returns 0 when mdns is set', () => {
    config.mdns = 'my-gateway'
    expect(deviceMdnsBadge()).toBe(0)
  })

  it('deviceWifi1Badge returns 1 when wifi_ssid is empty', () => {
    config.wifi_ssid = ''
    expect(deviceWifi1Badge()).toBe(1)
  })

  it('deviceWifi1Badge returns 0 when wifi_ssid is set', () => {
    config.wifi_ssid = 'my-network'
    expect(deviceWifi1Badge()).toBe(0)
  })

  it('deviceWifi2Badge returns 1 when both SSIDs are empty', () => {
    config.wifi_ssid = ''
    config.wifi_ssid2 = ''
    expect(deviceWifi2Badge()).toBe(1)
  })

  it('deviceWifi2Badge returns 0 when primary SSID is set', () => {
    config.wifi_ssid = 'my-network'
    config.wifi_ssid2 = ''
    expect(deviceWifi2Badge()).toBe(0)
  })

  it('pushSettingBadge always returns 0', () => {
    expect(pushSettingBadge()).toBe(0)
  })

  it('pushHttpPost1Badge returns 1 when no targets are configured', () => {
    expect(pushHttpPost1Badge()).toBe(1)
  })

  it('pushHttpPost1Badge returns 0 when any target is configured', () => {
    config.http_post_target = 'http://example.com'
    expect(pushHttpPost1Badge()).toBe(0)
  })

  it('pushHttpPost2Badge returns 1 when no targets are configured', () => {
    expect(pushHttpPost2Badge()).toBe(1)
  })

  it('pushHttpPost2Badge returns 0 when any target is configured', () => {
    config.influxdb2_target = 'http://influx.example.com'
    expect(pushHttpPost2Badge()).toBe(0)
  })

  it('pushHttpGetBadge returns 1 when no targets are configured', () => {
    expect(pushHttpGetBadge()).toBe(1)
  })

  it('pushHttpGetBadge returns 0 when any target is configured', () => {
    config.mqtt_target = 'mqtt://broker.example.com'
    expect(pushHttpGetBadge()).toBe(0)
  })

  it('pushHttpInfluxdb2Badge returns 1 when no targets are configured', () => {
    expect(pushHttpInfluxdb2Badge()).toBe(1)
  })

  it('pushHttpInfluxdb2Badge returns 0 when any target is configured', () => {
    config.http_get_target = 'http://api.example.com/data'
    expect(pushHttpInfluxdb2Badge()).toBe(0)
  })

  it('pushHttpMqttBadge returns 1 when no targets are configured', () => {
    expect(pushHttpMqttBadge()).toBe(1)
  })

  it('pushHttpMqttBadge returns 0 when any target is configured', () => {
    config.http_post2_target = 'http://example2.com'
    expect(pushHttpMqttBadge()).toBe(0)
  })

  it('pushHttpBluetoothBadge returns 1 when no targets are configured', () => {
    expect(pushHttpBluetoothBadge()).toBe(1)
  })

  it('pushHttpBluetoothBadge returns 0 when any target is configured', () => {
    config.mqtt_target = 'mqtt://broker'
    expect(pushHttpBluetoothBadge()).toBe(0)
  })

  it('multiple targets configured reduces all push badges to 0', () => {
    config.http_post_target = 'http://example.com'
    config.mqtt_target = 'mqtt://broker'
    config.influxdb2_target = 'http://influx.example.com'

    expect(pushHttpPost1Badge()).toBe(0)
    expect(pushHttpPost2Badge()).toBe(0)
    expect(pushHttpGetBadge()).toBe(0)
    expect(pushHttpInfluxdb2Badge()).toBe(0)
    expect(pushHttpMqttBadge()).toBe(0)
    expect(pushBadge()).toBe(0)
  })
})
