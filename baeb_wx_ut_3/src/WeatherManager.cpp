#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TaskSchedulerDeclarations.h>
#include "WeatherData.h"
#include "weather_credentials.h"  // secrets


extern void update_ui_from_weather(void);

WeatherData current_weather;
//StaticJsonDocument<1024> doc;
StaticJsonDocument<32768> doc;

void fetch_weather() 
{
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        delay(5000);  // Wait for connect
        if (WiFi.status() != WL_CONNECTED) return;
    }

    HTTPClient http;
    
    Serial.println("Fetched");

    //String url = "https://api.weatherapi.com/v1/current.json?key=";
    //String url = "https://api.weatherapi.com/v1/forecast.json?key=" + String(WEATHER_API_KEY) + "&q=" + String(WEATHER_LOCATION) + "&days=7&aqi=no&alerts=no";
    String url = "https://api.weatherapi.com/v1/current.json?key=" + String(WEATHER_API_KEY) + "&q=" + String(WEATHER_LOCATION) + "&aqi=no"; 
    //url += WEATHER_API_KEY;
    //url += "&q=";
    //url += WEATHER_LOCATION;
    //url += "&aqi=no";

    //String url = "https://api.weatherapi.com/v1/forecast.json?key=fff528869fbd42f69ec174849260601&q=Grass%20Valley,%20CA&days=7&aqi=no&alerts=no";
    //https://api.weatherapi.com/v1/forecast.json?key=fff528869fbd42f69ec174849260601&q=Grass Valley, CA&days=7&aqi=no&alerts=no

    Serial.println("Fetched started");
    Serial.println(url);
    http.begin(url);
    int httpCode = http.GET();
    Serial.println("Fetch complete - HTTP code: " + String(httpCode));
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();

        DeserializationError error = deserializeJson(doc, payload);
        if (error != DeserializationError::Code::Ok) {
            Serial.print("Parse error: ");
            Serial.println(error.c_str());
        } else {
            //Serial.println("=== RAW JSON RESPONSE ===");
            //Serial.println(payload);                    // Dumps the entire JSON string
            //Serial.println("========================");
            update_weather_from_json(doc);
            current_weather.last_update = millis();
            current_weather.valid = true;
            Serial.println("Fetched ok");
            Serial.println("Parsed temp_f: " + String(current_weather.temp_f));
            Serial.println("Parsed condition: " + String(current_weather.condition));
        }
    } else {
        // Keep last data
    }

    http.end();
}

void update_weather_from_json(const JsonDocument& doc) {

    current_weather.temp_f = doc["current"]["temp_f"];
    current_weather.condition = doc["current"]["condition"]["text"].as<const char*>();
    current_weather.humidity = doc["current"]["humidity"];
    current_weather.wind_speed = doc["current"]["wind_kph"];
}