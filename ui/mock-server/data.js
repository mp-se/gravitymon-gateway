/*
 * Project specific data objects, should contain configData and statusData as minimum
 *
 * (c) 2023-2024 Magnus Persson
 */

export var configData = {
  // Device configuration
  id: "7376ef",
  mdns: "gateway",
  temp_unit: "C",
  gravity_unit: "G",
  pressure_unit: "PSI",
  dark_mode: false, 
  // Hardware
  ble_enable: true,
  ble_active_scan: false,
  ble_scan_time: 5,
  timezone: "CET-1CEST,M3.5.0,M10.5.0/3",
  // Logging
  sd_log_files: 4,
  sd_log_min_time: 5, // Minutes
  display_layout_id: 0,
  // Wifi
  wifi_portal_timeout: 120,
  wifi_connect_timeout: 20,
  wifi_ssid: "network A",
  wifi_ssid2: "",
  wifi_pass: "password",
  wifi_pass2: "mypass",
  // Push - Generic
  token: "mytoken",
  push_timeout: 10,
  push_resend_time: 300, 

  // Push - Http Post 1
  http_post_target: 'http://post.home.arpa:9090/api/v1/ZYfjlUNeiuyu9N/telemetry',
  http_post_header1: 'Auth: Basic T7IF9DD9fF3RDddE=',
  http_post_header2: '',
  http_post_gravity: true,
  http_post_pressure: true,
  // Push - Http Post 2
  http_post2_target: 'http://post2.home.arpa/ispindel',
  http_post2_header1: '',
  http_post2_header2: '',
  http_post2_gravity: true,
  http_post2_pressure: true,
  // Push - Http Get
  http_get_target: 'http://get.home.arpa/ispindel',
  http_get_header1: '',
  http_get_header2: '',
  http_get_gravity: true,
  http_get_pressure: true,
  // Push - Influx
  influxdb2_target: 'http://influx.home.arpa:8086',
  influxdb2_org: 'myorg',
  influxdb2_bucket: 'mybucket',
  influxdb2_token: 'OijkU((jhfkh=',
  influxdb2_gravity: true,
  influxdb2_pressure: true,
  // Push - MQTT
  mqtt_target: 'mqtt.home.arpa',
  mqtt_port: 1883,
  mqtt_user: 'user',
  mqtt_pass: 'pass',
  mqtt_gravity: true,
  mqtt_pressure: true,
}

