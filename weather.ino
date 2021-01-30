#include <GxEPD.h>
#include <GxGDE0213B72/GxGDE0213B72.h> // 2.13" b/w
#include <Fonts/FreeMonoBold12pt7b.h>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>
#include <WiFi.h>
#include "openweathermap.h"
#define ELINK_BUSY 4
#define ELINK_RESET 16
#define ELINK_DC 17
#define ELINK_SS 5
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

void wifiStart()
{
  //WiFi.begin("Martin Router King", "unavoltahovistounleon3");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int i = 20;
  Serial.println("Try to Connect ...");
  while (i--)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println();
      Serial.println("Connect Success");
      break;
    }
    Serial.print(".");
    delay(1000);
  }
  
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.mode(WIFI_STA);
    Serial.println("SmartConfig Begin ...");
    WiFi.beginSmartConfig();

    while (!WiFi.smartConfigDone())
    {
      Serial.print(".");
      delay(1000);
    }
    Serial.println("SmartConfig Success");
    WiFi.setAutoConnect(true);
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
  display.eraseDisplay();
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextSize(1);

  // Get weather information
  while (!getWeather())
  {
    delay(1000);
  }

  i = getWeatherInfo().code.toInt();
  x = display.width() / 2 - 50;
  y = 0;
  const Weather_Show_t wicon = get_icon(i);
  display.setCursor(x, y);
  display.drawBitmap(x, y, wicon.img, wicon.w, wicon.h, GxEPD_BLACK);

  String temp = getWeatherInfo().temp + "*" + getWeatherInfo().temperature;
  String humidity = getWeatherInfo().humidity + "% ";
  String city = getWeatherInfo().city;
  String logo = getWeatherInfo().text;

  y = wicon.h + 18;
  displayText(temp, y, RIGHT_ALIGNMENT);

  y = display.getCursorY() + 1;
  displayText(humidity, y, RIGHT_ALIGNMENT);

  y = display.getCursorY() + 1;
  displayText(city, y, RIGHT_ALIGNMENT);

  y = display.getCursorY() + 25;
  //display.setTextSize(2);
  displayText(logo, y, CENTER_ALIGNMENT);

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
    }
  }
  else
  {
    // Get the weather for the first time and display it
    wifiStart();
    displayWeather();
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

