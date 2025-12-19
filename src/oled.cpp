#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include <SPI.h>
#include <string.h>
#include <TFT_eSPI.h>
#include "gpios.h"
#include "oled.h"

namespace {
constexpr int screenWidth = 240;
constexpr int screenHeight = 240;

// TFT_eSPI display object
TFT_eSPI display = TFT_eSPI();
} // namespace


bool oledSetup() {
    Serial.println("=== Display Initialization ===");
    Serial.print("RESET pin: "); Serial.println(DISPLAY_RESET);
    Serial.print("DC pin: "); Serial.println(DISPLAY_DC);
    Serial.print("BK pin: "); Serial.println(DISPLAY_BK);
    Serial.print("SCL pin: "); Serial.println(DISPLAY_SCL_PIN);
    Serial.print("SDA pin: "); Serial.println(DISPLAY_SDA_PIN);
    Serial.println("Using hardware SPI with custom pins");
    
    // Perform hardware reset sequence
    Serial.println("Performing hardware reset...");
    digitalWrite(DISPLAY_RESET, LOW);
    delay(10);
    digitalWrite(DISPLAY_RESET, HIGH);
    delay(150); // Give display time to stabilize after reset
    
    // Ensure backlight is on
    digitalWrite(DISPLAY_BK, HIGH);
    Serial.println("Backlight enabled");
    delay(100);
    
    // TFT_eSPI manages SPI initialization internally via build flags
    // Do NOT call SPI.begin() manually - it causes StoreProhibited errors
    Serial.println("Calling display.init()...");
    display.init();  // Initialize display - width/height come from build flags
    delay(100);
    
    Serial.println("Setting rotation...");
    display.setRotation(2); // Rotate 180 degrees
    delay(50);
    
    // Clear display to black
    display.fillScreen(TFT_BLACK);
    
    Serial.println("Display initialized successfully!");
    
    return true;
}

void oledDisplayText(const char *text) {
    display.fillScreen(TFT_BLACK);
    display.setTextSize(2);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setCursor(0, 0);
    display.println(text);
}

void oledDisplayLines(const char *line1, const char *line2) {
    display.fillScreen(TFT_BLACK);
    display.setTextSize(2);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setCursor(0, 0);
    display.println(line1);
    display.setCursor(0, 30);
    display.println(line2);
}

void oledClear() {
    display.fillScreen(TFT_BLACK);
}

