#ifndef OLED_H
#define OLED_H

bool oledSetup();
void oledDisplayText(const char *text);
void oledDisplayLines(const char *line1, const char *line2);

#endif // OLED_H

