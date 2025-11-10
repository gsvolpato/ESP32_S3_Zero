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

constexpr RgbColor colorSequence[] = {
    {255, 0, 0},
    {0, 255, 0},
    {0, 0, 255},
    {255, 255, 0},
    {0, 255, 255},
    {255, 0, 255}
};

constexpr size_t colorSequenceLength = sizeof(colorSequence) / sizeof(colorSequence[0]);

constexpr const char *colorNames[] = {
    "Red",
    "Green",
    "Blue",
    "Yellow",
    "Cyan",
    "Magenta"
};

size_t currentColorIndex = 0;

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
}

const char *ws2812bNextColor() {
    currentColorIndex = (currentColorIndex + 1) % colorSequenceLength;
    applyColor(colorSequence[currentColorIndex]);
    return colorNames[currentColorIndex];
}

void ws2812bPulse() {
    const RgbColor &color = colorSequence[currentColorIndex];
    constexpr uint8_t minBrightness = 5;
    constexpr uint8_t maxBrightness = 255;
    constexpr uint8_t step = 5;
    constexpr uint16_t delayMs = 20;
    
    for (int16_t brightness = minBrightness; brightness <= maxBrightness; brightness += step) {
        uint8_t r = (color.r * brightness) / 255;
        uint8_t g = (color.g * brightness) / 255;
        uint8_t b = (color.b * brightness) / 255;
        led.setPixelColor(0, r, g, b);
        led.show();
        delay(delayMs);
    }
    
    for (int16_t brightness = maxBrightness; brightness >= minBrightness; brightness -= step) {
        uint8_t r = (color.r * brightness) / 255;
        uint8_t g = (color.g * brightness) / 255;
        uint8_t b = (color.b * brightness) / 255;
        led.setPixelColor(0, r, g, b);
        led.show();
        delay(delayMs);
    }
}
