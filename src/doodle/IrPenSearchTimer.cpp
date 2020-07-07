#include "doodle/IrPenSearchTimer.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

IrPenSearchTimer::IrPenSearchTimer(const int timeoutMillis) :
    timeoutMillis_(timeoutMillis)
{
}

IrPenSearchTimer::~IrPenSearchTimer()
{
}

void IrPenSearchTimer::startTimeout(const int deviceId)
{
    // Cancel the timer for the spacified device.
    cancelTimeout(deviceId);
    // Create a new timer for the device.
    auto timer = std::make_shared<Timer>(deviceId, this, &IrPenSearchTimer::onTimerCompleted);
    timers_.emplace(std::piecewise_construct,
        std::forward_as_tuple(deviceId),
        std::forward_as_tuple(timer));
    timer->start(timeoutMillis_);

}

void IrPenSearchTimer::cancelTimeout(const int deviceId)
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

void IrPenSearchTimer::onTimerCompleted(const int deviceId, const bool cancelled)
{
    if (cancelled) return;

    for (auto&& l : listeners_)
    {
        l->onPenSearchTimeout(deviceId, cancelled);
    }
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
