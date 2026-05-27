#pragma once

#include <Arduino.h>

class Optocoupler
{
public:
    explicit Optocoupler(uint8_t pin);

    void begin();
    void update();

    bool activated() const;   // HIGH -> LOW
    bool deactivated() const; // LOW -> HIGH
    bool changed() const;     // any edge

    bool isActive() const;

private:
    uint8_t pin;
    int currentState;
    int previousState;

    int read() const;
};
