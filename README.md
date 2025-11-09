# ESP32 S3 Zero

ESP32 S3 Zero Projects. Each branch a different project.

## Branch: MPU6050 Gyroscope

### MPU6050 Gyroscope Module

MPU6050 6-axis motion sensor integration with I2C communication. Reads accelerometer and gyroscope data and calculates pitch and roll angles. Angles are displayed via serial communication only when the boot button is pressed.

**Pin Configuration:**
- `MPU6050_INT`: GPIO 1
- `MPU6050_AD0`: GPIO 2
- `MPU6050_XCL`: GPIO 3
- `MPU6050_XDA`: GPIO 4
- `MPU6050_SDA`: GPIO 5 (I2C Data)
- `MPU6050_SCL`: GPIO 6 (I2C Clock)

**Implementation:**
- `mpu6050.h` / `mpu6050.cpp`: MPU6050 initialization, data reading, and angle calculation
- `main.cpp`: Button-triggered angle reading and serial output

### LED Automatic Color Change

Automatically cycles the onboard NeoPixel through a rainbow of colors using the Arduino framework and Adafruit NeoPixel library. Initializes GPIOs for the boot button and drives the LED with a 500 ms transition interval, packaging the logic in `ws2312b.cpp` and invoked from `main.cpp`.
