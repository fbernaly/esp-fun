#ifndef _CHATGPTCLIENT_H
#define _CHATGPTCLIENT_H

#include <Arduino.h>

class ChatGPTClient {
  String apiKey;

public:
  ChatGPTClient(String apiKey);
  ~ChatGPTClient();
  String Chat(String message);
};

#endif  // _CHATGPTCLIENT_H
