#include "doodle/InputEventProcessor.h"
#include "doodle/PenEventProcessor.h"

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

PenEventProcessor::PenEventProcessor(const std::shared_ptr<IrPenManager>& penManager,
    const std::shared_ptr<ScreenManager>& screenManger,
    const std::shared_ptr<InputEventDispatcher>& dispatcher,
    const std::function<void(std::function<void()>)>& enqueue) :
    penManager_(penManager),
    screenManager_(screenManger),
    dispatcher_(dispatcher),
    enqueue_(enqueue)
{
}

PenEventProcessor::~PenEventProcessor()
{
}

void PenEventProcessor::processScreenChangeStart()
{
    // TODO: Use deque if needed to notify of the screen state immediately.
    screenChanging_ = true;

    strokeStatusBank_.foreach([this](auto id, auto status) {
        if (!status->isActive()) {
            return;
        }
        status->setActive(false);
        // Create a motion event with the cancel action to notify of an invalid stroke.
        queueCancelMotionEvent(id,
            status->getLastTrackingId(),
            status->getLastX(),
            status->getLastY());
    });
}

void PenEventProcessor::processScreenChangeEnd()
{
    coordinateMapper_ = screenManager_->getCurrentCoordinateMapper();
    screenChanging_ = false;
}

void PenEventProcessor::processDeviceUpdate(const std::shared_ptr<PenDeviceUpdateEvent>& e)
{
    strokeStatusBank_.clear();

    const size_t count = e->getDeviceCount();
    for (int i = 0; i < count; i++)
    {
        IrPenDeviceStatus state = e->getDeviceStatus(i);
        const int id = state.deviceId;
        auto stroke = std::make_shared<StrokeStatus>(id, false, Action::NONE, true);
        strokeStatusBank_.put(id, std::move(stroke));
        // Initialize the device state.
        bool ret = penManager_->setIrLedState(id, false);
        ret = ret && penManager_->setRgbLedState(id, RgbLedState::kBlink, state.rgbLedColor);
        if (!ret) { 
            std::cerr << "pen Manager error" << std::endl;
        }
    }
}

void PenEventProcessor::processPenActive(const std::shared_ptr<PenActiveEvent>& e)
{
    penTipButtonState_[e->getDeviceId()] = true;
    //const int deviceId = e->getDeviceId();

    //// Update the pen device state.
    //// The sensor will start detecting a stroke.
    //bool ret = penManager_->setIrLedState(deviceId, true);
    //if (!ret) {
    //    std::cerr << "pen Manager error" << std::endl;
    //}

    //auto& status = strokeStatusBank_.get(deviceId);
    //status->setActive(true);
    //status->setNextAction(Action::DOWN);
}

void PenEventProcessor::processPenInactive(const std::shared_ptr<PenInactiveEvent>& e)
{
    penTipButtonState_[e->getDeviceId()] = false;
    //const int deviceId = e->getDeviceId();

    //auto& status = strokeStatusBank_.get(deviceId);
    //if (status->isExpired())
    //{
    //    // Turn off the IR LED of the pen.
    //    bool ret = penManager_->setIrLedState(deviceId, false);
    //    // Note that no stroke will be detected after turning off the IR LED.
    //    if (!ret) {
    //        std::cerr << "pen Manager error" << std::endl;
    //    }
    //    return;
    //}
    //status->setActive(false);
    //status->setNextAction(Action::UP);
}

void PenEventProcessor::processPenTimeout(const std::shared_ptr<PenTimeoutEvent>& e)
{
    const int deviceId = e->getDeviceId();
    // Check whether the stroke status exists since this type of event
    // may be processed after a device state change.
    auto& status = strokeStatusBank_.tryGet(deviceId);
    if (!status) {
        return;
    }

    // Blink the RGB LED to notify of detection failure.
    bool ret = penManager_->setRgbLedState(deviceId, RgbLedState::kBlink);
    if (!ret) {
        std::cerr << "pen Manager error" << std::endl;
    }

    status->setExpired(true);
    // Create a motion event with the cancel action to notify of an invalid stroke.
    queueCancelMotionEvent(deviceId,
        status->getLastTrackingId(),
        status->getLastX(),
        status->getLastY());
}

void PenEventProcessor::queueMotionEvent(const int deviceId,
    const Action action,
    const unsigned char trackingId,
    const float x,
    const float y)
{
    using std::chrono::system_clock;
    using std::chrono::duration_cast;
    using std::chrono::microseconds;

    auto timestamp = system_clock::now();
    system_clock::duration timeEpoch = timestamp.time_since_epoch();
    int64_t timeMicros = duration_cast<microseconds>(timeEpoch).count();
    auto e = std::make_shared<PenMotionEvent>(deviceId, action, trackingId, x, y, timeMicros);
    // Queue the motion event to dispatch.
    auto task = [this, e]() {
        auto copy = e;
        processPenMotionEvent(copy);
    };
    enqueue_(task);
}

