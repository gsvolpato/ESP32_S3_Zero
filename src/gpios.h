#define DISABLE_ALL_LIBRARY_WARNINGS
#ifndef GPIOS_H
#define GPIOS_H

#define BOOT_BUTTON_PIN    0

#define DISPLAY_SCL_PIN    1
#define DISPLAY_SDA_PIN    2
#define DISPLAY_RESET      3

#define DISPLAY_DC         4
#define DISPLAY_BK         5


#define I2C_SCL_PIN        6
#define I2C_SDA_PIN        7

#define BUTTON_UP_PIN      8
#define BUTTON_ENTER_PIN   9
#define BUTTON_DOWN_PIN    10

#define VL53L0X_XSHUT_PIN  12
#define COMPASS_DRDY_PIN   13

#define LED_PIN            21

void gpios_setup();



#endif // GPIOS_H