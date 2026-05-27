#include <Optocoupler.h>

Optocoupler::Optocoupler(uint8_t pin)
    : pin(pin), currentState(HIGH), previousState(HIGH)
{
}

void Optocoupler::begin()
{
    pinMode(pin, INPUT_PULLUP);
    currentState = read();
    previousState = currentState;
}

void Optocoupler::update()
{
    previousState = currentState;
    currentState = read();
}

bool Optocoupler::activated() const
{
    return previousState == HIGH && currentState == LOW;
}

bool Optocoupler::deactivated() const
{
    return previousState == LOW && currentState == HIGH;
}

bool Optocoupler::changed() const
{
    return previousState != currentState;
}

bool Optocoupler::isActive() const
{
    return currentState == LOW;
}

int Optocoupler::read() const
{
    return digitalRead(pin);
}