void PenEventProcessor::queueCancelMotionEvent(const int deviceId,
    const unsigned char trackingId,
    const float x,
    const float y)
{
    queueMotionEvent(deviceId, Action::CANCEL, trackingId, x, y);
}

void PenEventProcessor::processPenMotionEvent(std::shared_ptr<PenMotionEvent>& e)
{
    const int deviceId = e->getDeviceId();
    bool isTipPressed = penTipButtonState_[deviceId];
    auto& stroke = strokeStatusBank_.tryGet(deviceId);
    if (!stroke) { return; }
    //edit action.
    bool isReturn = false;
    switch (e->getSensingState())
    {
    case HsvSensor::SensingState::DETECT:
        if (isTipPressed) { //down
            stroke->setActive(true);
            stroke->setNextAction(Action::DOWN);
        }
        else {
            //hsv sensor detect and  tip switch is not pressed. so judge no stroke
            isReturn = true;
        }
        break;
    case HsvSensor::SensingState::TRACK:
        if (isTipPressed && !stroke->isActive())
        {
            //down
            stroke->setActive(true);
            stroke->setNextAction(Action::DOWN);
        }
        else if (!isTipPressed && stroke->isActive())
        {
            //up
            stroke->setActive(false);
            stroke->setNextAction(Action::UP);
        }
        else if (isTipPressed && stroke->isActive()) {
            //track
        }
        else {
            //hsv sensor track and tip switch is not pressed. so judge no stroke 
            isReturn = true;
        }
        break;
    case HsvSensor::SensingState::LOST:
        if (isTipPressed)
        {
            //up
            stroke->setActive(false);
            stroke->setNextAction(Action::UP);
        }
        else {
            isReturn = true;
        }
        break;
    case HsvSensor::SensingState::NONE:
        break;
    }
    //if isReturn is true, hsv sensor detected, but no stroke event 
    if (isReturn) { return; }


    // Assign a device ID and set an action to the motion event.
    processPendingMotionEvent(e);

    if (!e->isDeviceIdValid()) return;

    const auto action = e->getAction();
    auto& status = strokeStatusBank_.tryGet(deviceId);
    if (status) {
        if (status->isFirstDownAction())
        {
            // FIXME: Workaround to restore RGB LED color.
            // This should be fixed on firmware.
            Color color;
            if (penManager_->getRgbLedColor(deviceId, &color))
            {
                penManager_->setRgbLedState(deviceId, RgbLedState::kTurnedOn, color);
            }
            else
            {
                penManager_->setRgbLedState(deviceId, RgbLedState::kTurnedOn);
            }
            // Activate the stroke.
            status->setExpired(false);
        }
    }

    switch (action)
    {
    case Action::NONE:
        break;
    case Action::DOWN:
        // Already updated the timeout timer.
        break;
    case Action::UP:
    case Action::CANCEL:
        // Already completed or cancelled the timeout timer.
        break;
    }

    if (shouldNotifyMotionEvent(e))
    {
        if (dispatcher_) dispatcher_->notifyMotion(e);
    }
}

bool PenEventProcessor::shouldNotifyMotionEvent(const std::shared_ptr<PenMotionEvent>& e)
{
    return (!screenChanging_ && e->isDeviceIdValid());
}

void PenEventProcessor::processPendingMotionEvent(std::shared_ptr<PenMotionEvent>& e)
{
    const auto count = strokeStatusBank_.size();
    if (count == 0) {
        return;
    }

    if (coordinateMapper_)
    {
        const auto point = coordinateMapper_->map(e->getX(), e->getY());
        e->setX(point->x);
        e->setY(point->y);
    }

    auto stroke = strokeStatusBank_.tryGet(e->getDeviceId());
    if (stroke->isActive())
    {
        handleActiveStroke(stroke, e);
    }
    else
    {
        handleInactiveStroke(stroke, e);
    }
}

void PenEventProcessor::handleActiveStroke(std::shared_ptr<StrokeStatus>& stroke,
    std::shared_ptr<PenMotionEvent>& e)
{
    if (!e->isDeviceIdValid()) e->setDeviceId(stroke->getDeviceId());

    switch (stroke->getNextAction())
    {
    case Action::DOWN:
        e->setAction(stroke->getNextAction());
        stroke->setNextAction(Action::NONE);
        break;
    default:
        break;
    }
    stroke->setLastMotionEvent(e);
}

void PenEventProcessor::handleInactiveStroke(std::shared_ptr<StrokeStatus>& stroke,
    std::shared_ptr<PenMotionEvent>& e)
{
    switch (stroke->getNextAction())
    {
    case Action::UP:
    case Action::CANCEL:
        if (!e->isDeviceIdValid()) e->setDeviceId(stroke->getDeviceId());
        e->setAction(stroke->getNextAction());
        stroke->setNextAction(Action::NONE);
        stroke->setLastMotionEvent(e);
        break;
    default:
        // Ignore other inactive events.
        break;
    }
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
