#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include <Optie.h>
#include <Led.h>
#include <Mqtt.h>

#include <config.h>

Optie openOptie(Config::Pin::Optie::FullOpen);
Optie closeOptie(Config::Pin::Optie::FullClose);
Optie obstructionOptie(Config::Pin::Optie::ObstructionSensor);

Led openLed(Config::Pin::LED::Open);
Led closeLed(Config::Pin::LED::Close);
Led stopLed(Config::Pin::LED::Stop);

Mqtt mqtt;

byte localState = Config::MQTT::Payload::State::Unknown;

void setupPins()
{
  openLed.begin();
  closeLed.begin();
  stopLed.begin();

  openOptie.begin();
  closeOptie.begin();
  obstructionOptie.begin();
}

void updateState(byte state, bool retained = true, bool force = false)
{
  if (force || localState != state)
  {
    mqtt.publishByte(Config::MQTT::Topic::Pub::State, state, retained);
    localState = state;
  }
}

void handleMqttMessage(const char *topic, const uint8_t *payload, size_t length)
{
  Serial.print("[MQTT] Received ");

  for (size_t i = 0; i < length; i++)
  {
    Serial.print("0x");

    if (payload[i] < 0x10)
    {
      Serial.print("0");
    }

    Serial.print(payload[i], HEX);
    Serial.print(" ");
  }

  Serial.print(" on topic ");
  Serial.print(topic);
  Serial.println();

  // * Target Topic

  if (strcmp(topic, Config::MQTT::Topic::Sub::Target) == 0)
  {
    if (length != 1)
      return;

    if (payload[0] == Config::MQTT::Payload::Target::Open)
    {
      if (openOptie.isActive())
      {
        updateState(Config::MQTT::Payload::State::Open);
      }
      else
      {
        openLed.blink(Config::RelayPulseDuration);
        updateState(Config::MQTT::Payload::State::Opening);
      }
    }
    else if (payload[0] == Config::MQTT::Payload::Target::Closed)
    {
      if (closeOptie.isActive())
      {
        updateState(Config::MQTT::Payload::State::Closed);
      }
      else
      {
        closeLed.blink(Config::RelayPulseDuration);
        updateState(Config::MQTT::Payload::State::Closing);
      }
    }
  }

  // * Stop Topic

  if (strcmp(topic, Config::MQTT::Topic::Sub::Stop) == 0)
  {
    stopLed.blink(Config::RelayPulseDuration);
    updateState(Config::MQTT::Payload::State::Stopped);
  }
}

void setup()
{
  Serial.begin(9600);

  while (!Serial)
    continue;

  setupPins();

  mqtt.setMessageHandler(handleMqttMessage);
  mqtt.begin();
}

void loop()
{
  openOptie.update();
  closeOptie.update();
  obstructionOptie.update();

  mqtt.update();

  openLed.update();
  closeLed.update();
  stopLed.update();

  if (openOptie.activated())
  {
    updateState(Config::MQTT::Payload::State::Open);
  }

  if (openOptie.deactivated())
  {
    updateState(Config::MQTT::Payload::State::Closing);
  }

  if (closeOptie.activated())
  {
    updateState(Config::MQTT::Payload::State::Closed);
  }

  if (closeOptie.deactivated())
  {
    updateState(Config::MQTT::Payload::State::Opening);
  }

  if (obstructionOptie.changed())
  {
    mqtt.publishByte(
        Config::MQTT::Topic::Pub::Obstruction,
        obstructionOptie.isActive()
            ? Config::MQTT::Payload::Obstruction::Obstructed
            : Config::MQTT::Payload::Obstruction::Unobstructed,
        true);

    if (localState == Config::MQTT::Payload::State::Closing)
    {
      updateState(Config::MQTT::Payload::State::Opening);
    }
  }
}
