#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ConfigStore.h>

class Mqtt
{
public:
    using MessageHandler = void (*)(const char *topic, const uint8_t *payload, size_t length);

    Mqtt();

    void begin(RuntimeConfig runtimeConfig);
    void update(RuntimeConfig runtimeConfig);

    bool publish(const char *topic, const char *payload);
    bool publish(const char *topic, const char *payload, bool retained);
    
    bool publishBytes(const char *topic, const uint8_t *payload, size_t length);
    bool publishBytes(const char *topic, const uint8_t *payload, size_t length, bool retained);
    
    bool publishByte(const char *topic, const uint8_t value);
    bool publishByte(const char *topic, const uint8_t value, bool retained);

    void setMessageHandler(MessageHandler handler);

private:
    WiFiClient wifiClient;
    PubSubClient client;

    MessageHandler messageHandler;

    void connectWifi(RuntimeConfig runtimeConfig);
    void connectMqtt(RuntimeConfig runtimeConfig);

    static Mqtt *instance;
    static void staticCallback(char *topic, uint8_t *payload, unsigned int length);
};
