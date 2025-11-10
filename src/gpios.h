#define DISABLE_ALL_LIBRARY_WARNINGS
#ifndef GPIOS_H
#define GPIOS_H

#define LED_PIN 21
#define BOOT_BUTTON_PIN 0
#define OLED_SCL_PIN 1
#define OLED_SDA_PIN 2
#define MPU6050_SCL_PIN 7
#define MPU6050_SDA_PIN 8
#define MPU6050_XDA_PIN 9
#define MPU6050_XCL_PIN 6
#define MPU6050_AD0_PIN 11
#define MPU6050_INT_PIN 12


void gpios_setup();



#endif // GPIOS_H