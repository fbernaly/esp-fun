#include "ChatGPTClient.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>

const char* gpt_server = "https://api.openai.com/v1/chat/completions";

ChatGPTClient::ChatGPTClient(String apiKey) {
  this->apiKey = apiKey;
}

ChatGPTClient::~ChatGPTClient() {
}

String ChatGPTClient::Chat(String message) {
  // Send request to OpenAI API
  String payload = "{\"model\": \"gpt-3.5-turbo\", \"max_tokens\" : 30, \"messages\": [{\"role\": \"system\", \"content\": \"You are a helpful assistant, that provide short ansers with no more than 200 charactes.\"}, {\"role\": \"user\",\"content\": \"" + message + "\"}]}";

  HTTPClient http;
  http.begin(gpt_server);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + apiKey);

  int httpResponseCode = http.POST(payload);
  if (httpResponseCode == 200) {
    String json = http.getString();

    // Parse JSON response
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return "I didn't quite catch that, would you mind repeating it?";
    }

    String response = doc["choices"][0]["message"]["content"];
    return response;
  } else {
    Serial.printf("Bad response from server. Error %i\n", httpResponseCode);
    return " I apologize, could you please repeat that?";
  }
}
