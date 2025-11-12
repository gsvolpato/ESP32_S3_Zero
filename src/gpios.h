#define DISABLE_ALL_LIBRARY_WARNINGS
#ifndef GPIOS_H
#define GPIOS_H

#define BOOT_BUTTON_PIN    0
#define I2C_SCL_PIN        1
#define I2C_SDA_PIN        2
#define COMPASS_DRDY_PIN   6
#define BUTTON_UP_PIN      8
#define BUTTON_ENTER_PIN   9
#define BUTTON_DOWN_PIN    10
#define LED_PIN            21
#define VL53L0X_XSHUT_PIN  12

void gpios_setup();



#endif // GPIOS_H