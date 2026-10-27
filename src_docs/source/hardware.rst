.. _hardware:

Hardware
########

Supported gateway boards
------------------------

The repository contains builds for these display equipped gateways:

* **Lolin ESP32 D32 Pro** with 2.4-inch TFT.
* **Lolin ESP32-S3 Pro** with 2.4-inch TFT and SD card support.
* **Waveshare ESP32-S3** with 2.8-inch TFT and SD card support.
* **Waveshare ESP32-S3** with 4.3-inch RGB TFT and SD card support.

The 4.3-inch Waveshare board uses an RGB display interface and an I/O expander
for board signals. Its display and SD setup differs from the SPI TFT boards.
Use only firmware built for the exact board variant.

The PlatformIO environments identify the firmware variants. In particular,
``gateway32pro-tft`` enables the Lolin ESP32 D32 Pro display but does not
enable SD logging; ``gateway32s3pro-tft-sd`` enables both the Lolin S3 display
and SPI SD support; ``gateway32s3wave-tft28-sd`` uses an SD_MMC connection; and
``gateway32s3wave-tft43-sd`` uses SPI SD on the Waveshare 4.3-inch board. A
board's physical SD socket does not by itself mean every firmware build enables
it.

Placement
---------

Place the gateway where it has reliable Wi-Fi coverage and can receive BLE
advertisements from the brewing devices. Metal enclosures, closed cabinets,
and distance can reduce radio range. For stable operation, use a suitable USB
power supply for the selected board and attached display.

Storage
-------

The SD-enabled builds write measurement history to CSV files on the card. The
web interface exposes the data under **Measurements → View** as graphs and
tables, and **Other → Tools → Secure Digital (SD)** lists card files. The
firmware uses an active ``/data.csv`` file and rotates it to numbered files
when it grows beyond 16 KB. The **Max number of log files on SD card** setting
controls the number of numbered rotated files retained in addition to the
active file. For example, a setting of 4 can retain ``/data.csv`` and
``/data1.csv`` through ``/data4.csv``. The SD log interval is configured
separately under **Measurements → Settings**; it limits how often data from
each BLE device is written. GravityMon and PressureMon Wi-Fi reports are
logged as they arrive and are not throttled by this BLE interval.

The card connection differs by board. The Lolin S3 and Waveshare 4.3-inch
variants use SPI SD, while the Waveshare 2.8-inch variant uses the ESP32
SD_MMC interface in one-bit mode. The Lolin S3 TFT and SD support are designed
to work together in the current firmware. The 2.8-inch SD_MMC connection and
the 4.3-inch I/O-expander chip-select arrangement are board-specific; do not
use those pins or assumptions for another board or an external module.

For first checks, insert the card before powering the gateway, open **Other →
Tools**, and confirm that SD is mounted and files are listed. Then check
**Measurements → View**. If the view reports no card, inspect the firmware's
serial log for SD initialization or mount retry messages. The firmware retries
mounting periodically on the current SD-enabled builds. Logging is unavailable
while the card is not mounted; normal BLE receiving and push behavior can still
be used without SD logging.

Some supported gateways also have an integrated TFT:

* The Lolin ESP32 D32 Pro, Lolin ESP32-S3 Pro, and Waveshare 2.8-inch models
  use 240 × 320 SPI displays.
* The Waveshare 4.3-inch model uses an 800 × 480 RGB display with separate
  display and touch hardware and PSRAM-backed display buffers.
* The display cycles through received measurements about every four seconds.
  The **Overview** and **Device focused** layouts are selected under
  **Device → Hardware**. Touch and swipe controls vary with the panel and
  firmware variant; the browser UI remains the place to change settings and
  inspect full measurement history.

Board definitions and pin assignments are maintained in the repository's
``boards`` directory and ``platformio.ini``. These files are the reference for
hardware revisions and custom builds.

