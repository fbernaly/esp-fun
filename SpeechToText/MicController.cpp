#include "MicController.h"
#include <base64.h>
#include <ArduinoJson.h>

#define SAMPLE_RATE 16000
#define BUFFER_SIZE 3000                                                          // It should be divisible by 8.
#define WAV_HEADER_SIZE 48                                                        // The size must be multiple of 3 for Base64 encoding. Additional byte size must be even because wave data is 16bit.
#define WAV_SAMPLE_SIZE (BUFFER_SIZE / 4)                                         // DO NOT CHANGE. This is the size of the WAV chunck after each i2s_read.
#define WAV_DATA_SIZE (NUM_SAMPLES * WAV_SAMPLE_SIZE)                             // It must be multiple of WAV_SAMPLE_SIZE.
#define WAV_SIZE (WAV_DATA_SIZE + WAV_HEADER_SIZE)                                // Size of the resulting WAV file.
#define WAV_ENCODED_SIZE (WAV_SIZE * 4 / 3)                                       // It must be an integer. 4/3 is from base64 encoding https://developer.mozilla.org/en-US/docs/Glossary/Base64#encoded_size_increase
#define NUM_SAMPLES ((int)ceil((float)DURATION * SAMPLE_RATE * 8 / BUFFER_SIZE))  // Number of WAV samples/chunks recorded.

int DURATION = 5;  // This is an approximate duration.

const char* STT_SERVER = "speech.googleapis.com";

// To get the certificate for your region run:
// openssl s_client -showcerts -connect speech.googleapis.com:443
// Copy the certificate (all lines between and including ---BEGIN CERTIFICATE---
// and --END CERTIFICATE--) to root.cert and put here on the root_cert variable.
const char* CERTIFICATE =
  "-----BEGIN CERTIFICATE-----\n"
  "MIIFljCCA36gAwIBAgINAgO8U1lrNMcY9QFQZjANBgkqhkiG9w0BAQsFADBHMQsw\n"
  "CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n"
  "MBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMjAwODEzMDAwMDQyWhcNMjcwOTMwMDAw\n"
  "MDQyWjBGMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp\n"
  "Y2VzIExMQzETMBEGA1UEAxMKR1RTIENBIDFDMzCCASIwDQYJKoZIhvcNAQEBBQAD\n"
  "ggEPADCCAQoCggEBAPWI3+dijB43+DdCkH9sh9D7ZYIl/ejLa6T/belaI+KZ9hzp\n"
  "kgOZE3wJCor6QtZeViSqejOEH9Hpabu5dOxXTGZok3c3VVP+ORBNtzS7XyV3NzsX\n"
  "lOo85Z3VvMO0Q+sup0fvsEQRY9i0QYXdQTBIkxu/t/bgRQIh4JZCF8/ZK2VWNAcm\n"
  "BA2o/X3KLu/qSHw3TT8An4Pf73WELnlXXPxXbhqW//yMmqaZviXZf5YsBvcRKgKA\n"
  "gOtjGDxQSYflispfGStZloEAoPtR28p3CwvJlk/vcEnHXG0g/Zm0tOLKLnf9LdwL\n"
  "tmsTDIwZKxeWmLnwi/agJ7u2441Rj72ux5uxiZ0CAwEAAaOCAYAwggF8MA4GA1Ud\n"
  "DwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwEgYDVR0T\n"
  "AQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQUinR/r4XN7pXNPZzQ4kYU83E1HScwHwYD\n"
  "VR0jBBgwFoAU5K8rJnEaK0gnhS9SZizv8IkTcT4waAYIKwYBBQUHAQEEXDBaMCYG\n"
  "CCsGAQUFBzABhhpodHRwOi8vb2NzcC5wa2kuZ29vZy9ndHNyMTAwBggrBgEFBQcw\n"
  "AoYkaHR0cDovL3BraS5nb29nL3JlcG8vY2VydHMvZ3RzcjEuZGVyMDQGA1UdHwQt\n"
  "MCswKaAnoCWGI2h0dHA6Ly9jcmwucGtpLmdvb2cvZ3RzcjEvZ3RzcjEuY3JsMFcG\n"
  "A1UdIARQME4wOAYKKwYBBAHWeQIFAzAqMCgGCCsGAQUFBwIBFhxodHRwczovL3Br\n"
  "aS5nb29nL3JlcG9zaXRvcnkvMAgGBmeBDAECATAIBgZngQwBAgIwDQYJKoZIhvcN\n"
  "AQELBQADggIBAIl9rCBcDDy+mqhXlRu0rvqrpXJxtDaV/d9AEQNMwkYUuxQkq/BQ\n"
  "cSLbrcRuf8/xam/IgxvYzolfh2yHuKkMo5uhYpSTld9brmYZCwKWnvy15xBpPnrL\n"
  "RklfRuFBsdeYTWU0AIAaP0+fbH9JAIFTQaSSIYKCGvGjRFsqUBITTcFTNvNCCK9U\n"
  "+o53UxtkOCcXCb1YyRt8OS1b887U7ZfbFAO/CVMkH8IMBHmYJvJh8VNS/UKMG2Yr\n"
  "PxWhu//2m+OBmgEGcYk1KCTd4b3rGS3hSMs9WYNRtHTGnXzGsYZbr8w0xNPM1IER\n"
  "lQCh9BIiAfq0g3GvjLeMcySsN1PCAJA/Ef5c7TaUEDu9Ka7ixzpiO2xj2YC/WXGs\n"
  "Yye5TBeg2vZzFb8q3o/zpWwygTMD0IZRcZk0upONXbVRWPeyk+gB9lm+cZv9TSjO\n"
  "z23HFtz30dZGm6fKa+l3D/2gthsjgx0QGtkJAITgRNOidSOzNIb2ILCkXhAd4FJG\n"
  "AJ2xDx8hcFH1mt0G/FX0Kw4zd8NLQsLxdxP8c4CU6x+7Nz/OAipmsHMdMqUybDKw\n"
  "juDEI/9bfU1lcKwrmz3O2+BtjjKAvpafkmO8l7tdufThcV4q5O8DIrGKZTqPwJNl\n"
  "1IXNDw9bg1kWRxYtnCQ6yICmJhSFm/Y3m6xv+cXDBlHz4n/FsRC6UfTd\n"
  "-----END CERTIFICATE-----\n";

