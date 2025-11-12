#ifndef VL53L0X_H
#define VL53L0X_H

#include "gpios.h"

bool vl53l0x_init();
uint16_t vl53l0x_readDistance();
bool vl53l0x_isConnected();

#endif // VL53L0X_H

