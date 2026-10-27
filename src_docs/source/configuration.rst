.. _configuration:

Configuration
#############

Open the gateway's mDNS address (``<name>.local``) or IP address in a browser.
The top navigation groups settings under **Device**, **Measurements**,
**Push targets**, and **Other**. A badge beside a menu item indicates a setup
item that may need attention. The **Save** button becomes active when the
current page contains unsaved changes. Save first, then use **Restart device**
where the page indicates a restart is needed.

Home page
=========

The home page summarizes the latest measurement received from each device.
Each card identifies the measurement class and source, shows the device name
when known, and reports the age of the latest update and push. Gravity cards
show gravity and temperature; pressure cards show pressure and temperature;
chamber cards show chamber and beer temperatures. Device status cards include
gateway Wi-Fi signal, IP address, memory, firmware, platform, device ID, and
uptime.

The update age helps distinguish a device that is currently reporting from
one whose last reading is stale. A missing measurement field means the source
did not provide a usable value; it should not be read as a measured zero.

Device settings
===============

Open **Device → Settings**.

* **mDNS** — the gateway's network host name. The ``.local`` suffix is added
  automatically, so entering ``gateway`` gives the address
  ``gateway.local``. Choose a name that is unique on the local network.
* **Temperature Format** — displays temperatures in Celsius or Fahrenheit.
* **Gravity Format** — displays gravity as Specific Gravity or Plato.
* **Pressure Units** — displays pressure in PSI, kPa, or Bar.
* **User Interface** — selects day or dark appearance for the browser UI.
* **Save** — stores changed settings.
* **Restart device** — restarts the gateway.
* **Restore factory defaults** — erases the configured settings and restores
  defaults. Export a backup first if you may need the current configuration.

The unit selections control presentation and template conversions. They do
not change the sensor's physical measurement.

Device hardware and BLE
=======================

Open **Device → Hardware**.

* **Enable Bluetooth** — turns BLE scanning on or off. Changing this setting
  requires a device restart.
* **Scan Mode** — choose **Passive** or **Active**. Passive scanning is used
  for GravityMon iBeacon and PressureMon iBeacon advertisements. Active
  scanning is needed for GravityMon Eddystone advertisements and requires a
  BLE 5.0 capable target such as the ESP32-S3.
* **Scanning Time** — the duration of a BLE scan cycle, in seconds. Longer
  scans can discover devices that advertise less often, while using more time
  for scanning. The scan duration field is in seconds.
* **Timezone** — sets the local timezone used when presenting or recording
  times. Select the zone that matches the gateway's location. The choices
  include common North American, European, African, Asian, Australian, and
  Pacific timezones.
* **Display Layout** — selects **Overview** or **Device focused** on supported
  TFT builds. The available display and touch behavior depends on the board.
* **Save** and **Restart device** — save settings and restart when required.

The UI currently documents GravityMon iBeacon, GravityMon Eddystone, and
PressureMon iBeacon formats. The project can also parse supported Tilt, RAPT,
and Chamber Controller advertisements; the exact advertisement type and BLE
capabilities determine what can be received. The UI recommends TiltBridge for
Tilt installations.

TFT display
===========

On supported TFT builds, the screen cycles through detected measurements
automatically, showing one device at a time. The display provides a compact
overview of recent devices or a device-focused view with more detail; the
available layout pair is selected to fit the board's screen size. On boards
whose touch driver supports it, swipe gestures move between displayed
measurements. Screen dimensions, touch hardware, and gesture support depend on
the exact board variant. The TFT is a status display; use the web interface for
full configuration and historical graphs.

Wi-Fi networks
==============

Open **Device → Wi-Fi** to select the primary and optional secondary network.
The page scans nearby networks and labels them with a signal quality estimate
and a lock indicator for secured networks. At least one network must be
configured. Enter the matching password for each selected SSID.

* **SSID #1 / Password #1** — primary network.
* **SSID #2 / Password #2** — optional alternate network. This gives the
  gateway another configured network to try if the primary is unavailable.
* **Portal timeout** — how long the Wi-Fi setup portal remains idle before
  timing out (10–240 seconds).
* **Connection timeout** — how long a connection attempt can wait (1–60
  seconds).
* **Scan for strongest AP**, when shown — scans for the strongest access point
  matching a configured network. Scanning can increase connection time.

If the Wi-Fi scan cannot list a hidden network, use the manual entry page
linked from the Wi-Fi page. After saving changed Wi-Fi credentials, restart
the gateway. Its IP address may change; use the router's client list or the
new mDNS name to find it again.

Wi-Fi Direct
============

Open **Device → Wi-Fi AP** to configure the gateway's Wi-Fi Direct network.
This lets a compatible source device connect directly to the gateway instead
of using the normal access point. It is useful when the source should report to
the gateway directly or when selecting the closest access point matters.

Set **Direct SSID** and **Direct Password** to the network credentials that
source devices will use. **Generate** creates a gateway-specific SSID and a
random password. Save changes and restart the gateway, then configure the
source device to use the matching network and gateway destination.

Measurement logging
===================

Open **Measurements → Settings**. These options apply when the selected board
supports SD storage:

* **Max number of log files on SD card** — maximum number of log files to
  retain, from 2 to 100. The active log is ``/data.csv``; rotated files are
  ``/data1.csv``, ``/data2.csv``, and so on. The firmware rotates the files as
  the active log grows.
* **Size of each log file on SD card** — currently fixed at 16 KB and shown
  for information. Rotation occurs after ``data.csv`` grows beyond that size.
* **Minimum time between logs to SD card (BLE)** — minimum interval between
  logging records from the same BLE device. Available intervals are 1, 3, 5,
  10, 15, 30, 45, and 60 minutes. Increase this interval to reduce card writes
  and preserve space. It only throttles BLE records; GravityMon and PressureMon
  Wi-Fi reports are logged as received.

The file count is the number of rotated files in addition to the active file.
For example, a limit of 4 can retain ``/data.csv`` and ``/data1.csv`` through
``/data4.csv``.

Use **Measurements → View** to browse stored data and graphs. The page is only
useful when SD support is available and a card is mounted. Select a device
type and device to see its recorded values as a graph and table. **Other →
Tools** also provides a Secure Digital file view when the card is mounted; use
it to confirm that the CSV logs exist. A card that is absent or failed to mount
leaves these SD features unavailable. Some builds retry mounting an SD card
periodically, so after inserting a card allow time for it to be detected and
check the gateway status again.

Push settings
=============

Open **Push targets → Settings**.

* **Token** — a shared value available to templates as ``${token}``. Some
  receiving services use it as an authentication or device token.
* **Push timeout** — maximum time to wait for a remote service to accept a
  connection (10–60 seconds).
* **Push minimum resend time** — minimum delay before a value can be sent again
  to a target (10–1800 seconds). This can prevent repeatedly forwarding the
  same reading.

Each push target has its own enable switch and **Skip interval**. A value of 0
means do not skip cycles; 1 means send every second cycle, 2 every third, and
so on. Use target-specific intervals when a service has rate limits or when
you want to reduce traffic.

Backups and maintenance
=======================

Under **Other**, **Backup & Restore** exports or restores settings, **Firmware
update** installs an update, **Support** shows diagnostic information including
mDNS discoveries, and **Tools** provides maintenance actions. Export a backup
before resetting settings, changing networks, or updating firmware. See
:doc:`installation` for the difference between a routine firmware update and
a full web-flasher install.

