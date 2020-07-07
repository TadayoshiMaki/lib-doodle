#ifndef LIB_DOODLE_FRAMEWORK_doodle_IrPenSearchTimer_H_
#define LIB_DOODLE_FRAMEWORK_doodle_IrPenSearchTimer_H_

#include <map>
#include <memory>
#include <vector>

#include "doodle/namespace.h"
#include "doodle/Timer.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class IrPenSearchTimerListener
{
public:
    virtual void onPenSearchTimeout(const int penId, const bool cancelled) = 0;
};

class IrPenSearchTimer
{
public:
    IrPenSearchTimer(const int timeoutMillis);
    virtual ~IrPenSearchTimer();

    void addListener(IrPenSearchTimerListener* l)
    {
        auto it = std::find(listeners_.begin(), listeners_.end(), l);
        if (it == listeners_.end())
        {
            listeners_.push_back(l);
        }
    };

    void removeListener(const IrPenSearchTimerListener* l)
    {
        auto it = std::find(listeners_.begin(), listeners_.end(), l);
        if (it != listeners_.end())
        {
            listeners_.erase(it);
        }
    };

    /**
     * @brief Updates the timeout timer for the device.
     * @param deviceId The device ID.
     */
    void startTimeout(const int deviceId);

    /**
     * @brief Cancels the timeout timer for the device.
     * @param deviceId The device ID.
     */
    void cancelTimeout(const int deviceId);



private:
    // The time until the event timeout, in milliseconds.
    const int timeoutMillis_;
    // The timers for each device.
    std::map<int, std::shared_ptr<Timer>> timers_;
    // The timeout listeners.
    std::vector<IrPenSearchTimerListener*> listeners_;



    /**
     * @brief Called when the timeout timer completes or is cancelled.
     * @param deviceId The device ID.
     * @param cancelled Whether the timer is cancelled or not.
     */
    void onTimerCompleted(const int deviceId, const bool cancelled);
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_IrPenSearchTimer_H_
