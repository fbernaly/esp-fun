/*
 * GoogleHomeNotifierServer
 * 
 * ESP32 runs a server that accepts GET requests for sending a 
 * message to a Google device in your home.
 * 
 * References: 
 *    - https://techtutorialsx.com/2020/04/17/esp32-mdns-address-resolution/
 *    - https://techtutorialsx.com/2017/04/24/esp32-connecting-to-a-wifi-network/
 *    - https://techtutorialsx.com/2017/12/01/esp32-arduino-asynchronous-http-webserver/
 *    - https://techtutorialsx.com/2017/12/09/esp32-arduino-asynchronous-http-webserver-simple-html/
 *    - https://techtutorialsx.com/2017/12/16/esp32-arduino-async-http-server-serving-a-html-page-from-flash-memory/
 *    - https://techtutorialsx.com/2017/12/17/esp32-arduino-http-server-getting-query-parameters/
 *    - https://techtutorialsx.com/2018/08/05/esp32-arduino-spiffs-writing-a-file/
 *    
 *    - https://www.dfrobot.com/blog-827.html
 *    - https://www.dfrobot.com/blog-1114.html
 *    - https://www.dfrobot.com/blog-1115.html
 *    - https://www.dfrobot.com/blog-1152.html
 *    - https://www.dfrobot.com/blog-1153.html
 *    
 *    - https://github.com/horihiro/esp8266-google-home-notifier
 *    - https://github.com/me-no-dev/ESPAsyncWebServer/
 *    - https://github.com/me-no-dev/AsyncTCP
 *    - https://github.com/me-no-dev/arduino-esp32fs-plugin
 * 
 */
 
#include <WiFi.h>
#include <ESPmDNS.h>
#include <esp8266-google-home-notifier.h>
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

const char* ssid = "ssid";
const char* password = "password";
const char* hostname = "hostname";

AsyncWebServer server(80);
GoogleHomeNotifier ghn;

typedef struct Message {
  String message;
  String device;
  String language;
};

#define maxSize 5
int i = 0; // counts messages enqued
int j = -1; // counts messages sent
Message messages[maxSize];
int c = 0;

void setup(){
  Serial.begin(115200);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  } else {
    Serial.println("SPIFFS mounted.");
  }

  // Connect to WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setHostname(hostname);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  // Set hostname
  while (!MDNS.begin(hostname)) {
    Serial.println("Error starting mDNS");
    delay(250);
  }
  
  Serial.println();
  Serial.println("connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Hostname: ");
  Serial.println(WiFi.getHostname());

  // Start server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/notify", HTTP_GET, handleNotifyRequest);

  server.begin();

  Serial.println();
  Serial.println("Server is ready :)");
  Serial.println();
}

void loop(){
  dequeueMessage();
  delay(100);
  c++;
  if (c > 60 * 10) {
    c = 0;
    ESP.restart();
  }
}

void handleNotifyRequest(AsyncWebServerRequest *request) {
  if (!request->hasParam("language")) {
    request->send(200, "text/plain", "Request is missing language parameter :(");
    return;
  }

  if (!request->hasParam("device")) {
    request->send(200, "text/plain", "Request is missing device parameter :(");
    return;
  }

  if (!request->hasParam("message")) {
    request->send(200, "text/plain", "Request is missing message parameter :(");
    return;
  }

  if (i > (maxSize - 1)) {
    request->send(200, "text/plain", "Buffer is full... try later :(");
    return;
  }

  AsyncWebParameter* pLanguage = request->getParam("language");
  AsyncWebParameter* pDevice = request->getParam("device");
  AsyncWebParameter* pMessage = request->getParam("message");

  String message = pMessage->value();
  String device = pDevice->value();
  String language = pLanguage->value();

  bool notify = enqueueMessage(message, device, language);

  if (notify == true && j < 0) {
    j = 0;      
  }
  
  request->send(200, "text/plain", "Message added to the queue, it will play soon :)");
}

bool enqueueMessage(String message, String device, String language) {
  if (i > (maxSize - 1)) {
    Serial.print("Cannot enqueue message, buffer size (");
    Serial.print(i + 1);
    Serial.print("/");
    Serial.print(maxSize);
    Serial.println(")");
    return false;
  }

  Serial.println();
  Serial.print("Enqueuing message (");
  Serial.print(i + 1);
  Serial.print("/");
  Serial.print(maxSize);
  Serial.print(") '");
  Serial.print(message);
  Serial.print("' to ");
  Serial.println(device);

  Message m;
  m.message = message;
  m.device = device;
  m.language = language;

  messages[i] = m;
  
  i++;

  return true;
}

void dequeueMessage() {
  if (j < 0) return;
  if (j > (maxSize - 1)) return;

  Serial.print("Dequeuing message ");
  Serial.println(j + 1);
  
  Message m = messages[j];

  sendGoogleMessage(m.message.c_str(),
                    m.device.c_str(),
                    m.language.c_str());

  j++;

  if (j == i) {
    Serial.println("Reset counters");
    i = 0;
    j = -1;
    c = 0;
  }
}

void sendGoogleMessage(const char message[], const char displayName[], const char language[]) {
  Serial.println("Connecting to Google Home...");
  if (ghn.device(displayName, language) != true) {
    Serial.print("Error: ");
    Serial.println(ghn.getLastError());
    return;
  }
  Serial.print("Found Google Home(");
  Serial.print(ghn.getIPAddress());
  Serial.print(":");
  Serial.print(ghn.getPort());
  Serial.println(")");
  
  Serial.print("Sending '");
  Serial.print(message);
  Serial.print("' to ");
  Serial.println(displayName);
  
  if (ghn.notify(message) != true) {
    Serial.print("Error: ");
    Serial.println(ghn.getLastError());
  } else {
    Serial.println("Message sent!!!");
  }
}
