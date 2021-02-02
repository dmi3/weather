#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "openweathermap.h"
#include "credentials.h"
#define _WURL(appid, ...) "GET /data/2.5/onecall?lat="LAT"&lon="LON"&APPID="APPID"&exclude=minutely,daily&units=metric  HTTP/1.0"
#define WURL(appid, ...) _WURL(__VA_ARGS__)
WiFiClient http;
StaticJsonDocument<8192> jsonBuffer;
static owth_api_t _result;

const Weather_Show_t get_icon(int wcode){
  //https://openweathermap.org/weather-conditions
  switch (wcode) {
  case 200:
  case 201:
  case 202:
  case 210:
  case 211:
  case 212:
  case 221:
  case 230:
  case 231:
  case 232:
  case 781:
    return {0, 100, 100, tornado};
    break;
  case 300 ... 321:
    return {9, 100, 100, drizzle};
    break;
  case 511:
  case 771:
    return {6, 100, 100, mixed_rain_and_sleet};
    break;
  case 500 ... 504:
  case 520 ... 531:
    return {11, 100, 100, showers};
    break;
  case 615:
  case 616:
    return {5, 100, 100, mixed_rain_and_snow};
    break; 
  case 600 ... 613:
  case 620 ... 622:
    return {13, 100, 100, snow_flurries};
    break; 
  case 701:
  case 741:
    return {21, 100, 100, foggy};
    break;
  case 711:
  case 731: 
  case 751:
  case 761:
  case 762:
    return {19, 100, 100, dust};
    break;
  case 721: 
    return {20, 100, 100, haze};
    break;
  case 800:
    return {31, 100, 100, clear};
    break;
  case 801:
  case 802: 
    return {44, 100, 100, partly_cloudy};
    break;
  case 803:
  case 804:
    return {26, 100, 100, cloudy};
  default:
    return {48, 100, 0, not_available};
    break;
  }
} 


static void parseJson(DynamicJsonDocument doc)
{ //https://arduinojson.org/v6/assistant/
//  float lat = doc["lat"]; // 52.52
//  float lon = doc["lon"]; // 13.405
//  const char* timezone = doc["timezone"]; // "Europe/Berlin"
//  int timezone_offset = doc["timezone_offset"]; // 3600

  JsonObject current = doc["current"];
//  long current_dt = current["dt"]; // 1612101228
//  long current_sunrise = current["sunrise"]; // 1612074267
//  long current_sunset = current["sunset"]; // 1612104568
  _result.current_temp = (int) current["temp"]; // 272.15
  _result.current_feels_like = (int) current["feels_like"]; // 267.64
//  int current_pressure = current["pressure"]; // 996
  _result.current_humidity = (int) current["humidity"]; // 69
//  float current_dew_point = current["dew_point"]; // 267.75
//  int current_uvi = current["uvi"]; // 0
//  int current_clouds = current["clouds"]; // 75
//  int current_visibility = current["visibility"]; // 10000
  _result.current_wind_speed = (int) current["wind_speed"]; // 2.57
//  int current_wind_deg = current["wind_deg"]; // 280

  JsonObject current_weather_0 = current["weather"][0];
  _result.current_weather_0_id = (int) current_weather_0["id"]; // 803
//  const char* current_weather_0_main = current_weather_0["main"]; // "Clouds"
  _result.current_weather_0_description = (const char*) current_weather_0["description"]; // "broken clouds"
//  const char* current_weather_0_icon = current_weather_0["icon"]; // "04d"

    int i = 0;

  for (JsonObject elem : doc["hourly"].as<JsonArray>()) {
//
//    long dt = elem["dt"]; // 1612098000, 1612101600, 1612105200, 1612108800, 1612112400, 1612116000, ...
//    float temp = elem["temp"]; // 272.15, 271.72, 270.65, 269.84, 269.17, 268.79, 268.19, 267.67, 267.75, ...
//    float feels_like = elem["feels_like"]; // 265.51, 265.92, 265.24, 264.61, 264.22, 264.07, 263.38, ...
//    int pressure = elem["pressure"]; // 996, 996, 996, 997, 997, 997, 997, 998, 998, 999, 999, 999, 999, ...
//    int humidity = elem["humidity"]; // 69, 81, 89, 94, 95, 96, 96, 97, 97, 97, 97, 98, 98, 98, 98, 98, 97, ...
//    float dew_point = elem["dew_point"]; // 267.75, 269.21, 269.27, 269.11, 268.57, 266.91, 266.55, 266.3, ...
//    float uvi = elem["uvi"]; // 0.15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.18, 0.3, 0.34, ...
//    int clouds = elem["clouds"]; // 75, 85, 84, 74, 75, 75, 79, 73, 79, 65, 65, 64, 89, 76, 69, 60, 62, 66, ...
//    int visibility = elem["visibility"]; // 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, ...
//    float wind_speed = elem["wind_speed"]; // 5.62, 4.67, 4.14, 3.87, 3.39, 3.03, 3.07, 3.21, 2.92, 3.17, ...
//    int wind_deg = elem["wind_deg"]; // 276, 275, 267, 258, 252, 241, 220, 209, 205, 200, 203, 204, 205, ...
//
//    JsonObject weather_0 = elem["weather"][0];
//    int weather_0_id = weather_0["id"]; // 803, 804, 803, 803, 803, 803, 803, 803, 803, 803, 803, 803, 804, ...
//    const char* weather_0_main = weather_0["main"]; // "Clouds", "Clouds", "Clouds", "Clouds", "Clouds", ...
//    const char* weather_0_description = weather_0["description"]; // "broken clouds", "overcast clouds", ...
//    const char* weather_0_icon = weather_0["icon"]; // "04d", "04d", "04n", "04n", "04n", "04n", "04n", ...
//
      _result.hourly_pop[i] = elem["pop"]; // 0.16, 0.13, 0.09, 0.06, 0.06, 0.04, 0, 0, 0, 0.06, 0.15, 0.23, 0.18, 0.1, ...
      i++;
    }

    
//
//  for (JsonObject elem : doc["alerts"].as<JsonArray>()) {
//
//    const char* sender_name = elem["sender_name"]; // "Latvian Environment, Geology and Meteorology Centre", ...
//    const char* event = elem["event"]; // "Yellow Snow-Ice Warning", "Orange Snow-Ice Warning"
//    long start = elem["start"]; // 1612062000, 1611993600
//    long end = elem["end"]; // 1612130340, 1612130340
//    const char* description = elem["description"]; // " 31.01.2021 by day, due to continuous snowfall, ...
//
//  }
}

bool getWeather()
{
  char status[32] = {0};
  if (!http.connect("api.openweathermap.org", 80))
  {
    Serial.println("Connection failed");
    http.stop();
    return false;
  }

  Serial.println("Connected!");
  http.println(WURL());
  http.println("Host: api.openweathermap.org");
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
