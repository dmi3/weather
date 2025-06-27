#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "openweathermap.h"
#include "credentials.h"
#define _WURL(appid, ...) "GET /forecast/"APPID"/"LAT","LON"?units=si&exclude=minutely,daily  HTTP/1.0"
#define WURL(appid, ...) _WURL(__VA_ARGS__)
WiFiClient http;
StaticJsonDocument<8192> jsonBuffer;
static owth_api_t _result;

const Weather_Show_t get_icon(String wcode) {
    if (wcode == "clear-day") {
        return wth[31]; // clear
    } else if (wcode == "clear-night") {
        return wth[33]; // fair_night
    } else if (wcode == "rain") {
        return wth[11]; // showers
    } else if (wcode == "snow") {
        return wth[16]; // snow
    } else if (wcode == "sleet") {
        return wth[18]; // sleet
    } else if (wcode == "wind") {
        return wth[24]; // windy
    } else if (wcode == "fog") {
        return wth[21]; // foggy
    } else if (wcode == "cloudy") {
        return wth[26]; // cloudy
    } else if (wcode == "partly-cloudy-day") {
        return wth[30]; // partly_cloudy_day
    } else if (wcode == "partly-cloudy-night") {
        return wth[29]; // partly_cloudy_night
    } else if (wcode == "thunderstorm") {
        return wth[4]; // thunderstorms
    } else if (wcode == "hail") {
        return wth[17]; // hail
    } else if (wcode == "mixed") {
        return wth[5]; // mixed_rain_and_snow
    } else {
        return wth[48]; // not_available for any unrecognized codes
    }
}

static void parseJson(DynamicJsonDocument doc) {
    JsonObject current = doc["currently"];    

    _result.current_temp = (int) current["temperature"];
    _result.current_feels_like = (int) current["apparentTemperature"];
    _result.current_humidity = (int)(current["humidity"].as<float>() * 100);
    _result.current_wind_speed = (int) current["windSpeed"];
    _result.current_weather_0_id = (const char*) current["icon"];
    _result.current_weather_0_description = (const char*) current["summary"];

    int i = 0;
    for (JsonObject elem : doc["hourly"]["data"].as<JsonArray>()) {
        _result.hourly_temp[i] = elem["temperature"];
        _result.hourly_pop[i] = elem["precipProbability"];
         i++;        
    }

    for (JsonObject elem : doc["alerts"].as<JsonArray>()) {
        //todo check severity
        _result.alerts=true;        
    }
}

bool getWeather()
{
  char status[32] = {0};
  if (!http.connect("api.pirateweather.net", 80))
  {
    Serial.println("Connection failed");
    http.stop();
    return false;
  }

  Serial.println("Connected!");
  http.println(WURL());
  http.println("Host: api.pirateweather.net");
  http.println("Connection: close");
 
  if (http.println() == 0)
  {
    Serial.println("Failed to send request");
    http.stop();
    return false;
  }
  // Check HTTP status
  http.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0)
  {
    Serial.print("Unexpected response: ");
    Serial.println(status);
    http.stop();
    return false;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!http.find(endOfHeaders))
  {
    Serial.println("Invalid response");
    http.stop();
    return false;
  }

  jsonBuffer.clear();

  DynamicJsonDocument doc(24576);
  auto error =  deserializeJson(doc, http);
 //= jsonBuffer.parseObject(http);

if (error) {
    Serial.println("parse data fail");
    http.stop();
    return false;
} 
  Serial.println("parse data success");
  parseJson(doc);

  http.stop();
  return true;
}


const owth_api_t &getWeatherInfo()
{
  return _result;
}
