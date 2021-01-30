#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "openweathermap.h"
#define _WURL(appid, ...) "GET /data/2.5/weather?q="#__VA_ARGS__"&units=metric&appid="#appid"  HTTP/1.0"
#define WURL(appid, ...) _WURL(appid, __VA_ARGS__)
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

float coord_lon = doc["coord"]["lon"]; // 10.79
float coord_lat = doc["coord"]["lat"]; // 43.54
JsonObject weather_0 = doc["weather"][0];
JsonObject main = doc["main"];
const char* weather_0_main = weather_0["main"]; // "Clouds"
_result.text  = (const char *)weather_0["description"]; // "broken clouds"
_result.code = (int)weather_0["id"]; 
_result.temperature = "C";
_result.temp = String((float)main["temp"]);
_result.humidity =  String((int)main["humidity"]);
  _result.city =  (const char*)doc["name"];
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
  http.println(WURL(APPID, LOCATION_Q));
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

  // Parsing json data returned by Yahoo
const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(13) + 300;
DynamicJsonDocument doc(capacity);
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