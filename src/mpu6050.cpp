#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "mpu6050.h"
#include "gpios.h"

bool mpu6050Init() {
    Wire.begin(MPU6050_SDA, MPU6050_SCL);
    Wire.setClock(400000);
    
    delay(100);
    
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(MPU6050_PWR_MGMT_1);
    Wire.write(0);
    uint8_t error = Wire.endTransmission();
    
    if (error != 0) {
        return false;
    }
    
    delay(100);
    return true;
}

bool mpu6050Read(Mpu6050Data* data) {
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(MPU6050_ACCEL_XOUT_H);
    uint8_t error = Wire.endTransmission(false);
    
    if (error != 0) {
        return false;
    }
    
    uint8_t bytesRead = Wire.requestFrom(MPU6050_ADDRESS, 14, true);
    
    if (bytesRead != 14) {
        return false;
    }
    
    data->accelX = (Wire.read() << 8) | Wire.read();
    data->accelY = (Wire.read() << 8) | Wire.read();
    data->accelZ = (Wire.read() << 8) | Wire.read();
    
    Wire.read();
    Wire.read();
    
    data->gyroX = (Wire.read() << 8) | Wire.read();
    data->gyroY = (Wire.read() << 8) | Wire.read();
    data->gyroZ = (Wire.read() << 8) | Wire.read();
    
    return true;
}

void mpu6050CalculateAngles(const Mpu6050Data* data, Mpu6050Angles* angles) {
    float accelX = data->accelX / 16384.0;
    float accelY = data->accelY / 16384.0;
    float accelZ = data->accelZ / 16384.0;
    
    angles->pitch = atan2(accelX, sqrt(accelY * accelY + accelZ * accelZ)) * 180.0 / PI;
    angles->roll = atan2(accelY, sqrt(accelX * accelX + accelZ * accelZ)) * 180.0 / PI;
}

