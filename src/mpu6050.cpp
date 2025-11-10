#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <math.h>
#include "gpios.h"
#include "mpu6050.h"

namespace {
Adafruit_MPU6050 mpu;
bool isInitialized = false;
bool isConnected = false;
} // namespace

bool mpu6050Setup() {
    if (!mpu.begin()) {
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
    
    Wire.beginTransmission(0x68);
    uint8_t error = Wire.endTransmission();
    isConnected = (error == 0);
    return isConnected;
}

bool mpu6050GetData(Mpu6050Data *data) {
    if (!isInitialized || !isConnected) {
        return false;
    }
    
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    
    if (!mpu.getEvent(&accel, &gyro, &temp)) {
        return false;
    }
    
    data->accelX = accel.acceleration.x;
    data->accelY = accel.acceleration.y;
    data->accelZ = accel.acceleration.z;
    data->gyroX = gyro.gyro.x;
    data->gyroY = gyro.gyro.y;
    data->gyroZ = gyro.gyro.z;
    data->temperature = temp.temperature;
    
    constexpr float radToDeg = 180.0 / M_PI;
    
    // Inverted to match standard level conventions:
    // Right tilt = positive roll, Left tilt = negative roll
    // Forward tilt = positive pitch, Backward tilt = negative pitch
    data->pitch = -atan2(data->accelX, sqrt(data->accelY * data->accelY + data->accelZ * data->accelZ)) * radToDeg;
    data->roll = -atan2(data->accelY, sqrt(data->accelX * data->accelX + data->accelZ * data->accelZ)) * radToDeg;
    
    return true;
}

