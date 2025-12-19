# ESP32 S3 Zero

Multi-tool project for the ESP32-S3-Zero development board featuring a menu-driven interface with distance measurement, level/inclination tool, and compass functionality.

## Hardware

- **Board**: Adafruit QT Py ESP32-S3 (4MB Flash, 2MB PSRAM)
- **Display**: SSD1306 OLED (128x64) via I2C
- **LED**: WS2812B NeoPixel (onboard)
- **Sensors**:
  - MPU6050 (Accelerometer/Gyroscope/Temperature) via I2C
  - QMC5883L (Magnetometer/Compass) via I2C
  - VL53L0X (Time-of-Flight Distance Sensor) via I2C
- **Inputs**: Boot button + 3 navigation buttons (UP, DOWN, ENTER)

## Pin Configuration

- **Boot Button**: GPIO 0
- **I2C SCL**: GPIO 1
- **I2C SDA**: GPIO 2
- **Compass DRDY**: GPIO 6
- **Button UP**: GPIO 8
- **Button ENTER**: GPIO 9
- **Button DOWN**: GPIO 10
- **VL53L0X XSHUT**: GPIO 12
- **NeoPixel LED**: GPIO 21

## Features

### Menu System
Navigate through modes using UP/DOWN buttons and select with ENTER:
- **Distance**: Real-time distance measurement (0-2500mm)
- **Level**: Tilt/inclination measurement with visual level indicator
- **Compass**: Magnetic heading display with compass rose

### Distance Mode
- Displays distance in millimeters or meters (auto-formats)
- Updates every 100ms
- Range: 0-2500mm
- Press ENTER to return to menu

### Level Mode
- Pitch and roll angle measurement
- Visual level indicator:
  - Two-circle level for near-flat orientations (<30°)
  - Water level effect for larger tilts (>30°)
- Displays temperature, pitch, and roll values
- Updates every 100ms
- Press ENTER to return to menu

### Compass Mode
- Real-time magnetic heading (0-360°)
- Visual compass rose with rotating arrow
- Cardinal direction markers (N, E, S, W)
- Updates every 100ms
- Press ENTER to return to menu

## Building

This project uses PlatformIO with the Arduino framework.

### Requirements

- PlatformIO IDE or CLI
- ESP32-S3 board support

### Libraries

All dependencies are defined in `platformio.ini`:
- Adafruit NeoPixel
- Adafruit SSD1306
- Adafruit GFX Library
- Adafruit MPU6050
- Adafruit VL53L0X

### Build and Upload

```bash
pio run -e esp32s3
pio run -e esp32s3 -t upload
```

### Monitor Serial Output

```bash
pio device monitor -e esp32s3
```

Serial output runs at 115200 baud and provides sensor status and debug information.

## Project Structure

```
src/
├── main.cpp          - Main application loop and menu logic
├── gpios.cpp/h       - GPIO pin definitions and setup
├── buttons.cpp/h     - Button debouncing and reading
├── oled.cpp/h        - OLED display functions
├── mpu6050.cpp/h     - MPU6050 sensor interface
├── compass.cpp/h     - QMC5883L compass sensor interface
├── vl53l0x.cpp/h     - VL53L0X distance sensor interface
└── ws2812b.cpp/h     - NeoPixel LED control
```

## Documentation

Hardware documentation and schematics are available in the `docs/` folder:
- ESP32-S3-Zero pinout diagram
- ESP32-S3-Zero schematics
- WS2812B LED datasheet

## Usage

1. Power on the device
2. Yellow LED blinks 3 times during initialization
3. Menu appears on OLED display
4. Use UP/DOWN buttons to navigate
5. Press ENTER to select a mode
6. Press ENTER again in any mode to return to menu

## Notes

- All sensors are optional - the device will continue to operate if sensors are not connected
- The OLED display is rotated 180° (rotation 2) for optimal viewing
- I2C bus speed is set to 100kHz for sensor compatibility
- Button debounce delay is 50ms