MicController::MicController(int pinI2sSck, int pinI2sWs, int pinI2sSd, int ledPin, String apiKey) {
  this->apiKey = apiKey;
  this->ledPin = ledPin;

  ValidateBufferSize();
  OpenSocket();
  ConfigureI2s(pinI2sSck, pinI2sWs, pinI2sSd);
}

MicController::~MicController() {
  client.stop();
  delete wavHeader;
}

void MicController::ValidateBufferSize() {
  float v = (float)(BUFFER_SIZE) / 4;
  if (v != WAV_SAMPLE_SIZE) {
    Serial.println("WAV_SAMPLE_SIZE should be an integer. Make sure that BUFFER_SIZE is divisible by 8.");
    abort();
  }
  if ((WAV_SAMPLE_SIZE % 2) == 1) {
    Serial.println("WAV_SAMPLE_SIZE should be even. Make sure that BUFFER_SIZE is divisible by 8.");
    abort();
  }
  Serial.println("BUFFER_SIZE is valid!");
}

void MicController::ValidateWavEncodedSize() {
  float v = (float)WAV_SIZE * 4 / 3;
  if (v != WAV_ENCODED_SIZE) {
    Serial.println("WAV_ENCODED_SIZE should be an integer. Make sure that WAV_SIZE can be multiplied by 4 and divided by 3.");
    abort();
  }
}

void MicController::OpenSocket() {
  client.setCACert(CERTIFICATE);
  client.setTimeout(5);
  if (client.connect(STT_SERVER, 443)) {
    Serial.println("Connected to Google server!");
  } else {
    Serial.println("Connection to Google server failed :(");
  }
}

