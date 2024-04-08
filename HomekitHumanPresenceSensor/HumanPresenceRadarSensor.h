#include <ld2410.h>

struct HumanPresenceRadarSensor : Service::OccupancySensor {
  SpanCharacteristic *presence;
  ld2410 radar;
  int presenceDetected = 0;
  int stationaryTargetDistance = 0;
  int stationaryTargetEnergy = 0;
  int movingTargetDistance = 0;
  int movingTargetEnergy = 0;

  HumanPresenceRadarSensor(int gpioRxPin, int gpioTxPin)
    : Service::OccupancySensor() {
    // radar.debug(Serial); //Uncomment to show debug information from the library on the Serial Monitor. By default this does not show sensor reads as they are very frequent.

    Serial1.begin(256000, SERIAL_8N1, gpioRxPin, gpioTxPin);  //UART for monitoring the radar
    delay(500);
    Serial.print("\nConnect LD2410 radar TX to GPIO:");
    Serial.println(gpioRxPin);
    Serial.print("Connect LD2410 radar RX to GPIO:");
    Serial.println(gpioTxPin);
    Serial.print("LD2410 radar sensor initialising: ");
    if (radar.begin(Serial1)) {
      Serial.println("OK");
      Serial.print("LD2410 firmware version: ");
      Serial.print(radar.firmware_major_version);
      Serial.print('.');
      Serial.print(radar.firmware_minor_version);
      Serial.print('.');
      Serial.println(radar.firmware_bugfix_version, HEX);
    } else {
      Serial.println("not connected");
    }

    presence = new Characteristic::OccupancyDetected(0);

    WEBLOG("Sensor is configured!");
  }

  void loop() {
    if (presence->timeVal() > 100) {  // check time elapsed since last update and proceed only if greater than 100 milliseconds
      radar.read();
      if (radar.isConnected()) {
        if (radar.presenceDetected()) {
          if (presenceDetected == 0) {
            presenceDetected = 1;
            WEBLOG("Presence detected");
          }
          if (radar.stationaryTargetDetected()) {
            int _stationaryTargetDistance = radar.stationaryTargetDistance();
            int _stationaryTargetEnergy = radar.stationaryTargetEnergy();
            if (stationaryTargetDistance != _stationaryTargetDistance || stationaryTargetEnergy != _stationaryTargetEnergy) {
              stationaryTargetDistance = _stationaryTargetDistance;
              stationaryTargetEnergy = _stationaryTargetEnergy;
              String message = String("Stationary target: ") + stationaryTargetDistance + String("cm, energy: ") + stationaryTargetEnergy;
              WEBLOG("%s", message.c_str());
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
              WEBLOG("%s", message.c_str());
            }
          } else {
            movingTargetDistance = 0;
            movingTargetEnergy = 0;
          }
        } else {
          if (presenceDetected) {
            WEBLOG("No presence detected");
            presenceDetected = 0;
            stationaryTargetDistance = 0;
            stationaryTargetEnergy = 0;
            movingTargetDistance = 0;
            movingTargetEnergy = 0;
          }
        }
        presence->setVal(presenceDetected);
      }
    }
  }
};
