#include "doodle/DeviceTimeoutManager.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

DeviceTimeoutManager::DeviceTimeoutManager(const int timeoutMillis) :
    timeoutMillis_(timeoutMillis)
{
}

DeviceTimeoutManager::~DeviceTimeoutManager()
{
}

void DeviceTimeoutManager::updateTimeout(const int deviceId)
{
    // Cancel the timer for the spacified device.
    cancelTimeout(deviceId);
    // Create a new timer for the device.
    auto timer = std::make_shared<Timer>(deviceId, this, &DeviceTimeoutManager::onTimerCompleted);
    timers_.emplace(std::piecewise_construct,
                    std::forward_as_tuple(deviceId),
                    std::forward_as_tuple(timer));
    timer->start(timeoutMillis_);
}

void DeviceTimeoutManager::cancelTimeout(const int deviceId)
{
    auto it = timers_.find(deviceId);
    bool cancelled = false;
    if (it != timers_.end())
    {
        it->second->cancel();
        cancelled = true;
    }

    if (cancelled)
    {
        timers_.erase(deviceId);
    }
}

void DeviceTimeoutManager::onTimerCompleted(const int deviceId, const bool cancelled)
{
    if (cancelled) return;

    for (auto&& l : listeners_)
    {
        l->onDeviceEventTimeout(deviceId);
    }
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
