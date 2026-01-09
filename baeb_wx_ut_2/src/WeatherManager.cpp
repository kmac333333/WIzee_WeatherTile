#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TaskSchedulerDeclarations.h>
#include "WeatherData.h"
#include "weather_credentials.h"  // secrets




WeatherData current_weather;
StaticJsonDocument<1024> doc;

void fetch_weather() 
{
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        delay(5000);  // Wait for connect
        if (WiFi.status() != WL_CONNECTED) return;
    }

    HTTPClient http;
    
    Serial.println("Fetched");
    String url = "https://api.weatherapi.com/v1/current.json?key=";
    url += WEATHER_API_KEY;
    url += "&q=";
    url += WEATHER_LOCATION;
    url += "&aqi=no";

    Serial.println("Fetched started");
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        
        deserializeJson(doc, payload);

        update_weather_from_json(doc);
        current_weather.last_update = millis();
        current_weather.valid = true;

        Serial.println("Fetched ok");
    } else {
        // Keep last data
    }

    http.end();
}

void update_weather_from_json(const JsonDocument& doc) {

    current_weather.temp_c = doc["current"]["temp_c"];
    current_weather.condition = doc["current"]["condition"]["text"].as<const char*>();
    current_weather.humidity = doc["current"]["humidity"];
    current_weather.wind_speed = doc["current"]["wind_kph"];
}