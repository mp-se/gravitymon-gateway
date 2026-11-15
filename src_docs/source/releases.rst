Releases
########

0.9.3
-----

* Added push support for BLE data transmission (GravityMon, Pressuremon 
  and RAPT devices. Readings are forwarded to the configured. RAPT does not 
  report battery voltage or interval, so those values are sent as 0.
* Added BLE token mapping to the web interface to support data push to BrewGraph 
  and other services.
* Added BrewGraph URL options for gravity, pressure, and automatic ingestion.
* Improved mDNS device management by limiting the number of devices and
  reducing memory use.
* Improved how the web interface saves configuration state.
* Updated web interface dependencies (including PurgeCSS v9) and the CI build
  workflow.

0.9.2
-----

* Updated the LVGL library dependency and adjusted the display task core
  assignment.
* Added a Sphinx documentation workflow and expanded the project documentation.

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

