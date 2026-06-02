#pragma once

#include <HTTPClient.h>
#include <ConfigStore.h>

struct UpdateInfo
{
    String url;
    String sha256;
    size_t size;
};

enum class OtaResult
{
    AlreadyUpdating,
    Success,
    FailHTTP,
    FailSize,
    FailFirstByte,
    FailUpdate,
};

class OtaUpdater
{
public:
    explicit OtaUpdater(WiFiClient &wifiClient);
    // ! Maybe move runtimeConfig to the ctor
    OtaResult start(const UpdateInfo &info, const RuntimeConfig &runtimeConfig);

    uint8_t getStatus() const;
    bool isUpdating() const;

private:
    void setStatus(uint8_t newStatus);
    OtaResult fail(OtaResult failResult);

    uint8_t status;
    WiFiClient &wifiClient;
    HTTPClient client;
};
