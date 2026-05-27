#include <Optie.h>

Optie::Optie(uint8_t pin)
    : pin(pin), currentState(HIGH), previousState(HIGH)
{
}

void Optie::begin()
{
    pinMode(pin, INPUT_PULLUP);
    currentState = read();
    previousState = currentState;
}

void Optie::update()
{
    previousState = currentState;
    currentState = read();
}

bool Optie::activated() const
{
    return previousState == HIGH && currentState == LOW;
}

bool Optie::deactivated() const
{
    return previousState == LOW && currentState == HIGH;
}

bool Optie::changed() const
{
    return previousState != currentState;
}

bool Optie::isActive() const
{
    return currentState == LOW;
}

int Optie::read() const
{
    return digitalRead(pin);
}
