// File: firmware/main/main.ino

#include <WiFi.h>
#include <SPI.h>
#include "../camera.h"
#include "../gyroscope.h"
#include "../mic.h"
#include "../audio.h"

const char* ssid = "HelloWorld";
const char* password = "Aa0978926291";

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // WiFi Connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");

  // Init Peripherals
  initCamera();
  initGyro(); // SPI
  initMic();  // I2S0 PDM
  initAudio(); // I2S1 Audio

  // Start Server
  startCameraServer();
  // Register Audio Alert Endpoint (using the camera's httpd server handle)
  startAudioServer(stream_httpd);

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("/stream' to connect");
  Serial.println("Audio Alert Ready! Use 'http://<IP>/alert' to trigger.");
}

void loop() {
  // Simple Gyro Check
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 2000) {
    readGyro();
    lastTime = millis();
  }
  delay(10);
}
