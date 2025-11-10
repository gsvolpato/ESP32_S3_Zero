#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "gpios.h"
#include "mpu6050.h"

namespace {
TwoWire mpuWire(1);
Adafruit_MPU6050 mpu;
bool isInitialized = false;
bool isConnected = false;
} // namespace

bool mpu6050Setup() {
    mpuWire.begin(MPU6050_SDA_PIN, MPU6050_SCL_PIN);
    delay(10);
    
    if (!mpu.begin(0x68, &mpuWire)) {
        isInitialized = false;
        isConnected = false;
        return false;
    }
    
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    
    isInitialized = true;
    isConnected = true;
    return true;
}

bool mpu6050IsConnected() {
    if (!isInitialized) {
        return false;
    }
    
    mpuWire.beginTransmission(0x68);
    uint8_t error = mpuWire.endTransmission();
    isConnected = (error == 0);
    return isConnected;
}

