.. _format-template:

Format templates
################

HTTP and MQTT targets use templates to turn a measurement into a service
payload. InfluxDB uses a line protocol template. Variables use the
``${variable-name}`` form and are replaced before the payload is sent. Each
push page has separate templates and enable controls for gravity and pressure.

Common variables
================

.. list-table:: Template variables
   :header-rows: 1

   * - Variable
     - Meaning
   * - ``${id}``
     - Source device identifier.
   * - ``${mdns}``
     - Gateway or configured device name used in the payload.
   * - ``${token}``, ``${token2}``
     - Configured push token values.
   * - ``${temp}``
     - Temperature in the selected temperature format.
   * - ``${temp-c}``, ``${temp-f}``
     - Temperature converted to Celsius or Fahrenheit.
   * - ``${temp-unit}``
     - Temperature unit label.
   * - ``${gravity}``
     - Gravity in the selected format.
   * - ``${gravity-sg}``, ``${gravity-plato}``
     - Gravity converted to Specific Gravity or Plato.
   * - ``${gravity-unit}``
     - Gravity unit label.
   * - ``${corr-gravity}``, ``${corr-gravity-sg}``, ``${corr-gravity-plato}``
     - Corrected gravity in the selected unit or the named unit.
   * - ``${angle}``, ``${tilt}``
     - Tilt angle in degrees.
   * - ``${velocity}``
     - Gravity velocity; currently set to ``0`` for Gateway GravityMon pushes.
   * - ``${pressure}``, ``${pressure-psi}``, ``${pressure-bar}``, ``${pressure-kpa}``
     - Pressure in the selected or named pressure unit.
   * - ``${pressure1}``, ``${pressure1-psi}``, ``${pressure1-bar}``, ``${pressure1-kpa}``
     - Second pressure channel in the selected or named pressure unit, when available.
   * - ``${pressure-unit}``
     - Pressure unit label.
   * - ``${battery}``, ``${battery-percent}``
     - Battery value or percentage where available.
   * - ``${rssi}``
     - Received signal strength in dBm where available.
   * - ``${sleep-interval}``
     - Source reporting interval where available.
   * - ``${run-time}``
     - Measurement runtime; currently set to ``0`` for Gateway GravityMon and
       PressureMon pushes.
   * - ``${app-ver}``, ``${app-build}``
     - Firmware version and build identifiers.

Use only variables that apply to the measurement source and target. The
Gateway's GravityMon and PressureMon push templates set velocity and run-time
to zero. Other unsupported or unavailable fields may be empty or have a
source-specific fallback.

Editing and testing
===================

Use **Predefined formats** to start from a known integration format. Edit the
template to match the receiving service's schema, then use **Preview format**
to inspect the substituted result. For JSON, confirm that the preview parses
as JSON and that numbers, strings, and escaping have the expected types. For
MQTT, check every ``topic:payload`` pair. For InfluxDB, validate line protocol
and field types.

Finally, run a push test for each enabled measurement type. A test against the
mock simulator verifies the generated request path, but a production service
may impose additional rules or rate limits.

