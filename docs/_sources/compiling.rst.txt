.. _compiling:

Building from source
####################

The firmware is built with PlatformIO. Install PlatformIO Core or the
PlatformIO IDE, clone the repository, and build the environment that matches
the target board.

For example, build the Lolin ESP32 Pro TFT target with:

.. code-block:: console

   pio run -e gateway32pro-tft

Other environment names are listed in ``platformio.ini``. The S3 targets use
board specific definitions and dependencies; select the matching environment
for the display and storage hardware.

The build scripts copy the generated firmware and partition table images into
``bin``. The web UI source is in ``ui`` and is bundled into firmware by the
PlatformIO pre-build script. Build the UI dependencies and assets according to
``ui/README.md`` when changing the frontend.

