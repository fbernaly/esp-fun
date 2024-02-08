# DoorSensor with Arduino and serial wireless communication.

In this project a magnetic switch turn on/off the power for the Arduino and the Arduino starts. After n `notifyAfter` seconds the Arduino sends a cmd using a wireless serial interface like bluetooth, TTL-WiFi, etc. A push button can be attached to `buttonPin` to turn on/off the count down process. A led in `LEDPin` indicates if the countdown process is running or not. An ESP32 should run a server to receive the cmd that the wireless interface is sending (see [related project](../ESPHomeServer/ESPHomeServer.ino)). ESP32 receives the cmd and plays a massage in a Google device in your home.

## References

- [ESP32 Bluetooth Classic with Arduino IDE](https://randomnerdtutorials.com/esp32-bluetooth-classic-arduino-ide/)
- [ESP32 Arduino: Getting the Bluetooth Device Address](https://techtutorialsx.com/2018/03/09/esp32-arduino-getting-the-bluetooth-device-address/)
- [Using the BlueSMiRF](https://learn.sparkfun.com/tutorials/using-the-bluesmirf/introduction/)
- [Bluetooth Data Module Command Reference & Advanced Information User’s Guide](https://cdn.sparkfun.com/assets/1/e/e/5/d/5217b297757b7fd3748b4567.pdf)
