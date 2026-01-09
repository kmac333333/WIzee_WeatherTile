# Software Design Proposal (SDP) - WIzee Weather Tile  
**Version**: 1.1  
**Date**: January 08, 2026  
**Status**: Locked  

## High-Level Architecture  
Arduino/C++ on ESP32-S3 with LovyanGFX + LVGL v8.3.6.  

Committed decisions:
- Framework: Arduino/C++  
- Display Driver: LovyanGFX ^1.1.12  
- GUI: LVGL v8.3.6 (v9 migration post-MVP)  
- Task Management: TaskScheduler  
- Status Indicator: On-screen (no RGB LED)  

## Component Breakdown  
- WeatherFetcher (HTTPClient + ArduinoJson)  
- DataModel (structs for current weather)  
- Scheduler (TaskScheduler for updates + MQTT heartbeat)  
- UI Tile (LVGL widgets + custom large fonts)  
- Touch Handler (tap-to-refresh with overlay/spinner)  

## OSB Roadmap (Current Status)  
1–5: Complete (display, fonts, overlay/spinner)  
6: Complete — Full manual refresh feedback  
7: Next — Real OWM fetch + data binding  
8: Scheduling (periodic + behavior-based)  
9: MQTT heartbeat  
10+: Polish, settings, enclosure

## Risks & Mitigations  
- High-res LVGL quirks → Isolated via OSBs and baselines.
- Font memory → Custom generated Montserrat (subsets).