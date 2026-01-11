# WIzee Weather Tile

A standalone embedded weather display for the Elecrow WIzee-ESP32 5" HMI (WZ8048C050).  
Duplicates the iconic weather tile from our PyQt6 dashboard project — now on a beautiful 5" capacitive touchscreen powered by ESP32-S3.

[![PlatformIO](https://img.shields.io/badge/PlatformIO-Embedded-blue?logo=platformio)](https://platformio.org)
[![LVGL](https://img.shields.io/badge/LVGL-v8.3.6-green)](https://lvgl.io)
[![WeatherAPI](https://img.shields.io/badge/WeatherAPI.com-Live-orange)](https://www.weatherapi.com)
[![TaskScheduler by Arkhipenko](https://img.shields.io/badge/TaskScheduler-v3.8.0-009688?logo=github&style=flat-square&logoColor=white)](https://github.com/arkhipenko/TaskScheduler)

## Hardware
- Elecrow WIzee-ESP32 5" (800x480 RGB TFT + GT911 capacitive touch)
- ESP32-S3-WROOM-1 module
- USB-powered (always-on)

## Software Stack
- Framework: Arduino/C++ on ESP32-S3
- GUI: LVGL v8.3.6 + LovyanGFX ^1.1.12
- Task Management: TaskScheduler ^3.8.0 (non-blocking)
- JSON Parsing: ArduinoJson ^6.21.4
- Weather API: WeatherAPI.com (current + forecast, reused from dashboard)
- Design Pattern: Single app-level Model, task-driven networking, serial CLI, modular files

## Current Status
- **v0.6.0 (OSB 6)**: Full manual refresh feedback (overlay + spinner + backlight pulse + status) ✓
- Huge custom Montserrat font for temperature ✓
- Tap-to-refresh with non-blocking animation ✓
## Current Status
- **v0.7.1 (OSB 7.1)**: Real-time weather fetch (WeatherAPI.com) + table-driven UI + decoupled display update task ✓  
- Huge custom Montserrat font for temperature ✓
- Tap-to-refresh with overlay/spinner/pulse/feedback ✓
- Stability: Non-blocking fetch, throttled UI task, WDT safety ✓
## Current Milestones
- **OSB 7 (v0.7.0)**: Real WeatherAPI.com fetch + table-driven UI + data binding ✓  
  See `docs/osb/OSB_07_Real_Weather_Fetch.md` for details
## Next Milestones (OSB 7.5 & 8+)
- **OSB 7.5**: Further refactor (async HTTP fetch, CLI expansion, full design guide compliance)
- **OSB 8**: Real icons (local map from condition.code), periodic/behavior scheduling, MQTT heartbeat
- **Future**: Async WiFi/HTTP libraries, SD icon cache, UI polish, enclosure OpenSCAD
## OSB 7.5 Intermediate: NTP sync, NVS storage, CLI directory functions
- Added NTP sync (myntp.cpp/h) — waits for WiFi, gets time, updates RTC
- Model init now loads from NVS (storage.cpp/h) — persistence across reboots
- CLI expanded with directory functions (ls, cd, pwd, etc.)
- Refactored to single Model pattern, task-driven WiFi, non-blocking loop
- Stability: Decoupled NTP/RTC from blocking calls
## Regression Baselines
Immutable golden unit test baselines — known-good states for regression testing, library upgrades, or new hardware validation.  
**Do not modify without consensus.**

- `baeb_wx_ut_1/` — Original hardware/display baseline (OSB 1)
- `baeb_wx_ut_2/` — Early stability baseline (pre-refactor)
- `baeb_wx_ut_3/` — Current refactored baseline (single Model, task-driven WiFi, CLI, modularity)

## Development Process
Following proven flow: SRD → SDP → SDD → STC → One Small Builds (OSBs)
Main development line is in top-level `src/` (starting OSB 7+ for real weather fetch).
Current focus: Async/non-blocking everything, file modularity, CLI for debug.

## Documentation
- Requirements & Design: See `docs/SRD_v1.3.md` and `docs/SDP_v1.1.md`
- Build Notes: Detailed per-OSB logs in `docs/osb/`
- Roadmap: `docs/Roadmap.md`

## Contributing / Notes
- Secrets (API key, WiFi) are encapsulated — never commit real values.
- Use baselines for regression testing.
- Questions? Check `docs/osb/` for build details.
