#ifndef _MICCONTROLLER_H
#define _MICCONTROLLER_H

#include <Arduino.h>
#include "driver/i2s.h"

#define SAMPLE_RATE 16000
#define BUFFER_SIZE 12000

class MicController {
  int pinI2sSck;
  int pinI2sWs;
  int pinI2sSd;

  void InitI2s();
  void CreateWavHeader(byte* header, int waveDataSize);

public:
  static const int dividedWavSize = BUFFER_SIZE / 4;
  static const int wavSize = 30 * dividedWavSize;  // It must be multiple of dividedWavSize.
  static const int wavHeaderSize = 44;

  char** wavData;                             // It's divided. Because large continuous memory area can't be allocated in esp32.
  byte wavHeader[wavHeaderSize + 4] = { 0 };  // The size must be multiple of 3 for Base64 encoding. Additional byte size must be even because wave data is 16bit.

  MicController(int pinI2sSck, int pinI2sWs, int pinI2sSd);
  ~MicController();
  void Record();
};

#endif  // _MICCONTROLLER_H
