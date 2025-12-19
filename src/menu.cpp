#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include "gpios.h"
#include "ws2812b.h"
#include "oled.h"
#include "mpu6050.h"
#include "buttons.h"
#include "compass.h"
#include "vl53l0x.h"

enum AppMode {
    MODE_MENU,
    MODE_DISTANCE,
    MODE_COMPASS,
    MODE_LEVEL
};

namespace {
    AppMode currentMode = MODE_MENU;
    int menuSelection = 0;
    constexpr int MENU_OPTIONS = 3;
    unsigned long lastUpdateTime = 0;
    constexpr unsigned long UPDATE_INTERVAL = 50; // Reduced for smoother updates - ESP32 can handle it
    
    // Device connection status (set once during boot)
    bool mpu6050Connected = false;
    bool compassConnected = false;
    bool distanceSensorConnected = false;
} // namespace

void handleMenuMode() {
    ButtonState buttonState = buttonsRead();
    
    switch (buttonState) {
        case BUTTON_UP_PRESSED:
            menuSelection = (menuSelection - 1 + MENU_OPTIONS) % MENU_OPTIONS;
            oledDisplayMenu(menuSelection, "Distance", "Level", "Compass");
            break;
            
        case BUTTON_DOWN_PRESSED:
            menuSelection = (menuSelection + 1) % MENU_OPTIONS;
            oledDisplayMenu(menuSelection, "Distance", "Level", "Compass");
            break;
            
        case BUTTON_ENTER_PRESSED:
            if (menuSelection == 0) {
                // Distance mode - check if device is connected
                if (!distanceSensorConnected) {
                    oledDisplayError("Distance sensor not connected");
                    return;
                }
                currentMode = MODE_DISTANCE;
                lastUpdateTime = 0; // Force immediate update when entering mode
            } else if (menuSelection == 1) {
                // Level mode - check if device is connected
                if (!mpu6050Connected) {
                    oledDisplayError("MPU6050 not connected");
                    return;
                }
                currentMode = MODE_LEVEL;
                lastUpdateTime = 0; // Force immediate update when entering mode
            } else if (menuSelection == 2) {
                // Compass mode - check if device is connected
                if (!compassConnected) {
                    oledDisplayError("Compass not connected");
                    return;
                }
                currentMode = MODE_COMPASS;
                lastUpdateTime = 0; // Force immediate update when entering mode
            }
            break;
            
        default:
            break;
    }
}

void handleDistanceMode() {
    ButtonState buttonState = buttonsRead();
    
    if (buttonState == BUTTON_ENTER_PRESSED) {
        currentMode = MODE_MENU;
        oledDisplayMenuReset(); // Reset menu cache when returning to menu
        oledDisplayMenu(menuSelection, "Distance", "Level", "Compass");
        return;
    }
    
    // Device connection was already checked at boot, but verify again
    if (!distanceSensorConnected) {
        static bool errorShown = false;
        if (!errorShown) {
            oledDisplayError("Distance sensor not connected");
            errorShown = true;
        }
        return;
    }
    
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
        lastUpdateTime = currentTime;
        
        uint16_t distance = vl53l0x_readDistance();
        oledDisplayDistance(distance);
        
        if (distance > 0) {
            Serial.print("Distance: ");
            Serial.print(distance);
            Serial.println(" mm");
        } else {
            static unsigned long lastErrorTime = 0;
            if (currentTime - lastErrorTime > 1000) {
                lastErrorTime = currentTime;
                Serial.println("Distance: No reading");
            }
        }
    }
}

