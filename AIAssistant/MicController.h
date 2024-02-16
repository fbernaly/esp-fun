#ifndef _MICCONTROLLER_H
#define _MICCONTROLLER_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "driver/i2s.h"

#define STT_BAD_RESPONSE "..."

class MicController {
  char* wavHeader;
  String apiKey;
  WiFiClientSecure client;
  int ledPin = -1;

  void ValidateBufferSize();
  void ValidateWavEncodedSize();
  void OpenSocket();
  void CreateWavHeader();
  void ConfigureI2s(int pinI2sSck, int pinI2sWs, int pinI2sSd);
  void SendHeader();
  void SendWavChunk(char* wavChunk);
  void SendEnd();
  void Send(String str);
  bool WaitForClientResponse();
  String ParseResponse();

public:
  MicController(int pinI2sSck, int pinI2sWs, int pinI2sSd, int ledPin, String apiKey);
  ~MicController();
  String Transcribe(int duration);
};

#endif  // _MICCONTROLLER_H
