#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <Wire.h>
#include "gpios.h"

void gpios_setup() {
    // Button pins
    pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
    pinMode(BUTTON_ENTER_PIN, INPUT_PULLUP);
    pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
    
    // Display pins
    pinMode(DISPLAY_RESET, OUTPUT);
    pinMode(DISPLAY_DC, OUTPUT);
    pinMode(DISPLAY_BK, OUTPUT);
    digitalWrite(DISPLAY_RESET, HIGH);
    digitalWrite(DISPLAY_DC, LOW);
    digitalWrite(DISPLAY_BK, HIGH); // Backlight on
    
    // VL53L0X XSHUT pin
    pinMode(VL53L0X_XSHUT_PIN, OUTPUT);
    digitalWrite(VL53L0X_XSHUT_PIN, LOW);
    delay(10);
    digitalWrite(VL53L0X_XSHUT_PIN, HIGH);
    delay(10);
    
    // I2C initialization (used by MPU6050, Compass, and VL53L0X)
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(100000);
}