void oledDisplayMpu6050Data(float pitch, float roll, float gyroX, float gyroY, float gyroZ, float temperature) {
    char buffer[64];
    
    // Don't clear entire screen - only clear the areas we're updating to reduce blinking
    // Blue area is top portion (y=0-200), info strip is bottom (y=200-240)
    constexpr int blueAreaHeight = 200;
    constexpr float tiltThreshold = 30.0; // Show water level when tilt > 30 degrees
    
    // Clear the blue area (will be redrawn below)
    display.fillRect(0, 0, screenWidth, blueAreaHeight, TFT_BLACK);
    
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
        float offsetDistance = normalizedTilt * 15.0;
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
        
        // Fill the blue area with blue background
        display.fillRect(0, 0, screenWidth, blueAreaHeight, TFT_BLUE);
        
        // Draw water (white) on one side of the line - optimized using scanline approach
        // Pre-calculate line equation: ax + by + c = 0, where (a,b) is perpendicular vector
        float a = -lineDy;  // Perpendicular to line direction
        float b = lineDx;
        float c = -(a * lineX1 + b * lineY1);
        
        // Test center point offset in tilt direction to determine water side
        float testOffset = 40.0;
        float testX = centerX + cos(tiltDirectionRad) * testOffset;
        float testY = centerY + sin(tiltDirectionRad) * testOffset;
        
        // Calculate which side of line the test point is on using line equation
        float testValue = a * testX + b * testY + c;
        bool waterOnPositiveSide = (testValue > 0);
        
        // Simplified approach: draw water as a filled half-plane using triangles or large rectangles
        // This avoids slow pixel-by-pixel operations
        // Calculate line intersection points with screen edges
        float intersections[4][2]; // Up to 4 intersection points
        int numIntersections = 0;
        
        // Check intersections with screen edges
        // Top edge (y=0)
        if (fabs(b) > 0.001) {
            float x = -(a * 0 + c) / b;
            if (x >= 0 && x < screenWidth) {
                intersections[numIntersections][0] = x;
                intersections[numIntersections][1] = 0;
                numIntersections++;
            }
        }
        // Bottom edge (y=blueAreaHeight)
        if (fabs(b) > 0.001) {
            float x = -(a * blueAreaHeight + c) / b;
            if (x >= 0 && x < screenWidth) {
                intersections[numIntersections][0] = x;
                intersections[numIntersections][1] = blueAreaHeight;
                numIntersections++;
            }
        }
        // Left edge (x=0)
        if (fabs(a) > 0.001) {
            float y = -(b * 0 + c) / a;
            if (y >= 0 && y < blueAreaHeight) {
                intersections[numIntersections][0] = 0;
                intersections[numIntersections][1] = y;
                numIntersections++;
            }
        }
        // Right edge (x=screenWidth)
        if (fabs(a) > 0.001) {
            float y = -(b * screenWidth + c) / a;
            if (y >= 0 && y < blueAreaHeight) {
                intersections[numIntersections][0] = screenWidth;
                intersections[numIntersections][1] = y;
                numIntersections++;
            }
        }
        
        // Fill water side using triangles or rectangles - much faster than pixel loops
        if (numIntersections >= 2) {
            // Fill entire area with water (white)
            display.fillRect(0, 0, screenWidth, blueAreaHeight, TFT_WHITE);
            
            // Clear non-water side by drawing blue triangles/rectangles
            // Determine which corner is on the non-water side
            float cornerTest = a * 0 + b * 0 + c;
            bool topLeftOnWater = (cornerTest > 0) == waterOnPositiveSide;
            
            // Draw blue background on non-water side using fillTriangle for irregular shapes
            // For simplicity and speed, use large rectangles that cover most of the non-water area
            if (topLeftOnWater) {
                // Water is top-left, clear bottom-right area
                if (intersections[0][0] < screenWidth/2 && intersections[1][0] < screenWidth/2) {
                    // Line goes through left side - clear right area
                    int clearX = (int)intersections[0][0];
                    if (clearX < screenWidth) {
                        display.fillRect(clearX, 0, screenWidth - clearX, blueAreaHeight, TFT_BLUE);
                    }
                } else {
                    // Line goes through right/bottom - clear accordingly
                    int clearY = (int)intersections[0][1];
                    if (clearY < blueAreaHeight) {
                        display.fillRect(0, clearY, screenWidth, blueAreaHeight - clearY, TFT_BLUE);
                    }
                }
            } else {
                // Water is bottom-right, clear top-left area
                if (intersections[0][0] > screenWidth/2 && intersections[1][0] > screenWidth/2) {
                    // Line goes through right side - clear left area
                    int clearX = (int)intersections[0][0];
                    if (clearX > 0) {
                        display.fillRect(0, 0, clearX, blueAreaHeight, TFT_BLUE);
                    }
                } else {
                    // Line goes through left/top - clear accordingly
                    int clearY = (int)intersections[0][1];
                    if (clearY > 0) {
                        display.fillRect(0, 0, screenWidth, clearY, TFT_BLUE);
                    }
                }
            }
        } else {
            // Fallback: fill entire area with water
            display.fillRect(0, 0, screenWidth, blueAreaHeight, TFT_WHITE);
        }
    } else {
        // Apple-style two-circle level for flat/near-flat orientation
        float centerX = screenWidth / 2.0;
        float centerY = blueAreaHeight / 2.0;
        
        // Fill background with blue
        display.fillRect(0, 0, screenWidth, blueAreaHeight, TFT_BLUE);
        
        // Calculate tilt magnitude and direction
        float tiltMagnitude = sqrt(pitch * pitch + roll * roll);
        
        // Fill screen with white when perfectly level (0°)
        if (tiltMagnitude < 0.5) {
            // Fill the blue area with white to indicate perfect level
            display.fillRect(0, 0, screenWidth, blueAreaHeight, TFT_WHITE);
        } else {
            // Circle parameters
            constexpr int circleRadius = 35;
            constexpr float maxOffset = 15.0; // Maximum offset for circles based on tilt
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
            display.fillCircle(circle1X, circle1Y, circleRadius, TFT_WHITE);
            display.fillCircle(circle2X, circle2Y, circleRadius, TFT_WHITE);
            
            // Draw the bubble (intersection) as a smaller filled circle at the center
            // This is MUCH faster than pixel-by-pixel loops and looks good
            float bubbleCenterX = (circle1X + circle2X) / 2.0;
            float bubbleCenterY = (circle1Y + circle2Y) / 2.0;
            
            // Calculate bubble radius based on circle separation
            float circleDist = sqrt((circle2X - circle1X) * (circle2X - circle1X) + 
                                   (circle2Y - circle1Y) * (circle2Y - circle1Y));
            // When circles are close, bubble is large; when far, bubble is small
            // Simple approximation: bubble radius = circleRadius - circleDist/2
            int bubbleRadius = (int)(circleRadius - circleDist / 2.5);
            if (bubbleRadius > 0) {
                display.fillCircle((int)bubbleCenterX, (int)bubbleCenterY, bubbleRadius, TFT_BLUE);
            }
            
            // Draw circle outlines to ensure clean edges
            display.drawCircle(circle1X, circle1Y, circleRadius, TFT_WHITE);
            display.drawCircle(circle2X, circle2Y, circleRadius, TFT_WHITE);
            
            // Display tilt angle in the bubble (already calculated above)
            
            // Show angle with degree symbol
            int displayAngle = (int)round(tiltMagnitude);
            display.setTextSize(2);
            display.setTextColor(TFT_WHITE, TFT_BLUE);
            snprintf(buffer, sizeof(buffer), "%d", displayAngle);
            int textWidth = strlen(buffer) * 12; // Approximate character width for text size 2
            display.setCursor(bubbleCenterX - textWidth / 2 - 6, bubbleCenterY - 6);
            display.print(buffer);
            // Draw degree symbol (small circle)
            display.setCursor(bubbleCenterX + textWidth / 2 - 2, bubbleCenterY - 8);
            display.print("o");
        }
    }
    
    // Display compact format on info strip at bottom (y=200-240, 40 pixels)
    display.setTextSize(1);
    display.setTextColor(TFT_YELLOW, TFT_BLACK);
    display.fillRect(0, blueAreaHeight, screenWidth, screenHeight - blueAreaHeight, TFT_BLACK);
    display.setCursor(5, blueAreaHeight + 5);
    snprintf(buffer, sizeof(buffer), "T %dC X %d  Y %d", 
             (int)temperature, (int)pitch, (int)roll);
    display.println(buffer);
}