export var formatData = {
  http_post_format_gravity: "%7B%22name%22%20%3A%20%22gravmon%22%2C%20%22ID%22%3A%20%22%24%7Bid%7D%22%2C%20%22token%22%20%3A%20%22gravmon%22%2C%20%22interval%22%3A%20%24%7Bsleep%2Dinterval%7D%2C%20%22temperature%22%3A%20%24%7Btemp%7D%2C%20%22temp%2Dunits%22%3A%20%22%24%7Btemp%2Dunit%7D%22%2C%20%22gravity%22%3A%20%24%7Bgravity%7D%2C%20%22angle%22%3A%20%24%7Bangle%7D%2C%20%22battery%22%3A%20%24%7Bbattery%7D%2C%20%22rssi%22%3A%20%24%7Brssi%7D%2C%20%22corr%2Dgravity%22%3A%20%24%7Bcorr%2Dgravity%7D%2C%20%22gravity%2Dunit%22%3A%20%22%24%7Bgravity%2Dunit%7D%22%2C%20%22run%2Dtime%22%3A%20%24%7Brun%2Dtime%7D%20%7D",
  http_post2_format_gravity: "%7B%22name%22%20%3A%20%22gravmon%22%2C%20%22ID%22%3A%20%22%24%7Bid%7D%22%2C%20%22token%22%20%3A%20%22gravmon%22%2C%20%22interval%22%3A%20%24%7Bsleep%2Dinterval%7D%2C%20%22temperature%22%3A%20%24%7Btemp%7D%2C%20%22temp%2Dunits%22%3A%20%22%24%7Btemp%2Dunit%7D%22%2C%20%22gravity%22%3A%20%24%7Bgravity%7D%2C%20%22angle%22%3A%20%24%7Bangle%7D%2C%20%22battery%22%3A%20%24%7Bbattery%7D%2C%20%22rssi%22%3A%20%24%7Brssi%7D%2C%20%22corr%2Dgravity%22%3A%20%24%7Bcorr%2Dgravity%7D%2C%20%22gravity%2Dunit%22%3A%20%22%24%7Bgravity%2Dunit%7D%22%2C%20%22run%2Dtime%22%3A%20%24%7Brun%2Dtime%7D%20%7D",
  http_get_format_gravity: "%7B%22name%22%20%3A%20%22gravmon%22%2C%20%22ID%22%3A%20%22%24%7Bid%7D%22%2C%20%22token%22%20%3A%20%22gravmon%22%2C%20%22interval%22%3A%20%24%7Bsleep%2Dinterval%7D%2C%20%22temperature%22%3A%20%24%7Btemp%7D%2C%20%22temp%2Dunits%22%3A%20%22%24%7Btemp%2Dunit%7D%22%2C%20%22gravity%22%3A%20%24%7Bgravity%7D%2C%20%22angle%22%3A%20%24%7Bangle%7D%2C%20%22battery%22%3A%20%24%7Bbattery%7D%2C%20%22rssi%22%3A%20%24%7Brssi%7D%2C%20%22corr%2Dgravity%22%3A%20%24%7Bcorr%2Dgravity%7D%2C%20%22gravity%2Dunit%22%3A%20%22%24%7Bgravity%2Dunit%7D%22%2C%20%22run%2Dtime%22%3A%20%24%7Brun%2Dtime%7D%20%7D",
  influxdb2_format_gravity: "measurement%2Chost%3D%24%7Bmdns%7D%2Cdevice%3D%24%7Bid%7D%2Ctemp%2Dformat%3D%24%7Btemp%2Dunit%7D%2Cgravity%2Dformat%3D%24%7Bgravity%2Dunit%7D%20gravity%3D%24%7Bgravity%7D%2Ccorr%2Dgravity%3D%24%7Bcorr%2Dgravity%7D%2Cangle%3D%24%7Bangle%7D%2Ctemp%3D%24%7Btemp%7D%2Cbattery%3D%24%7Bbattery%7D%2Crssi%3D%24%7Brssi%7D%0A",
  mqtt_format_gravity: "ispindel%2F%24%7Bmdns%7D%2Ftilt%3A%24%7Bangle%7D%7Cispindel%2F%24%7Bmdns%7D%2Ftemperature%3A%24%7Btemp%7D%7Cispindel%2F%24%7Bmdns%7D%2Ftemp%5Funits%3A%24%7Btemp%2Dunit%7D%7Cispindel%2F%24%7Bmdns%7D%2Fbattery%3A%24%7Bbattery%7D%7Cispindel%2F%24%7Bmdns%7D%2Fgravity%3A%24%7Bgravity%7D%7Cispindel%2F%24%7Bmdns%7D%2Finterval%3A%24%7Bsleep%2Dinterval%7D%7Cispindel%2F%24%7Bmdns%7D%2FRSSI%3A%24%7Brssi%7D%7C",

  http_post_format_pressure: "%7B%22name%22%20%3A%20%22gravmon%22%2C%20%22ID%22%3A%20%22%24%7Bid%7D%22%2C%20%22token%22%20%3A%20%22gravmon%22%2C%20%22interval%22%3A%20%24%7Bsleep%2Dinterval%7D%2C%20%22temperature%22%3A%20%24%7Btemp%7D%2C%20%22temp%2Dunits%22%3A%20%22%24%7Btemp%2Dunit%7D%22%2C%20%22gravity%22%3A%20%24%7Bgravity%7D%2C%20%22angle%22%3A%20%24%7Bangle%7D%2C%20%22battery%22%3A%20%24%7Bbattery%7D%2C%20%22rssi%22%3A%20%24%7Brssi%7D%2C%20%22corr%2Dgravity%22%3A%20%24%7Bcorr%2Dgravity%7D%2C%20%22gravity%2Dunit%22%3A%20%22%24%7Bgravity%2Dunit%7D%22%2C%20%22run%2Dtime%22%3A%20%24%7Brun%2Dtime%7D%20%7D",
  http_post2_format_pressure: "%7B%22name%22%20%3A%20%22gravmon%22%2C%20%22ID%22%3A%20%22%24%7Bid%7D%22%2C%20%22token%22%20%3A%20%22gravmon%22%2C%20%22interval%22%3A%20%24%7Bsleep%2Dinterval%7D%2C%20%22temperature%22%3A%20%24%7Btemp%7D%2C%20%22temp%2Dunits%22%3A%20%22%24%7Btemp%2Dunit%7D%22%2C%20%22gravity%22%3A%20%24%7Bgravity%7D%2C%20%22angle%22%3A%20%24%7Bangle%7D%2C%20%22battery%22%3A%20%24%7Bbattery%7D%2C%20%22rssi%22%3A%20%24%7Brssi%7D%2C%20%22corr%2Dgravity%22%3A%20%24%7Bcorr%2Dgravity%7D%2C%20%22gravity%2Dunit%22%3A%20%22%24%7Bgravity%2Dunit%7D%22%2C%20%22run%2Dtime%22%3A%20%24%7Brun%2Dtime%7D%20%7D",
  http_get_format_pressure: "%7B%22name%22%20%3A%20%22gravmon%22%2C%20%22ID%22%3A%20%22%24%7Bid%7D%22%2C%20%22token%22%20%3A%20%22gravmon%22%2C%20%22interval%22%3A%20%24%7Bsleep%2Dinterval%7D%2C%20%22temperature%22%3A%20%24%7Btemp%7D%2C%20%22temp%2Dunits%22%3A%20%22%24%7Btemp%2Dunit%7D%22%2C%20%22gravity%22%3A%20%24%7Bgravity%7D%2C%20%22angle%22%3A%20%24%7Bangle%7D%2C%20%22battery%22%3A%20%24%7Bbattery%7D%2C%20%22rssi%22%3A%20%24%7Brssi%7D%2C%20%22corr%2Dgravity%22%3A%20%24%7Bcorr%2Dgravity%7D%2C%20%22gravity%2Dunit%22%3A%20%22%24%7Bgravity%2Dunit%7D%22%2C%20%22run%2Dtime%22%3A%20%24%7Brun%2Dtime%7D%20%7D",
  influxdb2_format_pressure: "measurement%2Chost%3D%24%7Bmdns%7D%2Cdevice%3D%24%7Bid%7D%2Ctemp%2Dformat%3D%24%7Btemp%2Dunit%7D%2Cgravity%2Dformat%3D%24%7Bgravity%2Dunit%7D%20gravity%3D%24%7Bgravity%7D%2Ccorr%2Dgravity%3D%24%7Bcorr%2Dgravity%7D%2Cangle%3D%24%7Bangle%7D%2Ctemp%3D%24%7Btemp%7D%2Cbattery%3D%24%7Bbattery%7D%2Crssi%3D%24%7Brssi%7D%0A",
  mqtt_format_pressure: "ispindel%2F%24%7Bmdns%7D%2Ftilt%3A%24%7Bangle%7D%7Cispindel%2F%24%7Bmdns%7D%2Ftemperature%3A%24%7Btemp%7D%7Cispindel%2F%24%7Bmdns%7D%2Ftemp%5Funits%3A%24%7Btemp%2Dunit%7D%7Cispindel%2F%24%7Bmdns%7D%2Fbattery%3A%24%7Bbattery%7D%7Cispindel%2F%24%7Bmdns%7D%2Fgravity%3A%24%7Bgravity%7D%7Cispindel%2F%24%7Bmdns%7D%2Finterval%3A%24%7Bsleep%2Dinterval%7D%7Cispindel%2F%24%7Bmdns%7D%2FRSSI%3A%24%7Brssi%7D%7C"
}

