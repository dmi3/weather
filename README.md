# Weather for LilyGO TTGO e-ink ESP32 boards

![Weather for LilyGO TTGO e-ink ESP32 boards](https://developer.run/pic/weather1.jpeg)

* Forked from [autoscatto/MeTeo5](https://github.com/autoscatto/MeTeo5)
* See <https://developer.run/52>

### Setup

1. `mv credentials.h.SAMPLE credentials.h`
2. Set up credentials and [Pirate Weater API Key](https://pirate-weather.apiable.io), and <https://healthchecks.io> in [credentials.h](./credentials.h.SAMPLE)
3. Modify [weather.ino](./weather.ino), replace `#include <GxGDEW027W3/GxGDEW027W3.h>` with value [corresponding your screen](https://github.com/Xinyuan-LilyGO/LilyGo-T5-Epaper-Series/blob/05de6cd912d2dcfff040acc548d45830d2559c33/examples/GxEPD_Example/GxEPD_Example.ino#L18-L35)
4. Install [arduino-esp32](https://github.com/espressif/arduino-esp32#installation-instructions)
    - ⚠ Make sure to use `1.0.2` as later has issues with WiFi
5. Install dependencies:
    - [GxEPD](https://github.com/lewisxhe/GxEPD)
    - ⚠ Make sure to use `master` version, not the latest release
    - [ArduinoJson 6.13.0](https://github.com/bblanchon/ArduinoJson)
    - [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library)

### Troubleshooting

* Make sure that device is connected to USB port in your computer (no hubs, monitors etc.)
* Make sure `/dev/ttyUSB0` is selected in Arduino IDE (not `/dev/tty0`)
