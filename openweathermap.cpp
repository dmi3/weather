#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "openweathermap.h"
#include "credentials.h"
#define _WURL(appid, ...) "GET /forecast/"APPID"/"LAT","LON"?units=si&exclude=minutely,daily  HTTP/1.0"
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

static void setWeatherId(const char* icon) {
    if (strcmp(icon, "clear-day") == 0) {
        _result.current_weather_0_id = 800;
    } else if (strcmp(icon, "clear-night") == 0) {
        _result.current_weather_0_id = 800;
    } else if (strcmp(icon, "rain") == 0) {
        _result.current_weather_0_id = 500;
    } else if (strcmp(icon, "snow") == 0) {
        _result.current_weather_0_id = 600;
    } else if (strcmp(icon, "sleet") == 0) {
        _result.current_weather_0_id = 620;
    } else if (strcmp(icon, "wind") == 0) {
        _result.current_weather_0_id = 900;
    } else if (strcmp(icon, "fog") == 0) {
        _result.current_weather_0_id = 701;
    } else if (strcmp(icon, "cloudy") == 0) {
        _result.current_weather_0_id = 802;
    } else if (strcmp(icon, "partly-cloudy-day") == 0) {
        _result.current_weather_0_id = 802;
    } else if (strcmp(icon, "partly-cloudy-night") == 0) {
        _result.current_weather_0_id = 802;
    } else if (strcmp(icon, "thunderstorm") == 0) {
        _result.current_weather_0_id = 200;
    } else if (strcmp(icon, "hail") == 0) {
        _result.current_weather_0_id = 900;
    } else if (strcmp(icon, "mixed") == 0) {
        _result.current_weather_0_id = 800;
    } else {
        _result.current_weather_0_id = 999;
    }
}

static void parseJson(DynamicJsonDocument doc) {
    JsonObject current = doc["currently"];    

    _result.current_temp = (int) current["temperature"];
    _result.current_feels_like = (int) current["apparentTemperature"];
    _result.current_humidity = (int)(current["humidity"].as<float>() * 100);
    _result.current_wind_speed = (int) current["windSpeed"];

    const char* icon = current["icon"];
    setWeatherId(icon);
    
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
