#include <Mqtt.h>
#include <config.h>
#include <ConfigStore.h>

Mqtt *Mqtt::instance = nullptr;

Mqtt::Mqtt()
    : client(wifiClient),
      messageHandler(nullptr)
{
    instance = this;
}

void Mqtt::begin(RuntimeConfig runtimeConfig)
{
    connectWifi(runtimeConfig);

    client.setServer(runtimeConfig.mqttHost.c_str(), Config::MQTT::Port);
    client.setCallback(staticCallback);
}

void Mqtt::update(RuntimeConfig runtimeConfig)
{
    if (!client.connected())
    {
        connectMqtt(runtimeConfig);
    }

    client.loop();
}

bool Mqtt::publish(const char *topic, const char *payload)
{
    return publish(topic, payload, false);
}

bool Mqtt::publish(const char *topic, const char *payload, bool retained)
{
    Serial.print("[MQTT] Publishing ");
    Serial.print(payload);
    Serial.print(" on topic ");
    Serial.print(topic);
    Serial.println();

    return client.publish(topic, payload, retained);
}

bool Mqtt::publishBytes(const char *topic, const uint8_t *payload, size_t length)
{
    return publishBytes(topic, payload, length, false);
}

bool Mqtt::publishBytes(const char *topic, const uint8_t *payload, size_t length, bool retained)
{
    Serial.print("[MQTT] Publishing ");

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

    return client.publish(topic, payload, length, retained);
}

bool Mqtt::publishByte(const char *topic, uint8_t value)
{
    return publishByte(topic, value, false);
}

bool Mqtt::publishByte(const char *topic, uint8_t value, bool retained)
{
    return publishBytes(topic, &value, 1, retained);
}

void Mqtt::setMessageHandler(MessageHandler handler)
{
    messageHandler = handler;
}

void Mqtt::connectWifi(RuntimeConfig runtimeConfig)
{
    WiFi.begin(runtimeConfig.wifiSsid, runtimeConfig.wifiPassword);

    Serial.print("Connecting to Wi-Fi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(250);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("Connected. IP: ");
    Serial.println(WiFi.localIP());
}

void Mqtt::connectMqtt(RuntimeConfig runtimeConfig)
{
    while (!client.connected())
    {
        Serial.print("Connecting to MQTT... ");

        bool connectResult = client.connect(
            Config::MQTT::ClientID,
            runtimeConfig.mqttUsername.c_str(),
            runtimeConfig.mqttPassword.c_str(),

            // Will
            Config::MQTT::Topic::Pub::Availability,      // Topic
            1,                                           // QoS
            true,                                        // Retain
            Config::MQTT::Payload::Availability::Offline // Message
        );

        if (connectResult)
        {
            Serial.println("connected");

            client.publish(
                Config::MQTT::Topic::Pub::Availability,
                Config::MQTT::Payload::Availability::Online,
                true);
            client.publish(Config::MQTT::Topic::Log, (String(Config::MQTT::ClientID) + " connected").c_str());
            client.publish(Config::MQTT::Topic::NewDevice, Config::MQTT::ClientID);

            client.subscribe(Config::MQTT::Topic::Sub::Target);
            client.subscribe(Config::MQTT::Topic::Sub::Stop);
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.println(client.state());

            delay(5000);
        }
    }
}

void Mqtt::staticCallback(char *topic, uint8_t *payload, unsigned int length)
{
    if (instance && instance->messageHandler)
    {
        instance->messageHandler(topic, payload, length);
    }
}
