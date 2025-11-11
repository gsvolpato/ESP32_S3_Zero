#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include "vl53l0x.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

bool vl53l0x_init() {
    Serial.print("Initializing I2C on SDA: ");
    Serial.print(GENERAL_SDA_PIN);
    Serial.print(", SCL: ");
    Serial.println(GENERAL_SCL_PIN);
    
    Wire.begin(GENERAL_SDA_PIN, GENERAL_SCL_PIN);
    Wire.setClock(100000);
    delay(100);
    
    Serial.println("Scanning I2C bus...");
    byte error, address;
    int nDevices = 0;
    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) Serial.print("0");
            Serial.print(address, HEX);
            Serial.println();
            nDevices++;
        }
    }
    if (nDevices == 0) {
        Serial.println("No I2C devices found!");
    }
    Serial.println("I2C scan complete");
    delay(100);
    
    Serial.println("Starting VL53L0X sensor...");
    delay(50);
    if (!lox.begin()) {
        Serial.println("Failed to initialize VL53L0X sensor!");
        Serial.println("Check wiring and power supply");
        return false;
    }
    delay(100);
    
    Serial.println("VL53L0X sensor initialized successfully!");
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

