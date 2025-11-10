#include <Arduino.h>
#include "gpios.h"
#include "ws2812b.h"

int previousBootButtonState = HIGH;

void setup() {
    Serial.begin(115200);
    delay(10000);
    gpios_setup();
    ws2812bSetup();
}

void loop() {
    int currentBootButtonState = digitalRead(BOOT_BUTTON_PIN);

    if (currentBootButtonState == LOW && previousBootButtonState == HIGH) {
        const char *colorName = ws2812bNextColor();
        Serial.println(colorName);
    }

    previousBootButtonState = currentBootButtonState;
    delay(5);
}