#include <Button.h>

Button::Button(uint8_t pin)
    : pin(pin), currentState(HIGH), previousState(HIGH)
{
}

void Button::begin()
{
    pinMode(pin, INPUT_PULLUP);
    currentState = read();
    previousState = currentState;
}

void Button::update()
{
    previousState = currentState;
    currentState = read();
}

bool Button::pressed() const
{
    return previousState == HIGH && currentState == LOW;
}

bool Button::released() const
{
    return previousState == LOW && currentState == HIGH;
}

bool Button::changed() const
{
    return previousState != currentState;
}

bool Button::isDown() const
{
    return currentState == LOW;
}

int Button::read() const
{
    return digitalRead(pin);
}
