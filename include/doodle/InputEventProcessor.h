#ifndef LIB_DOODLE_FRAMEWORK_doodle_InputEventProcessor_H_
#define LIB_DOODLE_FRAMEWORK_doodle_InputEventProcessor_H_

#include <atomic>
#include <cmath>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <optional>

#include "doodle/namespace.h"
#include "doodle/ScreenManager.h"
#include "doodle/DoodleEvent.h"
#include "doodle/InputEventDispatcher.h"
#include "doodle/IrPenEvent.h"
#include "doodle/IrPenManager.h"
#include "doodle/ScreenEvent.h"
#include "doodle/ScreenEventListener.h"
#include "doodle/StrokeStatus.h"
#include "doodle/PenEventProcessor.h"
#include "doodle/IrPenHsvMapper.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class InputEventProcessor : public ScreenEventListener
{
    using Action = PenMotionEvent::Action;

public:
    // The invalid device ID. This will be assigned to pending motion events.
    static const int kInvalidDeviceId = IrPenManager::kInvalidDeviceId;

    InputEventProcessor(const std::shared_ptr<IrPenManager>& penManager,
                        const std::shared_ptr<ScreenManager>& screenManager,
                        const std::shared_ptr<InputEventDispatcher>& dispatcher,
                        const std::shared_ptr<HsvSensor> strokeDetector);
    virtual ~InputEventProcessor();

    void setEventTimeoutMillis(const int eventTimeoutMillis)
    {
        eventTimeoutMillis_ = eventTimeoutMillis;
    }

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
     * @param e The screens initialization event.
     */
    virtual void onScreenInitEvent(const std::shared_ptr<ScreenInitEvent>& e) override;

    /**
     * @brief Called when the current screen is changed.
     * @param e The screen change event.
     */
    virtual void onScreenChangeEvent(const std::shared_ptr<ScreenChangeEvent>& e) override;

    /**
     * @brief Called when devices are updated.
     * @param status The device status.
     */
    void onDevicesUpdated(const std::vector<IrPenDeviceStatus> statusList);
    /**
     * @brief Called when a pen state with the device Id is changed.
     * @param deviceId The device ID of a pen.
     * @param pressed Whether the pen tip switch is pressed.
     */
    void onPenStateChanged(const int deviceId, const bool pressed);
    /**
     * @brief Called when the color value is received.
     * @param deviceId The device ID of a pen.
     * @param color The color in 16bit.
     */
    void onColorValueReceived(const int deviceId, const Color color);
    /**
     * @brief Called when points are detected by a sensor.
     * @param results The detection results
     */
    void onStrokePointDetected(const std::vector<HsvSensor::SensingResult>& results);



private:
    // The invalid screen ID.
    static const int kInvalidScreenId = ScreenManager::kInvalidScreenId;
    // The default time until timeout for sequential events.
    static const int kDefaultTimeoutMillis = 3000;


    // Used to change a pen state.
    std::shared_ptr<IrPenManager> penManager_;
    // Used to handle screen changes.
    std::shared_ptr<ScreenManager> screenManager_;
    // Used to dispatch events to listeners.
    std::shared_ptr<InputEventDispatcher> dispatcher_;
    // Used to detect ir point
    std::shared_ptr<HsvSensor> strokeDetector_;
    // Used to Map hsv detect point Id and pen Id.
    std::optional<IrPenHsvMapper> penHsvMapper_;


    // The worker thread to process event data.
    std::thread processorThread_;
    // The condition for the event queue.
    std::condition_variable queueCondition_;
    // The lock object for thread operations.
    std::mutex processorMutex_;
    // The flag indicating whether the processing loop is started or not.
    std::atomic_bool started_;
    // The flag indicating whether the loop should exit or not.
    std::atomic_bool exitLoop_;

    // The event queue.
    std::queue<std::function<void()>> queue_;

    // The lock object for processing data.
    std::mutex queueMutex_;

    // The time until the event timeout, in milliseconds.
    int eventTimeoutMillis_;

    // This member will be removed.
    std::optional<PenEventProcessor> penEventProcessor_;

    void enqueue(std::function<void()> task);

    /**
     * @brief Processes received data in a loop.
     */
    void run();
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_InputEventProcessor_H_
