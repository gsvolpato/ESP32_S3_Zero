#ifndef OLED_H
#define OLED_H

// Color constants matching TFT_eSPI RGB565 format
#define OLED_BLACK   0x0000
#define OLED_WHITE   0xFFFF
#define OLED_RED     0xF800
#define OLED_GREEN   0x07E0
#define OLED_BLUE    0x001F
#define OLED_YELLOW  0xFFE0

bool oledSetup();
void oledDisplayText(const char *text);
void oledDisplayLines(const char *line1, const char *line2);
void oledClear();
void oledDisplayMpu6050Data(float pitch, float roll, float gyroX, float gyroY, float gyroZ, float temperature);
void oledDisplayMenu(int selectedIndex, const char* line1, const char* line2, const char* line3);
void oledDisplayMenuReset(); // Reset menu cache when switching modes
void oledDisplayCompass(float heading);
void oledDisplayDistance(uint16_t distanceMm);
void oledDisplayError(const char* message);

// Helper functions for status display
void oledFillScreen(uint16_t color);
void oledSetTextColor(uint16_t color);
void oledSetTextSize(uint8_t size);
void oledSetCursor(int16_t x, int16_t y);
void oledPrint(const char* text);
void oledPrintln(const char* text);

#endif // OLED_H

