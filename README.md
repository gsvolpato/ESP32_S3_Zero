# ESP32 S3 Zero

ESP32 S3 Zero Projects. Each branch a different project.

## VL53L0X Branch

This branch implements a VL53L0X Time-of-Flight (ToF) distance sensor using the Adafruit VL53L0X library.

### Features

- VL53L0X distance sensor integration
- I2C communication on custom pins (SDA: GPIO 2, SCL: GPIO 1)
- Serial monitor output of distance readings in millimeters
- Out-of-range detection and validation (max range: 2500mm)
- Boot button functionality for testing

### Hardware Configuration

- **Board**: Adafruit QT Py ESP32-S3 N4R2
- **I2C Pins**:
  - SDA: GPIO 2 (`GENERAL_SDA_PIN`)
  - SCL: GPIO 1 (`GENERAL_SCL_PIN`)
- **VL53L0X Sensor**: Connected via I2C at default address 0x29
- **Boot Button**: GPIO 0 (with pull-up)

### Software Components

- `src/main.cpp`: Main program loop with sensor reading and serial output
- `src/vl53l0x.h/cpp`: VL53L0X sensor driver implementation
- `src/gpios.h/cpp`: GPIO pin definitions and setup

### Library Dependencies

- `adafruit/Adafruit_VL53L0X @ ^1.2.4`

### Usage

1. Connect the VL53L0X sensor to the ESP32:
   - VCC → 3.3V or 5V
   - GND → GND
   - SDA → GPIO 2
   - SCL → GPIO 1
   - XSHUT → Leave unconnected or pull high

2. Upload the code to the ESP32 using PlatformIO

3. Open Serial Monitor at 115200 baud to view distance readings

### Distance Readings

- Valid range: 0-2500mm (0-2.5 meters)
- Readings beyond 2500mm are filtered out as out-of-range
- Distance is displayed in millimeters
- Status code validation prevents invalid readings (e.g., wraparound values)

### Serial Output

The program outputs:
- I2C bus scan results during initialization
- Distance readings in millimeters
- "Distance reading out of range" message for invalid readings
- Boot button press notifications
