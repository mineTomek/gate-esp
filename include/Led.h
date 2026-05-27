#pragma once

#include <Arduino.h>

class Led
{
public:
    explicit Led(uint8_t pin, uint8_t defaultState = LOW);

    void begin();

    void on();
    void off();
    void set(uint8_t state);

    void blink(unsigned long duration);
    bool update(); // Returns true if the LED's state has changed

private:
    uint8_t pin;
    uint8_t defaultState;

    bool blinking;
    unsigned long blinkStart;
    unsigned long blinkDuration;
};
