Releases
########

0.9.4
-----

* Added protection for firmware updates. The firmware file is now checked
  before it is flashed, both in the web UI (before the upload starts) and on
  the device (the first bytes of the upload). A file that is not a firmware
  image, is built for another chip, or has a different file name than the one
  the device reports is rejected.
* Added automatic rollback. If a newly flashed firmware crashes or hangs and
  never reports that it works, the device restarts into the previous working
  firmware after a few attempts. Rollback only works for updates made to a
  version that includes this feature, so the first update to this version is
  not protected. A firmware that fails before the startup code runs can only be
  recovered by flashing the device again.
* Added push support for Tilt devices. Tilt does not report a token or name, so
  the BLE token mapping (using the Tilt color as device id) is used. Tilt does
  not report angle, battery voltage or interval, so those values are sent as 0.
* Added Tilt weeks on battery and firmware version. They are shown in the
  device status and written to the SD card log. Tilt sends these in separate
  packets, the last known values are kept and added to each reading.
* Fixed Tilt signal strength (RSSI), it was always reported as 0.
* Fixed Tilt firmware version packets being handled as a measurement.
* Improved Tilt parsing so it no longer depends on the signedness of ``char``.

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