// Static cache variables (outside function to allow reset)
namespace {
    static int menuLastSelectedIndex = -1;
    static bool menuInitialized = false;
}

void oledDisplayMenuReset() {
    menuLastSelectedIndex = -1;
    menuInitialized = false;
}

void oledDisplayMenu(int selectedIndex, const char* line1, const char* line2, const char* line3) {
    display.setTextSize(3);
    
    // Initialize menu only once
    if (!menuInitialized) {
        display.fillScreen(TFT_BLACK);
        
        // Draw all three lines initially (without highlight first, then highlight selected)
        display.setTextColor(TFT_WHITE, TFT_BLACK);
        
        // Line 1
        display.setCursor(10, 20);
        if (selectedIndex == 0) {
            display.setTextColor(TFT_BLUE, TFT_BLACK);
            display.print("> ");
            display.setTextColor(TFT_WHITE, TFT_BLACK);
        } else {
            display.print("  ");
        }
        display.print(line1);
        
        // Line 2
        display.setCursor(10, 80);
        if (selectedIndex == 1) {
            display.setTextColor(TFT_BLUE, TFT_BLACK);
            display.print("> ");
            display.setTextColor(TFT_WHITE, TFT_BLACK);
        } else {
            display.print("  ");
        }
        display.print(line2);
        
        // Line 3
        display.setCursor(10, 140);
        if (selectedIndex == 2) {
            display.setTextColor(TFT_BLUE, TFT_BLACK);
            display.print("> ");
            display.setTextColor(TFT_WHITE, TFT_BLACK);
        } else {
            display.print("  ");
        }
        display.print(line3);
        
        menuInitialized = true;
        menuLastSelectedIndex = selectedIndex;
        return; // Done with initial draw
    }
    
    // Only update if selection changed
    if (menuLastSelectedIndex != selectedIndex) {
        // Update old selection line - remove highlight (change "> " to "  ")
        if (menuLastSelectedIndex >= 0 && menuLastSelectedIndex < 3) {
            int yPos = menuLastSelectedIndex * 60 + 20; // 20, 80, 140
            const char* lineText = (menuLastSelectedIndex == 0) ? line1 : 
                                   (menuLastSelectedIndex == 1) ? line2 : line3;
            
            // Clear and redraw the line without highlight
            display.fillRect(10, yPos - 2, 200, 28, TFT_BLACK); // Clear entire line area
            display.setCursor(10, yPos);
            display.setTextColor(TFT_WHITE, TFT_BLACK);
            display.print("  "); // Replace "> " with "  "
            display.print(lineText); // Redraw text
        }
        
        // Update new selection line - add highlight (change "  " to "> ")
        int yPos = selectedIndex * 60 + 20;
        const char* lineText = (selectedIndex == 0) ? line1 : 
                               (selectedIndex == 1) ? line2 : line3;
        
        // Clear and redraw the line with highlight
        display.fillRect(10, yPos - 2, 200, 28, TFT_BLACK); // Clear entire line area
        display.setCursor(10, yPos);
        display.setTextColor(TFT_BLUE, TFT_BLACK);
        display.print("> "); // Replace "  " with "> "
        display.setTextColor(TFT_WHITE, TFT_BLACK);
        display.print(lineText); // Redraw text in white
        
        menuLastSelectedIndex = selectedIndex;
    }
}

