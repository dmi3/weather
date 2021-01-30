Forked from [autoscatto/MeTeo5](https://github.com/autoscatto/MeTeo5)

MeTeo5 is a revision of https://github.com/lewisxhe/TTGO-T5/tree/master/TTGO-Yahoo-Weather
This "example" code used defunct query.yahooapis.com endpoint and don't work with my **T5 v2.3_2.13** board. (thanks to https://github.com/lewisxhe/GxEPD for the working screen patch)

### Setup

1. `mv credentials.h.SAMPLE credentials.h`
2. Set up credentials and [OpenWeather API Key](https://openweathermap.org/appid) in `credentials.h`
    - ⚠ Note that for some time, new key will produce `401` error. Give it some time
3. Modify <weather.ino>, replace `#include <GxGDEW027W3/GxGDEW027W3.h>` with value [corresponding your screen](https://github.com/Xinyuan-LilyGO/LilyGo-T5-Epaper-Series/blob/05de6cd912d2dcfff040acc548d45830d2559c33/examples/GxEPD_Example/GxEPD_Example.ino#L18-L35)
4. Install [arduino-esp32](https://github.com/espressif/arduino-esp32#installation-instructions)
    - ⚠ Make sure to use `1.0.2` as later has issues with WiFi
5. Install dependencies:
    - [GxEPD](https://github.com/lewisxhe/GxEPD)
    - ⚠ Make sure to use `master` version, not the latest release
    - [ArduinoJson 6.13.0](https://github.com/bblanchon/ArduinoJson)
    - [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library)

