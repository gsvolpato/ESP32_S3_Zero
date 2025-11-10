#ifndef MPU6050_H
#define MPU6050_H

struct Mpu6050Data {
    float accelX;
    float accelY;
    float accelZ;
    float gyroX;
    float gyroY;
    float gyroZ;
    float temperature;
    float pitch;
    float roll;
};

bool mpu6050Setup();
bool mpu6050IsConnected();
bool mpu6050GetData(Mpu6050Data *data);

#endif // MPU6050_H

