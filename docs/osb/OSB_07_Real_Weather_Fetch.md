# OSB 7: Real Weather Fetch + Data Binding (v0.7.0)

**Date**: January 09, 2026  
**Status**: Complete & Accepted  

## Objective
- Fetch and display real-time weather from WeatherAPI.com (current conditions)
- Implement table-driven UI creation for readability/maintainability
- Integrate TaskScheduler for non-blocking operations
- Ensure secrets encapsulation and stability

## Key Changes
- Switched to WeatherAPI.com forecast/current endpoint (reusing dashboard key)
- Added table-driven widget creation in `create_weather_tile()` (WidgetConfig struct + loop)
- TaskScheduler for async fetch (definitions in main.cpp, extern in manager)
- Separate display update task (decoupled from fetch)
- Secrets in `weather_credentials.h` (single inclusion in manager layer)
- Data binding: Fetch → parse → update buffers → invalidate screen
- Debug: Serial prints for raw JSON, HTTP code, parsed values

## Issues Encountered & Resolutions
- **Blocking fetch in setup()** → Initial UI delay → Fixed with non-blocking task trigger
- **Misalignments in table-driven layout** → Refined chaining with `lv_obj_align_to(prev, ...)` and tuned offsets
- **URL encoding missing** → Spaces in location caused HTTP 400 → Added manual encoder or switch to zip code
- **JSON parse error (InvalidInput)** → Large forecast payload → Switched to current.json + larger StaticJsonDocument<8192>
- **Periodic reboots (RTC_SW_CPU_RST)** → Task starvation + LoadProhibited → Throttled display task, added dirty flag, yield() in loop()
- **Double vs single dereferencing** → Corrected `text_ptr` from `**` to `*` in WidgetConfig

## Outcome
- Real weather data (temp, condition, humidity, wind) displays correctly on tap/fetch
- UI updates via buffers + invalidate (no widget recreation)
- Stable: No blocking, no reboots (with throttle + dirty flag)
- Table-driven UI is clean, extensible, and maintainable
- Async-ready foundation in place

## Next Steps (OSB 8+)
- Add real icons (local map from condition.code, no repeated fetches)
- Periodic + behavior-based scheduling (4x/day + morning/evening)
- MQTT "I'm alive" heartbeat
- UI polish (error states, transitions, final offset tuning)
- Async WiFi/HTTP libraries (ESPAsyncWiFiManager, AsyncTCP)

## Notes
- Intermediate check-in: Real-time weather live, table-driven validated
- Decoupled display task pattern (fetch → data → display) is stable and recommended
- Secrets: Single inclusion in manager — good encapsulation