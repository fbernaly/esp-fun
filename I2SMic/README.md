# I2S MIC SETUP

This project demostrates how to setup and connect the INMP441 mic with ESP32 and output the mic input in the serial ploter in Arduino IDE.

## Wiring up

Modify the wiring as needed.

<div align="center">
<img height="50%" width="50%" src="./circuit_mic_setup.png">
</div>

```c++
#define I2S_MIC_SCK 32
#define I2S_MIC_WS 25
#define I2S_MIC_SD 33
```

|INMP441 | ESP32| Info|
|---|---|---|
|VDD|3v3|Power - DO NOT USE 5V!|
|GND|GND| GND|
|L/R|GND|Left channel or right channel|
|WS|25|Left right clock|
|SCK|32|Serial clock|
|SD|33|Serial data|

## References

- [I2S - API Reference](https://docs.espressif.com/projects/esp-idf/en/v3.3.5/api-reference/peripherals/i2s.html)
- [Sound with ESP32 – I2S Protocol](https://dronebotworkshop.com/esp32-i2s/)
- [The Simplest Test Code for an I2S Microphone on the ESP32 I can Imagine](https://github.com/atomic14/esp32-i2s-mic-test/tree/main)
- [Cirkit Designer](https://zenluo.notion.site/Cirkit-Designer-Tutorials-4f5a47656b384709838eb5350596bf95)
