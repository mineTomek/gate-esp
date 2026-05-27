#include <Led.h>

Led::Led(uint8_t pin, uint8_t defaultState)
    : pin(pin),
      defaultState(defaultState),
      blinkStart(0),
      blinkDuration(0)
{
}

void Led::begin()
{
    pinMode(pin, OUTPUT);
    set(defaultState);
}

void Led::on()
{
    set(HIGH);
}

void Led::off()
{
    set(LOW);
}

void Led::set(uint8_t state)
{
    digitalWrite(pin, state);
    blinkDuration = 0;
}

void Led::blink(unsigned long duration)
{
    on();
    blinkStart = millis();
    blinkDuration = duration;
}

bool Led::update()
{
    if (blinkDuration == 0)
    {
        return false;
    }

    if (millis() - blinkStart >= blinkDuration)
    {
        off();
        return true;
    }

    return false;
}
