#pragma once

#include <Arduino.h>

class Button
{
public:
    explicit Button(uint8_t pin);

    void begin();
    void update();

    bool pressed() const;  // HIGH -> LOW
    bool released() const; // LOW -> HIGH
    bool changed() const;  // any edge

    bool isDown() const;

private:
    uint8_t pin;
    int currentState;
    int previousState;

    int read() const;
};
