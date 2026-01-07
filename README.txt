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
- TaskScheduler (cooperative multitasking)
- Future: OpenWeatherMap API, MQTT heartbeat, behavior-based scheduling

## Current Status
- **v0.1.0 (OSB 1)**: Display init, LVGL basic UI, capacitive touch reporting ✓

## Development Process
Following proven flow: SRD → SDP → SDD → STC → One Small Builds (OSBs)
