#ifndef OLED_H
#define OLED_H

bool oledSetup();
void oledDisplayText(const char *text);
void oledDisplayLines(const char *line1, const char *line2);
void oledClear();
void oledDisplayMpu6050Data(float pitch, float roll, float gyroX, float gyroY, float gyroZ, float temperature);
void oledDisplayMenu(int selectedIndex, const char* line1, const char* line2, const char* line3);
void oledDisplayCompass(float heading);
void oledDisplayDistance(uint16_t distanceMm);

#endif // OLED_H

