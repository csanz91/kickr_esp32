#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(int pin, int pinGND, std::function<void(int8_t)> pressCallback)
{
    // Initialize pins and callbacks
    _pin = pin;
    _pinGND = pinGND;
    _pressCallback = pressCallback;

    // Initialize state variables
    _lastButtonState = HIGH;
    _buttonState = HIGH;
    _lastDebounceTime = 0;
    _pressStartTime = 0;
    _isLongPressActive = false;
    _lastGearChangeTime = 0;

    // Set up the button pin
    pinMode(_pin, INPUT_PULLUP);
    pinMode(_pinGND, OUTPUT);
    digitalWrite(_pinGND, LOW);
}

void ButtonHandler::update()
{
    int reading = digitalRead(_pin);

    // Debounce the button
    if (reading != _lastButtonState)
    {
        _lastDebounceTime = millis();
    }

    if ((millis() - _lastDebounceTime) > DEBOUNCE_TIME)
    {
        // Button state changed after debounce period
        if (reading != _buttonState)
        {
            _buttonState = reading;

            // Handle button press (LOW with pull-up resistor)
            if (_buttonState == LOW)
            {
                _pressStartTime = millis();
                _isLongPressActive = false;
            }
            // Handle button release (HIGH with pull-up resistor)
            else if (_buttonState == HIGH)
            {
                if (!_isLongPressActive && (millis() - _pressStartTime) >= DEBOUNCE_TIME)
                {
                    // Short press action (single gear change)
                    if (_pressCallback)
                    {
                        _pressCallback(SHORT_PRESS_NUM_SHIFTS);
                    }
                }
                _isLongPressActive = false; // Reset long press flag on release
            }
        }
    }

    // Check for long press (only if button is still pressed)
    if (_buttonState == LOW && !_isLongPressActive)
    {
        if (millis() - _pressStartTime > LONG_PRESS_DURATION)
        {
            _isLongPressActive = true;
            _lastGearChangeTime = millis(); // Initialize interval timing

            // First gear change immediately when long press detected
            if (_pressCallback)
            {
                _pressCallback(LONG_PRESS_NUM_SHIFTS);
            }
        }
    }

    // Handle repeated gear changes during long press (only if button still pressed)
    if (_buttonState == LOW && _isLongPressActive)
    {
        if (millis() - _lastGearChangeTime >= LONG_PRESS_INTERVAL)
        {
            _lastGearChangeTime = millis();

            if (_pressCallback)
            {
                _pressCallback(LONG_PRESS_NUM_SHIFTS);
            }
        }
    }

    _lastButtonState = reading;
}