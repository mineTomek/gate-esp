#include <OtaUpdater.h>
#include <Update.h>
#include <HTTPClient.h>
#include <config.h>
#include <ConfigStore.h>

namespace UpdateStatus = Config::MQTT::Payload::UpdateStatus;

OtaUpdater::OtaUpdater()
    : status(UpdateStatus::Idle)
{
}

// TODO: In the future make this more async/work in loop()
OtaResult OtaUpdater::start(const UpdateInfo &info, const RuntimeConfig &runtimeConfig)
{
    if (isUpdating())
    {
        return OtaResult::AlreadyUpdating;
    }

    setStatus(UpdateStatus::Downloading);

    if (!client.begin(info.url)) {
        return fail(OtaResult::FailHTTP);
    }

    client.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    client.addHeader("Authorization", "Bearer " + runtimeConfig.githubPat);
    client.addHeader("Accept", "application/vnd.github+json");
    client.addHeader("User-Agent", "mineTomek/gate-esp");

    int statusCode = client.GET();

    if (statusCode != HTTP_CODE_OK)
    {
        return fail(OtaResult::FailHTTP);
    }

    int size = client.getSize();

    if (size <= 0 || size != info.size)
    {
        return fail(OtaResult::FailSize);
    }

    WiFiClient &stream = client.getStream();

    int firstByte = stream.peek();

    // ? ESP32 firmware starts with 0xE9
    if (firstByte != 0xE9)
    {
        return fail(OtaResult::FailFirstByte);
    }

    setStatus(UpdateStatus::Installing);

    if (!Update.begin(size))
    {
        Update.printError(Serial);
        return fail(OtaResult::FailUpdate);
    }

    size_t written = Update.writeStream(stream);

    if (written != static_cast<size_t>(size)) {
        Update.printError(Serial);
        return fail(OtaResult::FailUpdate);
    }

#ifdef OTA_DRY_RUN
    Update.abort();
    client.end();

    setStatus(UpdateStatus::Success);
    Serial.println("OTA dry run finished; update aborted intentionally.");
#else
    if (!Update.end() || !Update.isFinished())
    {
        Update.printError(Serial);
        return fail(OtaResult::FailUpdate);
    }

    client.end();

    setStatus(UpdateStatus::Success);

    // * Delay for MQTT statuses and serial
    delay(500);

    ESP.restart();
    #endif

    return OtaResult::Success;
}

uint8_t OtaUpdater::getStatus() const
{
    return status;
}

OtaResult OtaUpdater::fail(OtaResult failResult)
{
    client.end();
    setStatus(UpdateStatus::Failed);
    return failResult;
}

void OtaUpdater::setStatus(uint8_t newStatus)
{
    // TODO: Add MQTT
    status = newStatus;
}

bool OtaUpdater::isUpdating() const
{
    return status == UpdateStatus::Scheduled ||
           status == UpdateStatus::Downloading ||
           status == UpdateStatus::Installing;
}