export var statusData = {
  id: "7376ef",
  angle: 22.4,
  rssi: -56,
  mdns: "gateway",
  wifi_ssid: "wifi",
  total_heap: 1000,
  free_heap: 500,
  ip: "192.0.0.1",
  wifi_setup: false,
  uptime_seconds: 1,
  uptime_minutes: 2,
  uptime_hours: 3,
  uptime_days: 4,
  sd_mounted: true,

  gravity_device: [ 
    { device: "dev1", name: "test1", gravity: 1.100, temp: 24, update_time: 100, push_time: 10, source: "HTTP Post", type: "Gravitymon" },  
    { device: "dev2", name: "test2", gravity: 1.100, temp: 24, update_time: 200, push_time: 10, source: "HTTP Post", type: "Gravitymon" },
    { device: "dev3", name: "test3", gravity: 1.100, temp: 24, update_time: 300, push_time: 10, source: "HTTP Post", type: "Gravitymon" },  
    { device: "dev4", name: "test4", gravity: 1.100, temp: 24, update_time: 400, push_time: 10, source: "HTTP Post", type: "Gravitymon" },  
    { device: "dev5", name: "test5", gravity: 1.100, temp: 24, update_time: 500, push_time: 10, source: "HTTP Post", type: "Gravitymon" },  
    { device: "devA", name: "blue", gravity: 1.100, temp: 24, update_time: 100, push_time: 10, source: "BLE Beacon", type: "Tilt"  },  
    { device: "6d3fb9", name: "6d3fb9", gravity: 1.234567, velocity: 0, temp: 42.12344, update_time: 6, push_time: 104, source: "BLE Beacon", type: "RAPT" }
  ],
  pressure_device: [ 
    { device: "dev6", name: "test1", pressure: 1.23, temp: 24, update_time: 100, push_time: 10, source: "HTTP Post", type: "Pressuremon" },  
    { device: "dev7", name: "test2", pressure: 0.95, temp: 24, update_time: 200, push_time: 10, source: "HTTP Post", type: "Pressuremon" },
    { device: "dev8", name: "test3", pressure: 2.23, temp: 24, update_time: 300, push_time: 10, source: "HTTP Post", type: "Pressuremon" },  
  ],
  temperature_device: [ 
    { device: "dev9", name: "", chamber_temp: 1.23, beer_temp: 24, update_time: 100, push_time: 10, source: "BLE Beacon", type: "Chamber Controller" }
  ]
}

export var featureData = {
  board: 'D1_MINI',
  platform: 'esp32c3',
  app_ver: '2.0.0',
  app_build: 'gitrev',
  hardware: 'ispindel',
  firmware_file: 'firmware.bin',

  // Feature flags
  tft: true,
  sd: true,
}

// EOF