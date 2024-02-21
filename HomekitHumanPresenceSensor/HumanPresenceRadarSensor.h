#include <ld2410.h>

struct HumanPresenceRadarSensor : Service::OccupancySensor {
  SpanCharacteristic *presence;
  ld2410 radar;

  HumanPresenceRadarSensor(int gpioRxPin, int gpioTxPin)
    : Service::OccupancySensor() {
    // radar.debug(Serial); //Uncomment to show debug information from the library on the Serial Monitor. By default this does not show sensor reads as they are very frequent.

    Serial1.begin(256000, SERIAL_8N1, gpioRxPin, gpioTxPin);  //UART for monitoring the radar
    delay(500);
    Serial.print(F("\nConnect LD2410 radar TX to GPIO:"));
    Serial.println(gpioRxPin);
    Serial.print(F("Connect LD2410 radar RX to GPIO:"));
    Serial.println(gpioTxPin);
    Serial.print(F("LD2410 radar sensor initialising: "));
    if (radar.begin(Serial1)) {
      Serial.println(F("OK"));
      Serial.print(F("LD2410 firmware version: "));
      Serial.print(radar.firmware_major_version);
      Serial.print('.');
      Serial.print(radar.firmware_minor_version);
      Serial.print('.');
      Serial.println(radar.firmware_bugfix_version, HEX);
    } else {
      Serial.println(F("not connected"));
    }

    presence = new Characteristic::OccupancyDetected(0);

    LOG1("Configuring Sensor");
    LOG1("\n");
  }

  void loop() {
    if (presence->timeVal() > 100) {  // check time elapsed since last update and proceed only if greater than 100 milliseconds
      radar.read();
      if (radar.isConnected()) {
        if (radar.presenceDetected()) {
          if (radar.stationaryTargetDetected()) {
            Serial.print(F("Stationary target: "));
            Serial.print(radar.stationaryTargetDistance());
            Serial.print(F("cm energy:"));
            Serial.print(radar.stationaryTargetEnergy());
            Serial.print(' ');
          }
          if (radar.movingTargetDetected()) {
            Serial.print(F("Moving target: "));
            Serial.print(radar.movingTargetDistance());
            Serial.print(F("cm energy:"));
            Serial.print(radar.movingTargetEnergy());
          }
          Serial.println();
          presence->setVal(1);
        } else {
          Serial.println(F("No target"));
          presence->setVal(0);
        }
      }
    }
  }
};
