#include <Arduino.h>
#include "kickr_constants.h"
#include <functional>
#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

class ButtonHandler
{
private:
    // Button configuration
    int _pin;
    int _pinGND;

    bool _isUpButton;

    uint8_t _shortPressNumGears = 1;
    uint8_t _longPressNumGears = GEAR_LONG_PRESS_NUM_SHIFTS;

    // State tracking
    int _lastButtonState;
    int _buttonState;
    unsigned long _lastDebounceTime;

    // Press detection variables
    unsigned long _pressStartTime;
    bool _isLongPressActive;

    // Function pointers for callbacks
    std::function<void(int8_t)> _pressCallback;

public:
    // Constructor
    ButtonHandler(int pin, int pinGND, bool isUpButton, std::function<void(int8_t)> pressCallback);
    unsigned long _lastGearChangeTime;

    // Call this in each loop iteration
    void update();
};
#endif