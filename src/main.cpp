#include <Arduino.h>
#include "gpios.h"
#include "ws2812b.h"
#include "oled.h"
#include "mpu6050.h"

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    bool bootError = false;
    
    gpios_setup();
    
    if (!ws2812bSetup()) {
        bootError = true;
    }
    
    ws2812bBlinkYellow();
    ws2812bTurnOff();
    
    if (!oledSetup()) {
        bootError = true;
    }
    
    bool mpu6050Connected = mpu6050Setup();
    
    if (bootError) {
        oledDisplayText("Boot ERROR!");
    } else {
        const char *mpuStatus = mpu6050Connected ? "MPU6050: OK" : "MPU6050: FAIL";
        oledDisplayLines("Boot: OK", mpuStatus);
        delay(2000);
        oledClear();
    }
}

void loop() {
    Mpu6050Data data;
    if (mpu6050GetData(&data)) {
        oledDisplayMpu6050Data(data.pitch, data.roll,
                               data.gyroX, data.gyroY, data.gyroZ, 
                               data.temperature);
    }
    delay(100);
}