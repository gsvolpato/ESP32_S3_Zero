#define DISABLE_ALL_LIBRARY_WARNINGS
#include <Arduino.h>
#include "gpios.h"
#include "buttons.h"

namespace {
    constexpr unsigned long debounceDelay = 50;
    unsigned long lastUpDebounceTime = 0;
    unsigned long lastDownDebounceTime = 0;
    unsigned long lastEnterDebounceTime = 0;
    int lastUpState = HIGH;
    int lastDownState = HIGH;
    int lastEnterState = HIGH;
    bool upPressed = false;
    bool downPressed = false;
    bool enterPressed = false;
} // namespace

void buttonsSetup() {
    // Pins are already set up in gpios_setup()
}

ButtonState buttonsRead() {
    ButtonState result = BUTTON_NONE;
    unsigned long currentTime = millis();
    
    int upReading = digitalRead(BUTTON_UP_PIN);
    int downReading = digitalRead(BUTTON_DOWN_PIN);
    int enterReading = digitalRead(BUTTON_ENTER_PIN);
    
    if (upReading != lastUpState) {
        lastUpDebounceTime = currentTime;
    }
    if ((currentTime - lastUpDebounceTime) > debounceDelay) {
        if (upReading == LOW && !upPressed) {
            upPressed = true;
            result = BUTTON_UP_PRESSED;
        } else if (upReading == HIGH) {
            upPressed = false;
        }
    }
    lastUpState = upReading;
    
    if (downReading != lastDownState) {
        lastDownDebounceTime = currentTime;
    }
    if ((currentTime - lastDownDebounceTime) > debounceDelay) {
        if (downReading == LOW && !downPressed) {
            downPressed = true;
            if (result == BUTTON_NONE) {
                result = BUTTON_DOWN_PRESSED;
            }
        } else if (downReading == HIGH) {
            downPressed = false;
        }
    }
    lastDownState = downReading;
    
    if (enterReading != lastEnterState) {
        lastEnterDebounceTime = currentTime;
    }
    if ((currentTime - lastEnterDebounceTime) > debounceDelay) {
        if (enterReading == LOW && !enterPressed) {
            enterPressed = true;
            if (result == BUTTON_NONE) {
                result = BUTTON_ENTER_PRESSED;
            }
        } else if (enterReading == HIGH) {
            enterPressed = false;
        }
    }
    lastEnterState = enterReading;
    
    return result;
}

