#include <GxEPD.h>
// Use include depending on your screen type
// #include <GxGDEW027W3/GxGDEW027W3.h>    // 2.7" b/w
#include <GxGDEW027C44/GxGDEW027C44.h>    // 2.7" b/w/r
// #include <GxGDE0213B72/GxGDE0213B72.h> // 2.13" b/w
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>
#include <WiFi.h>
#include "wifihelper.h"
#include "openweathermap.h"
#define ELINK_BUSY 4
#define ELINK_RESET 16
#define ELINK_DC 17
#define ELINK_SS 5
#include "credentials.h"
GxIO_Class io(SPI, ELINK_SS, ELINK_DC, ELINK_RESET);
GxEPD_Class display(io, ELINK_RESET, ELINK_BUSY);

typedef enum
{
  RIGHT_ALIGNMENT = 0,
  LEFT_ALIGNMENT,
  CENTER_ALIGNMENT,
} Text_alignment;

#define uS_TO_S_FACTOR 1000000  // Conversion factor for micro seconds to seconds 
#define TIME_TO_SLEEP 60 * 30   // Time ESP32 will go to sleep (in seconds) 
#define UPDATE_WEATHER_PROID  6 // Three hour update cycle
RTC_DATA_ATTR int wakeupCount = 0;

#if defined(_GxGDEW0154Z04_H_) || defined(_GxGDEW0213Z16_H_) || defined(_GxGDEW029Z10_H_) || defined(_GxGDEW027C44_H_)
#define HAS_RED_COLOR
  #define CONTRAST_COLOR GxEPD_RED
#else
  #define CONTRAST_COLOR GxEPD_BLACK
#endif

void wifiStart()
{
  WiFi.disconnect(false,true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.printf("ssid : %s  passwd : %s\n", WiFi.SSID().c_str(), WiFi.psk().c_str());
  Serial.printf("IP Address : %s\n", WiFi.localIP().toString().c_str());
}

void displayText(const String &str, int16_t y, uint8_t alignment)
{
  int16_t x = 0;
  int16_t x1, y1;
  uint16_t w, h;
  
  display.setCursor(x, y);
  display.getTextBounds(str, x, y, &x1, &y1, &w, &h);

  switch (alignment)
  {
  case RIGHT_ALIGNMENT:
    display.setCursor(display.width() - w - x1, y);
    break;
  case LEFT_ALIGNMENT:
    display.setCursor(0, y);
    break;
  case CENTER_ALIGNMENT:
    display.setCursor(display.width() / 2 - ((w + x1) / 2), y);
    break;
  default:
    break;
  }
  display.println(str);
}

void displayWeather()
{
  int i;
  int16_t x, y;
  display.init();
  //display.eraseDisplay();
  display.setRotation(2);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextSize(1);

  // Get weather information
  while (!getWeather())
  {
    delay(1000);
  }

  owth_api_t weather = getWeatherInfo();

  i = weather.current_weather_0_id.toInt();
  x = display.width() / 2 - 50;
  y = 0;
  const Weather_Show_t wicon = get_icon(i);
  display.setCursor(x, y);
  display.drawBitmap(x, y, wicon.img, wicon.w, wicon.h, GxEPD_BLACK);

  String temp = weather.current_temp + " (" + weather.current_feels_like + ")" + "*C";
  String humidity = weather.current_humidity + "% " + weather.current_wind_speed + "m/s";
  String logo = weather.current_weather_0_description;
  bool alerts = weather.alerts;

  if (alerts) {
    display.setCursor(0, 0);    
    display.drawBitmap(0, 0, alert, 16, 16, CONTRAST_COLOR);
  }

  y = wicon.h + 5;

  display.setTextColor(CONTRAST_COLOR);
  displayText(logo, y, CENTER_ALIGNMENT);

  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  y = display.getCursorY() + 15;
  displayText(temp, y, RIGHT_ALIGNMENT);

  y = display.getCursorY() + 1;
  displayText(humidity, y, RIGHT_ALIGNMENT);

  // Draw  probability of precipitation for 25h
  for (int i = 0; i < 25; i++) {
    float pop = weather.hourly_pop[i]*20;
    display.fillRect(i*10, display.height() - pop, 10, pop, GxEPD_BLACK);
  }

  y = display.getCursorY() + 30;

  // Draw temperature for 25h
  for (int i = 1; i < 26; i++) {
    float temp1 = weather.hourly_temp[i-1]*2;
    float temp2 = weather.hourly_temp[i]*2;
    display.drawLine((i-1)*10, y-temp1, i*10, y-temp2, CONTRAST_COLOR);
  }

  display.setTextColor(CONTRAST_COLOR);
  display.setFont(&FreeMonoBold9pt7b);
  displayText(String(weather.hourly_temp[0], 0), y-weather.hourly_temp[0]*2, LEFT_ALIGNMENT);
  displayText(String(weather.hourly_temp[15], 0), y-weather.hourly_temp[15]*2, CENTER_ALIGNMENT);
  displayText(String(weather.hourly_temp[25], 0), y-weather.hourly_temp[25]*2, RIGHT_ALIGNMENT);

  display.update();
}

void setup()
{
  Serial.begin(115200);
  delay(500);

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER)
  {
    Serial.print("Boot Count : ");
    Serial.println(wakeupCount);

    // Reach the weather at the specified time
    if (++wakeupCount >= UPDATE_WEATHER_PROID)
    {
      wakeupCount = 0;
      wifiStart();
      displayWeather();
      pingHealthchecks();
    }
  }
  else
  {
    // Get the weather for the first time and display it
    //scanNetworks();
    wifiStart();
    displayWeather();
    pingHealthchecks();
  }

  /*
  First we configure the wake up source
  We set our ESP32 to wake up every 30 mintues
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  Serial.println("Going to sleep now");

  // delay(5000);
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop()
{
  //do nothing
}
