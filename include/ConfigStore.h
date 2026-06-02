#pragma once

#include <Arduino.h>
#include <Preferences.h>
#ifdef PROVISION_CONFIG
#include <provisioning.h>
#endif

struct RuntimeConfig
{
    uint8_t version;

    String wifiSsid;
    String wifiPassword;

    String mqttHost;

    String mqttUsername;
    String mqttPassword;

    String githubPat;

    bool isValid() const
    {
        return wifiSsid.length() > 0 &&
               mqttHost.length() > 0;
    }
};

class ConfigStore
{
public:
    RuntimeConfig load()
    {
        Preferences prefs;
        prefs.begin("gate", true);

        RuntimeConfig config{
            .version = prefs.getUChar("provision_ver", 0),

            .wifiSsid = prefs.getString("wifi_ssid", ""),
            .wifiPassword = prefs.getString("wifi_pass", ""),

            .mqttHost = prefs.getString("mqtt_host", ""),

            .mqttUsername = prefs.getString("mqtt_user", ""),
            .mqttPassword = prefs.getString("mqtt_pass", ""),

            .githubPat = prefs.getString("github_pat", "")};

        prefs.end();
        return config;
    }

    void save(const RuntimeConfig &config)
    {
        Preferences prefs;
        prefs.begin("gate", false);

        prefs.putUChar("provision_ver", config.version);

        prefs.putString("wifi_ssid", config.wifiSsid);
        prefs.putString("wifi_pass", config.wifiPassword);

        prefs.putString("mqtt_host", config.mqttHost);

        prefs.putString("mqtt_user", config.mqttUsername);
        prefs.putString("mqtt_pass", config.mqttPassword);

        prefs.putString("github_pat", config.githubPat);

        prefs.end();
    }

    bool attemptProvision()
    {
#ifdef PROVISION_CONFIG
        RuntimeConfig config = load();

        if (config.version >= Provisioning::Version)
        {
            return false;
        }

        Serial.printf(
            "Provisioning update %d -> %d\n",
            config.version,
            Provisioning::Version);

        RuntimeConfig initialConfig{
            .version = Provisioning::Version,

            .wifiSsid = Provisioning::WiFi::SSID,
            .wifiPassword = Provisioning::WiFi::Password,

            .mqttHost = Provisioning::MQTT::Address,

            .mqttUsername = Provisioning::MQTT::Username,
            .mqttPassword = Provisioning::MQTT::Password,

            .githubPat = Provisioning::GitHub::PAT,
        };

        save(initialConfig);

        Serial.println("Provisioning config saved to NVS");

        return true;
#endif

        return false;
    }
};
