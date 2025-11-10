#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include "gpios.h"
#include "oled.h"

namespace {
constexpr int screenWidth = 128;
constexpr int screenHeight = 64;
constexpr int oledReset = -1;

Adafruit_SSD1306 display(screenWidth, screenHeight, &Wire, oledReset);
} // namespace

bool oledSetup() {
    Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);
    delay(10);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 allocation failed"));
        return false;
    }
    
    display.setRotation(2);
    display.clearDisplay();
    display.display();
    return true;
}

void oledDisplayText(const char *text) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(text);
    display.display();
}

void oledDisplayLines(const char *line1, const char *line2) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(line1);
    display.setCursor(0, 16);
    display.println(line2);
    display.display();
}

void oledClear() {
    display.clearDisplay();
    display.display();
}

void oledDisplayMpu6050Data(float pitch, float roll, float gyroX, float gyroY, float gyroZ, float temperature) {
    char buffer[64];
    
    display.clearDisplay();
    
    // Blue area is y=0-47 (48 pixels), yellow strip is y=48-63 (16 pixels)
    constexpr int blueAreaHeight = 48;
    constexpr float tiltThreshold = 5.0; // Show level when tilt > 5 degrees
    
    // Check if device is tilted enough to show level (not flat on table)
    float absPitch = fabs(pitch);
    float absRoll = fabs(roll);
    
    if (absPitch > tiltThreshold || absRoll > tiltThreshold) {
        // Water level works in all orientations
        // The water level line is perpendicular to the tilt direction
        // Water (white) collects on the "low" side where gravity points
        
        float centerX = screenWidth / 2.0;
        float centerY = blueAreaHeight / 2.0;
        
        // Calculate tilt direction from pitch and roll
        // atan2(pitch, roll) gives the direction of tilt in screen coordinates
        // Pitch maps to screen Y (vertical/portrait), Roll maps to screen X (horizontal/landscape)
        float tiltDirectionRad = atan2(pitch, roll);
        
        // The water level line is perpendicular to the tilt direction
        // Line angle = tilt direction + 90 degrees
        float lineAngleRad = tiltDirectionRad + M_PI / 2.0;
        
        // Calculate tilt magnitude for visual feedback
        float tiltMagnitude = sqrt(pitch * pitch + roll * roll);
        float normalizedTilt = constrain(tiltMagnitude / 45.0, 0.0, 1.0);
        
        // Slight offset of line based on tilt (water shifts toward low side)
        float offsetDistance = normalizedTilt * 8.0;
        float offsetX = cos(tiltDirectionRad) * offsetDistance;
        float offsetY = sin(tiltDirectionRad) * offsetDistance;
        
        // Calculate line endpoints using parametric form to avoid slope issues
        // Line passes through center, offset by tilt, and extends diagonally
        float diagonalLength = sqrt(screenWidth * screenWidth + blueAreaHeight * blueAreaHeight);
        float lineLength = diagonalLength * 1.5; // Extend beyond screen to ensure coverage
        
        // Calculate two points on the line
        float lineX1 = centerX + offsetX - cos(lineAngleRad) * lineLength / 2.0;
        float lineY1 = centerY + offsetY - sin(lineAngleRad) * lineLength / 2.0;
        float lineX2 = centerX + offsetX + cos(lineAngleRad) * lineLength / 2.0;
        float lineY2 = centerY + offsetY + sin(lineAngleRad) * lineLength / 2.0;
        
        // Calculate line direction vector
        float lineDx = lineX2 - lineX1;
        float lineDy = lineY2 - lineY1;
        
        // Normalize line direction (handle edge case where line might be degenerate)
        float lineLen = sqrt(lineDx * lineDx + lineDy * lineDy);
        if (lineLen < 0.001) {
            // Degenerate case - line is a point, don't draw
            lineDx = 1.0;
            lineDy = 0.0;
            lineLen = 1.0;
        }
        lineDx /= lineLen;
        lineDy /= lineLen;
        
        // Test center point offset in tilt direction to determine water side
        float testOffset = 25.0;
        float testX = centerX + cos(tiltDirectionRad) * testOffset;
        float testY = centerY + sin(tiltDirectionRad) * testOffset;
        
        // Calculate which side of line the test point is on using cross product
        float testDx = testX - lineX1;
        float testDy = testY - lineY1;
        float crossProduct = lineDx * testDy - lineDy * testDx;
        bool waterOnPositiveSide = (crossProduct > 0);
        
        // Fill the blue area
        for (int y = 0; y < blueAreaHeight; y++) {
            for (int x = 0; x < screenWidth; x++) {
                // Calculate which side of line this pixel is on
                float pixelDx = x - lineX1;
                float pixelDy = y - lineY1;
                float pixelCrossProduct = lineDx * pixelDy - lineDy * pixelDx;
                
                bool onWaterSide = (waterOnPositiveSide && pixelCrossProduct > 0) || 
                                  (!waterOnPositiveSide && pixelCrossProduct <= 0);
                
                display.drawPixel(x, y, onWaterSide ? SSD1306_WHITE : SSD1306_BLACK);
            }
        }
    }
    // If flat (both angles < threshold), blue area remains black (nothing displayed)
    
    // Display compact format on yellow strip at bottom (y=48-63, 16 pixels)
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 48);
    snprintf(buffer, sizeof(buffer), "T %dC X %d  Y %d", 
             (int)temperature, (int)pitch, (int)roll);
    display.println(buffer);
    
    display.display();
}

