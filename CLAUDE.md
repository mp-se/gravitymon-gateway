# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
pio run -e gateway32pro-tft            # ESP32 + ILI9341 TFT (Lolin D32 Pro) — builds UI automatically
pio run -e gateway32s3pro-tft-sd       # ESP32-S3 + TFT + SPI SD (Lolin S3 Pro) — builds UI automatically
pio run -e gateway32s3wave-tft-sd      # ESP32-S3 + TFT + MMC SD (Waveshare S3 TFT) — builds UI automatically
pio run -e gateway32-unit              # Unit tests only (ESP32-C3)
pio run -t upload -e <env>             # Flash firmware
pio device monitor -e <env>            # Serial monitor at 115200
```

### UI Build Integration
The UI is built manually before firmware builds:
```bash
cd ui && npm run build               # Build the UI with Vite
pio run -e gateway32pro-tft          # Build firmware (pre-build step copies assets)
cd ui && npm run dev                 # Development with hot reload
```

UI source lives in `ui/` (Vue 3 + Vite). Build outputs (`dist/`, `node_modules/`) are excluded from git. See `UI_MERGE.md` for full integration details.

## Architecture

GravityMon Gateway is an ESP32 firmware that receives BLE broadcasts from gravity/pressure sensors (Tilt, GravityMon, Rapt, Pressuremon) and forwards data via HTTP/MQTT/InfluxDB. It has an optional TFT touchscreen UI built with LVGL.

### Feature Flags (compile-time)
Features are enabled via build flags in `platformio.ini`:
- `ENABLE_BLE` — NimBLE scanner
- `ENABLE_TFT` — TFT display + LVGL UI
- `ENABLE_SD` — SPI SD card
- `ENABLE_MMC` — MMC SD card (Waveshare boards)
- `ENABLE_LVGL` — LVGL UI framework (requires ENABLE_TFT)

### Board System
`script/board.py` runs at build time and auto-defines `<BOARD_ID>=1` and `BOARD="<BOARD_ID>"` from the PlatformIO board name. Custom board definitions live in `boards/<name>/` (JSON + `pins_arduino.h`). To add a new board: create the JSON + pins header, add a PlatformIO environment, and add the board name to `board.py` detection if any special handling is needed.

### Display Stack
Two display backends exist, selected at compile time:
- **TFT_eSPI** — for SPI displays (ILI9341, ST7789). LVGL is integrated via `lv_tft_espi_create()`. Configured entirely through `-D` build flags (driver, pins, SPI frequency).
- **LovyanGFX** — for RGB parallel displays (800×480). Uses `lv_display_create()` with a custom flush callback. Selected when `WAVESHARE_S3_TOUCH_43` is defined.

Touch controllers: **XPT2046** (SPI resistive, via TFT_eSPI), **CST328** (I2C capacitive, `lib/cst328/`), **GT911** (I2C capacitive, `lib/gt911/`). The `TOUCH_CS=-1` flag signals that a non-SPI touch driver is in use.

`src/display.cpp` owns all display/touch initialisation and feeds LVGL. The LVGL tick loop runs on FreeRTOS Core 0. UI screens are in `src/ui_gravitymon_gateway.cpp`.

### Data Flow
```
BLE scan (ble_gateway) → measurement structs → push_gateway (HTTP/MQTT/InfluxDB)
                                             → display (LVGL UI on TFT)
                                             → sdcard (CSV logging)
```

### Web Server & API
`src/web_gateway.cpp` (ESPAsyncWebServer) serves:
- Embedded SPA from `html/` (gzipped Vue.js UI built from `ui/` source)
- REST API for configuration, data query, and control
- Configuration stored as JSON in LittleFS at `/gravitymon-gw.json`

The web UI (`ui/`) is a Vue 3 + Vite project that builds to gzipped assets and is embedded in the firmware at build time.

### Key External Libraries
- `espframework` / `gravitymon` — shared sensor + push logic (external repos)
- `NimBLE-Arduino` — BLE scanning
- `TFT_eSPI v2.5.43` + `LVGL v9.4.0` — display/UI
- `ESPAsyncWebServer` + `AsyncTCP` — web server

## Code Style

`.clang-format` enforces LLVM style, 2-space indent, 100-char line limit, no include-order sorting. Run `clang-format` before committing. Pre-commit hooks are configured in `.pre-commit-config.yaml`.

## Unit Tests

Tests live in `test/` and use the AUnit framework. Run with `pio run -e gateway32-unit`. Tests cover measurement data parsing (`tests_measurement.cpp`).

## Important Constraints

- Do not commit code — the user handles all git operations.
- The `bin/` directory contains pre-built firmware binaries committed by CI (`.github/workflows/pio-build.yaml`). Do not manually modify it.
- `ENABLE_SD` (SPI) and `ENABLE_MMC` can both coexist with `ENABLE_TFT`.
