#ifndef MPU6050_H
#define MPU6050_H

#include <Arduino.h>

#define MPU6050_ADDRESS 0x68
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_GYRO_XOUT_H 0x43

struct Mpu6050Data {
    int16_t accelX;
    int16_t accelY;
    int16_t accelZ;
    int16_t gyroX;
    int16_t gyroY;
    int16_t gyroZ;
};

struct Mpu6050Angles {
    float pitch;
    float roll;
};

bool mpu6050Init();
bool mpu6050Read(Mpu6050Data* data);
void mpu6050CalculateAngles(const Mpu6050Data* data, Mpu6050Angles* angles);

#endif // MPU6050_H

