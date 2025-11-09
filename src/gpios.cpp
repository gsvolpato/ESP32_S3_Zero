#define DISABLE_ALL_LIBRARY_WARNINGS
#include "gpios.h"

void gpiosSetup() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
}