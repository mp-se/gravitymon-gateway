.. _functionality:

Functionality
#############

GravityMon Gateway collects measurements from supported brewing sensors and
forwards them to configured destinations. It can receive data using BLE and
Wi-Fi, and provides a browser based interface for configuration and monitoring.

Supported measurement sources
-----------------------------

The gateway firmware can process data from:

* GravityMon devices, including their supported BLE advertising formats and
  Wi-Fi gateway reporting.
* PressureMon devices, including pressure and temperature measurements over
  supported BLE formats and Wi-Fi reports sent to the gateway.
* Tilt and RAPT compatible BLE devices.
* Chamber Controller BLE devices.

Available fields depend on the source device and its report format. Missing
fields may be unavailable or represented by a source-specific fallback; a
displayed or forwarded zero is not always a measured zero. BLE signal strength
and transmit power are also recorded where provided.

Data forwarding
---------------

GravityMon and PressureMon measurements can be sent to configured destinations
using the gateway's push configuration. Supported integrations include HTTP
endpoints, MQTT, InfluxDB, and the services and templates described in
:doc:`services`. Multiple push targets can be configured. Tilt, RAPT, and
Chamber Controller BLE measurements can be received, displayed, and logged,
but are not currently forwarded through these push integrations.

The gateway also supports receiving Wi-Fi reports from GravityMon and
PressureMon devices and can forward those measurements using the same
configured gateway destinations.

Local monitoring
----------------

The built-in web interface shows connected devices and their latest readings,
provides configuration and maintenance pages, and supports viewing logged data
when SD storage is enabled. Supported TFT boards can show measurements on a
local display. Display layouts and capabilities vary by board.

mDNS discovery can associate BLE device identifiers with names discovered on
the local network. The gateway exposes the discovered information through its
authenticated ``/api/mdns`` endpoint.

