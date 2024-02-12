#include "MicController.h"

MicController::MicController(int pinI2sSck, int pinI2sWs, int pinI2sSd) {
  wavData = new char*[WAV_DATA_SIZE / DIVIDED_WAV_DATA_SIZE];
  for (int i = 0; i < WAV_DATA_SIZE / DIVIDED_WAV_DATA_SIZE; ++i) wavData[i] = new char[DIVIDED_WAV_DATA_SIZE];
  CreateWavHeader();
  ConfigureI2s(pinI2sSck, pinI2sWs, pinI2sSd);
}

MicController::~MicController() {
  for (int i = 0; i < WAV_DATA_SIZE / DIVIDED_WAV_DATA_SIZE; ++i) delete[] wavData[i];
  delete[] wavData;
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

void MicController::Record() {
  Serial.println("Recording...");
  char* buffer = (char*)calloc(BUFFER_SIZE, sizeof(char));
  for (int j = 0; j < WAV_DATA_SIZE / DIVIDED_WAV_DATA_SIZE; ++j) {
    size_t bytesRead = 0;
    i2s_read(I2S_NUM_0, buffer, BUFFER_SIZE, &bytesRead, portMAX_DELAY);
    for (int i = 0; i < BUFFER_SIZE / 8; ++i) {
      // saving buffer into wav data from one channel only
      wavData[j][2 * i] = buffer[8 * i + 2];
      wavData[j][2 * i + 1] = buffer[8 * i + 3];
    }
  }
  delete buffer;
  Serial.println("Recording Completed.");
}

void MicController::CreateWavHeader() {
  // WAVE PCM soundfile format: http://soundfile.sapp.org/doc/WaveFormat/
  wavHeader[0] = 'R';
  wavHeader[1] = 'I';
  wavHeader[2] = 'F';
  wavHeader[3] = 'F';
  unsigned int fileSize = WAV_DATA_SIZE + WAV_HEADER_SIZE - 8;
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
