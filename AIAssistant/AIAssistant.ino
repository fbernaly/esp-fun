#include "STTController.h"
#include "ChatGPTClient.h"
#include "Audio.h"

#define PIN_LED 27
#define PIN_PUSH_BUTTON 14

#define I2S_MIC_SCK 32
#define I2S_MIC_WS 25
#define I2S_MIC_SD 33

#define I2S_DIN 17
#define I2S_BCLK 18
#define I2S_LRC 19

const char* ssid = "<YOUR_SSID>";
const char* password = "<YOUR_PASSWORD>";
const char* googleApiKey = "<YOUR_API_KEY>";
const char* chatGPTApiKey = "<YOUR_API_KEY>";

int lastState = HIGH;  // the previous state from the input pin
int currentState;      // the current reading from the input pin

STTController* controller;
ChatGPTClient* chatGPTClient;
Audio* audio;

void setup() {
  Serial.begin(115200);

  // set I/O pins
  pinMode(PIN_PUSH_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  connectToWifi();

  // instantiate STTController object, by default it uses port I2S_NUM_0
  controller = new STTController(I2S_MIC_SCK, I2S_MIC_WS, I2S_MIC_SD, PIN_LED, googleApiKey);

  // instantiate ChatGPTClient object
  chatGPTClient = new ChatGPTClient(chatGPTApiKey);

  // instantiate Audio object
  // since STTController uses port I2S_NUM_0, set port I2S_NUM_1 for Audio
  audio = new Audio(false, 3, I2S_NUM_1);
  audio->setPinout(I2S_BCLK, I2S_LRC, I2S_DIN);
  audio->setVolume(60);

  // initial greeting
  String message = "AI Assistant is ready!";
  Serial.println(message);
  audio->connecttospeech(message.c_str(), "en");
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
      String message = "I apologize, I missed that. Could you repeat it, please?";
      audio->connecttospeech(message.c_str(), "en");
    } else {
      String response = chatGPTClient->Chat(stt);
      Serial.print("chat-gpt: ");
      Serial.println(response);
      audio->connecttospeech(response.c_str(), "en");
    }
  }

  // save the last state
  lastState = currentState;

  // play audio
  audio->loop();
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
