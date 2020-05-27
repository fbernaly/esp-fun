/*
 * GoogleHomeNotifierServer
 * 
 * ESP32 runs a server that accepts GET requests for sending a 
 * message to a Google device in your home.
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

const char* ssid = "ssid";
const char* password = "password";
const char* hostname = "hostname";
#define N 3
String devices[N] = { "Kitchen Display", "Hallway speaker", "Bathroom speaker" };

WiFiServer server(80);
GoogleHomeNotifier ghn;

void setup(){
  Serial.begin(115200);
  Serial.println("");
  Serial.print("connecting to ");
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

  server.begin();
}

int value = 0;

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println();
    Serial.println("New Client.");           // print a message out the serial port
    bool busy = false;
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<p>Send a message to your Google Home device.</p>");
            client.print("<form action=\"/get\">");
            client.print("<label for=\"device\">Device:</label>");
            client.print("<select id=\"device\" name=\"device\">");
            for (int i =0; i< N; i++) {
              client.print("<option value=\"");
              client.print(devices[i]);
              client.print("\">");
              client.print(devices[i]);
              client.print("</option>");
            }
            client.print("</select>");
            client.print("<br>");
            client.print("<label for=\"message\">Message:</label>");
            client.print("<input type=\"text\" id=\"message\" name=\"message\">");
            client.print("<input type=\"submit\" value=\"Send\">");
            client.print("</form>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        
        if (currentLine.startsWith("GET /get?device=") && currentLine.endsWith(" HTTP/1.1") && !busy) {
          busy = true;
          const char separator[] = "*";
          String payload = currentLine;
          payload.replace("GET /get?device=", "");
          payload.replace(" HTTP/1.1", "");
          payload.replace("+", " ");
          payload.replace("%21", "!");
          payload.replace("%2C", ",");
          payload.replace("%3F", "?");
          payload.replace("&message=", separator);

          char str[payload.length() + 1];
          payload.toCharArray(str, payload.length() + 1);

          char *deviceName;
          deviceName = strtok(str, separator);

          char *message;
          message = strtok(NULL, separator);

          Serial.println();
          Serial.print("device: ");
          Serial.println(deviceName);
          Serial.print("message: ");
          Serial.println(message);
          
          sendGoogleMessage(deviceName, message);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
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
    Serial.print("Message sent!!!");
  }
}
