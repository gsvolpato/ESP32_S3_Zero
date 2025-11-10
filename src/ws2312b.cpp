#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "gpios.h"
#include "ws2812b.h"

namespace {
constexpr uint8_t pixelCount = 1;

Adafruit_NeoPixel led(pixelCount, LED_PIN, NEO_GRB + NEO_KHZ800);

struct RgbColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

void applyColor(const RgbColor &color) {
    led.setPixelColor(0, color.r, color.g, color.b);
    led.show();
}
} // namespace

void ws2812bSetup() {
    led.begin();
    led.setBrightness(64);
    led.clear();
    led.show();
}

void ws2812bBlinkYellow() {
    constexpr RgbColor yellow = {255, 255, 0};
    constexpr uint16_t blinkDelayMs = 200;
    
    for (int i = 0; i < 3; i++) {
        applyColor(yellow);
        delay(blinkDelayMs);
        led.clear();
        led.show();
        delay(blinkDelayMs);
    }
}

void ws2812bTurnOff() {
    led.clear();
    led.show();
}
