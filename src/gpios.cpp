#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include "gpios.h"

void gpios_setup() {
    pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
    pinMode(MPU6050_AD0_PIN, OUTPUT);
    digitalWrite(MPU6050_AD0_PIN, LOW);
    pinMode(MPU6050_INT_PIN, INPUT_PULLUP);
}