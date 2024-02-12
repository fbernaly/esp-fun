#ifndef _CLOUDSPEECHCLIENT_H
#define _CLOUDSPEECHCLIENT_H

#include <WiFiClientSecure.h>
#include "MicController.h"

class CloudSpeechClient {
  String apiKey;
  int ledPin = -1;
  WiFiClientSecure client;

  void Send(String str);

public:
  bool connected = false;

  CloudSpeechClient(String apiKey, int ledPin);
  ~CloudSpeechClient();
  void Transcribe(MicController* mic);
};

#endif  // _CLOUDSPEECHCLIENT_H
