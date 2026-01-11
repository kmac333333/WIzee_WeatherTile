# WIzee Weather Tile Project Roadmap  
**Last Updated**: January 11, 2026  
**Current Milestone**: OSB 7.1 – Real WeatherAPI.com fetch + table-driven UI + data binding – **Complete & Accepted**

## Project Status Summary
- Real-time weather data is live and displaying correctly (current temp, condition, humidity, wind, high/low from forecast)
- Table-driven UI creation is working and maintainable
- Display update decoupled from data fetch (separate TaskScheduler task)
- Secrets encapsulated (single inclusion in manager layer)
- Stability improved (non-blocking patterns, throttled tasks)
- Remaining issues: Occasional reboots (likely WDT starvation), alignment fine-tuning, full 7-day forecast JSON buffer, no real icons yet

## Roadmap Table

| OSB #  | Milestone                                      | Status       | Estimated Timeline | Dependencies / Notes |
|--------|------------------------------------------------|--------------|--------------------|----------------------|
| 1–6    | Display init, fonts, overlay/spinner, refresh feedback | Complete     | Jan 2026           | Foundation locked |
| 7      | Real WeatherAPI.com fetch + table-driven UI + data binding | Complete     | Jan 10, 2026       | Live data, async fetch needed |
| 7.5    | Refactor baseline (_ut_3): async libs, full modularity, CLI expansion | In Progress  | Jan 11–20, 2026    | Current focus: async HTTP, CLI task, WDT fixes |
| 8      | Real icons (local map from condition.code, no repeated fetches) | Planned      | Jan 21–25, 2026    | Map codes → LVGL symbols |
| 9      | Periodic + behavior-based scheduling (4x/day + morning/evening) | Planned      | Jan 26–30, 2026    | TaskScheduler + time check |
| 10     | MQTT "I'm alive" heartbeat (unique hex-ID, periodic status) | Planned      | Feb 1–5, 2026      | PubSubClient integration |
| 11     | UI polish: final offsets, error states, transitions, theme | Planned      | Feb 6–10, 2026     | Offline fallback, visual feedback |
| 12     | Async WiFi/HTTP full migration (ESPAsyncWiFiManager, AsyncTCP) | Planned      | Feb 11–20, 2026    | Eliminate all blocking calls |
| 13+    | Final stability, enclosure OpenSCAD, SD cache for icons, LVGL v9 migration | Future       | Q1–Q2 2026         | Post-MVP polish |

## Key Focus Areas (Next 2–4 Weeks)
- **OSB 7.5** (current): Finish the refactor
  - Async HTTP fetch (ESPAsyncHTTPClient or similar)
  - Move CLI to dedicated TaskScheduler task
  - Final WDT stability (rtc_wdt_feed(), task throttling)
  - Lock _ut_3 baseline

- **Stability Priority**: Reboots (RTC_SW_CPU_RST) — throttle display task to 1s + dirty flag, add heap monitoring, feed RTC WDT

- **Data Flow**: Fetch → Model2 → Display task → UI (clean separation)

## Quick Wins Planned for OSB 8
- Local icon mapping (condition.code → LVGL symbols)
- Reduce fetch interval to 15–30 min (production rate)
- Add error state UI ("Offline", retry on tap)
- CLI expansion ("wifi", "status", "logs", "fetch")

The project is in a very strong position — real data is live, architecture is modular, and the next few OSBs will make it feel like a complete, production-ready weather tile.

Let me know if you'd like this expanded, shortened, or exported as a separate file for your other machine.