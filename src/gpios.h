#define DISABLE_ALL_LIBRARY_WARNINGS
#ifndef GPIOS_H
#define GPIOS_H

#define LED_PIN 21
#define BOOT_BUTTON_PIN 0

#define MPU6050_INT 1 
#define MPU6050_AD0 2  
#define MPU6050_XCL 3
#define MPU6050_XDA 4
#define MPU6050_SDA 5
#define MPU6050_SCL 6


void gpios_setup();



#endif // GPIOS_H