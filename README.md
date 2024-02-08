## ESPfun

Some fun project with ESP32 and Arduino.

### Door Sensor with ESP32

A magnetic switch turn on/off the power for the ESP32 and the ESP32 starts. After n `notifyAfter` seconds the ESP32 asks a Google device in your house to play a message. A push button can be attached to `buttonPin` to turn on/off the count down process. A led in `LEDPin` indicates if the countdown process is running or not.

### DoorSensor with Arduino and serial wireless communication.

A magnetic switch turn on/off the power for the Arduino and the Arduino starts.  After n `notifyAfter` seconds the Arduino sends a cmd using a wireless serial interface like bluetooth, TTL-WiFi, etc. A push button can be attached to `buttonPin` to turn on/off the count down process. A led in `LEDPin` indicates if the countdown process is running or not. An ESP32 should run a server to receive the cmd that the wireless interface is sending. ESP32 receives the cmd and plays a massage in a Google device in your home.

### Google Home Notifier

ESP32 sends a message to a Google device in your house to reproduce a message.

### Google Home Notifier Server

ESP32 runs a server that accepts GET requests for sending a message to a Google device in your home.

####  ESPHomeServer

ESP32 runs a multiple servers:

* a server that accepts GET requests for sending a message to a Google device in your home.
* a TCP server that accept commands to send a message
to a Google device in your home.

#### Reading LED Light

It uses an ESP32 and a push button to control an LED light and connects to Apple HomeKit using [HomeSpan linrary](https://github.com/HomeSpan/HomeSpan).

### References:

- [Connecting to a wifi network](https://techtutorialsx.com/2017/04/24/esp32-connecting-to-a-wifi-network/)
- [mDNS address resolution](https://techtutorialsx.com/2020/04/17/esp32-mdns-address-resolution/)
- [Google Home notifier](https://github.com/horihiro/esp8266-google-home-notifier)
- [ESP32 Bluetooth Classic with Arduino IDE](https://randomnerdtutorials.com/esp32-bluetooth-classic-arduino-ide/)
- [Talk to the ESP32 over WiFi](https://www.megunolink.com/articles/wireless/talk-esp32-over-wifi/)
- [ESP32 connects with nRF24L01 2.4 GHz wireless chip](http://www.iotsharing.com/2018/03/esp-and-raspberry-connect-with-nrf24l01.html)

