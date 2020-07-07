#ifndef LIB_DOODLE_FRAMEWORK_doodle_PenEventProcessor_H_
#define LIB_DOODLE_FRAMEWORK_doodle_PenEventProcessor_H_

#include "doodle/namespace.h"
#include "doodle/IrPenManager.h"
#include "doodle/ScreenManager.h"
#include "doodle/InputEventDispatcher.h"

#include "doodle/DoodleEvent.h"
#include "doodle/IrPenEvent.h"
#include "doodle/ScreenEvent.h"
#include "doodle/ScreenEventListener.h"
#include "doodle/StrokeStatus.h"
#include "doodle/StrokeStatusBank.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

class PenEventProcessor
{
    using Action = PenMotionEvent::Action;
public:
    PenEventProcessor(const std::shared_ptr<IrPenManager>& penManager,
        const std::shared_ptr<ScreenManager>& screenManager,
        const std::shared_ptr<InputEventDispatcher>& dispatcher,
        const std::function<void(std::function<void()>)>& enqueue);
    virtual ~PenEventProcessor();

    /**
     * @brief Processes a pen motion event.
     * @param The motion event.
     */
    void processPenMotionEvent(std::shared_ptr<PenMotionEvent>& e);

    void processScreenChangeStart();
    void processScreenChangeEnd();
    void processDeviceUpdate(const std::shared_ptr<PenDeviceUpdateEvent>& e);
    void processPenActive(const std::shared_ptr<PenActiveEvent>& e);
    void processPenInactive(const std::shared_ptr<PenInactiveEvent>& e);
    void processPenTimeout(const std::shared_ptr<PenTimeoutEvent>& e);

private:
    // Used to change a pen state.
    std::shared_ptr<IrPenManager> penManager_;
    std::map<int, bool> penTipButtonState_;
    // Used to handle screen changes.
    std::shared_ptr<ScreenManager> screenManager_;
    // Used to map motion events to the current screen.
    std::shared_ptr<ScreenCoordinateMapper> coordinateMapper_;
    // Used to dispatch events to listeners.
    std::shared_ptr<InputEventDispatcher> dispatcher_;

    // Whether the current screen is changing to another screen.
    bool screenChanging_ = false;

    std::function<void(std::function<void()>)> enqueue_;

    // This member will be removed.
    StrokeStatusBank strokeStatusBank_;

    /**
     * @brief Queues a motion event with the specified parameters.
     * @param deviceId The device ID.
     * @param action The action of the motion event.
     * @param trackingId The sensor tracking ID.
     * @param x The x-axis value.
     * @param y The y-axis value.
     */
    void queueMotionEvent(const int deviceId,
        const Action action,
        const unsigned char trackingId,
        const float x,
        const float y);

    /**
     * @brief Queues a motion event with the cancel action.
     * @param deviceId The device ID.
     * @param trackingId The sensor tracking ID.
     * @param x The x-axis value.
     * @param y The y-axis value.
     */
    void queueCancelMotionEvent(const int deviceId,
        const unsigned char trackingId,
        const float x,
        const float y);

    /**
     * @brief Determines whether the motion event should be notified.
     * @param e The motion event.
     */
    bool shouldNotifyMotionEvent(const std::shared_ptr<PenMotionEvent>& e);

    /**
     * @brief Processes the pending motion event.
     * @param e The pending motion event.
     */
    void processPendingMotionEvent(std::shared_ptr<PenMotionEvent>& e);

    /**
     * @brief Handles a new motion event with the active stroke.
     * @param stroke The target active stroke.
     * @param e The new motion event.
     */
    void handleActiveStroke(std::shared_ptr<StrokeStatus>& stroke,
        std::shared_ptr<PenMotionEvent>& e);

    /**
     * @brief Handles a new motion event with the inactive stroke.
     * @param stroke The target stroke.
     * @param e The new motion event.
     */
    void handleInactiveStroke(std::shared_ptr<StrokeStatus>& stroke,
        std::shared_ptr<PenMotionEvent>& e);
};

LIB_DOODLE_FRAMEWORK_namespace_doodle_END

#endif // LIB_DOODLE_FRAMEWORK_doodle_PenEventProcessor_H_
