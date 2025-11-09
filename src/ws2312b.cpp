#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "gpios.h"
#include "ws2812b.h"

namespace {
constexpr uint8_t pixelCount = 1;
constexpr uint32_t transitionIntervalMs = 500;

Adafruit_NeoPixel led(pixelCount, LED_PIN, NEO_GRB + NEO_KHZ800);

struct RgbColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

constexpr RgbColor colorSequence[] = {
    {255, 0, 0},
    {0, 255, 0},
    {0, 0, 255},
    {255, 255, 0},
    {0, 255, 255},
    {255, 0, 255}
};

constexpr size_t colorSequenceLength = sizeof(colorSequence) / sizeof(colorSequence[0]);

size_t currentColorIndex = 0;
uint32_t lastUpdateMs = 0;

void applyColor(const RgbColor &color) {
    led.setPixelColor(0, color.r, color.g, color.b);
    led.show();
}
} // namespace

void ws2812bSetup() {
    led.begin();
    led.setBrightness(64);
    led.clear();
    applyColor(colorSequence[currentColorIndex]);
    lastUpdateMs = millis();
}

void ws2812bUpdate() {
    const uint32_t now = millis();
    if (now - lastUpdateMs < transitionIntervalMs) {
        return;
    }

    lastUpdateMs = now;
    currentColorIndex = (currentColorIndex + 1) % colorSequenceLength;
    applyColor(colorSequence[currentColorIndex]);
}