void MicController::ConfigureI2s(int pinI2sSck, int pinI2sWs, int pinI2sSd) {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 128,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  i2s_pin_config_t pin_config = {
    .bck_io_num = pinI2sSck,
    .ws_io_num = pinI2sWs,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = pinI2sSd
  };
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
}

String MicController::Transcribe(int duration) {
  DURATION = duration;
  ValidateWavEncodedSize();
  CreateWavHeader();
  Serial.print("Recording ");
  Serial.print(NUM_SAMPLES);
  Serial.print(" samples, about ");
  Serial.print(DURATION);
  Serial.println(" seconds...");
  // turn ON LED when recording starts
  if (ledPin >= 0) digitalWrite(ledPin, HIGH);

  // send header to web socket
  SendHeader();

  // start recording data from mic
  char* buffer = new char[BUFFER_SIZE];
  char* wavChunck = new char[WAV_SAMPLE_SIZE];
  for (int j = 0; j < NUM_SAMPLES; ++j) {
    size_t bytesRead = 0;
    i2s_read(I2S_NUM_0, buffer, BUFFER_SIZE, &bytesRead, portMAX_DELAY);
    for (int i = 0; i < WAV_SAMPLE_SIZE / 2; ++i) {
      // saving buffer into wav chuck from one channel only
      wavChunck[2 * i] = buffer[8 * i + 2];
      wavChunck[2 * i + 1] = buffer[8 * i + 3];
    }
    // send wav chuck to web socket
    SendWavChunk(wavChunck);
  }
  delete buffer;
  delete wavChunck;

  // send end to web socket
  SendEnd();

  Serial.println("Recording Completed.");
  // turn OFF LED when recording ends
  if (ledPin >= 0) digitalWrite(ledPin, LOW);

  // wait for client response
  bool valid = WaitForClientResponse();
  if (!valid) {
    return STT_BAD_RESPONSE;
  }

  String stt = ParseResponse();
  return stt;
}

void MicController::SendHeader() {
  String httpBody1 = String("{\"config\":{\"encoding\":\"LINEAR16\",\"sampleRateHertz\":") + SAMPLE_RATE + String(",\"languageCode\":\"en-US\"},\"audio\":{\"content\":\"");
  String httpBody3 = "\"}}\r\n\r\n";
  String contentLength = String(httpBody1.length() + WAV_ENCODED_SIZE + httpBody3.length());

  String httpHeader1 = String("POST /v1/speech:recognize?key=") + apiKey;
  String httpHeader2 = String(" HTTP/1.1\r\nHost: ") + STT_SERVER + String("\r\nContent-Type: application/json\r\nContent-Length: ") + contentLength + String("\r\n\r\n");

  Send(httpHeader1);
  Send(httpHeader2);
  Send(httpBody1);
  String httpBody2 = base64::encode((byte*)wavHeader, WAV_HEADER_SIZE);
  httpBody2.replace("\n", "");  // delete last "\n"
  Send(httpBody2);              // httpBody2
}

void MicController::SendWavChunk(char* wavChunk) {
  String httpBody2 = base64::encode((byte*)wavChunk, WAV_SAMPLE_SIZE);
  httpBody2.replace("\n", "");  // delete last "\n"
  Send(httpBody2);              // httpBody2
}

void MicController::SendEnd() {
  String httpBody3 = "\"}}\r\n\r\n";
  Send(httpBody3);
}

void MicController::Send(String str) {
  client.print(str);
  // Serial.print(str);
}

bool MicController::WaitForClientResponse() {
  Serial.print("Waiting for client response...");
  int j = 0;
  while (!client.available()) {
    Serial.print(".");
    if (ledPin >= 0) digitalWrite(ledPin, (j++) % 2);
    delay(50);
    if (j > 100) {
      // return after timeout = 50 * 100 = 5 seconds
      // and re-open web socket
      if (ledPin >= 0) digitalWrite(ledPin, LOW);
      Serial.println("");
      client.stop();
      OpenSocket();
      return false;
    }
  }
  Serial.println("");
  if (ledPin >= 0) digitalWrite(ledPin, LOW);
  return true;
}

