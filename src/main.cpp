#include <Arduino.h>
#include "gpios.h"
#include "ws2812b.h"
#include "oled.h"

void setup() {
    Serial.begin(115200);
    delay(1000);
    gpios_setup();
    ws2812bSetup();
    ws2812bBlinkYellow();
    ws2812bTurnOff();
    oledSetup();
    oledDisplayText("Booted successfully!");
}

void loop() {
    delay(100);
}