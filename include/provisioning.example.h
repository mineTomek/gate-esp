#pragma once

#include <cstdint>

namespace Provisioning
{
    constexpr uint8_t Version = 1;

    namespace WiFi
    {
        constexpr const char *SSID = "WIFI_SSID";
        constexpr const char *Password = "WIFI_PASSWORD";
    }

    namespace MQTT
    {
        constexpr const char *Address = "MQTT_ADDRESS";

        constexpr const char *Username = "MQTT_USERNAME";
        constexpr const char *Password = "MQTT_PASSWORD";
    }
}
