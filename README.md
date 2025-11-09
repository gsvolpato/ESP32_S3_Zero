# ESP32 S3 Zero

ESP32 S3 Zero Projects. Each branch a different project.

## Branch: LED Automatic Color Change

Automatically cycles the onboard NeoPixel through a rainbow of colors using the Arduino framework and Adafruit NeoPixel library. Initializes GPIOs for the boot button and drives the LED with a 500 ms transition interval, packaging the logic in `ws2312b.cpp` and invoked from `main.cpp`.