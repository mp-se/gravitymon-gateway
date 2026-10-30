.. _installation:

Installation
############

Web flasher
-----------

The recommended installation method is the `GravityMon Web Flasher
<https://www.gravitymon.com/flasher/index.html>`_. Select the Gateway firmware
and the exact board family. The classic ESP32 Lolin and ESP32-S3 Lolin builds
are different and must not be interchanged. Waveshare display builds are also
board and display size specific.

The flasher performs a full flash operation. Back up the device configuration
before reinstalling if you need to keep existing settings.

Firmware images
---------------

Release images are published in the project's ``bin`` directory. Select the
image matching the hardware:

* ``firmware32pro-tft.bin`` — Lolin ESP32 D32 Pro with 2.4-inch TFT.
* ``firmware32s3pro-tft-sd.bin`` — Lolin ESP32-S3 Pro with 2.4-inch TFT and SD.
* ``firmware32s3wave-tft28-sd.bin`` — Waveshare ESP32-S3 with 2.8-inch TFT.
* ``firmware32s3wave-tft43-sd.bin`` — Waveshare ESP32-S3 with 4.3-inch TFT.

Partition table binaries are provided for the corresponding S3 Waveshare
builds. When using a low-level flashing tool, use the complete set of files and
addresses supplied for the selected board by the release or flasher.

First startup and Wi-Fi
-----------------------

On first startup, configure Wi-Fi using the flasher's Wi-Fi setup option when
available. Otherwise, connect to the gateway setup access point (SSID
``Gateway``, password ``password``) and open the setup page. The gateway also
starts this setup mode when it has no usable Wi-Fi configuration or detects a
double reset. Once connected to the local network, open the gateway by its
mDNS name or IP address. The default device name is ``gravitymon-gw``; the
router's client list can be used to find its address if ``.local`` name
resolution is unavailable.

The gateway requires a 2.4 GHz Wi-Fi network. Keep the gateway within range of
both the access point and the BLE sensors it monitors.

Updating firmware
-----------------

Use the web interface's firmware update feature for routine updates. Use the
web flasher when changing board targets or when a full reflash is needed.
Export a configuration backup before updating if you want a restorable copy of
your settings.

Serial output
-------------

For diagnostic output, connect a serial monitor at **115200 baud, 8-N-1** and
reset the gateway. Capture output from the beginning of startup when reporting
issues.