void oledDisplayCompass(float heading) {
    // Static variables to cache compass background and last arrow position
    static bool compassBackgroundDrawn = false;
    static float lastHeading = -999.0; // Invalid initial value
    static float lastArrowAngle = 0.0;
    
    constexpr int centerX = screenWidth / 2;
    constexpr int centerY = screenHeight / 2;
    constexpr int radius = 80;
    constexpr float degToRad = M_PI / 180.0;
    constexpr int arrowLength = radius - 10;
    constexpr int arrowHeadSize = 12;
    constexpr int tailLength = 15;
    
    // Only redraw background once, or if heading changed significantly
    if (!compassBackgroundDrawn || fabs(heading - lastHeading) > 45.0) {
        display.fillScreen(TFT_BLACK);
        
        // Draw outer compass circle
        display.drawCircle(centerX, centerY, radius, TFT_WHITE);
        display.drawCircle(centerX, centerY, radius - 1, TFT_WHITE);
        
        // Draw fixed compass rose - N, E, S, W markers (these stay fixed)
        display.setTextSize(2);
        display.setTextColor(TFT_WHITE, TFT_BLACK);
        
        // North marker at top
        display.setCursor(centerX - 6, centerY - radius - 20);
        display.print("N");
        
        // East marker at right
        display.setCursor(centerX + radius + 10, centerY - 8);
        display.print("E");
        
        // South marker at bottom
        display.setCursor(centerX - 6, centerY + radius + 8);
        display.print("S");
        
        // West marker at left
        display.setCursor(centerX - radius - 20, centerY - 8);
        display.print("W");
        
        // Draw tick marks for cardinal directions
        constexpr int tickLength = 8;
        display.drawLine(centerX, centerY - radius, centerX, centerY - radius + tickLength, TFT_WHITE);
        display.drawLine(centerX + radius, centerY, centerX + radius - tickLength, centerY, TFT_WHITE);
        display.drawLine(centerX, centerY + radius, centerX, centerY + radius - tickLength, TFT_WHITE);
        display.drawLine(centerX - radius, centerY, centerX - radius + tickLength, centerY, TFT_WHITE);
        
        // Draw intermediate tick marks (45 degrees) - unrolled loop for speed
        float angles[7] = {45.0, 90.0, 135.0, 180.0, 225.0, 270.0, 315.0};
        for (int i = 0; i < 7; i++) {
            float angle = angles[i] * degToRad;
            int x1 = centerX + (int)(cos(angle) * radius);
            int y1 = centerY + (int)(sin(angle) * radius);
            int x2 = centerX + (int)(cos(angle) * (radius - 4));
            int y2 = centerY + (int)(sin(angle) * (radius - 4));
            display.drawLine(x1, y1, x2, y2, TFT_WHITE);
        }
        
        compassBackgroundDrawn = true;
    }
    
    // Erase old arrow by drawing a black circle over it (faster than individual line clears)
    if (lastHeading > -900.0) {
        float oldArrowAngle = lastArrowAngle;
        int oldArrowTipX = centerX + (int)(cos(oldArrowAngle) * arrowLength);
        int oldArrowTipY = centerY + (int)(sin(oldArrowAngle) * arrowLength);
        // Clear old arrow area with a small black circle
        display.fillCircle(oldArrowTipX, oldArrowTipY, arrowHeadSize + 5, TFT_BLACK);
        display.fillCircle(centerX, centerY, tailLength + 5, TFT_BLACK);
        // Redraw center pivot and background circle where needed
        display.drawCircle(centerX, centerY, radius, TFT_WHITE);
    }
    
    // Calculate new arrow direction
    float arrowAngle = (heading + 90.0) * degToRad;
    lastHeading = heading;
    lastArrowAngle = arrowAngle;
    
    // Draw new arrow
    int arrowTipX = centerX + (int)(cos(arrowAngle) * arrowLength);
    int arrowTipY = centerY + (int)(sin(arrowAngle) * arrowLength);
    
    // Draw main arrow shaft
    display.drawLine(centerX, centerY, arrowTipX, arrowTipY, TFT_RED);
    
    // Draw arrowhead (triangle pointing forward)
    float perpAngle = arrowAngle + M_PI / 2.0;
    int headBase1X = arrowTipX - (int)(cos(arrowAngle) * arrowHeadSize) + (int)(cos(perpAngle) * 6);
    int headBase1Y = arrowTipY - (int)(sin(arrowAngle) * arrowHeadSize) + (int)(sin(perpAngle) * 6);
    int headBase2X = arrowTipX - (int)(cos(arrowAngle) * arrowHeadSize) - (int)(cos(perpAngle) * 6);
    int headBase2Y = arrowTipY - (int)(sin(arrowAngle) * arrowHeadSize) - (int)(sin(perpAngle) * 6);
    
    display.fillTriangle(arrowTipX, arrowTipY, headBase1X, headBase1Y, headBase2X, headBase2Y, TFT_RED);
    
    // Draw arrow tail
    int tailX = centerX - (int)(cos(arrowAngle) * tailLength);
    int tailY = centerY - (int)(sin(arrowAngle) * tailLength);
    display.drawLine(centerX, centerY, tailX, tailY, TFT_RED);
    
    // Draw center pivot point
    display.fillCircle(centerX, centerY, 5, TFT_RED);
    display.drawCircle(centerX, centerY, 5, TFT_BLACK);
    
    // Update heading text (only if changed significantly)
    static int lastDisplayHeading = -999;
    int displayHeading = (int)heading;
    if (displayHeading != lastDisplayHeading) {
        // Clear old text area
        display.fillRect(centerX - 60, screenHeight - 45, 120, 30, TFT_BLACK);
        // Draw new text
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.0f deg", heading);
        int textWidth = strlen(buffer) * 12;
        display.setTextSize(2);
        display.setTextColor(TFT_YELLOW, TFT_BLACK);
        display.setCursor(centerX - textWidth / 2, screenHeight - 25);
        display.print(buffer);
        lastDisplayHeading = displayHeading;
    }
}

