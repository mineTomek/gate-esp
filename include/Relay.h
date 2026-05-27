#pragma once

#include <Arduino.h>

class Relay
{
public:
    explicit Relay(uint8_t pin, uint8_t defaultState = LOW);

    void begin();

    void on();
    void off();
    void set(uint8_t state);

    void pulse(unsigned long duration);
    bool update(); // Returns true if the relay's state has changed

private:
    uint8_t pin;
    uint8_t defaultState;

    bool pulsing;
    unsigned long pulseStart;
    unsigned long pulseDuration;
};
