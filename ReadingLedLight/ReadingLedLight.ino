#include "HomeSpan.h"
#include "LedController.h"

int LED_PIN = 14;          // Pin of the LED
int PUSH_BUTTON_PIN = 33;  // Pin to the push button

void setup() {
  Serial.begin(115200);

  // Flash leds to signal start
  pinMode(LED_PIN, OUTPUT);
  for (int i = 0; i <= 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
  digitalWrite(LED_PIN, HIGH);

  homeSpan.setLogLevel(1);  // Sets Log Level to 1, which causes LOG1() messages to be output
  homeSpan.setPairingCode("11122333");
  homeSpan.begin(Category::Lighting, "esp32-test");

  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Identify();
  new Characteristic::Manufacturer("FBY");           // Manufacturer of the Accessory (arbitrary text string, and can be the same for every Accessory)
  new Characteristic::SerialNumber("test");          // Serial Number of the Accessory (arbitrary text string, and can be the same for every Accessory)
  new Characteristic::Model("Reading Light Strip");  // Model of the Accessory (arbitrary text string, and can be the same for every Accessory)
  new Characteristic::FirmwareRevision("1");         // Firmware of the Accessory (arbitrary text string, and can be the same for every Accessory)
  new LedController(LED_PIN, PUSH_BUTTON_PIN);
}

void loop() {
  homeSpan.poll();  // run HomeSpan!
}
