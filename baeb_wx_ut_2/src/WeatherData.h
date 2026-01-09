#ifndef WEATHER_DATA_H
#define WEATHER_DATA_H

#include <ArduinoJson.h>

struct WeatherData {
    float temp_c = 0.0;
    float temp_min = 0.0;
    float temp_max = 0.0;
    int humidity = 0;
    float wind_speed = 0.0;
    const char* condition = "Unknown";
    const char* icon_code = "01d";  // Default sunny
    uint32_t last_update = 0;
    bool valid = false;
};

extern WeatherData current_weather;

void update_weather_from_json(const JsonDocument& doc);

#endif