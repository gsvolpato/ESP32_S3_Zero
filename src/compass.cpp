#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "gpios.h"
#include "compass.h"

namespace {
    bool isInitialized = false;
    bool isConnected = false;
    uint8_t compassAddress = 0x0D;
    constexpr uint8_t QMC5883L_REG_DATA = 0x00;
    constexpr uint8_t QMC5883L_REG_STATUS = 0x06;
    constexpr uint8_t QMC5883L_REG_CTRL1 = 0x09;
    constexpr uint8_t QMC5883L_REG_CTRL2 = 0x0A;
    constexpr uint8_t QMC5883L_REG_SET_RESET = 0x0B;
} // namespace

bool compassSetup() {
    uint8_t addresses[] = {0x0D, 0x0C, 0x1E, 0x1C};
    bool found = false;
    
    for (int i = 0; i < 4; i++) {
        compassAddress = addresses[i];
        Wire.beginTransmission(compassAddress);
        uint8_t error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.println(String("Compass Connected (0x") + String(compassAddress, HEX) + ")");
            found = true;
            break;
        }
    }
    
    if (!found) {
        Serial.println("Compass Disconnected");
        isInitialized = false;
        isConnected = false;
        return false;
    }
    
    delay(10);
    
    Wire.beginTransmission(compassAddress);
    Wire.write(QMC5883L_REG_CTRL2);
    Wire.write(0x80);
    uint8_t error = Wire.endTransmission();
    
    if (error != 0) {
        Serial.println("Compass: Reset failed");
        isInitialized = false;
        isConnected = false;
        return false;
    }
    
    delay(50);
    
    Wire.beginTransmission(compassAddress);
    Wire.write(QMC5883L_REG_SET_RESET);
    Wire.write(0x01);
    error = Wire.endTransmission();
    
    if (error == 0) {
        delay(10);
    }
    
    Wire.beginTransmission(compassAddress);
    Wire.write(QMC5883L_REG_CTRL1);
    Wire.write(0x1D);
    error = Wire.endTransmission();
    
    if (error != 0) {
        Serial.println("Compass Disconnected");
        isInitialized = false;
        isConnected = false;
        return false;
    }
    
    delay(10);
    
    isInitialized = true;
    isConnected = true;
    
    delay(100);
    
    Serial.println("Compass Connected");
    return true;
}

bool compassGetData(CompassData *data) {
    if (!isInitialized || !isConnected) {
        return false;
    }
    
    uint8_t status = 0;
    bool dataReady = false;
    
    // Try to check status register with retries (up to 3 attempts)
    for (int retry = 0; retry < 3; retry++) {
        Wire.beginTransmission(compassAddress);
        Wire.write(QMC5883L_REG_STATUS);
        uint8_t error = Wire.endTransmission(false);
        if (error != 0) {
            if (error == 5) {
                isConnected = false;
            }
            // If status read fails, just proceed to read data anyway
            break;
        }
        
        uint8_t bytesReceived = Wire.requestFrom(compassAddress, (uint8_t)1, (uint8_t)true);
        if (bytesReceived == 1) {
            status = Wire.read();
            // Check if data is ready (bit 0 = DOR)
            if (status & 0x01) {
                dataReady = true;
                break;
            }
        }
        
        // Small delay before retry
        if (retry < 2) {
            delay(1);
        }
    }
    
    // If status check failed or data not ready after retries, read anyway
    // (the sensor might update faster than we can check)
    
    // Read the data registers
    Wire.beginTransmission(compassAddress);
    Wire.write(QMC5883L_REG_DATA);
    uint8_t error = Wire.endTransmission(false);
    if (error != 0) {
        if (error == 5) {
            isConnected = false;
        }
        return false;
    }
    
    uint8_t bytesReceived = Wire.requestFrom(compassAddress, (uint8_t)6, (uint8_t)true);
    if (bytesReceived != 6) {
        return false;
    }
    
    // Read X, Y, Z values (LSB first for QMC5883L)
    int16_t x = Wire.read();
    x |= Wire.read() << 8;
    int16_t y = Wire.read();
    y |= Wire.read() << 8;
    int16_t z = Wire.read();
    z |= Wire.read() << 8;
    
    data->x = x;
    data->y = y;
    data->z = z;
    
    // Validate data
    if (data->x == 0 && data->y == 0 && data->z == 0) {
        return false;
    }
    
    if (data->x == -1 && data->y == -1 && data->z == -1) {
        return false;
    }
    
    // Check for overflow (status bit 2 = OVL) - only if we got valid status
    if (dataReady && (status & 0x04)) {
        // Data overflow, skip this reading
        return false;
    }
    
    // Calculate heading
    // QMC5883L: X points to North, Y points to East when device is level
    float headingRad = atan2(data->y, data->x);
    float headingDeg = headingRad * 180.0 / M_PI;
    
    if (headingDeg < 0) {
        headingDeg += 360.0;
    }
    
    data->heading = headingDeg;
    
    return true;
}

