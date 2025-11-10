#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "gpios.h"
#include "oled.h"

namespace {
constexpr int screenWidth = 128;
constexpr int screenHeight = 64;
constexpr int oledReset = -1;

Adafruit_SSD1306 display(screenWidth, screenHeight, &Wire, oledReset);
} // namespace

bool oledSetup() {
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
    delay(10);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        return false;
    }
    
    display.setRotation(2);
    display.clearDisplay();
    display.display();
    return true;
}

void oledDisplayText(const char *text) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(text);
    display.display();
}

void oledDisplayLines(const char *line1, const char *line2) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(line1);
    display.setCursor(0, 16);
    display.println(line2);
    display.display();
}

void oledClear() {
    display.clearDisplay();
    display.display();
}

void oledDisplayMpu6050Data(float pitch, float roll, float gyroX, float gyroY, float gyroZ, float temperature) {
    char buffer[32];
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    display.setCursor(0, 0);
    snprintf(buffer, sizeof(buffer), "Pitch: %.1f", pitch);
    display.println(buffer);
    
    display.setCursor(0, 12);
    snprintf(buffer, sizeof(buffer), "Roll: %.1f", roll);
    display.println(buffer);
    
    display.setCursor(0, 24);
    snprintf(buffer, sizeof(buffer), "Gx:%.1f Gy:%.1f", gyroX, gyroY);
    display.println(buffer);
    
    display.setCursor(0, 36);
    snprintf(buffer, sizeof(buffer), "Gz:%.1f T:%.1fC", gyroZ, temperature);
    display.println(buffer);
    
    display.display();
}

