#ifndef _CLOUDSPEECHCLIENT_H
#define _CLOUDSPEECHCLIENT_H

#include <WiFiClientSecure.h>
#include "MicController.h"

class CloudSpeechClient {
  String apiKey;
  WiFiClientSecure client;

  void Send(String str);

public:
  CloudSpeechClient(String apiKey);
  ~CloudSpeechClient();
  void Transcribe(MicController* mic);
};

#endif  // _CLOUDSPEECHCLIENT_H
