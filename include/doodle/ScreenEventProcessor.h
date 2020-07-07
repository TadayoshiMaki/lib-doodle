#ifndef LIB_DOODLE_FRAMEWORK_doodle_ScreenEventProcessor_H_
#define LIB_DOODLE_FRAMEWORK_doodle_ScreenEventProcessor_H_

#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "doodle/namespace.h"
#include "doodle/DoodleEvent.h"
#include "doodle/ScreenEventDispatcher.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class ScreenEventProcessor
{
public:
    ScreenEventProcessor(const std::shared_ptr<ScreenEventDispatcher>& dispatcher);
    virtual ~ScreenEventProcessor();

    /**
     * @brief Starts processing event data.
     * @return true if successfully started.
     */
    void start();
    /**
     * @brief Stops processing event data.
     * @return true if successfully stopped.
     */
    void stop();
    /**
     * @brief Determines whether processing is started or not.
     * @return true if reading is started.
     */
    bool isStarted();

    /**
     * @brief Called when screens are initialized.
     * @param screens The properties of screens.
     */
    void handleScreensInitialized(std::vector<std::shared_ptr<ScreenProperty>> screens);

    /**
     * @brief Handles screen change data.
     * @param oldScreenId The old current screen ID which changed from.
     * @param newScreenId The new current screen ID which changed to.
     */
    void handleScreenChangeStarted(const int oldScreenId, const int newScreenId);

    /**
     * @brief Handles changed screen data.
     * @param oldScreenId The old current screen ID which changed from.
     * @param newScreenId The new current screen ID which changed to.
     */
    void handleCurrentScreenChanged(const int oldScreenId, const int newScreenId);

private:
    // The worker thread to process event data.
    std::thread processorThread_;
    // The condition for the event queue.
    std::condition_variable queueCondition_;
    // The lock object for thread operations.
    std::mutex processorMutex_;
    // The lock object for processing data.
    std::mutex queueMutex_;

    // Used to dispatch queued events to listeners.
    std::shared_ptr<ScreenEventDispatcher> dispatcher_;

    // The event queue.
    std::queue<std::shared_ptr<DoodleEvent>> queue_;

    // The flag indicating whether the processing loop is started or not.
    std::atomic_bool started_;
    // The flag indicating whether the loop should exit or not.
    std::atomic_bool exitLoop_;

    /**
     * @brief Processes received data in a loop.
     */
    void loop();
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_ScreenEventProcessor_H_
