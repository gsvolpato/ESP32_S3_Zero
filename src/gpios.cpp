#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include "gpios.h"

void gpios_setup() {
    pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
}