#ifndef BUTTONS_H
#define BUTTONS_H

enum ButtonState {
    BUTTON_NONE,
    BUTTON_UP_PRESSED,
    BUTTON_DOWN_PRESSED,
    BUTTON_ENTER_PRESSED
};

void buttonsSetup();
ButtonState buttonsRead();

#endif // BUTTONS_H

