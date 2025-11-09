#include <Arduino.h>
#include "gpios.h"
#include "ws2812b.h"

void setup() {
    Serial.begin(115200);
    gpiosSetup();
    ws2812bSetup();
}

void loop() {
    ws2812bUpdate();
}