#include <Arduino.h>
#include "gpios.h"
#include "ws2812b.h"
#include "oled.h"
#include "mpu6050.h"
#include "buttons.h"
#include "compass.h"

enum AppMode {
    MODE_MENU,
    MODE_COMPASS,
    MODE_LEVEL
};

namespace {
    AppMode currentMode = MODE_MENU;
    int menuSelection = 0;
    constexpr int MENU_OPTIONS = 2;
    unsigned long lastUpdateTime = 0;
    constexpr unsigned long UPDATE_INTERVAL = 100;
} // namespace

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    bool bootError = false;
    
    gpios_setup();
    buttonsSetup();
    
    if (!ws2812bSetup()) {
        bootError = true;
    }
    
    ws2812bBlinkYellow();
    ws2812bTurnOff();
    
    if (!oledSetup()) {
        bootError = true;
    }
    
    bool mpu6050Connected = mpu6050Setup();
    bool compassConnected = compassSetup();
    
    if (bootError) {
        oledDisplayText("Boot ERROR!");
        delay(2000);
    }
    
    if (compassConnected) {
        Serial.println("Compass: Ready");
    } else {
        Serial.println("Compass: Not connected");
    }
    
    currentMode = MODE_MENU;
    menuSelection = 0;
    oledDisplayMenu(menuSelection, "Compass", "Level");
}

void handleMenuMode() {
    ButtonState buttonState = buttonsRead();
    
    switch (buttonState) {
        case BUTTON_UP_PRESSED:
            menuSelection = (menuSelection - 1 + MENU_OPTIONS) % MENU_OPTIONS;
            oledDisplayMenu(menuSelection, "Compass", "Level");
            break;
            
        case BUTTON_DOWN_PRESSED:
            menuSelection = (menuSelection + 1) % MENU_OPTIONS;
            oledDisplayMenu(menuSelection, "Compass", "Level");
            break;
            
        case BUTTON_ENTER_PRESSED:
            if (menuSelection == 0) {
                currentMode = MODE_COMPASS;
            } else if (menuSelection == 1) {
                currentMode = MODE_LEVEL;
            }
            break;
            
        default:
            break;
    }
}

void handleCompassMode() {
    ButtonState buttonState = buttonsRead();
    
    if (buttonState == BUTTON_ENTER_PRESSED) {
        currentMode = MODE_MENU;
        oledDisplayMenu(menuSelection, "Compass", "Level");
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
            oledDisplayText("Compass: No Data");
        }
    }
}

void handleLevelMode() {
    ButtonState buttonState = buttonsRead();
    
    if (buttonState == BUTTON_ENTER_PRESSED) {
        currentMode = MODE_MENU;
        oledDisplayMenu(menuSelection, "Compass", "Level");
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
        }
    }
}

void loop() {
    switch (currentMode) {
        case MODE_MENU:
            handleMenuMode();
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