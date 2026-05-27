#include <Relay.h>

Relay::Relay(uint8_t pin, uint8_t defaultState)
    : pin(pin),
      defaultState(defaultState),
      pulseStart(0),
      pulseDuration(0)
{
}

void Relay::begin()
{
    pinMode(pin, OUTPUT);
    set(defaultState);
}

void Relay::on()
{
    set(HIGH);
}

void Relay::off()
{
    set(LOW);
}

void Relay::set(uint8_t state)
{
    digitalWrite(pin, state);
    pulseDuration = 0;
}

void Relay::pulse(unsigned long duration)
{
    on();
    pulseStart = millis();
    pulseDuration = duration;
}

bool Relay::update()
{
    if (pulseDuration == 0)
    {
        return false;
    }

    if (millis() - pulseStart >= pulseDuration)
    {
        off();
        return true;
    }

    return false;
}
