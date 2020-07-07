#ifndef LIB_DOODLE_FRAMEWORK_doodle_DeviceTimeoutManager_H_
#define LIB_DOODLE_FRAMEWORK_doodle_DeviceTimeoutManager_H_

#include <map>
#include <memory>
#include <vector>

#include "doodle/namespace.h"
#include "doodle/Timer.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class DeviceTimeoutListener
{
public:
    /**
     * @brief Called when an event for the device is timed out.
     * @param deviceId The device ID.
     */
    virtual void onDeviceEventTimeout(const int deviceId) = 0;
};



class DeviceTimeoutHandler :
    public DeviceTimeoutListener,
    public std::enable_shared_from_this<DeviceTimeoutHandler>
{
};



class DeviceTimeoutManager
{
public:

    using Callback = std::function<void(const int, const bool)>;

    DeviceTimeoutManager(const int timeoutMillis);
    virtual ~DeviceTimeoutManager();

    void addListener(const std::shared_ptr<DeviceTimeoutListener>& l)
    {
        auto it = std::find(listeners_.begin(), listeners_.end(), l);
        if (it == listeners_.end())
        {
            listeners_.push_back(l);
        }
    };

    void removeListener(const std::shared_ptr<DeviceTimeoutListener>& l)
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
    void updateTimeout(const int deviceId);

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
    std::vector<std::shared_ptr<DeviceTimeoutListener>> listeners_;



    /**
     * @brief Called when the timeout timer completes or is cancelled.
     * @param deviceId The device ID.
     * @param cancelled Whether the timer is cancelled or not.
     */
    void onTimerCompleted(const int deviceId, const bool cancelled);
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_DeviceTimeoutManager_H_
