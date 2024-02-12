#ifndef _MICCONTROLLER_H
#define _MICCONTROLLER_H

#include <Arduino.h>
#include "driver/i2s.h"

#define SAMPLE_RATE 16000
#define BUFFER_SIZE 3000  // It should be divisible by 8
#define DIVIDED_WAV_DATA_SIZE (BUFFER_SIZE / 4)
#define WAV_DATA_SIZE (160 * DIVIDED_WAV_DATA_SIZE)  // It must be multiple of DIVIDED_WAV_DATA_SIZE.
#define WAV_HEADER_SIZE 48

class MicController {
  void CreateWavHeader();
  void ConfigureI2s(int pinI2sSck, int pinI2sWs, int pinI2sSd);

public:
  char** wavData;                           // It's divided. Because large continuous memory area can't be allocated in esp32.
  byte wavHeader[WAV_HEADER_SIZE] = { 0 };  // The size must be multiple of 3 for Base64 encoding. Additional byte size must be even because wave data is 16bit.

  MicController(int pinI2sSck, int pinI2sWs, int pinI2sSd);
  ~MicController();
  void Record();
};

#endif  // _MICCONTROLLER_H
