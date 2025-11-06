
![download](https://img.shields.io/github/downloads/mp-se/gravitymon-gateway/total) 
![release](https://img.shields.io/github/release/mp-se/gravitymon-gateway?label=latest%20release)
![issues](https://img.shields.io/github/issues/mp-se/gravitymon-gateway)
![pr](https://img.shields.io/github/issues-pr/mp-se/gravitymon-gateway)
![dev_build](https://img.shields.io/github/actions/workflow/status/mp-se/gravitymon-gateway/pio-build.yaml?branch=dev)

# GravityMon Gateway

This is a companion device for use with GravityMon v2.0, it will support the new BLE options and Wifi Direct options introduced into v2.0. 

For documentation see www.gravitymon.com.

# About GravityMon & GravityMon Gateway

Visit the gravitymon homepage here for more information about the project: https://www.gravitymon.com

# Releases

### v0.8.1

* Now display file size in file system viewer
* Added option to configure the number of log files on SD card, default is 4 (4x16kb) but max limit is now 100 files, loading that many would be really slow.
* Added option to define how often to log BLE data to SD card in order to avoid filling the logs, default is 5 minutes. 
* Added throtteling of 30s on ble processing to avoid crashing
* Fixed unit conversion on main page (gravity + temperature)

### v0.8.0

Note! The build for ESP32pro will crash itermitent with BLE enabled this is due to a bug in Arduino 2 and this board can't support Arduino 3.

* Added support for external SD cards where all received data is stored 4 x 16 kb can be logged
* New measurement view in the UI to display data on the SD card and also graphs if a single device is selected
* Added support for receiving RAPT data (will only be displayed and not forwarded)
* Added feature api to easier adapt ui for the included build features
* Renamed firmware files to easier find the right one, and the current name is now shown in the firmware upload view.
* Optimized UI for size and updated library dependecies
* Updated to Arduino 3.x for stable ble scanning
* Added option to backup & restore of configuration
* BLE logging is restricted to once every 5 minutes or the logs will be filled up.

### v0.7.1

* Fixed crash when using large format templates (stack overflow)

### v0.7.0

Note! I have enabled the touch controller so on lolin TFT the touch screen will need to be calibrated once.,

* Enabling dark mode on TFT display
* Enable touch controller on TFT displays and options o switch between detected devices. Swipe left/right also works on Waveshare but on lolin devices the touch controller is to unresponsive.
* Added device name to web based UI
* Added push test option to UI so that format templates can be tested
* Created board definition for Waveshare
* Fixed printout of device id’s with leading zero
* Added defined pressure templates for one and two sensors.
* Fixed bug that blocked upload of firmware files over 1.8Mb
* Major update to also handle the new pressuremon device.
* Fixed wifi scanning that was broken in previous beta
* Sharing most of the codebase with gravitymon to simplify maintenance
* Now using new partition schema (32Mb) so full flashing is needed.
* Refactored push logic
* Refactored http post logic to do processing in background (speed up receive)
* Added support for chamber controller bluetooth
* Added Johannesburg timezone

### v0.6.0

* Updated all dependent libraries to latest versions Json 7, AsyncWebServer, BLE 2.x, Arduino 2.x.

### v0.5.0

* Support Gravitymon 2.0

### v0.4.0 and earlier

* Test versions
