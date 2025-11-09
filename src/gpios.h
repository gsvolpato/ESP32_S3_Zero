#define DISABLE_ALL_LIBRARY_WARNINGS
#ifndef GPIOS_H
#define GPIOS_H

#include <Arduino.h>

#define LED_PIN PIN_NEOPIXEL
#define BOOT_BUTTON_PIN 0

void gpiosSetup();

#endif // GPIOS_H