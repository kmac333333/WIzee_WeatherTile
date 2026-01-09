# Project Roadmap

| OSB # | Milestone                          | Status       |
|-------|------------------------------------|--------------|
| 1-5   | Display, fonts, overlay/spinner    | Complete     |
| 6     | Full manual refresh feedback       | Complete     |
| 7     | Real OpenWeatherMap fetch + bind   | Complete     |
| 8     | Scheduling (periodic + behavior)   | Pending      |
| 9     | MQTT heartbeat                     | Pending      |
| 10+   | UI polish, settings, enclosure     | Future       |

v9 LVGL migration: Post-MVP sub-project.
Custom PyQt font tool: Post-MVP sub-project.

# Project Roadmap (Updated Jan 09, 2026)

| OSB # | Milestone                          | Status              | Notes        |
|-------|------------------------------------|---------------------|--------------|-------------------------------------|
| 1–6   | Display, fonts, overlay/spinner, refresh feedback        | Complete     | Stability wins                      |
| 7     | Real WeatherAPI.com fetch + table-driven UI              | Complete     | Async fetch, decoupled display task |
| 7.5   | Refactor for design guide (async libs, modularity, CLI)  | Planned      | New baseline _ut_3                  |
| 8     | Icons (local map), periodic/behavior scheduling          | Pending      | Next                                |
| 9     | MQTT heartbeat                                           | Pending      |                                     |
| 10+   | UI polish, enclosure, final stability                    | Future       |                                     |