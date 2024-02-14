#include "MicController.h"

#define PIN_LED 27
#define PIN_PUSH_BUTTON 14

#define I2S_MIC_SCK 32
#define I2S_MIC_WS 25
#define I2S_MIC_SD 33

const char* ssid = "<YOUR_SSID>";
const char* password = "<YOUR_PASSWORD>";
const char* googleApiKey = "<YOUR_API_KEY>";

int lastState = HIGH;  // the previous state from the input pin
int currentState;      // the current reading from the input pin

MicController* controller;

void setup() {
  Serial.begin(115200);

  // set I/O pins
  pinMode(PIN_PUSH_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  connectToWifi();

  // instantiate MicController object, by default it uses port I2S_NUM_0
  controller = new MicController(I2S_MIC_SCK, I2S_MIC_WS, I2S_MIC_SD, PIN_LED, googleApiKey);
}

void loop() {
  // read the state of the switch/button:
  currentState = digitalRead(PIN_PUSH_BUTTON);

  // transcribe when pushbutton is initial pusshed
  if (lastState == HIGH && currentState == LOW) {
    int duration = 5;  // Duration in seconds
    String stt = controller->Transcribe(duration);
    Serial.print("speech-to-text: ");
    Serial.println(stt);

    if (stt == STT_BAD_RESPONSE) {
      Serial.println("Repeat that again, please...");
    }
  }

  // save the last state
  lastState = currentState;

  delay(100);
}

void connectToWifi() {
  WiFi.begin(ssid, password);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  Serial.print("Connecting to WiFi");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(PIN_LED, (i++) % 2);
    delay(50);
  }
  digitalWrite(PIN_LED, LOW);
  Serial.print("\nConnected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}
