#define DISABLE_ALL_LIBRARY_WARNINGS
#ifndef GPIOS_H
#define GPIOS_H

#define LED_PIN 21
#define BOOT_BUTTON_PIN 0
#define OLED_SCL_PIN 1
#define OLED_SDA_PIN 2
#define BUTTON_UP_PIN 8
#define BUTTON_ENTER_PIN 9
#define BUTTON_DOWN_PIN 10

void gpios_setup();



#endif // GPIOS_H