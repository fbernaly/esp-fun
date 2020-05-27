/*
 * DoorSensor (with ESP32)
 * 
 * A magnetic switch turn on/off the power for the ESP32 and the ESP32 starts.
 * After n `notifyAfter` seconds the ESP32 asks a Google device in your
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

#include <WiFi.h>
#include <ESPmDNS.h>
#include <esp8266-google-home-notifier.h>

const int LEDPin = 2;
const int buttonPin = 4;
const int dt = 100;
const int notifyAfter = 30; // in seconds

const char* ssid = "ssid";
const char* password = "password";
const char* hostname = "hostname";

const char deviceName[] = "Hallway speaker";
const char message[] = "Could someone close the bathroom door, please?";

GoogleHomeNotifier ghn;

int LEDState;
int buttonNew;
int buttonOld;
int count;
 
void setup() {
  Serial.begin(115200);

  Serial.println("");
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setHostname(hostname);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  while (!MDNS.begin(hostname)) {
    Serial.println("Error starting mDNS");
    delay(250);
  }

  Serial.println("");
  Serial.println("connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Hostname: ");
  Serial.println(WiFi.getHostname());

  pinMode(LEDPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  
  digitalWrite(LEDPin,HIGH);
  LEDState = 1;
  buttonOld = digitalRead(buttonPin);
  count = 0;
}
 
void loop() {
  buttonNew=digitalRead(buttonPin);
  if (buttonOld == 0 && buttonNew == 1) {
    if (LEDState == 0) {
      digitalWrite(LEDPin,HIGH);
      LEDState = 1;
      Serial.println("ON");
    } else {
      digitalWrite(LEDPin, LOW);
      LEDState = 0;
      Serial.println("OFF");
    }
  }
  buttonOld = buttonNew;
  delay(dt);

  int maxCount = notifyAfter * (1000 / dt);

  if (LEDState == 1) {
    count++;
    Serial.print(count);
    Serial.print("/");
    Serial.print(maxCount);
    Serial.println();
  } else {
    count = 0;
  }
  
  if (count >= maxCount) {
    Serial.println("Notify");
    sendGoogleMessage(deviceName, message);
    count = 0;
    delay(5 * 1000);
  }
}

void sendGoogleMessage(const char displayName[], const char message[]) {
 Serial.println("connecting to Google Home...");
  if (ghn.device(displayName, "en") != true) {
    Serial.println("error...");
    Serial.println(ghn.getLastError());
    return;
  }
  Serial.print("found Google Home(");
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
