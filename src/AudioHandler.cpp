#include "AudioHandler.h"

#include <math.h>

void AudioHandler::setup() {
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = 16000,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 8,
      .dma_buf_len = 64,
      .use_apll = false,
      .tx_desc_auto_clear = true};

  i2s_pin_config_t pin_config = {
      .bck_io_num = PIN_I2S_SPK_BCLK,
      .ws_io_num = PIN_I2S_SPK_LRC,
      .data_out_num = PIN_I2S_SPK_DIN,
      .data_in_num = I2S_PIN_NO_CHANGE};

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_zero_dma_buffer(I2S_NUM_0);
}

void AudioHandler::playBeep() {
  int sampleRate = 16000;
  float frequency = 880.0;
  float amplitude = 10000.0;

  size_t bytes_written;
  int16_t sample;

  for (int i = 0; i < (sampleRate / 10); i++) {
    sample = (int16_t)(amplitude * sin(2.0 * PI * frequency * i / sampleRate));
    i2s_write(I2S_NUM_0, &sample, sizeof(sample), &bytes_written, portMAX_DELAY);
  }

  i2s_zero_dma_buffer(I2S_NUM_0);
}

bool AudioHandler::isHearingSound() {
  return false;
}