void oledDisplayDistance(uint16_t distanceMm) {
    static bool distanceTitleDrawn = false;
    if (!distanceTitleDrawn) {
        display.fillScreen(TFT_BLACK);
        display.setTextSize(2);
        display.setTextColor(TFT_WHITE, TFT_BLACK);
        display.setCursor(20, 30);
        display.print("Distance");
        distanceTitleDrawn = true;
    }
    
    // Only update the distance value area
    display.fillRect(0, 90, screenWidth, 100, TFT_BLACK); // Clear area for distance value
    
    // Display distance in mm
    char buffer[32];
    if (distanceMm > 0) {
        if (distanceMm >= 1000) {
            float distanceM = distanceMm / 1000.0;
            snprintf(buffer, sizeof(buffer), "%.2f m", distanceM);
        } else {
            snprintf(buffer, sizeof(buffer), "%d mm", distanceMm);
        }
    } else {
        snprintf(buffer, sizeof(buffer), "No reading");
    }
    
    // Center the distance text with larger font
    display.setTextSize(4);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    int textWidth = strlen(buffer) * 24; // Approximate for text size 4
    int centerX = screenWidth / 2;
    display.setCursor(centerX - textWidth / 2, 120);
    display.print(buffer);
}

// Helper functions for status display
void oledFillScreen(uint16_t color) {
    display.fillScreen(color);
}

void oledSetTextColor(uint16_t color) {
    display.setTextColor(color, TFT_BLACK);
}

void oledSetTextSize(uint8_t size) {
    display.setTextSize(size);
}

void oledSetCursor(int16_t x, int16_t y) {
    display.setCursor(x, y);
}

void oledPrint(const char* text) {
    display.print(text);
}

void oledPrintln(const char* text) {
    display.println(text);
}

void oledDisplayError(const char* message) {
    display.fillScreen(OLED_BLACK);
    display.setTextSize(2);
    display.setTextColor(OLED_RED, OLED_BLACK);
    
    // Center the error message
    int textWidth = strlen(message) * 12; // Approximate for text size 2
    int centerX = screenWidth / 2;
    display.setCursor(centerX - textWidth / 2, screenHeight / 2 - 30);
    display.println(message);
    
    // Display subtitle
    display.setTextSize(1);
    display.setTextColor(OLED_YELLOW, OLED_BLACK);
    display.setCursor(centerX - 60, screenHeight / 2 + 20);
    display.println("Press ENTER to return");
}
