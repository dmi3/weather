Forked from [autoscatto/MeTeo5](https://github.com/autoscatto/MeTeo5)

MeTeo5 is a revision of https://github.com/lewisxhe/TTGO-T5/tree/master/TTGO-Yahoo-Weather
This "example" code used defunct query.yahooapis.com endpoint and don't work with my **T5 v2.3_2.13** board. (thanks to https://github.com/lewisxhe/GxEPD for the working screen patch)


### Dependencies
* [GxEPD](https://github.com/lewisxhe/GxEPD)
* [ArduinoJson 6.13.0](https://github.com/bblanchon/ArduinoJson)
* [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library)

### OpenWeather Reference
- [OpenWeather Documentation](https://openweathermap.org/current)
- [OpenWeather API Key](https://openweathermap.org/appid)

### Step:
- Change `LOCATION_Q` `APPID` `WIFI_SSID` and `WIFI_PASSWORD` macros in openweathermap.h
- Compile (for beginners I recommend https://platformio.org/) and burn to board
