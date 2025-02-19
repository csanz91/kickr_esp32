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

    // State tracking
    int _lastButtonState;
    int _buttonState;
    unsigned long _lastDebounceTime;

    // Press detection variables
    unsigned long _pressStartTime;
    unsigned long _lastGearChangeTime;
    bool _isLongPressActive;

    // Function pointers for callbacks
    std::function<void(int8_t)> _pressCallback;

public:
    // Constructor
    ButtonHandler(int pin, int pinGND, std::function<void(int8_t)> pressCallback);

    // Call this in each loop iteration
    void update();
};
#endif