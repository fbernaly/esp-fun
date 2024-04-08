
#include <Arduino_JSON.h>
#include <ESPAsyncWebServer.h>
#include <ld2410.h>
#include <WiFi.h>

#define GPIO_RX_PIN 4  // Connect this GPIO pin to TX in the LD2410 module
#define GPIO_TX_PIN 3  // Connect this GPIO pin to RX in the LD2410 module

// Replace with your network credentials
const char *ssid = "ssid";
const char *password = "password";

ld2410 radar;
int presenceDetected = 0;
int stationaryTargetDistance = 0;
int stationaryTargetEnergy = 0;
int movingTargetDistance = 0;
int movingTargetEnergy = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script src="https://code.highcharts.com/highcharts.js"></script>
  <link rel="icon" href="data:,">
  <style>
    html {
      font-family: Arial; 
      display: inline-block; 
      text-align: center;
    }
    p { 
      font-size: 1.2rem;
    }
    body {
      min-width: 310px;
    	max-width: 800px;
    	height: 400px;
      margin: 0 auto;
    }
    h2 {
      font-family: Arial;
      font-size: 3rem;
      text-align: center;
    }
    h3 {
      font-family: Arial;
      font-size: 2rem;
      text-align: center;
    }
    .content { 
      padding: 20px; 
    }
    .card { 
      background-color: white; 
      box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); 
    }
    .cards { 
      max-width: 800px; 
      margin: 0 auto; 
      display: grid; 
      grid-gap: 2rem; 
      grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { 
      font-size: 1.4rem;  
    }
  </style>
</head>
<body>
  <h2>ESP32 & LD2410</h2>
  <h3>Human Presence Body Induction Distance Detection Radar Sensor</h2>
  <div class="content">
    <div class="cards">
      <div class="card">
        <p>PRESENCE DETECTED</p>
        <p><span class="reading"><span id="pd">--</p>
      </div>
      <div class="card">
        <p>STATIONARY</p>
        <p><span class="reading"><span id="std">--</p>
      </div>
      <div class="card">
        <p>MOVING</p>
        <p><span class="reading"><span id="movd">--</p>
      </div>
    </div>
  </div>
  <div id="chart-distance" class="container"></div>
  <div id="chart-energy" class="container"></div>  
</body>
<script>
var chartDistance = new Highcharts.Chart({
  chart:{
    renderTo : 'chart-distance'
  },
  series: [
    {
      name: 'Stationary',
      color: '#059E8A'
    },
    {
      name: 'Moving',
      color: '#101D42'
    }
  ],
  title: { 
    text: 'Distance to target'
  },
  xAxis: {
    visible: false,
    type: 'datetime',
    showFirstLabel: false,
  },
  yAxis: {
    title: {
      text: 'Distance (cm)'
    },
    min: 0,
    max: 500
  },
  credits: { 
    enabled: false 
  }
});

var chartEnergy = new Highcharts.Chart({
  chart:{
    renderTo : 'chart-energy'
  },
  series: [
    {
      name: 'Stationary',
      color: '#059E8A'
    },
    {
      name: 'Moving',
      color: '#101D42'
    }
  ],
  title: { 
    text: 'Energy to target'
  },
  xAxis: {
    visible: false,
    type: 'datetime',
    showFirstLabel: false,
  },
  yAxis: {
    title: {
      text: 'Energy (%)'
    },
    min: 0,
    max: 110
  },
  credits: { 
    enabled: false 
  }
});

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var jsonValue = JSON.parse(this.responseText);

      var x = (new Date()).getTime();
      var ysd = Number(jsonValue["stationaryTargetDistance"]);
      var yse = Number(jsonValue["stationaryTargetEnergy"]);
      var ymd = Number(jsonValue["movingTargetDistance"]);
      var yme = Number(jsonValue["movingTargetEnergy"]);

      if(chartDistance.series[0].data.length > 40) {
        chartDistance.series[0].addPoint([x, ysd], true, true, true);
      } else {
        chartDistance.series[0].addPoint([x, ysd], true, false, true);
      }

      if(chartDistance.series[1].data.length > 40) {
        chartDistance.series[1].addPoint([x, ymd], true, true, true);
      } else {
        chartDistance.series[1].addPoint([x, ymd], true, false, true);
      }

      if(chartEnergy.series[0].data.length > 40) {
        chartEnergy.series[0].addPoint([x, yse], true, true, true);
      } else {
        chartEnergy.series[0].addPoint([x, yse], true, false, true);
      }

      if(chartEnergy.series[1].data.length > 40) {
        chartEnergy.series[1].addPoint([x, yme], true, true, true);
      } else {
        chartEnergy.series[1].addPoint([x, yme], true, false, true);
      }

      var presenceDetected = Number(jsonValue["presenceDetected"]) == 1 ? 'YES' : 'NO';
      document.getElementById("pd").innerHTML = Number(jsonValue["presenceDetected"]) == 1 ? 'YES' : 'NO';
      document.getElementById("std").innerHTML = ysd == 0 && yse == 0 ? 'NO' : 'YES';
      document.getElementById("movd").innerHTML = ymd == 0 && yme == 0 ? 'NO' : 'YES';
    }
  };
  xhttp.open("GET", "/readings", true);
  xhttp.send();
}, 50 ) ;
</script>
</html>
)rawliteral";

