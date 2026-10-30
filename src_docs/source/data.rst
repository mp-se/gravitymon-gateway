.. _data:

Measurement data
################

The gateway stores the latest measurement for each detected device and can
optionally write received data to SD storage on supported boards. Fields depend
on the device type and the BLE or Wi-Fi protocol used.

Typical values include device identifier and name, source, temperature,
gravity or pressure, battery information, timestamp, and BLE signal strength.
Fields that a source does not provide remain unavailable; they are not meant
to be interpreted as zero readings.

Measurements can be forwarded to configured targets or downloaded from local
storage when logging is enabled. The web status API presents current device
values, while the authenticated ``/api/mdns`` endpoint returns discovered
network device information.

