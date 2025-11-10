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

void oledSetup() {
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        return;
    }
    
    display.setRotation(2);
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Hello World"));
    display.display();
}

void oledDisplayText(const char *text) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(text);
    display.display();
}

