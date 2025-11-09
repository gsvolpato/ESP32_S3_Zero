#include <Arduino.h>
#include "gpios.h"
#include "ws2812b.h"
#include "mpu6050.h"

int previousBootButtonState = HIGH;

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    
    gpios_setup();
    ws2812bSetup();
    
    Serial.println("Initializing MPU6050...");
    if (mpu6050Init()) {
        Serial.println("MPU6050 initialized successfully");
    } else {
        Serial.println("Failed to initialize MPU6050");
    }
    Serial.println("Press boot button to read angles");
}

void loop() {
    int currentBootButtonState = digitalRead(BOOT_BUTTON_PIN);
    
    if (previousBootButtonState == HIGH && currentBootButtonState == LOW) {
        Mpu6050Data data;
        
        if (mpu6050Read(&data)) {
            Mpu6050Angles angles;
            mpu6050CalculateAngles(&data, &angles);
            
            Serial.print("Pitch: ");
            Serial.print(angles.pitch, 2);
            Serial.print(" deg | Roll: ");
            Serial.print(angles.roll, 2);
            Serial.println(" deg");
        } else {
            Serial.println("Failed to read MPU6050 data");
        }
        
        delay(50);
    }
    
    previousBootButtonState = currentBootButtonState;
    delay(10);
}