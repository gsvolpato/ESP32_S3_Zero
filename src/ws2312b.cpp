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
