#ifndef LIB_DOODLE_FRAMEWORK_doodle_Timer_H_
#define LIB_DOODLE_FRAMEWORK_doodle_Timer_H_

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>

#include "doodle/namespace.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

using Callback = std::function<void(const int deviceId, const bool)>;

class Timer
{
public:
    template<class ListenerClass>
    Timer(const int deviceId, ListenerClass* owner, void (ListenerClass::*listenerMethod)(const int deviceId, const bool)) :
        Timer(deviceId, std::bind(listenerMethod, owner, std::placeholders::_1, std::placeholders::_2))
    {
    }

    Timer(const int deviceId, Callback callback) : deviceId_(deviceId), callback_(std::make_shared<Callback>(callback))
    {
        cancelled_ = std::make_shared<bool>(false);
        mtx_ = std::make_shared<std::mutex>();
        condition_ = std::make_shared<std::condition_variable>();
    }

    virtual ~Timer()
    {
        callback_.reset();
        if (timer_)
        {
            // Wait for cancellation or result.
            bool cancelled = timer_->get();
            timer_.reset();
        }
        mtx_.reset();
        condition_.reset();
        cancelled_.reset();
    }

    /**
     * @brief Sets this timer for the specified time and turns on.
     * @param timeMillis The time to set, in milliseconds.
     */
    void start(const int timeMillis)
    {
        // This instance will be destroyed before the timer completion, so the variables should keep on the timer local scope.
        timer_ = std::make_shared<std::future<bool>>(std::async(
            std::launch::async,
            std::bind(&Timer::run, this, deviceId_, callback_, mtx_, condition_, cancelled_, timeMillis)));
    }

    /**
     * @brief Cancels this timer.
     */
    void cancel()
    {
        if (*cancelled_) return;
        *cancelled_ = true;
        condition_->notify_one();
    }

    /**
     * @brief Detemines whether this timer is canceled or not.
     */
    bool isCancelled()
    {
        return *cancelled_;
    }

private:
    // The target device ID of the timer.
    const int deviceId_;
    // Used to notify of when the timer completes.
    std::shared_ptr<Callback> callback_;
    // The lock object for the state of the timer.
    std::shared_ptr<std::mutex> mtx_;
    // Used to notify of timer cancellation.
    std::shared_ptr<std::condition_variable> condition_;

    // The flag indicationg whether the timer is cancelled.
    std::shared_ptr<bool> cancelled_;
    // The asynchronous timer task.
    std::shared_ptr<std::future<bool>> timer_;

    bool run(const int deviceId,
             const std::shared_ptr<Callback>& callback,
             const std::shared_ptr<std::mutex>& mtx,
             const std::shared_ptr<std::condition_variable>& condition,
             std::shared_ptr<bool>& cancelled,
             const int timeMillis)
    {
        const auto time = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeMillis);
        std::unique_lock<std::mutex> lock(*mtx);
        while (!*cancelled)
        {
            std::cv_status result = condition->wait_until(lock, time);
            if (result == std::cv_status::timeout)
            {
                // Elapsed the spacified duration.
                if (callback) (*callback)(deviceId, *cancelled);
                return *cancelled_;
            }
        }

        // The timer was cancelled.
        if (callback) (*callback)(deviceId, true);

        return *cancelled_;
    }
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_Timer_H_
