# Software Requirements Document (SRD) - ESP32 Weather Tile Display  
**Project Name**: WIzee Weather Tile  
**Version**: 1.3  
**Date**: January 08, 2026  
**Authors**: kmac3 & Grok (Collaborative Draft)  
**Status**: Locked  

## 1. Project Overview  
### 1.1 Purpose  
Create a standalone embedded weather display device using the Elecrow WIzee-ESP32 5" HMI board (WZ8048C050). Duplicate the weather tile functionality from our PyQt dashboard project: fetch real-time weather data via OpenWeatherMap API, parse it, and render it in a clean, visually appealing tile using LVGL.  

### 1.2 Scope  
- Single full-screen weather tile (large icon, huge temperature, condition, high/low, humidity, wind).  
- Hardware: Elecrow WIzee-ESP32 5" (800x480 RGB TFT, capacitive touch, ESP32-S3).  
- Power: Always-on via USB.  
- Out of Scope (Initial): Multi-tile, advanced animations, non-weather features.  

### 1.3 Success Criteria  
Functional prototype that fetches and displays real weather data reliably, with manual tap-to-refresh and visual feedback.

## 2. Functional Requirements  
- **FR-1: Network Connectivity** — WiFi + HTTPS to OpenWeatherMap (reuse existing API key).  
- **FR-2: Data Parsing & Display** — Parse JSON, render with LVGL (custom large fonts for temp).  
- **FR-3: Scheduling & Updates** — Periodic (~4x/day default), behavior-based (morning/evening), manual tap refresh. Non-blocking.  
- **FR-4: Hardware Integration** — TFT via LovyanGFX, capacitive touch (tap-to-refresh priority).  
- **FR-5: MQTT Infrastructure Support** — Periodic "I'm alive" heartbeat with unique hex-ID.  

### 2.2 User Interactions  
- Primary: Tap anywhere for manual refresh (with spinner overlay feedback).  

### 2.3 Offline Handling  
Display last known data or error on failure.

## 3. Non-Functional Requirements  
- Performance: Responsive UI, <50% SRAM usage.  
- Reliability: Graceful WiFi/API errors.  
- Development: OSB process with baselines (`baeb_wx_ut_1/2`).

## 4. Architectural Considerations  
- Framework: Arduino/C++ (committed).  
- GUI: LVGL v8.3.6 (committed, v9 sub-project later).  
- Display Driver: LovyanGFX ^1.1.12.  
- Task Management: TaskScheduler (committed).  

## 5. Assumptions & Risks  
- Always-available WiFi/power.  
- Existing OWM API key valid.  
- Risks: LVGL high-res quirks (mitigated via OSBs).