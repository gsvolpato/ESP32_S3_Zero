#include <Arduino.h>
#include "gpios.h"
#include "ws2812b.h"

void setup() {
    Serial.begin(115200);
    delay(10000);
    gpios_setup();
    ws2812bSetup();
}

void loop() {
    const char *colorName = ws2812bNextColor();
    Serial.println(colorName);
    delay(1000);
}