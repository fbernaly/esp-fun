#include "MicController.h"
#include "CloudSpeechClient.h"

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

MicController* mic;
CloudSpeechClient* cloudSpeechClient;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_PUSH_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  connectToWifi();

  mic = new MicController(I2S_MIC_SCK, I2S_MIC_WS, I2S_MIC_SD);
  cloudSpeechClient = new CloudSpeechClient(googleApiKey, PIN_LED);
}

void loop() {
  // read the state of the switch/button:
  currentState = digitalRead(PIN_PUSH_BUTTON);

  if (lastState == HIGH && currentState == LOW) {
    digitalWrite(PIN_LED, HIGH);
    mic->Record();
    digitalWrite(PIN_LED, LOW);
    if (cloudSpeechClient->connected) cloudSpeechClient->Transcribe(mic);
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
