#include "doodle/InputEventProcessor.h"

#include <limits>

LIB_DOODLE_FRAMEWORK_namespace_doodle_BEGIN

InputEventProcessor::InputEventProcessor(const std::shared_ptr<IrPenManager>& penManager,
                                         const std::shared_ptr<ScreenManager>& screenManager,
                                         const std::shared_ptr<InputEventDispatcher>& dispatcher,
                                         const std::shared_ptr<HsvSensor> strokeDetector) :
    penManager_(penManager),
    screenManager_(screenManager),
    dispatcher_(dispatcher),
    strokeDetector_(strokeDetector),
    started_(false),
    exitLoop_(false)
{
    penManager_->setDevicesUpdatedCallback(this, &InputEventProcessor::onDevicesUpdated);
    penManager_->setStateChangedCallback(this, &InputEventProcessor::onPenStateChanged);
    penManager_->setColorReceivedCallback(this, &InputEventProcessor::onColorValueReceived);

    penEventProcessor_.emplace(
        penManager_,
        screenManager_,
        dispatcher_,
        [this](std::function<void()> task) -> void {
            enqueue(task);
        });
    penHsvMapper_.emplace(
        penManager_, 
        strokeDetector_);
    penHsvMapper_->setDetectionResultCallback(this, &InputEventProcessor::onStrokePointDetected);
    penHsvMapper_->start();
}

InputEventProcessor::~InputEventProcessor()
{
    penManager_.reset();
    screenManager_.reset();
    dispatcher_.reset();
    stop();
}

void InputEventProcessor::start()
{
    std::lock_guard<std::mutex> lock(processorMutex_);
    if (started_) return;

    processorThread_ = std::thread([this] { run(); });
}

void InputEventProcessor::stop()
{
    std::lock_guard<std::mutex> lock(processorMutex_);
    if (!started_) return;

    exitLoop_ = true;
    queueCondition_.notify_one();

    if (processorThread_.joinable())
    {
        processorThread_.join();
    }
}

bool InputEventProcessor::isStarted()
{
    std::lock_guard<std::mutex> lock(processorMutex_);
    return started_;
}

void InputEventProcessor::enqueue(std::function<void()> task)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    queue_.push(task);
    queueCondition_.notify_one();
}

void InputEventProcessor::onScreenInitEvent(const std::shared_ptr<ScreenInitEvent>& e)
{
    auto task = [this, e]() {
        if (dispatcher_) {
            dispatcher_->notifyScreenInit(e);
        }
    };
    enqueue(task);
}

void InputEventProcessor::onScreenChangeEvent(const std::shared_ptr<ScreenChangeEvent>& e)
{
    auto task = [this, e]() {
        switch (e->getAction())
        {
        case ScreenChangeEvent::Action::START:
            penEventProcessor_->processScreenChangeStart();
            break;
        case ScreenChangeEvent::Action::COMPLETE:
            penEventProcessor_->processScreenChangeEnd();
            break;
        default:
            break;
        }

        if (dispatcher_) {
            dispatcher_->notifyChangeCurrentScreen(e);
        }
    };
    enqueue(task);
}

void InputEventProcessor::onDevicesUpdated(const std::vector<IrPenDeviceStatus> statusList)
{
    auto e = std::make_shared<PenDeviceUpdateEvent>(statusList);
    auto task = [this, e]() {
        penEventProcessor_->processDeviceUpdate(e);
        penHsvMapper_->devicesUpdate(e);
        if (dispatcher_) {
            dispatcher_->notifyDeviceUpdate(e);
        }
    };
    enqueue(task);
}

void InputEventProcessor::onPenStateChanged(const int deviceId, const bool pressed)
{
    if (pressed)
    {
        auto e = std::make_shared<PenActiveEvent>(deviceId);
        auto task = [this, e]() {
            penEventProcessor_->processPenActive(e);
        };
        enqueue(task);
    }
    else
    {
        auto e = std::make_shared<PenInactiveEvent>(deviceId);
        auto task = [this, e]() {
            penEventProcessor_->processPenInactive(e);
        };
        enqueue(task);
    }
}

void InputEventProcessor::onColorValueReceived(const int deviceId, const Color color)
{
    auto e = std::make_shared<PenColorEvent>(deviceId, color);
    auto task = [this, e]() {
        if (dispatcher_) {
            dispatcher_->notifyColor(e);
        }
    };
    enqueue(task);
}

void InputEventProcessor::onStrokePointDetected(const std::vector<HsvSensor::SensingResult>& results)
{
    std::lock_guard<std::mutex> lock(queueMutex_);
    for (auto&& result : results)
    {
        auto e = std::make_shared<PenMotionEvent>((int)result.trackingId, result);
        auto task = [this, e]() {
            auto copy = e;
            penEventProcessor_->processPenMotionEvent(copy);
        };
        queue_.push(task);
    }
    queueCondition_.notify_one();
}

void InputEventProcessor::run()
{
    started_ = true;
    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            while (queue_.empty())
            {
                if (exitLoop_) break;
                queueCondition_.wait(lock);
            }

            if (exitLoop_) break;
            if (queue_.empty()) continue;

            auto& e = queue_.front();
            e();
            queue_.pop();
        }
    }
    started_ = false;
}

LIB_DOODLE_FRAMEWORK_namespace_doodle_END
