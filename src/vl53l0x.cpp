#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include "vl53l0x.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

bool vl53l0x_init() {
    // XSHUT pin and I2C are already configured in gpios_setup()
    if (!lox.begin()) {
        Serial.println("VL53L0X disconnected");
        return false;
    }
    Serial.println("VL53L0X Connected");
    return true;
}

bool vl53l0x_isConnected() {
    return true;
}

uint16_t vl53l0x_readDistance() {
    VL53L0X_RangingMeasurementData_t measure;
    
    lox.rangingTest(&measure, false);
    
    if (measure.RangeStatus != 4) {
        uint16_t distance = measure.RangeMilliMeter;
        if (distance > 0 && distance <= 2500) {
            return distance;
        }
    }
    
    return 0;
}

