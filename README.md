# WIzee Weather Tile

A standalone embedded weather display for the Elecrow WIzee-ESP32 5" HMI (WZ8048C050).

Duplicates the clean, iconic weather tile from our PyQt dashboard project—now on a beautiful 5" capacitive touchscreen powered by ESP32-S3.

## Hardware
- Elecrow WIzee-ESP32 5" (800x480 RGB TFT + GT911 capacitive touch)
- ESP32-S3-WROOM-1 module
- USB-powered (always-on)

## Software Stack
- Arduino Framework (ESP32-S3)
- LovyanGFX ^1.1.12
- LVGL v8.3.6
- TaskScheduler (cooperative multitasking, future)
- OpenWeatherMap API, MQTT heartbeat, behavior-based scheduling (in progress)

## Current Status
- **v0.6.0 (OSB 6)**: Full manual refresh feedback (overlay + spinner + backlight pulse + status) ✓
- Huge custom Montserrat font for temperature ✓
- Tap-to-refresh with non-blocking animation ✓

## Regression Baselines
These folders contain **immutable golden unit test baselines** — minimal known-good builds for display, touch, and core stack.
Used for regression testing, library upgrades, or new hardware validation.
**Do not modify without consensus.**

- `baeb_wx_ut_1/` : Original baseline (OSB 1 state, proven stable)
- `baeb_wx_ut_2/` : Secondary baseline (copied from _ut_1, available for controlled experiments)

## Development Process
Following proven flow: SRD → SDP → SDD → STC → One Small Builds (OSBs)

Main development line is in top-level `src/` (starting OSB 7+ for real weather fetch).