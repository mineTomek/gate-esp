// * NEEDS TO BE NAMED `config.h`

#pragma once

#include <cstdint>

using byte = uint8_t;

namespace Config
{
    namespace WiFi
    {
        constexpr const char *SSID = "WIFI_SSID";
        constexpr const char *Password = "WIFI_PASSWORD";
    }

    namespace MQTT
    {
        constexpr const char *Address = "MQTT_ADDRESS";
        constexpr uint16_t Port = 1883;

        constexpr const char *ClientID = "CLIENT_ID";

        constexpr const char *Username = "MQTT_USERNAME";
        constexpr const char *Password = "MQTT_PASSWORD";

        namespace Topic
        {
            // Utility

            constexpr const char *Log = "gate/log";
            constexpr const char *NewDevice = "gate/device/new";

            namespace Sub
            {
                constexpr const char *Target = "gate/target/set";
                constexpr const char *Stop = "gate/stop/trigger";
            }

            namespace Pub
            {
                constexpr const char *State = "gate/current";
                constexpr const char *Obstruction = "gate/obstruction";
                constexpr const char *Availability = "gate/availability";
            }
        }

        namespace Payload
        {
            namespace Target
            {
                constexpr byte Open = 0x00;
                constexpr byte Closed = 0x01;
            }

            // * Stop has no payload

            namespace State
            {
                constexpr byte Open = 0x00;
                constexpr byte Closed = 0x01;
                constexpr byte Opening = 0x02;
                constexpr byte Closing = 0x03;
                constexpr byte Stopped = 0x04;
                constexpr byte Unknown = 0x0f;
            }

            namespace Obstruction
            {
                constexpr byte Unobstructed = 0x00;
                constexpr byte Obstructed = 0x01;
            }

            namespace Availability
            {
                constexpr const char *Offline = "offline";
                constexpr const char *Online = "online";
            }
        }
    }

    namespace Pin
    {
        namespace LED
        {
            constexpr byte Open = 4;  // ! MAKE SURE TO CHANGE
            constexpr byte Close = 5; // ! MAKE SURE TO CHANGE
            constexpr byte Stop = 18; // ! MAKE SURE TO CHANGE
        }

        namespace Button
        {
            constexpr byte FullOpen = 32;          // ! MAKE SURE TO CHANGE
            constexpr byte FullClose = 33;         // ! MAKE SURE TO CHANGE
            constexpr byte ObstructionSensor = 25; // ! MAKE SURE TO CHANGE
        }
    }

    constexpr unsigned long RelayPulseDuration = 300; // ms
}
