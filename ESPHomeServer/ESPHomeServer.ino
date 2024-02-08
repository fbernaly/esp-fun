/*
 * ESPHomeServer
 * 
 * ESP32 runs a multiple servers: 
 * - a server that accepts GET requests for sending a message
 *   to a Google device in your home.
 * - a TCP server that accept commands to send a message
 *   to a Google device in your home.
 *   
 * References: 
 *    - https://techtutorialsx.com/2017/04/24/esp32-connecting-to-a-wifi-network/
 *    - https://techtutorialsx.com/2020/04/17/esp32-mdns-address-resolution/
 *    - https://github.com/horihiro/esp8266-google-home-notifier
 *    - https://www.megunolink.com/articles/wireless/talk-esp32-over-wifi/
 * 
 */
 
#include <WiFi.h>
#include <ESPmDNS.h>
#include <esp8266-google-home-notifier.h>

const char* ssid = "ssid";
const char* password = "password";
const char* hostname = "hostname";

const byte lowBatteryCMD_B = 0xB0;
const byte closeDoorCMD_B = 0xB1;
const char speaker_B[]  = "Hallway speaker";
const char lowBatteryMessage_B[] = "The upstairs bathroom door sensor is running out of battery. Let's charge it.";
const char closeDoorMessage_B[] = "Could someone close the upstairs bathroom door, please?";

GoogleHomeNotifier ghn;

const uint ServerPort = 6000;
WiFiServer Server(ServerPort);
WiFiClient RemoteClient;
bool notifyBattery;

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
  Serial.println("Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Hostname: ");
  Serial.println(WiFi.getHostname());

  Server.begin();
  
  Serial.println("Ready...");
}

void loop() {
  checkForConnections();
}

void checkForConnections() {
  if (Server.hasClient()) {
    RemoteClient = Server.available();
    Serial.println("New connection...");
    notifyBattery = true;
  }

  if (RemoteClient.available()) {
    byte c = (byte)RemoteClient.read();
    Serial.print("Received: ");
    Serial.println(int(c));

    if (c == lowBatteryCMD_B && notifyBattery == true) {
      notifyBattery = false;
      RemoteClient.write(c);
      sendGoogleMessage(speaker_B, lowBatteryMessage_B);
    } else if (c == closeDoorCMD_B) {
      sendGoogleMessage(speaker_B, closeDoorMessage_B);
    }
  }
}

void sendGoogleMessage(const char displayName[], const char message[]) {
  Serial.println("Connecting to Google Home...");
  if (ghn.device(displayName, "en") != true) {
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
