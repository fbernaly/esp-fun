#include "MicController.h"

MicController::MicController(int pinI2sSck, int pinI2sWs, int pinI2sSd) {
  this->pinI2sSck = pinI2sSck;
  this->pinI2sWs = pinI2sWs;
  this->pinI2sSd = pinI2sSd;
  wavData = new char*[wavSize / dividedWavSize];
  for (int i = 0; i < wavSize / dividedWavSize; ++i) wavData[i] = new char[dividedWavSize];
  InitI2s();
  CreateWavHeader(wavHeader, wavSize);
}

MicController::~MicController() {
  for (int i = 0; i < wavSize / dividedWavSize; ++i) delete[] wavData[i];
  delete[] wavData;
}

void MicController::InitI2s() {
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

void MicController::Record() {
  Serial.println("Recording...");
  char* buffer = (char*)calloc(BUFFER_SIZE, sizeof(char));
  for (int j = 0; j < wavSize / dividedWavSize; ++j) {
    size_t bytesRead = 0;
    i2s_read(I2S_NUM_0, buffer, BUFFER_SIZE, &bytesRead, portMAX_DELAY);
    for (int i = 0; i < BUFFER_SIZE / 8; ++i) {
      wavData[j][2 * i] = buffer[8 * i + 2];
      wavData[j][2 * i + 1] = buffer[8 * i + 3];
    }
  }
  delete buffer;
  Serial.println("Recording Completed.");
}

void MicController::CreateWavHeader(byte* wavHeader, int wavSize) {
  wavHeader[0] = 'R';
  wavHeader[1] = 'I';
  wavHeader[2] = 'F';
  wavHeader[3] = 'F';
  unsigned int fileSize = wavSize + wavHeaderSize - 8;
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
  wavHeader[24] = 0x80;  // sampling rate 16000
  wavHeader[25] = 0x3E;
  wavHeader[26] = 0x00;
  wavHeader[27] = 0x00;
  wavHeader[28] = 0x00;  // Byte/sec = 16000x2x1 = 32000
  wavHeader[29] = 0x7D;
  wavHeader[30] = 0x00;
  wavHeader[31] = 0x00;
  wavHeader[32] = 0x02;  // 16bit monoral
  wavHeader[33] = 0x00;
  wavHeader[34] = 0x10;  // 16bit
  wavHeader[35] = 0x00;
  wavHeader[36] = 'd';
  wavHeader[37] = 'a';
  wavHeader[38] = 't';
  wavHeader[39] = 'a';
  wavHeader[40] = (byte)(wavSize & 0xFF);
  wavHeader[41] = (byte)((wavSize >> 8) & 0xFF);
  wavHeader[42] = (byte)((wavSize >> 16) & 0xFF);
  wavHeader[43] = (byte)((wavSize >> 24) & 0xFF);
}
