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
bool OtaUpdater::parseUpdateInfo(const uint8_t *payload, size_t size, UpdateInfo &out)
{
    String msg;
    msg.reserve(size);

    for (size_t i = 0; i < size; i++)
    {
        msg += static_cast<char>(payload[i]);
    }

    msg.trim();

    int firstNl = msg.indexOf('\n');

    if (firstNl < 0)
        return false;

    int secondNl = msg.indexOf('\n', firstNl + 1);

    if (secondNl < 0)
        return false;

    // * URL

    String url = msg.substring(0, firstNl);

    url.trim();

    if (!url.startsWith("https://"))
        return false;

    // * SHA-256

    String sha256 = msg.substring(firstNl + 1, secondNl);

    sha256.trim();

    if (sha256.length() != 64)
        return false;

    for (char c : sha256)
    {
        bool isHex =
            (c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'f') ||
            (c >= 'A' && c <= 'F');

        if (!isHex)
            return false;
    }

    // * Size

    String sizeStr = msg.substring(secondNl + 1);

    sizeStr.trim();

    if (sizeStr.length() == 0)
        return false;

    for (char c : sizeStr)
    {
        bool isDigit = c >= '0' && c <= '9';

        if (!isDigit)
            return false;
    }

    size_t firmwareSize = static_cast<size_t>(sizeStr.toInt());

    if (firmwareSize == 0)
        return false;

    out.url = url;
    out.sha256 = sha256;
    out.size = firmwareSize;

    return true;
}

OtaResult OtaUpdater::start(const UpdateInfo &info, const RuntimeConfig &runtimeConfig)
{
    if (isUpdating())
    {
        return OtaResult::AlreadyUpdating;
    }

    setStatus(UpdateStatus::Downloading);

    if (!client.begin(info.url))
    {
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

    int httpSize = client.getSize();

    if (httpSize <= 0)
    {
        return fail(OtaResult::FailSize);
    }

    size_t size = static_cast<size_t>(httpSize);

    if (size != info.size)
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

    if (written != static_cast<size_t>(size))
    {
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

bool OtaUpdater::isGateSafeForUpdate(uint8_t gateState)
{
    return gateState == Config::MQTT::Payload::State::Closed ||
           gateState == Config::MQTT::Payload::State::Open;
}
