#ifndef LIB_DOODLE_FRAMEWORK_doodle_Ticker_H_
#define LIB_DOODLE_FRAMEWORK_doodle_Ticker_H_

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>

#include "doodle/namespace.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class Ticker
{
public:
    using Callback = std::function<void()>;

    Ticker(const int intervalMillis) :
        intervalMillis_(intervalMillis),
        started_(false),
        exitLoop_(false)
    {
    }

    virtual ~Ticker()
    {
    }

    template<class ListenerClass>
    void start(ListenerClass* owner, void (ListenerClass::*listenerMethod)())
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (started_) return;

            setCallback(owner, listenerMethod);
            exitLoop_ = false;
        }
        thread_ = std::thread([this] { loop(); });
    }

    void stop()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!started_) return;

            callback_.reset();
            exitLoop_ = true;
        }

        if (thread_.joinable())
        {
            thread_.join();
        }
    }

    bool isStarted()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return started_;
    }

private:
    // The worker thread to run a loop at an interval.
    std::thread thread_;
    // The lock object for thread operations.
    std::mutex mutex_;
    // Used to wait for time interval.
    std::condition_variable condition_;
    // The loop interval.
    int intervalMillis_;

    // Indicates whether the worker thread is started or not.
    std::atomic_bool started_ = false;
    // Indicates whether the worker thread should exit the loop.
    std::atomic_bool exitLoop_ = false;

    // The callable object to notify of a tick.
    std::shared_ptr<Callback> callback_;



    /**
     * @brief Sets the callback to subscribe ticks.
     * @param owner The owner class of the callable method.
     * @param listenerMethod The callable method.
     */
    template<class ListenerClass>
    void setCallback(ListenerClass* owner, void (ListenerClass::*listenerMethod)())
    {
        setCallback(std::bind(listenerMethod, owner));
    }

    void setCallback(const Callback& callback)
    {
        callback_ = std::make_shared<Callback>(callback);
    }

    /**
     * @brief Loops at an interval.
     */
    void loop()
    {
        started_.store(true);
        while (true)
        {
            {
                std::unique_lock<std::mutex> lock(mutex_);
                // Wait for the specified time, in milliseconds.
                condition_.wait_for(
                    lock,
                    std::chrono::milliseconds(intervalMillis_),
                    [this] { return exitLoop_.load(); });

                if (exitLoop_.load())
                {
                    break;
                }

                if (callback_) (*callback_)();
            }
        }
        started_.store(false);
    }
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_Ticker_H_
