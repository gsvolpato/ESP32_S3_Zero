#include <Arduino.h>
#include "gpios.h"
#include "vl53l0x.h"

int previousBootButtonState = HIGH;

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    
    gpios_setup();
    delay(100);
    
    if (!vl53l0x_init()) {
        Serial.println("Failed to initialize VL53L0X sensor!");
        Serial.println("Please check:");
        Serial.println("1. Wiring connections (SDA, SCL, VCC, GND)");
        Serial.println("2. Power supply to the sensor");
        Serial.println("3. I2C pull-up resistors (if required)");
        while (1) {
            delay(1000);
        }
    }
    delay(100);
}

void loop() {
    uint16_t distance = vl53l0x_readDistance();
    
    if (distance > 0) {
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" mm");
    } else {
        Serial.println("Distance reading out of range");
    }
    
    int currentBootButtonState = digitalRead(BOOT_BUTTON_PIN);
    if (currentBootButtonState == LOW && previousBootButtonState == HIGH) {
        Serial.println("Boot button pressed!");
    }
    previousBootButtonState = currentBootButtonState;
    
    delay(100);
}