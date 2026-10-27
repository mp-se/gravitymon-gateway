Releases
########

0.9.1
-----

* Added support for the Waveshare ESP32-S3 4.3-inch TFT gateway board and
  refreshed display layouts.
* Updated the web interface and integrated its build into the firmware project.
* Improved BLE parsing and reporting for GravityMon, PressureMon, Chamber
  Controller, Tilt, and RAPT devices, including battery percentage, RSSI, and
  transmit power where available.
* Preserve unavailable BLE sensor readings instead of converting them to zero.
* Added mDNS discovery to associate BLE device IDs with network names.
* Improved status data and added an authenticated mDNS API endpoint.
* Updated ESP32, LVGL, and supporting framework dependencies and build workflows.
* Added a DS18B20 sensor checker utility.

Earlier versions
----------------

For earlier gateway releases, see the `project README
<https://github.com/mp-se/gravitymon-gateway#releases>`_.

