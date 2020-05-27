/*
 * GoogleHomeNotifier
 * 
 * ESP32 sends a message to a Google device in your house to reproduce a message.
 * 
 * References: 
 *    - https://techtutorialsx.com/2017/04/24/esp32-connecting-to-a-wifi-network/
 *    - https://github.com/horihiro/esp8266-google-home-notifier
 * 
 */
 
#include <WiFi.h>
#include <esp8266-google-home-notifier.h>

const char* ssid = "ssid";
const char* password = "password";
const char* hostname = "hostname";

const char displayName[] = "Hallway speaker";
const char message[] = "Could someone close the bathroom door, please?";

GoogleHomeNotifier ghn;

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.print("connecting to ");
  Serial.print(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

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
    Serial.println(ghn.getLastError());
    return;
  }
  Serial.println("Done.");
}

void loop() {
}
