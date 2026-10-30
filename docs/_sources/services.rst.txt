.. _services:

Push targets and service integration
####################################

Configure destinations under **Push targets**. The gateway provides two HTTP
POST targets, one HTTP GET target, InfluxDB v2, and MQTT. Each target has
separate settings, enable switches for gravity and pressure data, and an
independent skip interval. The gateway can send to more than one target.
Scheduled forwarding currently applies to GravityMon and PressureMon
measurements. Other supported BLE types can appear in the gateway UI and SD
logs, but are not sent to these push targets.

Before configuring a target, collect its endpoint or broker details and any
credentials required by the service. Do not include real credentials in
screenshots or public support requests.

Shared target controls
======================

HTTP targets have a URL and up to two optional HTTP header lines. Each target
also has separate gravity and pressure format templates and enable switches.
The **Predefined URLs**, **Predefined headers**, and **Predefined formats**
controls can insert starting values. Review inserted values for account
specific details before saving.

**Preview format** renders the current template with representative data.
**Run push gravity test** and **Run push pressure test** send test requests
using the matching template. A successful test shows that the gateway reached
the endpoint and received a response; check the receiving service to confirm
that it interpreted the fields as intended.

**Skip interval** controls how many eligible measurement send cycles to skip:
0 sends on every eligible cycle, 1 sends every second cycle, 2 sends every
third cycle, and so on up to 5. The available range is 0–5. Use a nonzero
value for service rate limits or to reduce network traffic. This setting is
separate for each target.

Gravity and pressure are configured independently. Where both measurement
types are enabled in the firmware, each has its own **Enable** switch, template,
predefined format selector, preview, and test button. A switch controls
scheduled pushes for that type; use its test button to check the template and
connection while setting up. The test uses sample data and does not prove that
the receiving service stores or displays the result correctly.

Some fields can be disabled while **Wi-Fi Direct** is enabled. Save a target
before running its test, then check both the response shown by the gateway and
the service or broker receiving the data.

HTTP POST #1
============

Open **Push targets → HTTP Post**. This is the first independent HTTP POST
destination.

* **HTTP URL** — complete endpoint URL, including ``http://`` or ``https://``.
  The **URL** control opens predefined endpoint choices.
* **HTTP Header #1 / #2** — optional raw header lines. The **Header** control
  inserts predefined values. Enter each header as ``Name: value``. Leave a
  line empty if the endpoint does not require it. For example, a JSON endpoint
  may require ``Content-Type: application/json``.
* **Skip interval** — target-specific send frequency, from 0 to 5.
* **Data format (gravity / pressure)** — separate payload templates. Use the
  matching **Enable** switch to include that measurement type in scheduled
  sends. **Predefined formats** inserts a starting template; **Preview format**
  shows the substituted sample.
* **Run push gravity test / Run push pressure test** — sends a sample using
  that target's corresponding template. Configure and save the endpoint and
  credentials first.

An advanced firmware build may expose **Use tcp**. In that mode the endpoint
field changes to **Server** in ``server:port`` form and the HTTP URL, headers,
and skip interval controls are hidden. This option is not shown in the usual
build.

HTTP POST #2
============

Open **Push targets → HTTP Post 2**. It has the same controls as HTTP Post #1,
but stores its own URL, headers, skip interval, templates, and enable switches.
Use it for a second service or for a second payload arrangement. Updating one
POST target does not configure the other. Its URL accepts ``http://`` or
``https://``. Each header uses ``Name: value`` syntax and may be left blank.
Choose a predefined format, inspect its preview, enable gravity and/or pressure,
then save and run the matching test.

HTTP GET
========

Open **Push targets → HTTP Get**. This target has a URL, two optional headers,
skip interval, separate gravity and pressure templates and enable switches,
predefined formats, previews, and test buttons. The template is appended to
the URL as query data. Include the query separator and parameter names in the
template as required by the endpoint; a typical query string starts with ``?``
and separates parameters with ``&``. Parameters can include a device name, ID,
temperature, gravity or pressure, and battery. Match parameter names and
encoding to what the receiving service expects. Use the optional headers when
the service requires them, then check the preview and run a test.

InfluxDB v2
===========

Open **Push targets → Influxdb v2**. This target writes line protocol to an
InfluxDB 2.x server.

* **Server** — base URL of the InfluxDB service, including scheme and port
  when required.
* **Organisation** — organization identifier or name used by the server.
* **Bucket** — destination bucket.
* **Authentication token** — token with write access to the bucket.
* **Skip interval** — target send frequency, from 0 to 5.
* **Data format (gravity / pressure)** — separate line protocol templates
  with enable switches, predefined formats, previews, and push tests.

A line protocol line contains a measurement name, optional comma-separated
tags, a space, and comma-separated fields. Keep tag and field types consistent
with the InfluxDB schema. After a successful test, verify the point in the
configured bucket.

MQTT
====

Open **Push targets → MQTT**.

* **Server** — broker host name or address; enter the host without a URL
  scheme.
* **Port** — broker listener (0–65535). The UI describes ``1883`` as standard
  MQTT, ``8883`` as standard SSL, and ports ``8000`` and above as SSL. Leave
  ``0`` for the default. Check which listener your broker actually exposes.
* **User name / Password** — optional broker credentials; leave blank if the
  broker does not require authentication. Each field accepts up to 20
  characters.
* **Skip interval** — target send frequency, from 0 to 5.
* **Data format (gravity / pressure)** — separate topic/payload templates,
  enable switches, predefined formats, previews, and push tests.

An MQTT template contains topic and payload pairs separated by ``|``. Each
pair uses ``topic:payload``. This lets one measurement publish multiple values
to separate topics, for example gravity and temperature. Check the preview to
confirm topic spelling and payload format expected by subscribers. Configure
retain behavior at the broker or subscriber as needed; there is no retain
switch on this page.

Template syntax
===============

Use **Predefined formats** to start from a known service format, then adapt it
to the receiver's requirements. For JSON, ensure the preview remains valid
JSON and uses the expected string and numeric types. InfluxDB templates must
be valid line protocol; MQTT formats must contain the expected topic and
payload pairs. See :doc:`format-template` for variables and examples.

Common integrations
===================

* **Brewfather** — use its iSpindel compatible HTTP POST endpoint or custom
  stream endpoint with a matching JSON format. See the `Brewfather integration
  documentation <https://docs.brewfather.app/integrations>`_.
* **Home Assistant** — MQTT can publish individual topics or use a discovery
  template to create entities. Verify topics and payloads against your broker
  setup.
* **InfluxDB** — use the InfluxDB v2 target and a line protocol template.
* **Other HTTP or MQTT services** — use a custom template to map gateway
  fields into the payload and topics required by the service.

Service APIs change over time. Refer to each provider's current documentation
for endpoint paths, token permissions, TLS requirements, and field names.

