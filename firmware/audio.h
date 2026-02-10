// File: firmware/audio.h
#ifndef AUDIO_H
#define AUDIO_H

#include <driver/i2s.h>
#include <esp_http_server.h>

// I2S Speaker Pins (MAX98357)
#define I2S_SPK_BCLK 7
#define I2S_SPK_LRCK 8
#define I2S_SPK_DIN  9

#define I2S_SPK_PORT I2S_NUM_1

void initAudio() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = true
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SPK_BCLK,
    .ws_io_num = I2S_SPK_LRCK,
    .data_out_num = I2S_SPK_DIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_SPK_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_SPK_PORT, &pin_config);
  i2s_zero_dma_buffer(I2S_SPK_PORT);

  Serial.println("Audio Initialized (MAX98357 I2S)");
}

// Generate a simple beep tone (Sine Wave)
void playTone() {
  size_t bytes_written;
  int16_t samples[256];

  // Create a sine wave buffer
  for(int i=0; i<256; i++) {
    samples[i] = (int16_t)(3000 * sin(2 * PI * i / 256.0 * 10)); // ~1700Hz
  }

  // Play for ~0.5 seconds
  for(int j=0; j<100; j++) {
     i2s_write(I2S_SPK_PORT, samples, sizeof(samples), &bytes_written, portMAX_DELAY);
  }
}

// HTTP Handler for /alert
static esp_err_t alert_handler(httpd_req_t *req) {
  playTone();
  httpd_resp_send(req, "Alert Played", HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

// Register the /alert endpoint
void startAudioServer(httpd_handle_t server) {
  httpd_uri_t alert_uri = {
    .uri       = "/alert",
    .method    = HTTP_GET,
    .handler   = alert_handler,
    .user_ctx  = NULL
  };
  httpd_register_uri_handler(server, &alert_uri);
  Serial.println("Audio Alert Server Registered");
}

#endif
