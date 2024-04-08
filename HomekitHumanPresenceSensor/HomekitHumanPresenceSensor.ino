#include "HomeSpan.h"
#include "HumanPresenceRadarSensor.h"

#define GPIO_RX_PIN 4  // Connect this GPIO pin to TX in the LD2410 module
#define GPIO_TX_PIN 3  // Connect this GPIO pin to RX in the LD2410 module

void setup(void) {
  Serial.begin(115200);

  homeSpan.setLogLevel(1);                                      // Sets Log Level to 1, which causes LOG1() messages to be output
  homeSpan.enableWebLog(100, "pool.ntp.org", "UTC", "sensor");  // creates a web log on the URL /HomeSpan-[DEVICE-ID].local:[TCP-PORT]/sensor
  homeSpan.setPairingCode("11122333");
  homeSpan.begin(Category::Sensors, "esp32-sensors");

  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Identify();
  new Characteristic::Name("Human Presence Sensor");   // Name of the Accessory (arbitrary text string, and can be the same for every Accessory)
  new Characteristic::Manufacturer("FBY");             // Manufacturer of the Accessory (arbitrary text string, and can be the same for every Accessory)
  new Characteristic::SerialNumber("test");            // Serial Number of the Accessory (arbitrary text string, and can be the same for every Accessory)
  new Characteristic::Model("Human Presence Sensor");  // Model of the Accessory (arbitrary text string, and can be the same for every Accessory)
  new Characteristic::FirmwareRevision("1");           // Firmware of the Accessory (arbitrary text string, and can be the same for every Accessory)
  new HumanPresenceRadarSensor(GPIO_RX_PIN, GPIO_TX_PIN);
}

void loop() {
  homeSpan.poll();  // run HomeSpan!
}
