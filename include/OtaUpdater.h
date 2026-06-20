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
    explicit OtaUpdater();

    // ! Maybe move runtimeConfig to the ctor
    OtaResult start(const UpdateInfo &info, const RuntimeConfig &runtimeConfig);

    uint8_t getStatus() const;
    bool isUpdating() const;

private:
    bool parseUpdateInfo(const uint8_t *payload, size_t length, UpdateInfo &out);

    void setStatus(uint8_t newStatus);
    OtaResult fail(OtaResult failResult);

    bool isGateSafeForUpdate(uint8_t gateState);

    uint8_t status;
    HTTPClient client;
};
