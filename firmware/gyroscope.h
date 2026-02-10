// File: firmware/gyroscope.h

#ifndef GYROSCOPE_H
#define GYROSCOPE_H

#include <SPI.h>

// ICM42688P SPI Pins (XIAO ESP32S3)
#define IMU_SPI_SCK  1
#define IMU_SPI_MOSI 2
#define IMU_SPI_MISO 3
#define IMU_SPI_CS   4

#define PWR_MGMT0_REG  0x4E
#define ACCEL_DATA_X1  0x1F

// SPI Settings
#define SPI_CLK_FREQ 1000000 // 1MHz for safety

void initGyro() {
  pinMode(IMU_SPI_CS, OUTPUT);
  digitalWrite(IMU_SPI_CS, HIGH); // CS Inactive

  SPI.begin(IMU_SPI_SCK, IMU_SPI_MISO, IMU_SPI_MOSI, IMU_SPI_CS);
  delay(10);

  // Wake up ICM42688P (SPI Write)
  // Register 0x4E = 0x0F (Gyro LN, Accel LN)

  SPI.beginTransaction(SPISettings(SPI_CLK_FREQ, MSBFIRST, SPI_MODE0));
  digitalWrite(IMU_SPI_CS, LOW);
  SPI.transfer(PWR_MGMT0_REG); // Write Address
  SPI.transfer(0x0F);          // Data
  digitalWrite(IMU_SPI_CS, HIGH);
  SPI.endTransaction();

  Serial.println("IMU Initialized (ICM42688P via SPI)");
}

void readGyro() {
  // Read burst (SPI Read)
  // Register 0x1F (ACCEL_DATA_X1) | 0x80 for Read bit

  uint8_t buffer[12];

  SPI.beginTransaction(SPISettings(SPI_CLK_FREQ, MSBFIRST, SPI_MODE0));
  digitalWrite(IMU_SPI_CS, LOW);
  SPI.transfer(ACCEL_DATA_X1 | 0x80); // Read command
  for(int i=0; i<12; i++){
    buffer[i] = SPI.transfer(0x00);
  }
  digitalWrite(IMU_SPI_CS, HIGH);
  SPI.endTransaction();

  int16_t accelX = (buffer[0] << 8) | buffer[1];
  int16_t accelY = (buffer[2] << 8) | buffer[3];
  int16_t accelZ = (buffer[4] << 8) | buffer[5];
  int16_t gyroX = (buffer[6] << 8) | buffer[7];
  int16_t gyroY = (buffer[8] << 8) | buffer[9];
  int16_t gyroZ = (buffer[10] << 8) | buffer[11];

  // Print to Serial for verification
  Serial.printf("IMU: A[%d, %d, %d] G[%d, %d, %d]\n", accelX, accelY, accelZ, gyroX, gyroY, gyroZ);
}

#endif
