#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <Wire.h>
#include <string.h>
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
    constexpr float tiltThreshold = 30.0; // Show water level when tilt > 30 degrees
    
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
    } else {
        // Apple-style two-circle level for flat/near-flat orientation
        float centerX = screenWidth / 2.0;
        float centerY = blueAreaHeight / 2.0;
        
        // Calculate tilt magnitude and direction
        float tiltMagnitude = sqrt(pitch * pitch + roll * roll);
        
        // Fill screen with white when perfectly level (0°)
        if (tiltMagnitude < 0.5) {
            // Fill the blue area with white to indicate perfect level
            display.fillRect(0, 0, screenWidth, blueAreaHeight, SSD1306_WHITE);
        } else {
            // Circle parameters
            constexpr int circleRadius = 20;
            constexpr float maxOffset = 8.0; // Maximum offset for circles based on tilt
            constexpr float maxTiltForCircles = 30.0; // Use circles up to 30 degrees
            
            float tiltDirectionRad = atan2(pitch, roll);
            
            // Normalize tilt for circle offset (0-30 degrees maps to 0-maxOffset)
            float normalizedTilt = constrain(tiltMagnitude / maxTiltForCircles, 0.0, 1.0);
            float offsetDistance = normalizedTilt * maxOffset;
            
            // Calculate offset for the two circles
            // Circle 1 offset: opposite to tilt direction (top-left when tilted)
            // Circle 2 offset: same as tilt direction (bottom-right when tilted)
            float circle1OffsetX = -cos(tiltDirectionRad) * offsetDistance;
            float circle1OffsetY = -sin(tiltDirectionRad) * offsetDistance;
            float circle2OffsetX = cos(tiltDirectionRad) * offsetDistance;
            float circle2OffsetY = sin(tiltDirectionRad) * offsetDistance;
            
            int circle1X = centerX + circle1OffsetX;
            int circle1Y = centerY + circle1OffsetY;
            int circle2X = centerX + circle2OffsetX;
            int circle2Y = centerY + circle2OffsetY;
            
            // Fill both circles with white
            display.fillCircle(circle1X, circle1Y, circleRadius, SSD1306_WHITE);
            display.fillCircle(circle2X, circle2Y, circleRadius, SSD1306_WHITE);
            
            // Calculate and unfill the intersection (black bubble) so text is visible
            // The intersection is where both circles overlap - leave this area black
            for (int y = 0; y < blueAreaHeight; y++) {
                for (int x = 0; x < screenWidth; x++) {
                    // Distance from pixel to center of each circle
                    float dist1 = sqrt((x - circle1X) * (x - circle1X) + (y - circle1Y) * (y - circle1Y));
                    float dist2 = sqrt((x - circle2X) * (x - circle2X) + (y - circle2Y) * (y - circle2Y));
                    
                    // Pixel is in intersection if it's inside both circles - make it black
                    if (dist1 <= circleRadius && dist2 <= circleRadius) {
                        display.drawPixel(x, y, SSD1306_BLACK);
                    }
                }
            }
            
            // Draw circle outlines to ensure clean edges
            display.drawCircle(circle1X, circle1Y, circleRadius, SSD1306_WHITE);
            display.drawCircle(circle2X, circle2Y, circleRadius, SSD1306_WHITE);
            
            // Display tilt angle in the bubble (intersection center)
            float bubbleCenterX = (circle1X + circle2X) / 2.0;
            float bubbleCenterY = (circle1Y + circle2Y) / 2.0;
            
            // Show angle with degree symbol
            int displayAngle = (int)round(tiltMagnitude);
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            snprintf(buffer, sizeof(buffer), "%d", displayAngle);
            int textWidth = strlen(buffer) * 6; // Approximate character width
            display.setCursor(bubbleCenterX - textWidth / 2 - 3, bubbleCenterY - 3);
            display.print(buffer);
            // Draw degree symbol (small circle)
            display.setCursor(bubbleCenterX + textWidth / 2 - 1, bubbleCenterY - 4);
            display.print("o");
        }
    }
    
    // Display compact format on yellow strip at bottom (y=48-63, 16 pixels)
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 48);
    snprintf(buffer, sizeof(buffer), "T %dC X %d  Y %d", 
             (int)temperature, (int)pitch, (int)roll);
    display.println(buffer);
    
    display.display();
}

void oledDisplayMenu(int selectedIndex, const char* line1, const char* line2) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    // Display first line
    display.setCursor(0, 0);
    display.print(line1);
    if (selectedIndex == 0) {
        display.print(" <");
    }
    
    // Display second line
    display.setCursor(0, 16);
    display.print(line2);
    if (selectedIndex == 1) {
        display.print(" <");
    }
    
    display.display();
}

void oledDisplayCompass(float heading) {
    display.clearDisplay();
    
    constexpr int centerX = screenWidth / 2;
    constexpr int centerY = screenHeight / 2;
    constexpr int radius = 25;
    
    display.drawCircle(centerX, centerY, radius, SSD1306_WHITE);
    display.drawCircle(centerX, centerY, radius + 1, SSD1306_WHITE);
    
    constexpr float degToRad = M_PI / 180.0;
    float headingRad = heading * degToRad;
    
    int needleX = centerX + (int)(cos(headingRad) * radius * 0.8);
    int needleY = centerY + (int)(sin(headingRad) * radius * 0.8);
    
    display.drawLine(centerX, centerY, needleX, needleY, SSD1306_WHITE);
    
    display.fillCircle(centerX, centerY, 2, SSD1306_WHITE);
    
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.1f deg", heading);
    int textWidth = strlen(buffer) * 6;
    display.setCursor(centerX - textWidth / 2, centerY + radius + 5);
    display.print(buffer);
    
    display.setCursor(centerX - 2, centerY - radius - 10);
    display.print("N");
    
    display.display();
}

