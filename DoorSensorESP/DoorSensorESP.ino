/*
 * DoorSensor (with ESP32)
 * 
 * A magnetic switch turn on/off the power for the ESP and the ESP starts.
 * After 60 seconds the ESP asks a Google device in your
 * house to play a message.
 * A push button can be attached to `buttonPin` to turn on/off
 * the count down process.
 * A led in `LEDPin` indicates if the countdown process is running or not.
 * 
 * References: 
 *    - https://techtutorialsx.com/2017/04/24/esp32-connecting-to-a-wifi-network/
 *    - https://techtutorialsx.com/2020/04/17/esp32-mdns-address-resolution/
 *    - https://github.com/horihiro/esp8266-google-home-notifier
 * 
 */

#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
// Turn the LED on (Note that LOW is the voltage level
// but actually the LED is on; this is because
// it is active low on the ESP-01)
#define ON LOW
#define OFF HIGH
#endif

#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#define ON HIGH
#define OFF LOW
#endif

#include <esp8266-google-home-notifier.h>

const int LEDPin = 2;
const int buttonPin = 0;
const int dt = 100;

const char* ssid = "ssid";
const char* password = "password";

const char deviceName[] = "Hallway speaker";
const char message[] = "Could someone close the upstairs bathroom door, please?";

GoogleHomeNotifier ghn;
int LEDState;
int buttonNew;
int buttonOld;
unsigned long start;
 
void setup() {
  // Start counter
  start = millis();
  
  configureGPIO();
  
  Serial.begin(115200);
  
  connectToWiFi();
}
 
void loop() { 
  buttonNew=digitalRead(buttonPin);
  if (buttonOld == 0 && buttonNew == 1) {
    if (LEDState == 0) {
      digitalWrite(LEDPin, ON);
      LEDState = 1;
      Serial.println("ON");
      // Start counter
      start = millis();
    } else {
      digitalWrite(LEDPin, OFF);
      LEDState = 0;
      Serial.println("OFF");
    }
  }
  buttonOld = buttonNew;
  delay(dt);

  if (LEDState == 1) {
    unsigned long duration = millis() - start;
    unsigned long period = 1000 * 60 * 1; // 1 minutes
    Serial.println(duration);
    if (duration > period) {
      // Send message
      Serial.println("Notify");
      sendGoogleMessage(deviceName, message);
      delay(5 * 1000);
      // Start counter
      start = millis();
    }
  }
}

void configureGPIO() {
  pinMode(LEDPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  
  digitalWrite(LEDPin, ON);
  LEDState = 1;
  buttonOld = digitalRead(buttonPin);
}

void connectToWiFi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected. IP address: ");
  Serial.println(WiFi.localIP());
}

void sendGoogleMessage(const char deviceName[], const char message[]) {
 Serial.println("Connecting to Google Home...");
  if (ghn.device(deviceName, "en") != true) {
    Serial.println("error...");
    Serial.println(ghn.getLastError());
    return;
  }
  Serial.print("Found Google Home (");
  Serial.print(ghn.getIPAddress());
  Serial.print(":");
  Serial.print(ghn.getPort());
  Serial.println(")");
  
  if (ghn.notify(message) != true) {
    Serial.println("error...");
    Serial.println(ghn.getLastError());
  } else {
    Serial.println("Message sent!!!");
  }
}