void connectToWifi() {
  WiFi.begin(ssid, password);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  Serial.print("Connecting to WiFi");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.print("\nConnected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);
}

String getJsonReadingData() {
  // Json Variable to Hold Sensor Readings
  JSONVar readings;
  readings["presenceDetected"] = String(presenceDetected);
  readings["stationaryTargetDistance"] = String(stationaryTargetDistance);
  readings["stationaryTargetEnergy"] = String(stationaryTargetEnergy);
  readings["movingTargetDistance"] = String(movingTargetDistance);
  readings["movingTargetEnergy"] = String(movingTargetEnergy);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}

void getSensorReading() {
  radar.read();
  if (radar.isConnected()) {
    if (radar.presenceDetected()) {
      if (presenceDetected == 0) {
        presenceDetected = 1;
        Serial.println("Presence detected");
      }
      if (radar.stationaryTargetDetected()) {
        int _stationaryTargetDistance = radar.stationaryTargetDistance();
        int _stationaryTargetEnergy = radar.stationaryTargetEnergy();
        if (stationaryTargetDistance != _stationaryTargetDistance || stationaryTargetEnergy != _stationaryTargetEnergy) {
          stationaryTargetDistance = _stationaryTargetDistance;
          stationaryTargetEnergy = _stationaryTargetEnergy;
          String message = String("Stationary target: ") + stationaryTargetDistance + String("cm, energy: ") + stationaryTargetEnergy;
          Serial.println(message);
        }
      } else {
        stationaryTargetDistance = 0;
        stationaryTargetEnergy = 0;
      }
      if (radar.movingTargetDetected()) {
        int _movingTargetDistance = radar.movingTargetDistance();
        int _movingTargetEnergy = radar.movingTargetEnergy();
        if (movingTargetDistance != _movingTargetDistance || movingTargetEnergy != _movingTargetEnergy) {
          movingTargetDistance = _movingTargetDistance;
          movingTargetEnergy = _movingTargetEnergy;
          String message = String("Moving target: ") + movingTargetDistance + String("cm, energy: ") + movingTargetEnergy;
          Serial.println(message);
        }
      } else {
        movingTargetDistance = 0;
        movingTargetEnergy = 0;
      }
    } else {
      if (presenceDetected) {
        Serial.println("No presence detected");
        presenceDetected = 0;
        stationaryTargetDistance = 0;
        stationaryTargetEnergy = 0;
        movingTargetDistance = 0;
        movingTargetEnergy = 0;
      }
    }
  }
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Connect to Wi-Fi
  connectToWifi();

  // Configure LD2410
  Serial1.begin(256000, SERIAL_8N1, GPIO_RX_PIN, GPIO_TX_PIN);
  if (radar.begin(Serial1)) {
    Serial.println("OK");
    Serial.print("LD2410 firmware version: ");
    Serial.print(radar.firmware_major_version);
    Serial.print('.');
    Serial.print(radar.firmware_minor_version);
    Serial.print('.');
    Serial.println(radar.firmware_bugfix_version, HEX);
  } else {
    Serial.println("LD2410 not connected");
  }

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  // Request for the latest sensor readings
  server.on("/readings", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", getJsonReadingData());
  });

  // Start server
  server.begin();

  Serial.println("Server started!");
}

void loop() {
  getSensorReading();
}
