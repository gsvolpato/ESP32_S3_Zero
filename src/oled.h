#ifndef OLED_H
#define OLED_H

bool oledSetup();
void oledDisplayText(const char *text);
void oledDisplayLines(const char *line1, const char *line2);
void oledClear();
void oledDisplayMpu6050Data(float pitch, float roll, float gyroX, float gyroY, float gyroZ, float temperature);

#endif // OLED_H