void handleCompassMode() {
    ButtonState buttonState = buttonsRead();
    
    if (buttonState == BUTTON_ENTER_PRESSED) {
        currentMode = MODE_MENU;
        oledDisplayMenuReset(); // Reset menu cache when returning to menu
        oledDisplayMenu(menuSelection, "Distance", "Level", "Compass");
        return;
    }
    
    // Device connection was already checked at boot, but verify again
    if (!compassConnected) {
        static bool errorShown = false;
        if (!errorShown) {
            oledDisplayError("Compass not connected");
            errorShown = true;
        }
        return;
    }
    
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
        lastUpdateTime = currentTime;
        
        CompassData compassData;
        if (compassGetData(&compassData)) {
            oledDisplayCompass(compassData.heading);
            Serial.print("Compass: Heading=");
            Serial.print(compassData.heading);
            Serial.print(" X=");
            Serial.print(compassData.x);
            Serial.print(" Y=");
            Serial.print(compassData.y);
            Serial.print(" Z=");
            Serial.println(compassData.z);
        } else {
            static unsigned long lastErrorTime = 0;
            if (currentTime - lastErrorTime > 1000) {
                lastErrorTime = currentTime;
                Serial.println("Compass: No Data");
            }
            oledDisplayError("Compass not connected");
        }
    }
}

void handleLevelMode() {
    ButtonState buttonState = buttonsRead();
    
    if (buttonState == BUTTON_ENTER_PRESSED) {
        currentMode = MODE_MENU;
        oledDisplayMenuReset(); // Reset menu cache when returning to menu
        oledDisplayMenu(menuSelection, "Distance", "Level", "Compass");
        return;
    }
    
    // Device connection was already checked at boot, but verify again
    if (!mpu6050Connected) {
        static bool errorShown = false;
        if (!errorShown) {
            oledDisplayError("MPU6050 not connected");
            errorShown = true;
        }
        return;
    }
    
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
        lastUpdateTime = currentTime;
        
        Mpu6050Data data;
        if (mpu6050GetData(&data)) {
            oledDisplayMpu6050Data(data.pitch, data.roll,
                                   data.gyroX, data.gyroY, data.gyroZ, 
                                   data.temperature);
            
            Serial.print("Pitch: ");
            Serial.print(data.pitch, 2);
            Serial.print(" deg  |  Roll: ");
            Serial.print(data.roll, 2);
            Serial.print(" deg  |  Accel X: ");
            Serial.print(data.accelX, 2);
            Serial.print("  Y: ");
            Serial.print(data.accelY, 2);
            Serial.print("  Z: ");
            Serial.println(data.accelZ, 2);
        } else {
            // If GetData fails even though device is connected, show error
            oledDisplayError("MPU6050 not connected");
        }
    }
}

void menuSetup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== ESP32-S3 Zero Boot ===");
    
    // GPIO setup
    gpios_setup();
    
    // Initialize display
    bool displayOk = oledSetup();
    
    // Buttons
    buttonsSetup();
    
    // LED (optional)
    ws2812bSetup();
    
    // Sensors - check connection once during boot
    Serial.print("MPU6050: ");
    mpu6050Connected = mpu6050Setup();
    Serial.println(mpu6050Connected ? "OK" : "Not connected");
    
    Serial.print("Compass: ");
    compassConnected = compassSetup();
    Serial.println(compassConnected ? "OK" : "Not connected");
    
    Serial.print("Distance: ");
    distanceSensorConnected = vl53l0x_init();
    Serial.println(distanceSensorConnected ? "OK" : "Not connected");
    
    Serial.println("=== Boot Complete ===\n");
    
    // Enter menu mode
    currentMode = MODE_MENU;
    menuSelection = 0;
    if (displayOk) {
        oledDisplayMenuReset(); // Reset menu cache on initial menu display
        oledDisplayMenu(menuSelection, "Distance", "Level", "Compass");
    }
}

void menuLoop() {
    switch (currentMode) {
        case MODE_MENU:
            handleMenuMode();
            break;
            
        case MODE_DISTANCE:
            handleDistanceMode();
            break;
            
        case MODE_COMPASS:
            handleCompassMode();
            break;
            
        case MODE_LEVEL:
            handleLevelMode();
            break;
    }
    
    delay(10);
}
