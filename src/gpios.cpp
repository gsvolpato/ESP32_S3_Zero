#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include "gpios.h"

void gpios_setup() {
    pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
    pinMode(BUTTON_ENTER_PIN, INPUT_PULLUP);
    pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
}