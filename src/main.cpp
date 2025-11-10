#include <Arduino.h>
#include "gpios.h"
#include "ws2812b.h"
#include "oled.h"

void setup() {
    Serial.begin(115200);
    delay(1000);
    gpios_setup();
    ws2812bSetup();
    oledSetup();
}

void loop() {
    const char *colorName = ws2812bNextColor();
    Serial.println(colorName);
    ws2812bPulse();
}