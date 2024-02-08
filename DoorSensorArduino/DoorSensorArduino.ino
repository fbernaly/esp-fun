/*
 * DoorSensor (with Arduino and serial wireless communication)
 * 
 * A magnetic switch turn on/off the power for the Arduino and the Arduino starts.
 * After n `notifyAfter` seconds the Arduino sends a cmd using a wireless serial interface
 * like bluetooth, TTL-WiFi, etc.
 * A push button can be attached to `buttonPin` to turn on/off
 * the count down process.
 * A led in `LEDPin` indicates if the countdown process is running or not.
 * An ESP32 should run a server to receive the cmd that the wireless interface is sending.
 * ESP32 receives the cmd and plays a massage in a Google device in your home.
 * 
 * References: 
 *    - https://randomnerdtutorials.com/esp32-bluetooth-classic-arduino-ide/
 *    - https://techtutorialsx.com/2018/03/09/esp32-arduino-getting-the-bluetooth-device-address/
 *    - https://learn.sparkfun.com/tutorials/using-the-bluesmirf/introduction
 *    - https://cdn.sparkfun.com/assets/1/e/e/5/d/5217b297757b7fd3748b4567.pdf
 * 
 */
 
#include <SoftwareSerial.h>  

const int TX = 13;  // TX-O pin of Serial device
const int RX = 12;  // RX-I pin of Serial device

const int LEDPin = 10;
const int buttonPin = 11;
const int batteryPin = A1;
const int loopDelay = 1000;
const int messageDelay = 5 * 1000;
const int notifyAfter = 60; // in seconds

const byte lowBatteryCMD = 0xB0;
const byte closeDoorCMD = 0xB1;

SoftwareSerial SDevice(TX, RX);

int LEDState;
int buttonNew;
int buttonOld;
int count;
bool allowToNotifyBattery;
bool connected;

void setup() {
  Serial.begin(115200);
  SDevice.begin(115200);

  pinMode(LEDPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  digitalWrite(LEDPin, HIGH);
  LEDState = 1;
  buttonOld = digitalRead(buttonPin);
  count = 0;
  allowToNotifyBattery = true;
  connected = true;

  SDevice.print("$$$");
  Serial.println("Starting...");
}

void loop() {
  readSerial();
  checkBattery();
  checkTimer();
  delay(loopDelay);
}

void readSerial() {
  if (SDevice.available()) {
    Serial.print("Received: ");
    int n = SDevice.available();
    if (n == 1) {
      byte response = (byte)SDevice.read();
      if (response == lowBatteryCMD)
        allowToNotifyBattery = false;
      Serial.println(int(response));
    } else if (n > 1) {
      String str = "";
      while (SDevice.available() > 0) {
        char c = (char)SDevice.read();
        if (int(c) >= 32)
          str += c;
      }
      Serial.println(str);
      if (str == "CMD") {
        Serial.println("Connect...");
        SDevice.println("C,4C11AEEB9D86");
        connected = true;
        delay(2000);
      }
    }
  }
}

void checkBattery() {
  int battery = analogRead(batteryPin);
  Serial.print("Battery level: ");
  Serial.println(battery);
  double minLevel = 1024 * 2.8 / 3.3;
  if ((battery < int(minLevel)) && connected && allowToNotifyBattery) {
    Serial.println("Notify: low battery");
    SDevice.write(lowBatteryCMD);
    delay(messageDelay);
  }
}

void checkTimer() {
  buttonNew = digitalRead(buttonPin);
  if (buttonOld == 0 && buttonNew == 1) {
    if (LEDState == 0) {
      digitalWrite(LEDPin, HIGH);
      LEDState = 1;
      Serial.println("ON");
    } else {
      digitalWrite(LEDPin, LOW);
      LEDState = 0;
      Serial.println("OFF");
    }
  }
  buttonOld = buttonNew;
  
  int maxCount = notifyAfter * (1000 / loopDelay);

  if (LEDState == 1) {
    count++;
    Serial.print(count);
    Serial.print("/");
    Serial.print(maxCount);
    Serial.println();
  } else {
    count = 0;
  }
  
  if (count >= maxCount && connected) {
    Serial.println("Notify: close door");
    digitalWrite(LEDPin, LOW);
    delay(250);
    digitalWrite(LEDPin, HIGH);
    SDevice.write(closeDoorCMD);
    count = 0;
    delay(messageDelay);
  }
}
