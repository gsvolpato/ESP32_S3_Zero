#include <Arduino.h>
#include "gpios.h"

int previousBootButtonState = HIGH;

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    gpios_setup();
}

void loop() {
    int currentBootButtonState = digitalRead(BOOT_BUTTON_PIN);

    if (currentBootButtonState == LOW && previousBootButtonState == HIGH) {
        Serial.println("Hello, world!");
    }

    previousBootButtonState = currentBootButtonState;
    delay(5);
}