String MicController::ParseResponse() {
  String response = "";
  while (client.available()) {
    char temp = client.read();
    response = response + temp;
  }
  Serial.println(response);

  int start = response.indexOf('{');
  int end = response.lastIndexOf('}') + 1;
  String json = response.substring(start, end);

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return STT_BAD_RESPONSE;
  }

  String stt = "";
  for (JsonObject result : doc["results"].as<JsonArray>()) {
    const char* transcript = result["alternatives"][0]["transcript"];
    stt = stt + " " + String(transcript);
  }
  stt.replace("  ", " ");
  return stt;
}

void MicController::CreateWavHeader() {
  // WAVE PCM soundfile format: http://soundfile.sapp.org/doc/WaveFormat/
  wavHeader = new char[WAV_HEADER_SIZE];
  wavHeader[0] = 'R';
  wavHeader[1] = 'I';
  wavHeader[2] = 'F';
  wavHeader[3] = 'F';
  unsigned int fileSize = WAV_SIZE - 8;
  wavHeader[4] = (byte)(fileSize & 0xFF);
  wavHeader[5] = (byte)((fileSize >> 8) & 0xFF);
  wavHeader[6] = (byte)((fileSize >> 16) & 0xFF);
  wavHeader[7] = (byte)((fileSize >> 24) & 0xFF);
  wavHeader[8] = 'W';
  wavHeader[9] = 'A';
  wavHeader[10] = 'V';
  wavHeader[11] = 'E';
  wavHeader[12] = 'f';
  wavHeader[13] = 'm';
  wavHeader[14] = 't';
  wavHeader[15] = ' ';
  wavHeader[16] = 0x10;  // linear PCM
  wavHeader[17] = 0x00;
  wavHeader[18] = 0x00;
  wavHeader[19] = 0x00;
  wavHeader[20] = 0x01;  // linear PCM
  wavHeader[21] = 0x00;
  wavHeader[22] = 0x01;  // monoral
  wavHeader[23] = 0x00;
  wavHeader[24] = (byte)(SAMPLE_RATE & 0xFF);  // sampling rate
  wavHeader[25] = (byte)((SAMPLE_RATE >> 8) & 0xFF);
  wavHeader[26] = (byte)((SAMPLE_RATE >> 16) & 0xFF);
  wavHeader[27] = (byte)((SAMPLE_RATE >> 24) & 0xFF);
  // ByteRate = SampleRate * NumChannels * BitsPerSample/8
  unsigned int byteRate = SAMPLE_RATE * 1 * 16 / 8;  // Byte/sec = 16000x2x1 = 32000
  wavHeader[28] = (byte)(byteRate & 0xFF);
  wavHeader[29] = (byte)((byteRate >> 8) & 0xFF);
  wavHeader[30] = (byte)((byteRate >> 16) & 0xFF);
  wavHeader[31] = (byte)((byteRate >> 24) & 0xFF);
  // BlockAlign = NumChannels * BitsPerSample/8
  unsigned int blockAlign = 1 * 16 / 8;
  wavHeader[32] = (byte)(blockAlign & 0xFF);
  wavHeader[33] = (byte)((blockAlign >> 8) & 0xFF);
  unsigned int bitsPerSample = 16;
  wavHeader[34] = (byte)(bitsPerSample & 0xFF);
  wavHeader[35] = (byte)((bitsPerSample >> 8) & 0xFF);
  wavHeader[36] = 'd';
  wavHeader[37] = 'a';
  wavHeader[38] = 't';
  wavHeader[39] = 'a';
  wavHeader[40] = (byte)(WAV_DATA_SIZE & 0xFF);
  wavHeader[41] = (byte)((WAV_DATA_SIZE >> 8) & 0xFF);
  wavHeader[42] = (byte)((WAV_DATA_SIZE >> 16) & 0xFF);
  wavHeader[43] = (byte)((WAV_DATA_SIZE >> 24) & 0xFF);
}
